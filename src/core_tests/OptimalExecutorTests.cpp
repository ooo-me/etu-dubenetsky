// ============================================================================
// OptimalExecutorTests.cpp
// Описание: Тесты поиска оптимального исполнителя (КЛЮЧЕВОЙ ФАЙЛ)
// ============================================================================

#include "TestFixture.h"

namespace core::test
{

class OptimalExecutorTest : public TariffServiceTestFixture
{
protected:
    // Создание типа услуги "Грузоперевозка" с параметрами
    ServiceType CreateTransportServiceType()
    {
        auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
        auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);

        // Единицы измерения
        auto tonUnit = CreateTestUnit("t", "Тонна");
        auto m3Unit = CreateTestUnit("m3", "Кубический метр");
        auto hourUnit = CreateTestUnit("hour", "Час");
        auto kmUnit = CreateTestUnit("km", "Километр");

        // Параметры для грузоперевозки
        auto weightParam = CreateTestParameter("WEIGHT", "Вес груза", 0, tonUnit.id);
        auto volumeParam = CreateTestParameter("VOLUME", "Объём груза", 0, m3Unit.id);
        auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);
        auto kmCityParam = CreateTestParameter("KM_CITY", "Пробег по городу", 0, kmUnit.id);
        auto kmRegionParam = CreateTestParameter("KM_REGION", "Пробег по области", 0, kmUnit.id);

        AddServiceTypeParameter(serviceType.id, weightParam.id, true);
        AddServiceTypeParameter(serviceType.id, volumeParam.id, true);
        AddServiceTypeParameter(serviceType.id, timeParam.id, true);
        AddServiceTypeParameter(serviceType.id, kmCityParam.id, false);
        AddServiceTypeParameter(serviceType.id, kmRegionParam.id, false);

        return serviceType;
    }

    // Создание типа услуги "Ответственное хранение"
    ServiceType CreateStorageServiceType()
    {
        auto cls = CreateTestClass("STORAGE", "Складские услуги");
        auto serviceType = CreateTestServiceType("RESP_STORAGE", "Ответственное хранение", cls.id);

        // Единицы измерения
        auto palletUnit = CreateTestUnit("pallet", "Паллет");
        auto dayUnit = CreateTestUnit("day", "Сутки");

        // Параметры для хранения
        auto palletParam = CreateTestParameter("PALLET_COUNT", "Количество паллет", 0, palletUnit.id);
        auto daysParam = CreateTestParameter("STORAGE_DAYS", "Срок хранения", 0, dayUnit.id);

        AddServiceTypeParameter(serviceType.id, palletParam.id, true);
        AddServiceTypeParameter(serviceType.id, daysParam.id, true);

        return serviceType;
    }
};

// ============================================================================
// Сценарий 2.1: Выбор перевозчика для лёгкого груза
// ============================================================================
TEST_F(OptimalExecutorTest, FindCheapestTransportForLightCargo)
{
    // 1. Подготовка данных
    auto serviceType = CreateTransportServiceType();
    
    // Создаём единицу измерения явно для этого теста
    auto hourUnit = CreateTestUnit("hour", "Час");
    
    auto executor1 = CreateTestExecutor("TC_FAST", "ТК Быстрый", true);
    auto executor2 = CreateTestExecutor("TC_ECON", "ТК Экономный", true);
    auto executor3 = CreateTestExecutor("TC_RELIABLE", "ТК Надёжный", true);
    
    // Создаём тарифы с разными ставками (тариф на 4 часа)
    // Данные из файла: 0.5т - Тариф 4ч = 1900 руб, Стоимость часа = 420 руб
    auto tariff1 = CreateTestTariff(serviceType.id, executor1.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff1.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    
    auto tariff2 = CreateTestTariff(serviceType.id, executor2.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff2.id, "HOUR_RATE", "Стоимость часа", 420.0, hourUnit.id); // Минимальная ставка
    
    auto tariff3 = CreateTestTariff(serviceType.id, executor3.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff3.id, "HOUR_RATE", "Стоимость часа", 550.0, hourUnit.id);
    
    // 2. Поиск оптимального исполнителя
    auto results = service_->FindOptimalExecutor(serviceType.id, "2025-01-15");
    
    // 3. Проверки
    ASSERT_FALSE(results.empty()) << "Не найдено ни одного исполнителя";
    EXPECT_EQ(results[0].executorName, "ТК Экономный") << "Выбран не самый дешёвый исполнитель";
    
    // Проверяем, что результаты отсортированы по стоимости
    if (results.size() >= 2)
    {
        EXPECT_LT(results[0].estimatedCost, results[1].estimatedCost) 
            << "Результаты не отсортированы по возрастанию стоимости";
    }
}

// ============================================================================
// Сценарий 2.2: Выбор перевозчика для тяжёлого груза
// ============================================================================
TEST_F(OptimalExecutorTest, FindTransportForHeavyCargo)
{
    // 1. Подготовка данных
    auto serviceType = CreateTransportServiceType();
    
    // Создаём единицы измерения явно
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto kmUnit = CreateTestUnit("km", "Километр");
    
    auto executor1 = CreateTestExecutor("TC_HEAVY1", "ТК Тяжеловоз 1", true);
    auto executor2 = CreateTestExecutor("TC_HEAVY2", "ТК Тяжеловоз 2", true);
    
    // Тарифы для тяжёлого груза (5т, 20-30м³)
    // Данные из файла: 5т - Тариф 8ч = 6280 руб, Стоимость часа = 785 руб, 
    // руб/км город = 23, руб/км область = 25
    auto tariff1 = CreateTestTariff(serviceType.id, executor1.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff1.id, "HOUR_RATE", "Стоимость часа", 785.0, hourUnit.id);
    CreateTestTariffRate(tariff1.id, "KM_CITY", "Стоимость км по городу", 23.0, kmUnit.id);
    CreateTestTariffRate(tariff1.id, "KM_REGION", "Стоимость км по области", 25.0, kmUnit.id);
    
    auto tariff2 = CreateTestTariff(serviceType.id, executor2.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff2.id, "HOUR_RATE", "Стоимость часа", 800.0, hourUnit.id);
    CreateTestTariffRate(tariff2.id, "KM_CITY", "Стоимость км по городу", 22.0, kmUnit.id);
    CreateTestTariffRate(tariff2.id, "KM_REGION", "Стоимость км по области", 24.0, kmUnit.id);
    
    // 2. Поиск оптимального исполнителя
    auto results = service_->FindOptimalExecutor(serviceType.id, "2025-01-15");
    
    // 3. Проверки
    ASSERT_FALSE(results.empty()) << "Не найдено ни одного исполнителя для тяжёлого груза";
    EXPECT_GE(results.size(), 2) << "Должно быть как минимум 2 исполнителя";
    
    // Проверяем, что все исполнители имеют расчётную стоимость
    for (const auto& result : results)
    {
        EXPECT_GT(result.estimatedCost, 0.0) << "Стоимость должна быть больше нуля";
    }
}

// ============================================================================
// Сценарий 2.3: Выбор склада для ответственного хранения
// ============================================================================
TEST_F(OptimalExecutorTest, FindOptimalStorageForPallets)
{
    // 1. Подготовка данных
    auto serviceType = CreateStorageServiceType();
    
    // Создаём единицы измерения явно
    auto dayUnit = CreateTestUnit("day", "Сутки");
    
    auto storage1 = CreateTestExecutor("WAREHOUSE_1", "Склад №1", true);
    auto storage2 = CreateTestExecutor("WAREHOUSE_2", "Склад №2", true);
    auto storage3 = CreateTestExecutor("WAREHOUSE_3", "Склад №3", true);
    
    // Тарифы на хранение
    // Данные из файла: Хранение паллет = 8-16 руб/сутки
    auto tariff1 = CreateTestTariff(serviceType.id, storage1.id, "2025-01-01", "2025-12-31", true, 20.0);
    CreateTestTariffRate(tariff1.id, "STORAGE_RATE", "Хранение паллет", 16.0, dayUnit.id);
    
    auto tariff2 = CreateTestTariff(serviceType.id, storage2.id, "2025-01-01", "2025-12-31", true, 20.0);
    CreateTestTariffRate(tariff2.id, "STORAGE_RATE", "Хранение паллет", 10.0, dayUnit.id); // Средний
    
    auto tariff3 = CreateTestTariff(serviceType.id, storage3.id, "2025-01-01", "2025-12-31", true, 20.0);
    CreateTestTariffRate(tariff3.id, "STORAGE_RATE", "Хранение паллет", 8.0, dayUnit.id); // Минимальный
    
    // 2. Поиск оптимального склада для хранения 100 паллет на 30 дней
    auto results = service_->FindOptimalExecutor(serviceType.id, "2025-01-15");
    
    // 3. Проверки
    ASSERT_FALSE(results.empty()) << "Не найдено ни одного склада";
    EXPECT_EQ(results[0].executorName, "Склад №3") << "Выбран не самый дешёвый склад";
    
    // Проверяем сортировку по стоимости
    if (results.size() >= 2)
    {
        EXPECT_LT(results[0].estimatedCost, results[1].estimatedCost);
    }
    if (results.size() >= 3)
    {
        EXPECT_LT(results[1].estimatedCost, results[2].estimatedCost);
    }
}

// ============================================================================
// Сценарий 2.4: Фильтрация по дате действия тарифа
// ============================================================================
TEST_F(OptimalExecutorTest, FilterByTariffValidityDate)
{
    // 1. Подготовка данных
    auto serviceType = CreateTransportServiceType();
    
    // Создаём единицу измерения явно
    auto hourUnit = CreateTestUnit("hour", "Час");
    
    auto executor1 = CreateTestExecutor("TC_2024", "ТК Старый тариф", true);
    auto executor2 = CreateTestExecutor("TC_2025", "ТК Новый тариф", true);
    auto executor3 = CreateTestExecutor("TC_FUTURE", "ТК Будущий тариф", true);
    
    // Тариф, который уже истёк
    auto tariff1 = CreateTestTariff(serviceType.id, executor1.id, "2024-01-01", "2024-12-31", true, 18.0);
    CreateTestTariffRate(tariff1.id, "HOUR_RATE", "Стоимость часа", 400.0, hourUnit.id);
    
    // Действующий тариф
    auto tariff2 = CreateTestTariff(serviceType.id, executor2.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff2.id, "HOUR_RATE", "Стоимость часа", 450.0, hourUnit.id);
    
    // Тариф, который ещё не вступил в силу
    auto tariff3 = CreateTestTariff(serviceType.id, executor3.id, "2026-01-01", "2026-12-31", true, 18.0);
    CreateTestTariffRate(tariff3.id, "HOUR_RATE", "Стоимость часа", 500.0, hourUnit.id);
    
    // 2. Поиск оптимального исполнителя на дату 15 января 2025
    auto results = service_->FindOptimalExecutor(serviceType.id, "2025-01-15");
    
    // 3. Проверки
    ASSERT_FALSE(results.empty()) << "Не найдено ни одного действующего тарифа";
    
    // Должен быть выбран только действующий на эту дату тариф
    bool foundExpired = false;
    bool foundFuture = false;
    bool foundCurrent = false;
    
    for (const auto& result : results)
    {
        if (result.executorName == "ТК Старый тариф") foundExpired = true;
        if (result.executorName == "ТК Новый тариф") foundCurrent = true;
        if (result.executorName == "ТК Будущий тариф") foundFuture = true;
    }
    
    EXPECT_FALSE(foundExpired) << "Найден истёкший тариф";
    EXPECT_TRUE(foundCurrent) << "Не найден действующий тариф";
    EXPECT_FALSE(foundFuture) << "Найден тариф, который ещё не вступил в силу";
}

// ============================================================================
// Сценарий 2.5: Учёт НДС
// ============================================================================
TEST_F(OptimalExecutorTest, VatCalculation)
{
    // 1. Подготовка данных
    auto serviceType = CreateTransportServiceType();
    
    // Создаём единицу измерения явно
    auto hourUnit = CreateTestUnit("hour", "Час");
    
    auto executor1 = CreateTestExecutor("TC_WITH_VAT_18", "ТК с НДС 18%", true);
    auto executor2 = CreateTestExecutor("TC_WITH_VAT_20", "ТК с НДС 20%", true);
    auto executor3 = CreateTestExecutor("TC_NO_VAT", "ТК без НДС", true);
    
    // Тариф с НДС 18% (старая ставка)
    auto tariff1 = CreateTestTariff(serviceType.id, executor1.id, "2024-01-01", "2024-12-31", true, 18.0);
    CreateTestTariffRate(tariff1.id, "HOUR_RATE", "Стоимость часа", 420.0, hourUnit.id);
    
    // Тариф с НДС 20% (текущая ставка)
    auto tariff2 = CreateTestTariff(serviceType.id, executor2.id, "2025-01-01", "2025-12-31", true, 20.0);
    CreateTestTariffRate(tariff2.id, "HOUR_RATE", "Стоимость часа", 420.0, hourUnit.id);
    
    // Тариф без НДС
    auto tariff3 = CreateTestTariff(serviceType.id, executor3.id, "2025-01-01", "2025-12-31", false, 0.0);
    CreateTestTariffRate(tariff3.id, "HOUR_RATE", "Стоимость часа", 420.0, hourUnit.id);
    
    // 2. Проверяем настройки НДС
    auto retrieved1 = service_->GetTariff(tariff1.id);
    auto retrieved2 = service_->GetTariff(tariff2.id);
    auto retrieved3 = service_->GetTariff(tariff3.id);
    
    EXPECT_TRUE(retrieved1.isWithVat);
    EXPECT_DOUBLE_EQ(retrieved1.vatRate, 18.0);
    
    EXPECT_TRUE(retrieved2.isWithVat);
    EXPECT_DOUBLE_EQ(retrieved2.vatRate, 20.0);
    
    EXPECT_FALSE(retrieved3.isWithVat);
    
    // 3. Поиск оптимального (на 2025 год)
    auto results = service_->FindOptimalExecutor(serviceType.id, "2025-01-15");
    
    // 4. Проверки
    ASSERT_FALSE(results.empty());
    
    // Тариф без НДС должен быть дешевле при равных базовых ставках
    bool foundWithVat = false;
    bool foundNoVat = false;
    double costWithVat = 0.0;
    double costNoVat = 0.0;
    
    for (const auto& result : results)
    {
        if (result.executorName == "ТК с НДС 20%")
        {
            foundWithVat = true;
            costWithVat = result.estimatedCost;
        }
        if (result.executorName == "ТК без НДС")
        {
            foundNoVat = true;
            costNoVat = result.estimatedCost;
        }
    }
    
    if (foundWithVat && foundNoVat)
    {
        EXPECT_LT(costNoVat, costWithVat) << "Тариф без НДС должен быть дешевле";
    }
}

// ============================================================================
// Дополнительный тест: Сравнение нескольких перевозчиков с реальными данными
// ============================================================================
TEST_F(OptimalExecutorTest, RealWorldScenarioComparison)
{
    // Сценарий: Перевозка 1.5т груза закрытым авто на 4 часа
    // Данные из файла ТАРИФЫ НА ГРУЗОПЕРЕВОЗКИ ПО ГОРОДУ:
    // 1.5т до 10м³: Тариф 4ч = 2220 руб, Стоимость часа = 495 руб, 
    // руб/км город = 16, руб/км область = 18
    
    auto serviceType = CreateTransportServiceType();
    
    // Создаём единицы измерения явно
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto kmUnit = CreateTestUnit("km", "Километр");
    
    auto executor1 = CreateTestExecutor("TC_COMPANY_A", "Компания А", true);
    auto executor2 = CreateTestExecutor("TC_COMPANY_B", "Компания Б", true);
    auto executor3 = CreateTestExecutor("TC_COMPANY_C", "Компания В", true);
    
    // Компания А - средние цены
    auto tariff1 = CreateTestTariff(serviceType.id, executor1.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff1.id, "HOUR_RATE", "Стоимость часа", 495.0, hourUnit.id);
    CreateTestTariffRate(tariff1.id, "KM_CITY", "Стоимость км по городу", 16.0, kmUnit.id);
    CreateTestTariffRate(tariff1.id, "KM_REGION", "Стоимость км по области", 18.0, kmUnit.id);
    
    // Компания Б - дороже на 10%
    auto tariff2 = CreateTestTariff(serviceType.id, executor2.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff2.id, "HOUR_RATE", "Стоимость часа", 545.0, hourUnit.id);
    CreateTestTariffRate(tariff2.id, "KM_CITY", "Стоимость км по городу", 18.0, kmUnit.id);
    CreateTestTariffRate(tariff2.id, "KM_REGION", "Стоимость км по области", 20.0, kmUnit.id);
    
    // Компания В - дешевле на 5%
    auto tariff3 = CreateTestTariff(serviceType.id, executor3.id, "2025-01-01", "2025-12-31", true, 18.0);
    CreateTestTariffRate(tariff3.id, "HOUR_RATE", "Стоимость часа", 470.0, hourUnit.id);
    CreateTestTariffRate(tariff3.id, "KM_CITY", "Стоимость км по городу", 15.0, kmUnit.id);
    CreateTestTariffRate(tariff3.id, "KM_REGION", "Стоимость км по области", 17.0, kmUnit.id);
    
    // Поиск оптимального
    auto results = service_->FindOptimalExecutor(serviceType.id, "2025-01-15");
    
    // Проверки
    ASSERT_GE(results.size(), 3) << "Должны быть найдены все 3 компании";
    EXPECT_EQ(results[0].executorName, "Компания В") << "Самой дешёвой должна быть Компания В";
    EXPECT_EQ(results[2].executorName, "Компания Б") << "Самой дорогой должна быть Компания Б";
}

} // namespace core::test
