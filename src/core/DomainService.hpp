// ============================================================================
// DomainService - Слой оркестрации бизнес-логики и персистентности
// Язык: C++20
// Описание: Центральный сервис для управления всеми доменными сущностями,
//           обеспечивающий целостность данных между C++ моделью и PostgreSQL
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include "../utils/Database.hpp"
#include "../utils/Repository.hpp"
#include "../model/Service.hpp"
#include "../model/Tariff.hpp"
#include "../model/Order.hpp"
#include "../model/Classifier.hpp"
#include "../model/Parameter.hpp"
#include "../model/Rule.hpp"
#include "RuleEngine.hpp"
#include "CostCalculator.hpp"
#include "OptimalSearch.hpp"

#include <memory>
#include <vector>
#include <map>
#include <optional>
#include <stdexcept>

namespace TariffSystem::Core {

using namespace TariffSystem::Database;
using namespace TariffSystem::Repository;
using namespace TariffSystem::Model;

/**
 * Исключение для ошибок в DomainService
 */
class DomainServiceException : public std::runtime_error {
public:
    explicit DomainServiceException(const String& message) 
        : std::runtime_error(message) {}
};

/**
 * Результат операции с информацией о выполнении
 */
struct OperationResult {
    bool success;
    String message;
    std::optional<Integer> entityId;
    
    OperationResult(bool s, const String& m, std::optional<Integer> id = std::nullopt)
        : success(s), message(m), entityId(id) {}
};

/**
 * DomainService - центральный оркестратор всех операций
 * 
 * Обязанности:
 * 1. Управление жизненным циклом всех доменных объектов
 * 2. Обеспечение персистентности в PostgreSQL через Repository
 * 3. Применение бизнес-правил через RuleEngine
 * 4. Расчет стоимости через CostCalculator
 * 5. Поиск оптимальных тарифов через OptimalSearcher
 * 6. Валидация данных и целостность
 * 7. Транзакционность операций
 * 8. Кэширование для производительности
 */
class DomainService {
public:
    /**
     * Конструктор с подключением к БД
     */
    explicit DomainService(std::shared_ptr<DatabaseManager> db)
        : db_(db)
        , repository_(std::make_unique<TariffSystemRepository>(db))
        , ruleEngine_(std::make_unique<RuleEngine>())
        , costCalculator_(std::make_unique<CostCalculator>(ruleEngine_.get()))
        , optimalSearcher_(std::make_unique<OptimalSearcher>())
    {
        if (!db || !db->isConnected()) {
            throw DomainServiceException("База данных не подключена");
        }
    }
    
    // ========================================================================
    // УПРАВЛЕНИЕ КЛАССИФИКАТОРАМИ
    // ========================================================================
    
    /**
     * Создание нового классификатора
     * Создает запись в БД и возвращает объект с ID
     */
    std::shared_ptr<Classifier> createClassifier(
        const String& code, 
        const String& name,
        Integer level,
        std::optional<Integer> parentId = std::nullopt,
        const String& note = ""
    ) {
        try {
            db_->beginTransaction();
            
            // Валидация
            if (code.empty() || name.empty()) {
                throw DomainServiceException("Код и название классификатора обязательны");
            }
            
            if (level < 1 || level > 10) {
                throw DomainServiceException("Уровень должен быть от 1 до 10");
            }
            
            // Проверка родителя
            if (parentId) {
                auto parent = repository_->classifiers().getClassById(*parentId);
                if (!parent) {
                    throw DomainServiceException("Родительский классификатор не найден");
                }
            }
            
            // Создание в БД
            Integer id = repository_->classifiers().createClass(code, name, parentId, note);
            
            // Создание объекта
            auto classifier = std::make_shared<Classifier>(id, code, name, level);
            if (parentId) {
                classifier->setParentId(*parentId);
            }
            classifier->setNote(note);
            
            // Кэширование
            classifierCache_[id] = classifier;
            
            db_->commit();
            
            return classifier;
            
        } catch (const std::exception& e) {
            db_->rollback();
            throw DomainServiceException(
                String("Ошибка создания классификатора: ") + e.what()
            );
        }
    }
    
    /**
     * Получение классификатора по ID с кэшированием
     */
    std::shared_ptr<Classifier> getClassifier(Integer id) {
        // Проверка кэша
        auto it = classifierCache_.find(id);
        if (it != classifierCache_.end()) {
            return it->second;
        }
        
        // Загрузка из БД
        auto classifier = repository_->classifiers().getClassById(id);
        if (classifier) {
            classifierCache_[id] = classifier;
        }
        
        return classifier;
    }
    
    /**
     * Получение всех дочерних классификаторов
     */
    std::vector<std::shared_ptr<Classifier>> getChildClassifiers(Integer parentId) {
        return repository_->classifiers().getChildClasses(parentId);
    }
    
    // ========================================================================
    // УПРАВЛЕНИЕ УСЛУГАМИ
    // ========================================================================
    
    /**
     * Создание услуги с сохранением в БД
     */
    std::shared_ptr<Service> createService(
        Integer classId,
        const String& code,
        const String& name,
        const String& description = ""
    ) {
        try {
            db_->beginTransaction();
            
            // Валидация
            if (code.empty() || name.empty()) {
                throw DomainServiceException("Код и название услуги обязательны");
            }
            
            // Проверка класса
            auto classifier = getClassifier(classId);
            if (!classifier) {
                throw DomainServiceException("Класс услуги не найден");
            }
            
            // Создание объекта в БД через INS_OB
            Integer serviceId = repository_->objects().createObject(
                classId, code, name, description
            );
            
            // Создание C++ объекта
            auto service = std::make_shared<Service>(serviceId, code, name);
            service->setDescription(description);
            
            // Кэширование
            serviceCache_[serviceId] = service;
            
            db_->commit();
            
            return service;
            
        } catch (const std::exception& e) {
            db_->rollback();
            throw DomainServiceException(
                String("Ошибка создания услуги: ") + e.what()
            );
        }
    }
    
    /**
     * Получение услуги по ID
     */
    std::shared_ptr<Service> getService(Integer id) {
        auto it = serviceCache_.find(id);
        if (it != serviceCache_.end()) {
            return it->second;
        }
        
        auto service = repository_->objects().getObjectById(id);
        if (service) {
            serviceCache_[id] = service;
        }
        
        return service;
    }
    
    /**
     * Добавление параметра к услуге
     */
    OperationResult addServiceParameter(
        Integer serviceId,
        Integer parameterId,
        const String& paramName,
        ParameterType paramType,
        bool required = false
    ) {
        try {
            db_->beginTransaction();
            
            auto service = getService(serviceId);
            if (!service) {
                throw DomainServiceException("Услуга не найдена");
            }
            
            // Создание параметра в C++ модели
            auto parameter = std::make_shared<Parameter>(
                parameterId, paramName, paramType
            );
            parameter->setRequired(required);
            
            service->addParameter(parameter);
            
            // Сохранение связи в БД
            repository_->objects().addParameterToObject(serviceId, parameterId);
            
            db_->commit();
            
            return OperationResult(true, "Параметр добавлен", parameterId);
            
        } catch (const std::exception& e) {
            db_->rollback();
            return OperationResult(false, 
                String("Ошибка добавления параметра: ") + e.what());
        }
    }
    
    // ========================================================================
    // УПРАВЛЕНИЕ ТАРИФАМИ
    // ========================================================================
    
    /**
     * Создание тарифа с правилами
     */
    std::shared_ptr<Tariff> createTariff(
        Integer serviceId,
        const String& code,
        const String& name,
        const String& provider,
        const String& description = ""
    ) {
        try {
            db_->beginTransaction();
            
            // Валидация
            if (code.empty() || name.empty() || provider.empty()) {
                throw DomainServiceException(
                    "Код, название и поставщик тарифа обязательны"
                );
            }
            
            // Проверка услуги
            auto service = getService(serviceId);
            if (!service) {
                throw DomainServiceException("Услуга не найдена");
            }
            
            // Создание в БД
            Integer tariffId = repository_->objects().createObject(
                serviceId, code, name, description
            );
            
            // Создание C++ объекта
            auto tariff = std::make_shared<Tariff>(tariffId, code, name);
            tariff->setDescription(description);
            tariff->setProvider(provider);
            tariff->activate();
            
            // Кэширование
            tariffCache_[tariffId] = tariff;
            
            db_->commit();
            
            return tariff;
            
        } catch (const std::exception& e) {
            db_->rollback();
            throw DomainServiceException(
                String("Ошибка создания тарифа: ") + e.what()
            );
        }
    }
    
    /**
     * Получение тарифа по ID с загрузкой правил
     */
    std::shared_ptr<Tariff> getTariff(Integer id) {
        auto it = tariffCache_.find(id);
        if (it != tariffCache_.end()) {
            return it->second;
        }
        
        // Загрузка из БД
        auto tariff = repository_->objects().getTariffById(id);
        if (tariff) {
            // Загрузка правил для тарифа
            auto rules = repository_->objects().getRulesForTariff(id);
            for (const auto& rule : rules) {
                tariff->addRule(rule);
            }
            
            tariffCache_[id] = tariff;
        }
        
        return tariff;
    }
    
    /**
     * Добавление правила к тарифу
     */
    OperationResult addTariffRule(
        Integer tariffId,
        std::shared_ptr<Rule> rule
    ) {
        try {
            db_->beginTransaction();
            
            auto tariff = getTariff(tariffId);
            if (!tariff) {
                throw DomainServiceException("Тариф не найден");
            }
            
            // Валидация правила
            if (!rule || !rule->getCondition() || !rule->getAction()) {
                throw DomainServiceException("Некорректное правило");
            }
            
            // Добавление в C++ модель
            tariff->addRule(rule);
            
            // Сохранение в БД через INS_DEC_F
            Integer ruleId = repository_->objects().addRuleToTariff(
                tariffId, rule
            );
            
            db_->commit();
            
            return OperationResult(true, "Правило добавлено", ruleId);
            
        } catch (const std::exception& e) {
            db_->rollback();
            return OperationResult(false,
                String("Ошибка добавления правила: ") + e.what());
        }
    }
    
    /**
     * Получение всех активных тарифов для услуги
     */
    std::vector<std::shared_ptr<Tariff>> getActiveTariffsForService(Integer serviceId) {
        return repository_->objects().getActiveTariffsForService(serviceId);
    }
    
    // ========================================================================
    // УПРАВЛЕНИЕ ЗАКАЗАМИ
    // ========================================================================
    
    /**
     * Создание заказа с валидацией
     */
    std::shared_ptr<Order> createOrder(
        Integer serviceId,
        Integer tariffId,
        const String& customerName,
        const String& note = ""
    ) {
        try {
            db_->beginTransaction();
            
            // Валидация
            if (customerName.empty()) {
                throw DomainServiceException("Имя заказчика обязательно");
            }
            
            // Проверка услуги и тарифа
            auto service = getService(serviceId);
            if (!service) {
                throw DomainServiceException("Услуга не найдена");
            }
            
            auto tariff = getTariff(tariffId);
            if (!tariff) {
                throw DomainServiceException("Тариф не найден");
            }
            
            if (!tariff->isActive()) {
                throw DomainServiceException("Тариф неактивен");
            }
            
            // Создание в БД
            String orderCode = generateOrderCode();
            Integer orderId = repository_->objects().createObject(
                serviceId, orderCode, "Заказ " + customerName, note
            );
            
            // Создание C++ объекта
            auto order = std::make_shared<Order>(orderId, orderCode);
            order->setStatus(OrderStatus::DRAFT);
            order->setCustomerName(customerName);
            
            // Кэширование
            orderCache_[orderId] = order;
            
            db_->commit();
            
            return order;
            
        } catch (const std::exception& e) {
            db_->rollback();
            throw DomainServiceException(
                String("Ошибка создания заказа: ") + e.what()
            );
        }
    }
    
    /**
     * Получение заказа по ID
     */
    std::shared_ptr<Order> getOrder(Integer id) {
        auto it = orderCache_.find(id);
        if (it != orderCache_.end()) {
            return it->second;
        }
        
        auto order = repository_->objects().getOrderById(id);
        if (order) {
            // Загрузка параметров
            auto params = repository_->objects().getOrderParameters(id);
            for (const auto& [paramId, value] : params) {
                order->addParameter(paramId, value);
            }
            
            orderCache_[id] = order;
        }
        
        return order;
    }
    
    /**
     * Установка параметра заказа с сохранением в БД
     */
    OperationResult setOrderParameter(
        Integer orderId,
        Integer parameterId,
        const ParameterValue& value
    ) {
        try {
            db_->beginTransaction();
            
            auto order = getOrder(orderId);
            if (!order) {
                throw DomainServiceException("Заказ не найден");
            }
            
            if (order->getStatus() == OrderStatus::COMPLETED ||
                order->getStatus() == OrderStatus::CANCELLED) {
                throw DomainServiceException(
                    "Нельзя изменять завершенный или отмененный заказ"
                );
            }
            
            // Установка в C++ модели
            order->addParameter(parameterId, value);
            
            // Сохранение в БД через UPDATE_VAL_ROLE
            repository_->objects().setParameterValue(orderId, parameterId, value);
            
            db_->commit();
            
            return OperationResult(true, "Параметр установлен");
            
        } catch (const std::exception& e) {
            db_->rollback();
            return OperationResult(false,
                String("Ошибка установки параметра: ") + e.what());
        }
    }
    
    /**
     * Расчет стоимости заказа по тарифу
     */
    Double calculateOrderCost(Integer orderId, Integer tariffId) {
        try {
            auto order = getOrder(orderId);
            if (!order) {
                throw DomainServiceException("Заказ не найден");
            }
            
            auto tariff = getTariff(tariffId);
            if (!tariff) {
                throw DomainServiceException("Тариф не найден");
            }
            
            // Валидация заказа через VALIDATE_ORDER
            bool isValid = repository_->calculations().validateOrder(orderId);
            if (!isValid) {
                throw DomainServiceException(
                    "Заказ не валиден: не заполнены обязательные параметры"
                );
            }
            
            // Расчет через CostCalculator и RuleEngine
            Double cost = costCalculator_->calculateCost(*order, *tariff);
            
            // Сохранение стоимости в заказ
            order->setCost(cost);
            
            return cost;
            
        } catch (const std::exception& e) {
            throw DomainServiceException(
                String("Ошибка расчета стоимости: ") + e.what()
            );
        }
    }
    
    /**
     * Подтверждение заказа (переход в статус CONFIRMED)
     */
    OperationResult confirmOrder(Integer orderId, Integer tariffId) {
        try {
            db_->beginTransaction();
            
            auto order = getOrder(orderId);
            if (!order) {
                throw DomainServiceException("Заказ не найден");
            }
            
            if (order->getStatus() != OrderStatus::DRAFT) {
                throw DomainServiceException("Можно подтвердить только черновик заказа");
            }
            
            // Расчет стоимости
            Double cost = calculateOrderCost(orderId, tariffId);
            
            // Обновление статуса
            order->setStatus(OrderStatus::CONFIRMED);
            order->setCost(cost);
            
            // Сохранение в БД
            repository_->objects().updateOrderStatus(orderId, OrderStatus::CONFIRMED);
            repository_->objects().updateOrderCost(orderId, cost);
            
            db_->commit();
            
            return OperationResult(true, 
                "Заказ подтвержден, стоимость: " + std::to_string(cost), 
                orderId);
            
        } catch (const std::exception& e) {
            db_->rollback();
            return OperationResult(false,
                String("Ошибка подтверждения заказа: ") + e.what());
        }
    }
    
    /**
     * Отмена заказа
     */
    OperationResult cancelOrder(Integer orderId, const String& reason) {
        try {
            db_->beginTransaction();
            
            auto order = getOrder(orderId);
            if (!order) {
                throw DomainServiceException("Заказ не найден");
            }
            
            if (order->getStatus() == OrderStatus::COMPLETED) {
                throw DomainServiceException("Нельзя отменить завершенный заказ");
            }
            
            order->setStatus(OrderStatus::CANCELLED);
            
            repository_->objects().updateOrderStatus(orderId, OrderStatus::CANCELLED);
            repository_->objects().addOrderNote(orderId, "Причина отмены: " + reason);
            
            db_->commit();
            
            return OperationResult(true, "Заказ отменен", orderId);
            
        } catch (const std::exception& e) {
            db_->rollback();
            return OperationResult(false,
                String("Ошибка отмены заказа: ") + e.what());
        }
    }
    
    // ========================================================================
    // ПОИСК ОПТИМАЛЬНЫХ ТАРИФОВ
    // ========================================================================
    
    /**
     * Поиск оптимального тарифа для заказа
     */
    struct TariffComparisonResult {
        std::shared_ptr<Tariff> tariff;
        Double cost;
        Double savings;  // Экономия относительно самого дорогого
    };
    
    std::vector<TariffComparisonResult> findOptimalTariffs(
        Integer orderId,
        Integer serviceId,
        int topN = 5
    ) {
        try {
            auto order = getOrder(orderId);
            if (!order) {
                throw DomainServiceException("Заказ не найден");
            }
            
            // Получение всех активных тарифов для услуги
            auto tariffs = getActiveTariffsForService(serviceId);
            
            if (tariffs.empty()) {
                throw DomainServiceException("Нет активных тарифов для услуги");
            }
            
            // Расчет стоимости по каждому тарифу
            std::vector<TariffComparisonResult> results;
            Double maxCost = 0.0;
            
            for (const auto& tariff : tariffs) {
                try {
                    Double cost = costCalculator_->calculateCost(*order, *tariff);
                    results.push_back({tariff, cost, 0.0});
                    
                    if (cost > maxCost) {
                        maxCost = cost;
                    }
                } catch (const std::exception&) {
                    // Пропускаем тарифы, для которых не удалось рассчитать стоимость
                    continue;
                }
            }
            
            if (results.empty()) {
                throw DomainServiceException(
                    "Не удалось рассчитать стоимость ни по одному тарифу"
                );
            }
            
            // Сортировка по возрастанию стоимости
            std::sort(results.begin(), results.end(),
                [](const auto& a, const auto& b) {
                    return a.cost < b.cost;
                });
            
            // Расчет экономии
            for (auto& result : results) {
                result.savings = maxCost - result.cost;
            }
            
            // Ограничение до topN
            if (results.size() > static_cast<size_t>(topN)) {
                results.resize(topN);
            }
            
            return results;
            
        } catch (const std::exception& e) {
            throw DomainServiceException(
                String("Ошибка поиска оптимальных тарифов: ") + e.what()
            );
        }
    }
    
    // ========================================================================
    // УТИЛИТЫ
    // ========================================================================
    
    /**
     * Очистка всех кэшей (полезно для тестов)
     */
    void clearCache() {
        classifierCache_.clear();
        serviceCache_.clear();
        tariffCache_.clear();
        orderCache_.clear();
    }
    
    /**
     * Получение статистики кэша
     */
    struct CacheStats {
        size_t classifiers;
        size_t services;
        size_t tariffs;
        size_t orders;
    };
    
    CacheStats getCacheStats() const {
        return {
            classifierCache_.size(),
            serviceCache_.size(),
            tariffCache_.size(),
            orderCache_.size()
        };
    }
    
    /**
     * Проверка подключения к БД
     */
    bool isDatabaseConnected() const {
        return db_ && db_->isConnected();
    }

private:
    /**
     * Генерация уникального кода заказа
     */
    String generateOrderCode() {
        static int counter = 1000;
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << "ORD-" << timestamp << "-" << (counter++);
        return ss.str();
    }
    
    // Зависимости
    std::shared_ptr<DatabaseManager> db_;
    std::unique_ptr<TariffSystemRepository> repository_;
    std::unique_ptr<RuleEngine> ruleEngine_;
    std::unique_ptr<CostCalculator> costCalculator_;
    std::unique_ptr<OptimalSearcher> optimalSearcher_;
    
    // Кэши для производительности
    std::map<Integer, std::shared_ptr<Classifier>> classifierCache_;
    std::map<Integer, std::shared_ptr<Service>> serviceCache_;
    std::map<Integer, std::shared_ptr<Tariff>> tariffCache_;
    std::map<Integer, std::shared_ptr<Order>> orderCache_;
};

} // namespace TariffSystem::Core
