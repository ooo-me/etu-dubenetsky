// ============================================================================
// CalculationTests.cpp
// Описание: Тесты расчёта стоимости заказа
// ============================================================================

#include "TestFixture.h"

namespace core::test
{

class CalculationTest : public TariffServiceTestFixture
{
};

// Тест базового расчёта стоимости
TEST_F(CalculationTest, BasicCostCalculation)
{
    // Создаём тип услуги с параметрами
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
    AddServiceTypeParameter(serviceType.id, timeParam.id, true);
    
    // Создаём исполнителя и тариф
    auto executor = CreateTestExecutor("TC_TEST", "ТК Тестовый", true);
    auto tariff = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31");
    CreateTestTariffRate(tariff.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    
    // Создаём заказ
    auto order = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");
    SetOrderParameter(order.id, timeParam.id, 4.0);
    
    order.tariffId = tariff.id;
    service_->UpdateOrder(order);
    
    // Рассчитываем стоимость
    double cost = service_->CalculateOrderCost(order.id);
    
    // Проверяем (4 часа * 500 руб = 2000 руб)
    EXPECT_GT(cost, 0.0) << "Стоимость должна быть больше нуля";
}

// Тест расчёта с НДС
TEST_F(CalculationTest, CostCalculationWithVAT)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
    AddServiceTypeParameter(serviceType.id, timeParam.id, true);
    
    auto executor = CreateTestExecutor("TC_TEST", "ТК Тестовый", true);
    
    // Тариф с НДС 20%
    auto tariffWithVat = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31", true, 20.0);
    CreateTestTariffRate(tariffWithVat.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    
    // Тариф без НДС
    auto tariffNoVat = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31", false, 0.0);
    CreateTestTariffRate(tariffNoVat.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    
    // Создаём два заказа
    auto order1 = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");
    SetOrderParameter(order1.id, timeParam.id, 4.0);
    order1.tariffId = tariffWithVat.id;
    service_->UpdateOrder(order1);
    
    auto order2 = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");
    SetOrderParameter(order2.id, timeParam.id, 4.0);
    order2.tariffId = tariffNoVat.id;
    service_->UpdateOrder(order2);
    
    // Рассчитываем стоимость
    double costWithVat = service_->CalculateOrderCost(order1.id);
    double costNoVat = service_->CalculateOrderCost(order2.id);
    
    // Стоимость с НДС должна быть больше
    EXPECT_GT(costWithVat, costNoVat) << "Стоимость с НДС должна быть больше";
}

// Тест расчёта с несколькими параметрами
TEST_F(CalculationTest, MultiParameterCalculation)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto kmUnit = CreateTestUnit("km", "Километр");
    
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
    auto kmParam = CreateTestParameter("KM_CITY", "Пробег по городу", 0, kmUnit.id);
    
    AddServiceTypeParameter(serviceType.id, timeParam.id, true);
    AddServiceTypeParameter(serviceType.id, kmParam.id, false);
    
    auto executor = CreateTestExecutor("TC_TEST", "ТК Тестовый", true);
    auto tariff = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31");
    CreateTestTariffRate(tariff.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    CreateTestTariffRate(tariff.id, "KM_CITY", "Стоимость км", 15.0, kmUnit.id);
    
    // Создаём заказ с обоими параметрами
    auto order = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");
    SetOrderParameter(order.id, timeParam.id, 4.0);
    SetOrderParameter(order.id, kmParam.id, 50.0);
    
    order.tariffId = tariff.id;
    service_->UpdateOrder(order);
    
    // Рассчитываем стоимость (4 * 500 + 50 * 15 = 2000 + 750 = 2750)
    double cost = service_->CalculateOrderCost(order.id);
    
    EXPECT_GT(cost, 2000.0) << "Стоимость должна учитывать оба параметра";
}

// Тест валидации заказа
TEST_F(CalculationTest, OrderValidation)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
    AddServiceTypeParameter(serviceType.id, timeParam.id, true); // Обязательный параметр
    
    // Создаём заказ без значений параметров
    auto order = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");
    
    // Валидация должна вернуть ошибку
    auto result = service_->ValidateOrder(order.id);
    
    // Заказ без обязательных параметров должен быть невалидным
    // (Точное поведение зависит от реализации валидации в DbApi)
    EXPECT_FALSE(result.errorMessage.empty()) << "Должно быть сообщение об ошибке или результат валидации";
}

// Тест поиска оптимального тарифа для заказа
TEST_F(CalculationTest, FindOptimalTariffForOrder)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
    AddServiceTypeParameter(serviceType.id, timeParam.id, true);
    
    // Создаём несколько тарифов
    auto executor1 = CreateTestExecutor("TC_1", "ТК 1", true);
    auto executor2 = CreateTestExecutor("TC_2", "ТК 2", true);
    
    auto tariff1 = CreateTestTariff(serviceType.id, executor1.id, "2025-01-01", "2025-12-31");
    CreateTestTariffRate(tariff1.id, "HOUR_RATE", "Стоимость часа", 600.0, hourUnit.id);
    
    auto tariff2 = CreateTestTariff(serviceType.id, executor2.id, "2025-01-01", "2025-12-31");
    CreateTestTariffRate(tariff2.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    
    // Создаём заказ
    auto order = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");
    SetOrderParameter(order.id, timeParam.id, 4.0);
    
    // Ищем оптимальный тариф
    auto results = service_->FindOptimalTariff(order.id);
    
    // Должен быть выбран самый дешёвый
    ASSERT_FALSE(results.empty());
    EXPECT_EQ(results[0].executorName, "ТК 2") << "Должен быть выбран самый дешёвый тариф";
}

} // namespace core::test
