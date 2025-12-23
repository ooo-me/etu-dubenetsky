#include "DbApi.h"

#include "DbApi.h"

#include <fstream>
#include <sstream>

namespace db
{

DbApi::DbApi(std::shared_ptr<DatabaseManager> db)
    : db_(db)
{
}

DbApi::~DbApi() = default;

void DbApi::InitializeSchema()
{
    // Execute table creation script
    ExecuteSchemaFile("database/schema/01_tables.sql");
    // Execute index creation script
    ExecuteSchemaFile("database/schema/02_indexes.sql");
    // Execute procedure scripts
    ExecuteSchemaFile("database/procedures/constructor/constructor.sql");
    ExecuteSchemaFile("database/procedures/calculator/calculator.sql");
    ExecuteSchemaFile("database/procedures/utils/utils.sql");
}

void DbApi::ExecuteSchemaFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw Exception("Cannot open schema file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sql = buffer.str();

    db_->Execute(sql);
}

// ==================== Units of Measure ====================

int DbApi::CreateUnit(const std::string& code, const std::string& name, const std::string& note)
{
    std::string query = "SELECT INS_EI($1, $2, $3)";
    std::vector<std::string> params = {code, name, note.empty() ? "NULL" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать единицу измерения");
}

void DbApi::UpdateUnit(int id, const std::string& code, const std::string& name, const std::string& note)
{
    std::string query = "SELECT UPD_EI($1, $2, $3, $4)";
    std::vector<std::string> params = {std::to_string(id), code, name, note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteUnit(int id)
{
    std::string query = "SELECT DEL_EI($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<UnitOfMeasure> DbApi::GetAllUnits()
{
    std::string query = "SELECT * FROM GET_ALL_EI()";
    auto result = db_->ExecuteQuery(query);
    std::vector<UnitOfMeasure> units;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        UnitOfMeasure unit;
        unit.id = result->GetInt(i, 0).value_or(0);
        unit.code = result->GetValue(i, 1).value_or("");
        unit.name = result->GetValue(i, 2).value_or("");
        unit.note = result->GetValue(i, 3).value_or("");
        units.push_back(unit);
    }
    return units;
}

// ==================== Enumerations ====================

int DbApi::CreateEnum(const std::string& code, const std::string& name, const std::string& note)
{
    std::string query = "SELECT INS_ENUM($1, $2, $3)";
    std::vector<std::string> params = {code, name, note.empty() ? "NULL" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать перечисление");
}

void DbApi::DeleteEnum(int id)
{
    std::string query = "DELETE FROM ENUM_VAL_R WHERE ID_ENUM = $1";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<EnumInfo> DbApi::GetAllEnums()
{
    std::string query = "SELECT * FROM GET_ALL_ENUMS()";
    auto result = db_->ExecuteQuery(query);
    std::vector<EnumInfo> enums;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        EnumInfo info;
        info.id = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.name = result->GetValue(i, 2).value_or("");
        info.note = result->GetValue(i, 3).value_or("");
        enums.push_back(info);
    }
    return enums;
}

int DbApi::CreateEnumValue(int enumId, const std::string& code, const std::string& name, int position,
                           const std::string& note)
{
    std::string query = "SELECT INS_VAL_ENUM($1, $2, $3, $4, $5)";
    std::vector<std::string> params = {std::to_string(enumId), code, name, std::to_string(position),
                                       note.empty() ? "NULL" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать значение перечисления");
}

void DbApi::DeleteEnumValue(int id)
{
    std::string query = "DELETE FROM POS_ENUM WHERE ID_POS_ENUM = $1";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<EnumValue> DbApi::GetEnumValues(int enumId)
{
    std::string query = "SELECT * FROM GET_ENUM_VALUES($1)";
    auto result = db_->executeQuery(query, {std::to_string(enumId)});
    std::vector<EnumValue> values;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        EnumValue val;
        val.id = result->GetInt(i, 0).value_or(0);
        val.code = result->GetValue(i, 1).value_or("");
        val.name = result->GetValue(i, 2).value_or("");
        val.position = result->GetInt(i, 3).value_or(0);
        val.note = result->GetValue(i, 4).value_or("");
        values.push_back(val);
    }
    return values;
}

// ==================== Classes ====================

int DbApi::CreateClass(const std::string& code, const std::string& name, std::optional<int> parentId,
                       const std::string& note)
{
    std::string query = "SELECT INS_CLASS($1, $2, $3, $4)";
    std::vector<std::string> params = {code, name, parentId ? std::to_string(*parentId) : "NULL", note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать класс");
}

void DbApi::UpdateClass(int id, const std::string& code, const std::string& name, const std::string& note)
{
    std::string query = "SELECT UPD_CLASS($1, $2, $3, $4)";
    std::vector<std::string> params = {std::to_string(id), code, name, note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteClass(int id)
{
    std::string query = "SELECT DEL_CLASS($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<ClassInfo> DbApi::GetAllClasses()
{
    std::string query = "SELECT * FROM GET_ALL_CLASSES()";
    auto result = db_->ExecuteQuery(query);
    std::vector<ClassInfo> classes;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        ClassInfo info;
        info.id = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.name = result->GetValue(i, 2).value_or("");
        info.parentId = result->GetInt(i, 3);
        info.level = result->GetInt(i, 4).value_or(0);
        info.note = result->GetValue(i, 5).value_or("");
        classes.push_back(info);
    }
    return classes;
}

// ==================== Parameters ====================

int DbApi::CreateParameter(const std::string& code, const std::string& name, std::optional<int> classId, int type,
                           std::optional<int> unitId, const std::string& note)
{
    std::string query = "SELECT INS_PARAMETR($1, $2, $3, $4, $5, $6)";
    std::vector<std::string> params = {code,
                                       name,
                                       classId ? std::to_string(*classId) : "NULL",
                                       std::to_string(type),
                                       unitId ? std::to_string(*unitId) : "NULL",
                                       note.empty() ? "NULL" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать параметр");
}

void DbApi::UpdateParameter(int id, const std::string& code, const std::string& name, int type,
                            std::optional<int> unitId, const std::string& note)
{
    std::string query = "SELECT UPD_PARAMETR($1, $2, $3, $4, $5, $6)";
    std::vector<std::string> params = {std::to_string(id),
                                       code,
                                       name,
                                       std::to_string(type),
                                       unitId ? std::to_string(*unitId) : "NULL",
                                       note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteParameter(int id)
{
    std::string query = "SELECT DEL_PARAMETR($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<ParameterInfo> DbApi::GetAllParameters()
{
    std::string query = "SELECT * FROM GET_ALL_PARAMETERS()";
    auto result = db_->ExecuteQuery(query);
    std::vector<ParameterInfo> params;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        ParameterInfo info;
        info.id = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.name = result->GetValue(i, 2).value_or("");
        info.classId = result->GetInt(i, 3);
        info.type = result->GetInt(i, 4).value_or(0);
        info.typeName = result->GetValue(i, 5).value_or("");
        info.unitId = result->GetInt(i, 6);
        info.unitName = result->GetValue(i, 7).value_or("");
        info.note = result->GetValue(i, 8).value_or("");
        params.push_back(info);
    }
    return params;
}

// ==================== Service Types ====================

int DbApi::CreateServiceType(const std::string& code, const std::string& name, int classId, const std::string& note)
{
    std::string query = "SELECT INS_SERVICE_TYPE($1, $2, $3, $4)";
    std::vector<std::string> params = {code, name, std::to_string(classId), note.empty() ? "NULL" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать тип услуги");
}

void DbApi::UpdateServiceType(int id, const std::string& code, const std::string& name, const std::string& note)
{
    std::string query = "SELECT UPD_SERVICE_TYPE($1, $2, $3, $4)";
    std::vector<std::string> params = {std::to_string(id), code, name, note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteServiceType(int id)
{
    std::string query = "SELECT DEL_SERVICE_TYPE($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<ServiceTypeInfo> DbApi::GetAllServiceTypes()
{
    std::string query = "SELECT * FROM GET_ALL_SERVICE_TYPES()";
    auto result = db_->ExecuteQuery(query);
    std::vector<ServiceTypeInfo> types;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        ServiceTypeInfo info;
        info.id = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.name = result->GetValue(i, 2).value_or("");
        info.classId = result->GetInt(i, 3).value_or(0);
        info.className = result->GetValue(i, 4).value_or("");
        info.note = result->GetValue(i, 5).value_or("");
        types.push_back(info);
    }
    return types;
}

void DbApi::AddServiceTypeParam(int serviceTypeId, int parId, bool isRequired, std::optional<double> defaultNum,
                                const std::string& defaultStr, std::optional<double> minVal,
                                std::optional<double> maxVal)
{
    std::string query = "SELECT INS_SERVICE_TYPE_PARAM($1, $2, $3, $4, $5, $6, $7)";
    std::vector<std::string> params = {std::to_string(serviceTypeId),
                                       std::to_string(parId),
                                       std::to_string(isRequired ? 1 : 0),
                                       defaultNum ? std::to_string(*defaultNum) : "NULL",
                                       defaultStr.empty() ? "NULL" : defaultStr,
                                       minVal ? std::to_string(*minVal) : "NULL",
                                       maxVal ? std::to_string(*maxVal) : "NULL"};
    db_->executeQuery(query, params);
}

void DbApi::RemoveServiceTypeParam(int serviceTypeId, int parId)
{
    std::string query = "DELETE FROM SERVICE_TYPE_PARAM WHERE ID_SERVICE_TYPE = $1 AND ID_PAR = $2";
    db_->executeQuery(query, {std::to_string(serviceTypeId), std::to_string(parId)});
}

std::vector<ServiceTypeParamInfo> DbApi::GetServiceTypeParams(int serviceTypeId)
{
    std::string query = "SELECT * FROM GET_SERVICE_TYPE_PARAMS($1)";
    auto result = db_->executeQuery(query, {std::to_string(serviceTypeId)});
    std::vector<ServiceTypeParamInfo> params;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        ServiceTypeParamInfo info;
        info.parId = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.name = result->GetValue(i, 2).value_or("");
        info.type = result->GetInt(i, 3).value_or(0);
        info.isRequired = result->GetInt(i, 4).value_or(0) == 1;
        info.defaultValNum = result->GetDouble(i, 5);
        info.defaultValStr = result->GetValue(i, 6).value_or("");
        info.minVal = result->GetDouble(i, 7);
        info.maxVal = result->GetDouble(i, 8);
        info.unitName = result->GetValue(i, 9).value_or("");
        params.push_back(info);
    }
    return params;
}

// ==================== Executors ====================

int DbApi::CreateExecutor(const std::string& code, const std::string& name, const std::string& address,
                          const std::string& phone, const std::string& email, bool isActive, const std::string& note)
{
    std::string query = "SELECT INS_EXECUTOR($1, $2, $3, $4, $5, $6, $7)";
    std::vector<std::string> params = {code,
                                       name,
                                       address.empty() ? "NULL" : address,
                                       phone.empty() ? "NULL" : phone,
                                       email.empty() ? "NULL" : email,
                                       std::to_string(isActive ? 1 : 0),
                                       note.empty() ? "NULL" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать исполнителя");
}

void DbApi::UpdateExecutor(int id, const std::string& code, const std::string& name, const std::string& address,
                           const std::string& phone, const std::string& email, bool isActive, const std::string& note)
{
    std::string query = "SELECT UPD_EXECUTOR($1, $2, $3, $4, $5, $6, $7, $8)";
    std::vector<std::string> params = {std::to_string(id),
                                       code,
                                       name,
                                       address.empty() ? "NULL" : address,
                                       phone.empty() ? "NULL" : phone,
                                       email.empty() ? "NULL" : email,
                                       std::to_string(isActive ? 1 : 0),
                                       note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteExecutor(int id)
{
    std::string query = "SELECT DEL_EXECUTOR($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<ExecutorInfo> DbApi::GetAllExecutors()
{
    std::string query = "SELECT * FROM GET_ALL_EXECUTORS()";
    auto result = db_->ExecuteQuery(query);
    std::vector<ExecutorInfo> executors;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        ExecutorInfo info;
        info.id = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.name = result->GetValue(i, 2).value_or("");
        info.address = result->GetValue(i, 3).value_or("");
        info.phone = result->GetValue(i, 4).value_or("");
        info.email = result->GetValue(i, 5).value_or("");
        info.isActive = result->GetInt(i, 6).value_or(0) == 1;
        info.note = result->GetValue(i, 7).value_or("");
        executors.push_back(info);
    }
    return executors;
}

// ==================== Tariffs ====================

int DbApi::CreateTariff(int serviceTypeId, const std::string& code, const std::string& name,
                        std::optional<int> executorId, const std::string& dateBegin, const std::string& dateEnd,
                        bool isWithVat, double vatRate, bool isActive, const std::string& note)
{
    std::string query = "SELECT INS_TARIFF($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)";
    std::vector<std::string> params = {std::to_string(serviceTypeId),
                                       code,
                                       name,
                                       executorId ? std::to_string(*executorId) : "NULL",
                                       dateBegin.empty() ? "NULL" : dateBegin,
                                       dateEnd.empty() ? "NULL" : dateEnd,
                                       std::to_string(isWithVat ? 1 : 0),
                                       std::to_string(vatRate),
                                       std::to_string(isActive ? 1 : 0),
                                       note.empty() ? "NULL" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать тариф");
}

void DbApi::UpdateTariff(int id, const std::string& code, const std::string& name, std::optional<int> executorId,
                         const std::string& dateBegin, const std::string& dateEnd, bool isWithVat, double vatRate,
                         bool isActive, const std::string& note)
{
    std::string query = "SELECT UPD_TARIFF($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)";
    std::vector<std::string> params = {std::to_string(id),
                                       code,
                                       name,
                                       executorId ? std::to_string(*executorId) : "NULL",
                                       dateBegin.empty() ? "NULL" : dateBegin,
                                       dateEnd.empty() ? "NULL" : dateEnd,
                                       std::to_string(isWithVat ? 1 : 0),
                                       std::to_string(vatRate),
                                       std::to_string(isActive ? 1 : 0),
                                       note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteTariff(int id)
{
    std::string query = "SELECT DEL_TARIFF($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<TariffInfo> DbApi::GetAllTariffs()
{
    std::string query = "SELECT * FROM GET_ALL_TARIFFS()";
    auto result = db_->ExecuteQuery(query);
    std::vector<TariffInfo> tariffs;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        TariffInfo info;
        info.id = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.name = result->GetValue(i, 2).value_or("");
        info.serviceTypeId = result->GetInt(i, 3).value_or(0);
        info.serviceName = result->GetValue(i, 4).value_or("");
        info.executorId = result->GetInt(i, 5);
        info.executorName = result->GetValue(i, 6).value_or("");
        info.dateBegin = result->GetValue(i, 7).value_or("");
        info.dateEnd = result->GetValue(i, 8).value_or("");
        info.isWithVat = result->GetInt(i, 9).value_or(0) == 1;
        info.vatRate = result->GetDouble(i, 10).value_or(0.0);
        info.isActive = result->GetInt(i, 11).value_or(0) == 1;
        info.note = result->GetValue(i, 12).value_or("");
        tariffs.push_back(info);
    }
    return tariffs;
}

int DbApi::CreateTariffRate(int tariffId, const std::string& code, const std::string& name, double value,
                            std::optional<int> unitId, const std::string& note)
{
    std::string query = "SELECT INS_TARIFF_RATE($1, $2, $3, $4, $5, NULL, $6)";
    std::vector<std::string> params = {std::to_string(tariffId),
                                       code,
                                       name,
                                       std::to_string(value),
                                       unitId ? std::to_string(*unitId) : "NULL",
                                       note.empty() ? "NULL" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать ставку тарифа");
}

void DbApi::UpdateTariffRate(int id, const std::string& code, const std::string& name, double value,
                             std::optional<int> unitId, const std::string& note)
{
    std::string query = "SELECT UPD_TARIFF_RATE($1, $2, $3, $4, $5, $6)";
    std::vector<std::string> params = {std::to_string(id),
                                       code,
                                       name,
                                       std::to_string(value),
                                       unitId ? std::to_string(*unitId) : "NULL",
                                       note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteTariffRate(int id)
{
    std::string query = "SELECT DEL_TARIFF_RATE($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<TariffRateInfo> DbApi::GetTariffRates(int tariffId)
{
    std::string query = "SELECT * FROM GET_TARIFF_RATES($1)";
    auto result = db_->executeQuery(query, {std::to_string(tariffId)});
    std::vector<TariffRateInfo> rates;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        TariffRateInfo info;
        info.id = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.name = result->GetValue(i, 2).value_or("");
        info.value = result->GetDouble(i, 3).value_or(0.0);
        info.unitId = result->GetInt(i, 4);
        info.unitName = result->GetValue(i, 5).value_or("");
        info.note = result->GetValue(i, 6).value_or("");
        rates.push_back(info);
    }
    return rates;
}

void DbApi::AddTariffCoefficient(int tariffId, int coeffId, double value)
{
    std::string query = "SELECT INS_TARIFF_COEFFICIENT($1, $2, $3)";
    db_->executeQuery(query, {std::to_string(tariffId), std::to_string(coeffId), std::to_string(value)});
}

void DbApi::RemoveTariffCoefficient(int tariffId, int coeffId)
{
    std::string query = "DELETE FROM TARIFF_COEFFICIENT WHERE ID_TARIFF = $1 AND ID_COEFFICIENT = $2";
    db_->executeQuery(query, {std::to_string(tariffId), std::to_string(coeffId)});
}

// ==================== Orders ====================

int DbApi::CreateOrder(const std::string& code, int serviceTypeId, const std::string& orderDate,
                       const std::string& executionDate, int status, std::optional<int> executorId,
                       std::optional<int> tariffId, const std::string& note)
{
    std::string query = "SELECT INS_ORDER($1, $2, $3, $4, $5, $6, $7, $8)";
    std::vector<std::string> params = {code,
                                       std::to_string(serviceTypeId),
                                       orderDate.empty() ? "NULL" : orderDate,
                                       executionDate.empty() ? "NULL" : executionDate,
                                       std::to_string(status),
                                       executorId ? std::to_string(*executorId) : "NULL",
                                       tariffId ? std::to_string(*tariffId) : "NULL",
                                       note.empty() ? "NULL" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать заказ");
}

void DbApi::UpdateOrder(int id, const std::string& code, const std::string& executionDate, int status,
                        std::optional<int> executorId, std::optional<int> tariffId, std::optional<double> totalCost,
                        const std::string& note)
{
    std::string query = "SELECT UPD_ORDER($1, $2, $3, $4, $5, $6, $7, $8)";
    std::vector<std::string> params = {std::to_string(id),
                                       code,
                                       executionDate.empty() ? "NULL" : executionDate,
                                       std::to_string(status),
                                       executorId ? std::to_string(*executorId) : "NULL",
                                       tariffId ? std::to_string(*tariffId) : "NULL",
                                       totalCost ? std::to_string(*totalCost) : "NULL",
                                       note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteOrder(int id)
{
    std::string query = "SELECT DEL_ORDER($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<OrderInfo> DbApi::GetAllOrders()
{
    std::string query = "SELECT * FROM GET_ALL_ORDERS()";
    auto result = db_->ExecuteQuery(query);
    std::vector<OrderInfo> orders;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        OrderInfo info;
        info.id = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.serviceTypeId = result->GetInt(i, 2).value_or(0);
        info.serviceName = result->GetValue(i, 3).value_or("");
        info.orderDate = result->GetValue(i, 4).value_or("");
        info.executionDate = result->GetValue(i, 5).value_or("");
        info.status = result->GetInt(i, 6).value_or(0);
        info.statusName = result->GetValue(i, 7).value_or("");
        info.executorId = result->GetInt(i, 8);
        info.executorName = result->GetValue(i, 9).value_or("");
        info.tariffId = result->GetInt(i, 10);
        info.tariffName = result->GetValue(i, 11).value_or("");
        info.totalCost = result->GetDouble(i, 12);
        info.note = result->GetValue(i, 13).value_or("");
        orders.push_back(info);
    }
    return orders;
}

void DbApi::SetOrderParam(int orderId, int parId, std::optional<double> valNum, const std::string& valStr,
                          const std::string& valDate, std::optional<int> enumId)
{
    std::string query = "SELECT INS_ORDER_PARAM($1, $2, $3, $4, $5, $6)";
    std::vector<std::string> params = {std::to_string(orderId),
                                       std::to_string(parId),
                                       valNum ? std::to_string(*valNum) : "NULL",
                                       valStr.empty() ? "NULL" : valStr,
                                       valDate.empty() ? "NULL" : valDate,
                                       enumId ? std::to_string(*enumId) : "NULL"};
    db_->executeQuery(query, params);
}

void DbApi::RemoveOrderParam(int orderId, int parId)
{
    std::string query = "DELETE FROM ORDER_PARAM WHERE ID_ORDER = $1 AND ID_PAR = $2";
    db_->executeQuery(query, {std::to_string(orderId), std::to_string(parId)});
}

std::vector<OrderParamInfo> DbApi::GetOrderParams(int orderId)
{
    std::string query = "SELECT * FROM GET_ORDER_PARAMS($1)";
    auto result = db_->executeQuery(query, {std::to_string(orderId)});
    std::vector<OrderParamInfo> params;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        OrderParamInfo info;
        info.parId = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.name = result->GetValue(i, 2).value_or("");
        info.type = result->GetInt(i, 3).value_or(0);
        info.valNum = result->GetDouble(i, 4);
        info.valStr = result->GetValue(i, 5).value_or("");
        info.valDate = result->GetValue(i, 6).value_or("");
        info.enumId = result->GetInt(i, 7);
        info.enumName = result->GetValue(i, 8).value_or("");
        info.unitName = result->GetValue(i, 9).value_or("");
        params.push_back(info);
    }
    return params;
}

// ==================== Coefficients ====================

int DbApi::CreateCoefficient(const std::string& code, const std::string& name, double valueMin, double valueMax,
                             double valueDefault, const std::string& note)
{
    std::string query = "SELECT INS_COEFFICIENT($1, $2, $3, $4, $5, $6)";
    std::vector<std::string> params = {code,
                                       name,
                                       std::to_string(valueMin),
                                       std::to_string(valueMax),
                                       std::to_string(valueDefault),
                                       note.empty() ? "NULL" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать коэффициент");
}

void DbApi::UpdateCoefficient(int id, const std::string& code, const std::string& name, double valueMin,
                              double valueMax, double valueDefault, const std::string& note)
{
    std::string query = "SELECT UPD_COEFFICIENT($1, $2, $3, $4, $5, $6, $7)";
    std::vector<std::string> params = {std::to_string(id),
                                       code,
                                       name,
                                       std::to_string(valueMin),
                                       std::to_string(valueMax),
                                       std::to_string(valueDefault),
                                       note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteCoefficient(int id)
{
    std::string query = "SELECT DEL_COEFFICIENT($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

std::vector<CoefficientInfo> DbApi::GetAllCoefficients()
{
    std::string query = "SELECT * FROM GET_ALL_COEFFICIENTS()";
    auto result = db_->ExecuteQuery(query);
    std::vector<CoefficientInfo> coeffs;

    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        CoefficientInfo info;
        info.id = result->GetInt(i, 0).value_or(0);
        info.code = result->GetValue(i, 1).value_or("");
        info.name = result->GetValue(i, 2).value_or("");
        info.valueMin = result->GetDouble(i, 3).value_or(0.0);
        info.valueMax = result->GetDouble(i, 4).value_or(0.0);
        info.valueDefault = result->GetDouble(i, 5).value_or(0.0);
        info.note = result->GetValue(i, 6).value_or("");
        coeffs.push_back(info);
    }
    return coeffs;
}

// ==================== Functions ====================

int DbApi::CreateFunction(const std::string& code, const std::string& name, int type, const std::string& operation,
                          const std::string& note)
{
    std::string query = "SELECT INS_FUNCT($1, $2, $3, $4, $5)";
    std::vector<std::string> params = {code, name, std::to_string(type), operation.empty() ? "NULL" : operation, note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать функцию");
}

void DbApi::UpdateFunction(int id, const std::string& code, const std::string& name, int type,
                           const std::string& operation, const std::string& note)
{
    std::string query = "SELECT UPD_FUNCT($1, $2, $3, $4, $5, $6)";
    std::vector<std::string> params = {std::to_string(id),
                                       code,
                                       name,
                                       std::to_string(type),
                                       operation.empty() ? "NULL" : operation,
                                       note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteFunction(int id)
{
    std::string query = "SELECT DEL_FUNCT($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

int DbApi::AddArgument(int functionId, int argNumber, std::optional<int> classArg, const std::string& name,
                       const std::string& note)
{
    std::string query = "SELECT INS_ARG_FUN($1, $2, $3, $4, $5)";
    std::vector<std::string> params = {std::to_string(functionId),
                                       std::to_string(argNumber),
                                       classArg ? std::to_string(*classArg) : "NULL",
                                       name.empty() ? "NULL" : name,
                                       note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось добавить аргумент");
}

int DbApi::CreateObject(int classId, const std::string& code, const std::string& name, const std::string& note)
{
    std::string query = "SELECT INS_OB($1, $2, $3, $4, $5)";
    std::vector<std::string> params = {std::to_string(classId), code, name, "NULL", note.empty() ? "" : note};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() > 0)
    {
        auto id = result->GetInt(0, 0);
        if (id)
            return *id;
    }
    throw Exception("Не удалось создать объект");
}

void DbApi::UpdateObject(int id, const std::string& code, const std::string& name, const std::string& note)
{
    std::string query = "SELECT UPD_OB($1, $2, $3, $4)";
    std::vector<std::string> params = {std::to_string(id), code, name, note.empty() ? "NULL" : note};
    db_->executeQuery(query, params);
}

void DbApi::DeleteObject(int id)
{
    std::string query = "SELECT DEL_OB($1)";
    db_->executeQuery(query, {std::to_string(id)});
}

void DbApi::UpdateRoleValue(int functionId, int objectId, std::optional<double> numValue)
{
    std::string query = "SELECT UPDATE_VAL_ROLE($1, $2, NULL, NULL, $3, NULL, NULL, NULL, NULL)";
    std::vector<std::string> params = {std::to_string(functionId),
                                       std::to_string(objectId),
                                       numValue ? std::to_string(*numValue) : "NULL"};
    db_->executeQuery(query, params);
}

// ==================== Calculations ====================

double DbApi::CalculateValue(int functionId, int objectId, std::optional<int> tariffId)
{
    std::string query = "SELECT CALC_VAL_F($1, $2, $3)";
    std::vector<std::string> params = {std::to_string(functionId),
                                       std::to_string(objectId),
                                       tariffId ? std::to_string(*tariffId) : "NULL"};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() == 0)
        throw Exception("Не удалось вычислить значение");
    auto val = result->GetDouble(0, 0);
    if (!val)
        throw Exception("Некорректный результат вычисления");
    return *val;
}

double DbApi::CalculateOrderCost(int orderId, std::optional<int> tariffId)
{
    std::string query = "SELECT CALC_ORDER_COST($1, $2)";
    std::vector<std::string> params = {std::to_string(orderId), tariffId ? std::to_string(*tariffId) : "NULL"};
    auto result = db_->executeQuery(query, params);
    if (result->GetRowCount() == 0)
        throw Exception("Не удалось рассчитать стоимость заказа");
    auto val = result->GetDouble(0, 0);
    if (!val)
        throw Exception("Некорректный результат расчета");
    return *val;
}

double DbApi::CalculateOrderItemCost(int orderItemId, int tariffId)
{
    std::string query = "SELECT CALC_ORDER_ITEM_COST($1, $2)";
    auto result = db_->executeQuery(query, {std::to_string(orderItemId), std::to_string(tariffId)});
    if (result->GetRowCount() == 0)
        throw Exception("Не удалось рассчитать стоимость позиции");
    auto val = result->GetDouble(0, 0);
    if (!val)
        throw Exception("Некорректный результат расчета");
    return *val;
}

ValidationResult DbApi::ValidateOrder(int orderId)
{
    std::string query = "SELECT * FROM VALIDATE_ORDER($1)";
    auto result = db_->executeQuery(query, {std::to_string(orderId)});
    if (result->GetRowCount() == 0)
        return {false, "Заказ не найден"};

    ValidationResult res;
    auto isValid = result->GetValue(0, 0);
    res.isValid = isValid && (*isValid == "t" || *isValid == "true" || *isValid == "1");
    res.errorMessage = result->GetValue(0, 1).value_or("");
    return res;
}

std::vector<OptimalExecutorInfo> DbApi::FindOptimalExecutor(int serviceTypeId, const std::string& targetDate)
{
    std::string query = "SELECT * FROM FIND_OPTIMAL_EXECUTOR($1, $2)";
    std::vector<std::string> params = {std::to_string(serviceTypeId), targetDate.empty() ? "NULL" : targetDate};
    auto result = db_->executeQuery(query, params);

    std::vector<OptimalExecutorInfo> executors;
    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        OptimalExecutorInfo info;
        info.executorId = result->GetInt(i, 0).value_or(0);
        info.executorName = result->GetValue(i, 1).value_or("");
        info.tariffId = result->GetInt(i, 2).value_or(0);
        info.tariffName = result->GetValue(i, 3).value_or("");
        info.estimatedCost = result->GetDouble(i, 4).value_or(0.0);
        executors.push_back(info);
    }
    return executors;
}

std::vector<OptimalExecutorInfo> DbApi::FindOptimalTariff(int orderId)
{
    std::string query = "SELECT * FROM FIND_OPTIMAL_TARIFF($1)";
    auto result = db_->executeQuery(query, {std::to_string(orderId)});

    std::vector<OptimalExecutorInfo> tariffs;
    for (int i = 0; i < result->GetRowCount(); ++i)
    {
        OptimalExecutorInfo info;
        info.tariffId = result->GetInt(i, 0).value_or(0);
        info.tariffName = result->GetValue(i, 1).value_or("");
        info.executorName = result->GetValue(i, 2).value_or("");
        info.estimatedCost = result->GetDouble(i, 3).value_or(0.0);
        tariffs.push_back(info);
    }
    return tariffs;
}

} // namespace db

