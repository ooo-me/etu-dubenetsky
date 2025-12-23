#pragma once

#pragma once

#include "Models.h"

#include <db/DbApi.h>

#include <memory>
#include <vector>

namespace core
{

class TariffService
{
public:
    explicit TariffService(std::shared_ptr<db::DbApi> api);
    ~TariffService();

    // Инициализация схемы БД
    void InitializeDatabase();

    // ==================== Единицы измерения ====================
    std::vector<Unit> GetAllUnits();
    Unit CreateUnit(const Unit& unit);
    void UpdateUnit(const Unit& unit);
    void DeleteUnit(int id);

    // ==================== Перечисления ====================
    std::vector<Enumeration> GetAllEnumerations();
    Enumeration CreateEnumeration(const Enumeration& enumeration);
    void DeleteEnumeration(int id);

    std::vector<EnumValue> GetEnumValues(int enumId);
    EnumValue CreateEnumValue(const EnumValue& value);
    void DeleteEnumValue(int id);

    // ==================== Классы ====================
    std::vector<Class> GetAllClasses();
    Class CreateClass(const Class& cls);
    void UpdateClass(const Class& cls);
    void DeleteClass(int id);

    // ==================== Параметры ====================
    std::vector<Parameter> GetAllParameters();
    Parameter CreateParameter(const Parameter& param);
    void UpdateParameter(const Parameter& param);
    void DeleteParameter(int id);

    // ==================== Типы услуг ====================
    std::vector<ServiceType> GetAllServiceTypes();
    ServiceType GetServiceType(int id);
    ServiceType CreateServiceType(const ServiceType& serviceType);
    void UpdateServiceType(const ServiceType& serviceType);
    void DeleteServiceType(int id);

    void AddServiceTypeParameter(int serviceTypeId, const ServiceTypeParameter& param);
    void RemoveServiceTypeParameter(int serviceTypeId, int parameterId);

    // ==================== Исполнители ====================
    std::vector<Executor> GetAllExecutors();
    Executor CreateExecutor(const Executor& executor);
    void UpdateExecutor(const Executor& executor);
    void DeleteExecutor(int id);

    // ==================== Тарифы ====================
    std::vector<Tariff> GetAllTariffs();
    Tariff GetTariff(int id);
    Tariff CreateTariff(const Tariff& tariff);
    void UpdateTariff(const Tariff& tariff);
    void DeleteTariff(int id);

    TariffRate CreateTariffRate(int tariffId, const TariffRate& rate);
    void UpdateTariffRate(const TariffRate& rate);
    void DeleteTariffRate(int id);

    // ==================== Заказы ====================
    std::vector<Order> GetAllOrders();
    Order GetOrder(int id);
    Order CreateOrder(const Order& order);
    void UpdateOrder(const Order& order);
    void DeleteOrder(int id);

    void SetOrderParameter(int orderId, const OrderParameterValue& param);
    void RemoveOrderParameter(int orderId, int parameterId);

    // ==================== Коэффициенты ====================
    std::vector<Coefficient> GetAllCoefficients();
    Coefficient CreateCoefficient(const Coefficient& coeff);
    void UpdateCoefficient(const Coefficient& coeff);
    void DeleteCoefficient(int id);

    // ==================== Расчеты ====================
    double CalculateOrderCost(int orderId, std::optional<int> tariffId = std::nullopt);
    ValidationResult ValidateOrder(int orderId);
    std::vector<OptimalExecutor> FindOptimalExecutor(int serviceTypeId, const std::string& targetDate = "");
    std::vector<OptimalExecutor> FindOptimalTariff(int orderId);

private:
    std::shared_ptr<db::DbApi> api_;
};

} // namespace core

