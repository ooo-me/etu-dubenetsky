#include "TariffService.h"

#include "TariffService.h"

namespace core
{

TariffService::TariffService(std::shared_ptr<db::DbApi> api)
    : api_(api)
{
}

TariffService::~TariffService() = default;

void TariffService::InitializeDatabase()
{
    api_->InitializeSchema();
}

// ==================== Единицы измерения ====================

std::vector<Unit> TariffService::GetAllUnits()
{
    auto dbUnits = api_->GetAllUnits();
    std::vector<Unit> units;
    units.reserve(dbUnits.size());
    for (const auto& u : dbUnits)
    {
        Unit unit;
        unit.id = u.id;
        unit.code = u.code;
        unit.name = u.name;
        unit.note = u.note;
        units.push_back(unit);
    }
    return units;
}

Unit TariffService::CreateUnit(const Unit& unit)
{
    Unit result = unit;
    result.id = api_->CreateUnit(unit.code, unit.name, unit.note);
    return result;
}

void TariffService::UpdateUnit(const Unit& unit)
{
    api_->UpdateUnit(unit.id, unit.code, unit.name, unit.note);
}

void TariffService::DeleteUnit(int id)
{
    api_->DeleteUnit(id);
}

// ==================== Перечисления ====================

std::vector<Enumeration> TariffService::GetAllEnumerations()
{
    auto dbEnums = api_->GetAllEnums();
    std::vector<Enumeration> enums;
    enums.reserve(dbEnums.size());
    for (const auto& e : dbEnums)
    {
        Enumeration enumeration;
        enumeration.id = e.id;
        enumeration.code = e.code;
        enumeration.name = e.name;
        enumeration.note = e.note;
        enums.push_back(enumeration);
    }
    return enums;
}

Enumeration TariffService::CreateEnumeration(const Enumeration& enumeration)
{
    Enumeration result = enumeration;
    result.id = api_->CreateEnum(enumeration.code, enumeration.name, enumeration.note);
    return result;
}

void TariffService::DeleteEnumeration(int id)
{
    api_->DeleteEnum(id);
}

std::vector<EnumValue> TariffService::GetEnumValues(int enumId)
{
    auto dbValues = api_->GetEnumValues(enumId);
    std::vector<EnumValue> values;
    values.reserve(dbValues.size());
    for (const auto& v : dbValues)
    {
        EnumValue value;
        value.id = v.id;
        value.enumId = enumId;
        value.code = v.code;
        value.name = v.name;
        value.position = v.position;
        value.note = v.note;
        values.push_back(value);
    }
    return values;
}

EnumValue TariffService::CreateEnumValue(const EnumValue& value)
{
    EnumValue result = value;
    result.id = api_->CreateEnumValue(value.enumId, value.code, value.name, value.position, value.note);
    return result;
}

void TariffService::DeleteEnumValue(int id)
{
    api_->DeleteEnumValue(id);
}

// ==================== Классы ====================

std::vector<Class> TariffService::GetAllClasses()
{
    auto dbClasses = api_->GetAllClasses();
    std::vector<Class> classes;
    classes.reserve(dbClasses.size());
    for (const auto& c : dbClasses)
    {
        Class cls;
        cls.id = c.id;
        cls.code = c.code;
        cls.name = c.name;
        cls.parentId = c.parentId;
        cls.level = c.level;
        cls.note = c.note;
        classes.push_back(cls);
    }
    return classes;
}

Class TariffService::CreateClass(const Class& cls)
{
    Class result = cls;
    result.id = api_->CreateClass(cls.code, cls.name, cls.parentId, cls.note);
    return result;
}

void TariffService::UpdateClass(const Class& cls)
{
    api_->UpdateClass(cls.id, cls.code, cls.name, cls.note);
}

void TariffService::DeleteClass(int id)
{
    api_->DeleteClass(id);
}

// ==================== Параметры ====================

std::vector<Parameter> TariffService::GetAllParameters()
{
    auto dbParams = api_->GetAllParameters();
    std::vector<Parameter> params;
    params.reserve(dbParams.size());
    for (const auto& p : dbParams)
    {
        Parameter param;
        param.id = p.id;
        param.code = p.code;
        param.name = p.name;
        param.classId = p.classId;
        param.type = p.type;
        param.unitId = p.unitId;
        param.unitName = p.unitName;
        param.note = p.note;
        params.push_back(param);
    }
    return params;
}

Parameter TariffService::CreateParameter(const Parameter& param)
{
    Parameter result = param;
    result.id = api_->CreateParameter(param.code, param.name, param.classId, param.type, param.unitId, param.note);
    return result;
}

void TariffService::UpdateParameter(const Parameter& param)
{
    api_->UpdateParameter(param.id, param.code, param.name, param.type, param.unitId, param.note);
}

void TariffService::DeleteParameter(int id)
{
    api_->DeleteParameter(id);
}

// ==================== Типы услуг ====================

std::vector<ServiceType> TariffService::GetAllServiceTypes()
{
    auto dbTypes = api_->GetAllServiceTypes();
    std::vector<ServiceType> types;
    types.reserve(dbTypes.size());
    for (const auto& t : dbTypes)
    {
        ServiceType type;
        type.id = t.id;
        type.code = t.code;
        type.name = t.name;
        type.classId = t.classId;
        type.className = t.className;
        type.note = t.note;
        types.push_back(type);
    }
    return types;
}

ServiceType TariffService::GetServiceType(int id)
{
    auto types = GetAllServiceTypes();
    for (auto& t : types)
    {
        if (t.id == id)
        {
            auto params = api_->GetServiceTypeParams(id);
            for (const auto& p : params)
            {
                ServiceTypeParameter param;
                param.parameterId = p.parId;
                param.code = p.code;
                param.name = p.name;
                param.type = p.type;
                param.isRequired = p.isRequired;
                param.defaultValue = p.defaultValNum;
                param.defaultValueStr = p.defaultValStr;
                param.minValue = p.minVal;
                param.maxValue = p.maxVal;
                param.unitName = p.unitName;
                t.parameters.push_back(param);
            }
            return t;
        }
    }
    throw std::runtime_error("Тип услуги не найден");
}

ServiceType TariffService::CreateServiceType(const ServiceType& serviceType)
{
    ServiceType result = serviceType;
    result.id = api_->CreateServiceType(serviceType.code, serviceType.name, serviceType.classId, serviceType.note);
    
    for (const auto& p : serviceType.parameters)
    {
        api_->AddServiceTypeParam(result.id, p.parameterId, p.isRequired, 
                                   p.defaultValue, p.defaultValueStr, p.minValue, p.maxValue);
    }
    
    return result;
}

void TariffService::UpdateServiceType(const ServiceType& serviceType)
{
    api_->UpdateServiceType(serviceType.id, serviceType.code, serviceType.name, serviceType.note);
}

void TariffService::DeleteServiceType(int id)
{
    api_->DeleteServiceType(id);
}

void TariffService::AddServiceTypeParameter(int serviceTypeId, const ServiceTypeParameter& param)
{
    api_->AddServiceTypeParam(serviceTypeId, param.parameterId, param.isRequired,
                               param.defaultValue, param.defaultValueStr, param.minValue, param.maxValue);
}

void TariffService::RemoveServiceTypeParameter(int serviceTypeId, int parameterId)
{
    api_->RemoveServiceTypeParam(serviceTypeId, parameterId);
}

// ==================== Исполнители ====================

std::vector<Executor> TariffService::GetAllExecutors()
{
    auto dbExecutors = api_->GetAllExecutors();
    std::vector<Executor> executors;
    executors.reserve(dbExecutors.size());
    for (const auto& e : dbExecutors)
    {
        Executor executor;
        executor.id = e.id;
        executor.code = e.code;
        executor.name = e.name;
        executor.address = e.address;
        executor.phone = e.phone;
        executor.email = e.email;
        executor.isActive = e.isActive;
        executor.note = e.note;
        executors.push_back(executor);
    }
    return executors;
}

Executor TariffService::CreateExecutor(const Executor& executor)
{
    Executor result = executor;
    result.id = api_->CreateExecutor(executor.code, executor.name, executor.address,
                                      executor.phone, executor.email, executor.isActive, executor.note);
    return result;
}

void TariffService::UpdateExecutor(const Executor& executor)
{
    api_->UpdateExecutor(executor.id, executor.code, executor.name, executor.address,
                          executor.phone, executor.email, executor.isActive, executor.note);
}

void TariffService::DeleteExecutor(int id)
{
    api_->DeleteExecutor(id);
}

// ==================== Тарифы ====================

std::vector<Tariff> TariffService::GetAllTariffs()
{
    auto dbTariffs = api_->GetAllTariffs();
    std::vector<Tariff> tariffs;
    tariffs.reserve(dbTariffs.size());
    for (const auto& t : dbTariffs)
    {
        Tariff tariff;
        tariff.id = t.id;
        tariff.code = t.code;
        tariff.name = t.name;
        tariff.serviceTypeId = t.serviceTypeId;
        tariff.serviceName = t.serviceName;
        tariff.executorId = t.executorId;
        tariff.executorName = t.executorName;
        tariff.dateBegin = t.dateBegin;
        tariff.dateEnd = t.dateEnd;
        tariff.isWithVat = t.isWithVat;
        tariff.vatRate = t.vatRate;
        tariff.isActive = t.isActive;
        tariff.note = t.note;
        tariffs.push_back(tariff);
    }
    return tariffs;
}

Tariff TariffService::GetTariff(int id)
{
    auto tariffs = GetAllTariffs();
    for (auto& t : tariffs)
    {
        if (t.id == id)
        {
            auto rates = api_->GetTariffRates(id);
            for (const auto& r : rates)
            {
                TariffRate rate;
                rate.id = r.id;
                rate.tariffId = id;
                rate.code = r.code;
                rate.name = r.name;
                rate.value = r.value;
                rate.unitId = r.unitId;
                rate.unitName = r.unitName;
                rate.note = r.note;
                t.rates.push_back(rate);
            }
            return t;
        }
    }
    throw std::runtime_error("Тариф не найден");
}

Tariff TariffService::CreateTariff(const Tariff& tariff)
{
    Tariff result = tariff;
    result.id = api_->CreateTariff(tariff.serviceTypeId, tariff.code, tariff.name,
                                    tariff.executorId, tariff.dateBegin, tariff.dateEnd,
                                    tariff.isWithVat, tariff.vatRate, tariff.isActive, tariff.note);
    
    for (auto& rate : result.rates)
    {
        rate.tariffId = result.id;
        rate.id = api_->CreateTariffRate(result.id, rate.code, rate.name, rate.value, rate.unitId, rate.note);
    }
    
    return result;
}

void TariffService::UpdateTariff(const Tariff& tariff)
{
    api_->UpdateTariff(tariff.id, tariff.code, tariff.name, tariff.executorId,
                        tariff.dateBegin, tariff.dateEnd, tariff.isWithVat, tariff.vatRate,
                        tariff.isActive, tariff.note);
}

void TariffService::DeleteTariff(int id)
{
    api_->DeleteTariff(id);
}

TariffRate TariffService::CreateTariffRate(int tariffId, const TariffRate& rate)
{
    TariffRate result = rate;
    result.tariffId = tariffId;
    result.id = api_->CreateTariffRate(tariffId, rate.code, rate.name, rate.value, rate.unitId, rate.note);
    return result;
}

void TariffService::UpdateTariffRate(const TariffRate& rate)
{
    api_->UpdateTariffRate(rate.id, rate.code, rate.name, rate.value, rate.unitId, rate.note);
}

void TariffService::DeleteTariffRate(int id)
{
    api_->DeleteTariffRate(id);
}

// ==================== Заказы ====================

std::vector<Order> TariffService::GetAllOrders()
{
    auto dbOrders = api_->GetAllOrders();
    std::vector<Order> orders;
    orders.reserve(dbOrders.size());
    for (const auto& o : dbOrders)
    {
        Order order;
        order.id = o.id;
        order.code = o.code;
        order.serviceTypeId = o.serviceTypeId;
        order.serviceName = o.serviceName;
        order.orderDate = o.orderDate;
        order.executionDate = o.executionDate;
        order.status = static_cast<OrderStatus>(o.status);
        order.executorId = o.executorId;
        order.executorName = o.executorName;
        order.tariffId = o.tariffId;
        order.tariffName = o.tariffName;
        order.totalCost = o.totalCost;
        order.note = o.note;
        orders.push_back(order);
    }
    return orders;
}

Order TariffService::GetOrder(int id)
{
    auto orders = GetAllOrders();
    for (auto& o : orders)
    {
        if (o.id == id)
        {
            auto params = api_->GetOrderParams(id);
            for (const auto& p : params)
            {
                OrderParameterValue param;
                param.parameterId = p.parId;
                param.code = p.code;
                param.name = p.name;
                param.type = p.type;
                param.numValue = p.valNum;
                param.strValue = p.valStr;
                param.dateValue = p.valDate;
                param.enumId = p.enumId;
                param.enumName = p.enumName;
                param.unitName = p.unitName;
                o.parameters.push_back(param);
            }
            return o;
        }
    }
    throw std::runtime_error("Заказ не найден");
}

Order TariffService::CreateOrder(const Order& order)
{
    Order result = order;
    result.id = api_->CreateOrder(order.code, order.serviceTypeId, order.orderDate,
                                   order.executionDate, static_cast<int>(order.status),
                                   order.executorId, order.tariffId, order.note);
    
    for (const auto& p : order.parameters)
    {
        api_->SetOrderParam(result.id, p.parameterId, p.numValue, p.strValue, p.dateValue, p.enumId);
    }
    
    return result;
}

void TariffService::UpdateOrder(const Order& order)
{
    api_->UpdateOrder(order.id, order.code, order.executionDate, static_cast<int>(order.status),
                       order.executorId, order.tariffId, order.totalCost, order.note);
}

void TariffService::DeleteOrder(int id)
{
    api_->DeleteOrder(id);
}

void TariffService::SetOrderParameter(int orderId, const OrderParameterValue& param)
{
    api_->SetOrderParam(orderId, param.parameterId, param.numValue, param.strValue, param.dateValue, param.enumId);
}

void TariffService::RemoveOrderParameter(int orderId, int parameterId)
{
    api_->RemoveOrderParam(orderId, parameterId);
}

// ==================== Коэффициенты ====================

std::vector<Coefficient> TariffService::GetAllCoefficients()
{
    auto dbCoeffs = api_->GetAllCoefficients();
    std::vector<Coefficient> coeffs;
    coeffs.reserve(dbCoeffs.size());
    for (const auto& c : dbCoeffs)
    {
        Coefficient coeff;
        coeff.id = c.id;
        coeff.code = c.code;
        coeff.name = c.name;
        coeff.valueMin = c.valueMin;
        coeff.valueMax = c.valueMax;
        coeff.valueDefault = c.valueDefault;
        coeff.note = c.note;
        coeffs.push_back(coeff);
    }
    return coeffs;
}

Coefficient TariffService::CreateCoefficient(const Coefficient& coeff)
{
    Coefficient result = coeff;
    result.id = api_->CreateCoefficient(coeff.code, coeff.name, coeff.valueMin, 
                                         coeff.valueMax, coeff.valueDefault, coeff.note);
    return result;
}

void TariffService::UpdateCoefficient(const Coefficient& coeff)
{
    api_->UpdateCoefficient(coeff.id, coeff.code, coeff.name, coeff.valueMin,
                             coeff.valueMax, coeff.valueDefault, coeff.note);
}

void TariffService::DeleteCoefficient(int id)
{
    api_->DeleteCoefficient(id);
}

// ==================== Расчеты ====================

double TariffService::CalculateOrderCost(int orderId, std::optional<int> tariffId)
{
    return api_->CalculateOrderCost(orderId, tariffId);
}

ValidationResult TariffService::ValidateOrder(int orderId)
{
    auto result = api_->ValidateOrder(orderId);
    return {result.isValid, result.errorMessage};
}

std::vector<OptimalExecutor> TariffService::FindOptimalExecutor(int serviceTypeId, const std::string& targetDate)
{
    auto dbResults = api_->FindOptimalExecutor(serviceTypeId, targetDate);
    std::vector<OptimalExecutor> results;
    results.reserve(dbResults.size());
    for (const auto& r : dbResults)
    {
        OptimalExecutor executor;
        executor.executorId = r.executorId;
        executor.executorName = r.executorName;
        executor.tariffId = r.tariffId;
        executor.tariffName = r.tariffName;
        executor.estimatedCost = r.estimatedCost;
        results.push_back(executor);
    }
    return results;
}

std::vector<OptimalExecutor> TariffService::FindOptimalTariff(int orderId)
{
    auto dbResults = api_->FindOptimalTariff(orderId);
    std::vector<OptimalExecutor> results;
    results.reserve(dbResults.size());
    for (const auto& r : dbResults)
    {
        OptimalExecutor executor;
        executor.executorId = 0; // Not available in this query
        executor.executorName = r.executorName;
        executor.tariffId = r.tariffId;
        executor.tariffName = r.tariffName;
        executor.estimatedCost = r.estimatedCost;
        results.push_back(executor);
    }
    return results;
}

} // #include "TariffService.h"

namespace core

