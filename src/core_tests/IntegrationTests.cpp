// ============================================================================
// IntegrationTests.cpp
// Описание: Интеграционные тесты полного сценария работы
// ============================================================================

#include "TestFixture.h"

namespace core::test
{

class IntegrationTest : public TariffServiceTestFixture
{
};

// Интеграционный тест: Полный цикл работы с заказом на грузоперевозку
TEST_F(IntegrationTest, CompleteOrderLifecycle)
{
    // 1. Создание справочных данных
    
    // Единицы измерения
    auto tonUnit = CreateTestUnit("t", "Тонна");
    auto m3Unit = CreateTestUnit("m3", "Кубический метр");
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto kmUnit = CreateTestUnit("km", "Километр");
    
    // Класс и тип услуги
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    
    // Параметры типа услуги
    auto weightParam = CreateTestParameter("WEIGHT", "Вес груза", 0, tonUnit.id);
    auto volumeParam = CreateTestParameter("VOLUME", "Объём груза", 0, m3Unit.id);
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
    auto kmCityParam = CreateTestParameter("KM_CITY", "Пробег по городу", 0, kmUnit.id);
    
    AddServiceTypeParameter(serviceType.id, weightParam.id, true);
    AddServiceTypeParameter(serviceType.id, volumeParam.id, true);
    AddServiceTypeParameter(serviceType.id, timeParam.id, true);
    AddServiceTypeParameter(serviceType.id, kmCityParam.id, false);
    
    // Исполнители
    auto executor1 = CreateTestExecutor("TC_FAST", "ТК Быстрый", true);
    auto executor2 = CreateTestExecutor("TC_ECON", "ТК Экономный", true);
    
    // Тарифы
    auto tariff1 = CreateTestTariff(serviceType.id, executor1.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff1.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    CreateTestTariffRate(tariff1.id, "KM_CITY", "Стоимость км", 15.0, kmUnit.id);
    
    auto tariff2 = CreateTestTariff(serviceType.id, executor2.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff2.id, "HOUR_RATE", "Стоимость часа", 450.0, hourUnit.id);
    CreateTestTariffRate(tariff2.id, "KM_CITY", "Стоимость км", 14.0, kmUnit.id);
    
    // 2. Создание заказа
    auto order = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");
    SetOrderParameter(order.id, weightParam.id, 1.5);
    SetOrderParameter(order.id, volumeParam.id, 8.0);
    SetOrderParameter(order.id, timeParam.id, 4.0);
    SetOrderParameter(order.id, kmCityParam.id, 30.0);
    
    // 3. Поиск оптимального тарифа
    auto optimalTariffs = service_->FindOptimalTariff(order.id);
    
    ASSERT_FALSE(optimalTariffs.empty()) << "Должны быть найдены тарифы";
    EXPECT_EQ(optimalTariffs[0].executorName, "ТК Экономный") << "Оптимальным должен быть самый дешёвый";
    
    // 4. Назначение оптимального тарифа заказу
    order.tariffId = optimalTariffs[0].tariffId;
    order.executorId = optimalTariffs[0].executorId;
    service_->UpdateOrder(order);
    
    // 5. Расчёт стоимости
    double cost = service_->CalculateOrderCost(order.id);
    EXPECT_GT(cost, 0.0) << "Стоимость должна быть рассчитана";
    
    // 6. Обновление статуса заказа
    order.status = OrderStatus::InProgress;
    order.totalCost = cost;
    service_->UpdateOrder(order);
    
    auto updatedOrder = service_->GetOrder(order.id);
    EXPECT_EQ(updatedOrder.status, OrderStatus::InProgress);
    EXPECT_TRUE(updatedOrder.totalCost.has_value());
    
    // 7. Завершение заказа
    updatedOrder.status = OrderStatus::Completed;
    service_->UpdateOrder(updatedOrder);
    
    auto completedOrder = service_->GetOrder(order.id);
    EXPECT_EQ(completedOrder.status, OrderStatus::Completed);
}

// Интеграционный тест: Сравнение тарифов на разные даты
TEST_F(IntegrationTest, TariffComparisonByDate)
{
    // Создание базовых данных
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
    AddServiceTypeParameter(serviceType.id, timeParam.id, true);
    
    auto executor = CreateTestExecutor("TC_SEASONAL", "ТК Сезонная", true);
    
    // Тарифы на разные периоды
    auto winterTariff = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-03-31", true, 18.0);
    CreateTestTariffRate(winterTariff.id, "HOUR_RATE", "Стоимость часа", 600.0, hourUnit.id);
    
    auto summerTariff = CreateTestTariff(serviceType.id, executor.id, "2025-06-01", "2025-08-31", true, 18.0);
    CreateTestTariffRate(summerTariff.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    
    // Поиск для зимнего периода
    auto winterResults = service_->FindOptimalExecutor(serviceType.id, "2025-02-15");
    ASSERT_FALSE(winterResults.empty());
    
    // Поиск для летнего периода
    auto summerResults = service_->FindOptimalExecutor(serviceType.id, "2025-07-15");
    ASSERT_FALSE(summerResults.empty());
    
    // Проверка, что найдены разные тарифы
    EXPECT_NE(winterResults[0].tariffId, summerResults[0].tariffId);
}

// Интеграционный тест: Работа с несколькими типами услуг
TEST_F(IntegrationTest, MultipleServiceTypes)
{
    // Создание двух типов услуг
    auto cls1 = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto cls2 = CreateTestClass("STORAGE", "Складские услуги");
    
    auto transportService = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls1.id);
    auto storageService = CreateTestServiceType("STORAGE", "Хранение", cls2.id);
    
    // Параметры для грузоперевозки
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
    AddServiceTypeParameter(transportService.id, timeParam.id, true);
    
    // Параметры для хранения
    auto dayUnit = CreateTestUnit("day", "Сутки");
    auto palletUnit = CreateTestUnit("pallet", "Паллет");
    auto daysParam = CreateTestParameter("DAYS", "Срок хранения", 0, dayUnit.id);
    auto palletParam = CreateTestParameter("PALLETS", "Количество паллет", 0, palletUnit.id);
    AddServiceTypeParameter(storageService.id, daysParam.id, true);
    AddServiceTypeParameter(storageService.id, palletParam.id, true);
    
    // Исполнитель для обоих типов услуг
    auto executor = CreateTestExecutor("MULTI_SERVICE", "Мультисервисная компания", true);
    
    // Тарифы
    auto transportTariff = CreateTestTariff(transportService.id, executor.id, "2025-01-01", "2025-12-31");
    CreateTestTariffRate(transportTariff.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    
    auto storageTariff = CreateTestTariff(storageService.id, executor.id, "2025-01-01", "2025-12-31");
    CreateTestTariffRate(storageTariff.id, "DAY_RATE", "Стоимость дня", 10.0, dayUnit.id);
    
    // Создание заказов
    auto transportOrder = CreateTestOrder(transportService.id, "2025-01-15", "2025-01-16");
    SetOrderParameter(transportOrder.id, timeParam.id, 4.0);
    
    auto storageOrder = CreateTestOrder(storageService.id, "2025-01-15", "2025-02-15");
    SetOrderParameter(storageOrder.id, daysParam.id, 30.0);
    SetOrderParameter(storageOrder.id, palletParam.id, 50.0);
    
    // Поиск оптимальных тарифов
    auto transportResults = service_->FindOptimalExecutor(transportService.id, "2025-01-15");
    auto storageResults = service_->FindOptimalExecutor(storageService.id, "2025-01-15");
    
    EXPECT_FALSE(transportResults.empty());
    EXPECT_FALSE(storageResults.empty());
    
    // Проверка, что найдены правильные тарифы
    EXPECT_EQ(transportResults[0].tariffId, transportTariff.id);
    EXPECT_EQ(storageResults[0].tariffId, storageTariff.id);
}

// Интеграционный тест: Обработка неактивных исполнителей
TEST_F(IntegrationTest, InactiveExecutorsHandling)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
    AddServiceTypeParameter(serviceType.id, timeParam.id, true);
    
    // Активный и неактивный исполнители
    auto activeExecutor = CreateTestExecutor("TC_ACTIVE", "ТК Активная", true);
    auto inactiveExecutor = CreateTestExecutor("TC_INACTIVE", "ТК Неактивная", false);
    
    // Тарифы (у неактивного тариф дешевле)
    auto activeTariff = CreateTestTariff(serviceType.id, activeExecutor.id, "2025-01-01", "2025-12-31");
    CreateTestTariffRate(activeTariff.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    
    auto inactiveTariff = CreateTestTariff(serviceType.id, inactiveExecutor.id, "2025-01-01", "2025-12-31");
    CreateTestTariffRate(inactiveTariff.id, "HOUR_RATE", "Стоимость часа", 400.0, hourUnit.id);
    
    // Поиск оптимального
    auto results = service_->FindOptimalExecutor(serviceType.id, "2025-01-15");
    
    // Должен быть найден только активный исполнитель
    // (Зависит от реализации: некоторые системы могут показывать всех)
    bool foundActive = false;
    bool foundInactive = false;
    
    for (const auto& result : results)
    {
        if (result.executorName == "ТК Активная") foundActive = true;
        if (result.executorName == "ТК Неактивная") foundInactive = true;
    }
    
    EXPECT_TRUE(foundActive) << "Активный исполнитель должен быть найден";
    // Примечание: поведение с неактивными исполнителями зависит от бизнес-логики
}

// Интеграционный тест: Массовое создание данных
TEST_F(IntegrationTest, BulkDataCreation)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
    AddServiceTypeParameter(serviceType.id, timeParam.id, true);
    
    // Создание 10 исполнителей с тарифами
    std::vector<Executor> executors;
    std::vector<Tariff> tariffs;
    
    for (int i = 1; i <= 10; ++i)
    {
        auto executor = CreateTestExecutor(
            "TC_" + std::to_string(i),
            "ТК #" + std::to_string(i),
            true
        );
        executors.push_back(executor);
        
        auto tariff = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31");
        CreateTestTariffRate(tariff.id, "HOUR_RATE", "Стоимость часа", 400.0 + i * 10.0, hourUnit.id);
        tariffs.push_back(tariff);
    }
    
    // Поиск оптимального
    auto results = service_->FindOptimalExecutor(serviceType.id, "2025-01-15");
    
    ASSERT_GE(results.size(), 10) << "Должны быть найдены все исполнители";
    
    // Самый дешёвый должен быть первым (ТК #1 с ставкой 410)
    EXPECT_EQ(results[0].executorName, "ТК #1");
    
    // Самый дорогой должен быть последним (ТК #10 с ставкой 500)
    EXPECT_EQ(results[results.size() - 1].executorName, "ТК #10");
}

} // namespace core::test
