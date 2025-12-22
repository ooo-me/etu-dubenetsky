// ============================================================================
// Repository для работы с тарифами
// Язык: C++20
// Описание: Репозиторий для работы с SQL процедурами тарифов
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include "../utils/Database.hpp"
#include "../model/Service.hpp"
#include "../model/Tariff.hpp"
#include "../model/Order.hpp"
#include "../model/Classifier.hpp"
#include "../model/Parameter.hpp"
#include <memory>
#include <vector>
#include <optional>

namespace TariffSystem::Repository {

using namespace TariffSystem::Database;
using namespace TariffSystem::Model;

/**
 * Репозиторий для работы с классификатором
 */
class ClassifierRepository {
public:
    explicit ClassifierRepository(std::shared_ptr<DatabaseManager> db) : db_(db) {}
    
    /**
     * Создание класса через процедуру INS_CLASS
     */
    Integer createClass(const String& code, const String& name, 
                       std::optional<Integer> parentId, 
                       const String& note = "") {
        String query = "SELECT INS_CLASS($1, $2, $3, $4)";
        std::vector<String> params = {
            code,
            name,
            parentId ? std::to_string(*parentId) : "NULL",
            note
        };
        
        auto result = db_->executeQuery(query, params);
        if (result->getRowCount() > 0) {
            auto id = result->getInt(0, 0);
            if (id) return *id;
        }
        throw DatabaseException("Не удалось создать класс");
    }
    
    /**
     * Получение класса по ID
     */
    std::shared_ptr<Classifier> getClassById(Integer id) {
        String query = "SELECT ID_CHEM, COD_CHEM, NAME_CHEM, PARENT_ID, LEV, NOTE "
                      "FROM CHEM_CLASS WHERE ID_CHEM = $1";
        
        auto result = db_->executeQuery(query, {std::to_string(id)});
        
        if (result->getRowCount() == 0) {
            return nullptr;
        }
        
        auto code = result->getValue(0, 1);
        auto name = result->getValue(0, 2);
        auto level = result->getInt(0, 4);
        
        if (!code || !name || !level) {
            throw DatabaseException("Некорректные данные класса");
        }
        
        auto classifier = std::make_shared<Classifier>(id, *code, *name, *level);
        
        auto parentId = result->getInt(0, 3);
        if (parentId) {
            classifier->setParentId(*parentId);
        }
        
        auto note = result->getValue(0, 5);
        if (note) {
            classifier->setNote(*note);
        }
        
        return classifier;
    }
    
    /**
     * Получение дочерних классов
     */
    std::vector<std::shared_ptr<Classifier>> getChildClasses(Integer parentId) {
        String query = "SELECT ID_CHEM, COD_CHEM, NAME_CHEM, LEV "
                      "FROM CHEM_CLASS WHERE PARENT_ID = $1 ORDER BY ID_CHEM";
        
        auto result = db_->executeQuery(query, {std::to_string(parentId)});
        
        std::vector<std::shared_ptr<Classifier>> children;
        for (int i = 0; i < result->getRowCount(); ++i) {
            auto id = result->getInt(i, 0);
            auto code = result->getValue(i, 1);
            auto name = result->getValue(i, 2);
            auto level = result->getInt(i, 3);
            
            if (id && code && name && level) {
                auto classifier = std::make_shared<Classifier>(*id, *code, *name, *level);
                classifier->setParentId(parentId);
                children.push_back(classifier);
            }
        }
        
        return children;
    }

private:
    std::shared_ptr<DatabaseManager> db_;
};

/**
 * Репозиторий для работы с функциями и правилами
 */
class FunctionRepository {
public:
    explicit FunctionRepository(std::shared_ptr<DatabaseManager> db) : db_(db) {}
    
    /**
     * Создание функции через процедуру INS_FUNCT
     */
    Integer createFunction(const String& code, const String& name,
                          Integer type, const String& operation = "",
                          const String& note = "") {
        String query = "SELECT INS_FUNCT($1, $2, $3, $4, $5)";
        std::vector<String> params = {
            code,
            name,
            std::to_string(type),
            operation.empty() ? "NULL" : operation,
            note
        };
        
        auto result = db_->executeQuery(query, params);
        if (result->getRowCount() > 0) {
            auto id = result->getInt(0, 0);
            if (id) return *id;
        }
        throw DatabaseException("Не удалось создать функцию");
    }
    
    /**
     * Добавление аргумента к функции через INS_ARG_FUN
     */
    Integer addArgument(Integer functionId, Integer argNumber,
                       std::optional<Integer> classArg = std::nullopt,
                       const String& name = "",
                       const String& note = "") {
        String query = "SELECT INS_ARG_FUN($1, $2, $3, $4, $5)";
        std::vector<String> params = {
            std::to_string(functionId),
            std::to_string(argNumber),
            classArg ? std::to_string(*classArg) : "NULL",
            name.empty() ? "NULL" : name,
            note
        };
        
        auto result = db_->executeQuery(query, params);
        if (result->getRowCount() > 0) {
            auto id = result->getInt(0, 0);
            if (id) return *id;
        }
        throw DatabaseException("Не удалось добавить аргумент");
    }

private:
    std::shared_ptr<DatabaseManager> db_;
};

/**
 * Репозиторий для работы с объектами (услуги, тарифы, заказы)
 */
class ObjectRepository {
public:
    explicit ObjectRepository(std::shared_ptr<DatabaseManager> db) : db_(db) {}
    
    /**
     * Создание объекта через процедуру INS_OB
     */
    Integer createObject(Integer classId, const String& code, const String& name,
                        std::optional<Integer> parentId = std::nullopt,
                        const String& note = "") {
        String query = "SELECT INS_OB($1, $2, $3, $4, $5)";
        std::vector<String> params = {
            std::to_string(classId),
            code,
            name,
            parentId ? std::to_string(*parentId) : "NULL",
            note
        };
        
        auto result = db_->executeQuery(query, params);
        if (result->getRowCount() > 0) {
            auto id = result->getInt(0, 0);
            if (id) return *id;
        }
        throw DatabaseException("Не удалось создать объект");
    }
    
    /**
     * Получение объекта по ID
     */
    std::tuple<Integer, String, String, std::optional<String>> getObject(Integer id) {
        String query = "SELECT CLASS_PR, COD_PR, NAME_PR, NOTE "
                      "FROM PROD WHERE ID_PR = $1";
        
        auto result = db_->executeQuery(query, {std::to_string(id)});
        
        if (result->getRowCount() == 0) {
            throw DatabaseException("Объект не найден");
        }
        
        auto classId = result->getInt(0, 0);
        auto code = result->getValue(0, 1);
        auto name = result->getValue(0, 2);
        auto note = result->getValue(0, 3);
        
        if (!classId || !code || !name) {
            throw DatabaseException("Некорректные данные объекта");
        }
        
        return {*classId, *code, *name, note};
    }
    
    /**
     * Получение параметров объекта через FIND_VAL_ALL_PAR
     */
    std::map<Integer, ParameterValue> getObjectParameters(Integer objectId) {
        String query = "SELECT * FROM FIND_VAL_ALL_PAR($1)";
        
        auto result = db_->executeQuery(query, {std::to_string(objectId)});
        
        std::map<Integer, ParameterValue> params;
        
        for (int i = 0; i < result->getRowCount(); ++i) {
            auto parId = result->getInt(i, 0);
            auto parType = result->getInt(i, 3);
            
            if (!parId || !parType) continue;
            
            // В зависимости от типа параметра, читаем соответствующее значение
            switch (*parType) {
                case 0: { // INTEGER/DOUBLE
                    auto val = result->getDouble(i, 4);
                    if (val) params[*parId] = *val;
                    break;
                }
                case 1: { // STRING
                    auto val = result->getValue(i, 5);
                    if (val) params[*parId] = *val;
                    break;
                }
                // Добавить другие типы по необходимости
            }
        }
        
        return params;
    }
    
    /**
     * Обновление значения роли через UPDATE_VAL_ROLE
     */
    void updateRoleValue(Integer functionId, Integer objectId,
                        std::optional<Double> numValue = std::nullopt) {
        String query = "SELECT UPDATE_VAL_ROLE($1, $2, NULL, NULL, $3, NULL, NULL, NULL, NULL)";
        std::vector<String> params = {
            std::to_string(functionId),
            std::to_string(objectId),
            numValue ? std::to_string(*numValue) : "NULL"
        };
        
        db_->executeQuery(query, params);
    }

private:
    std::shared_ptr<DatabaseManager> db_;
};

/**
 * Репозиторий для расчетов
 */
class CalculationRepository {
public:
    explicit CalculationRepository(std::shared_ptr<DatabaseManager> db) : db_(db) {}
    
    /**
     * Расчет значения функции через CALC_VAL_F
     */
    Double calculateValue(Integer functionId, Integer objectId,
                         std::optional<Integer> tariffId = std::nullopt) {
        String query = "SELECT CALC_VAL_F($1, $2, $3)";
        std::vector<String> params = {
            std::to_string(functionId),
            std::to_string(objectId),
            tariffId ? std::to_string(*tariffId) : "NULL"
        };
        
        auto result = db_->executeQuery(query, params);
        
        if (result->getRowCount() == 0) {
            throw DatabaseException("Не удалось вычислить значение");
        }
        
        auto value = result->getDouble(0, 0);
        if (!value) {
            throw DatabaseException("Некорректный результат вычисления");
        }
        
        return *value;
    }
    
    /**
     * Валидация заказа через VALIDATE_ORDER
     */
    std::pair<bool, String> validateOrder(Integer orderId) {
        String query = "SELECT * FROM VALIDATE_ORDER($1)";
        
        auto result = db_->executeQuery(query, {std::to_string(orderId)});
        
        if (result->getRowCount() == 0) {
            return {false, "Не удалось выполнить валидацию"};
        }
        
        // Предполагается, что процедура возвращает is_valid, error_message
        auto isValid = result->getValue(0, 0);
        auto message = result->getValue(0, 1);
        
        bool valid = isValid && (*isValid == "t" || *isValid == "true" || *isValid == "1");
        String msg = message ? *message : "";
        
        return {valid, msg};
    }

private:
    std::shared_ptr<DatabaseManager> db_;
};

/**
 * Главный репозиторий - фасад для всех операций с БД
 */
class TariffSystemRepository {
public:
    explicit TariffSystemRepository(std::shared_ptr<DatabaseManager> db)
        : db_(db)
        , classifierRepo_(db)
        , functionRepo_(db)
        , objectRepo_(db)
        , calculationRepo_(db)
    {}
    
    // Геттеры для специализированных репозиториев
    ClassifierRepository& classifiers() { return classifierRepo_; }
    FunctionRepository& functions() { return functionRepo_; }
    ObjectRepository& objects() { return objectRepo_; }
    CalculationRepository& calculations() { return calculationRepo_; }
    
    /**
     * Начало транзакции
     */
    void beginTransaction() {
        db_->beginTransaction();
    }
    
    /**
     * Подтверждение транзакции
     */
    void commit() {
        db_->commit();
    }
    
    /**
     * Откат транзакции
     */
    void rollback() {
        db_->rollback();
    }

private:
    std::shared_ptr<DatabaseManager> db_;
    ClassifierRepository classifierRepo_;
    FunctionRepository functionRepo_;
    ObjectRepository objectRepo_;
    CalculationRepository calculationRepo_;
};

} // namespace TariffSystem::Repository
