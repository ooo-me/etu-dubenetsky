// ============================================================================
// TariffTests.cpp
// Описание: Тесты тарифов и ставок
// ============================================================================

#include "TestFixture.h"

namespace core::test
{

class TariffTest : public TariffServiceTestFixture
{
};

// Тест создания тарифа
TEST_F(TariffTest, CreateTariff)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    auto executor = CreateTestExecutor("TC_FAST", "ТК Быстрый", true);

    Tariff tariff;
    tariff.code = "TARIFF_2025";
    tariff.name = "Тариф на грузоперевозки 2025";
    tariff.serviceTypeId = serviceType.id;
    tariff.executorId = executor.id;
    tariff.dateBegin = "2025-01-01";
    tariff.dateEnd = "2025-12-31";
    tariff.isWithVat = true;
    tariff.vatRate = 20.0;
    tariff.isActive = true;
    tariff.note = "Базовый тариф";

    auto created = service_->CreateTariff(tariff);

    EXPECT_GT(created.id, 0);
    EXPECT_EQ(created.code, tariff.code);
    EXPECT_EQ(created.isWithVat, tariff.isWithVat);
    EXPECT_DOUBLE_EQ(created.vatRate, tariff.vatRate);
}

// Тест создания ставок тарифа
TEST_F(TariffTest, CreateTariffRates)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    auto executor = CreateTestExecutor("TC_FAST", "ТК Быстрый", true);
    auto tariff = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31");

    // Создаём единицы измерения
    auto hourUnit = CreateTestUnit("hour", "Час");
    auto kmUnit = CreateTestUnit("km", "Километр");

    // Создаём ставки
    auto hourRate = CreateTestTariffRate(tariff.id, "HOUR_RATE", "Стоимость часа", 420.0, hourUnit.id);
    auto kmRateCity = CreateTestTariffRate(tariff.id, "KM_CITY", "Стоимость км по городу", 14.0, kmUnit.id);
    auto kmRateRegion = CreateTestTariffRate(tariff.id, "KM_REGION", "Стоимость км по области", 16.0, kmUnit.id);

    // Получаем тариф со ставками
    auto retrieved = service_->GetTariff(tariff.id);

    EXPECT_EQ(retrieved.rates.size(), 3);
    EXPECT_DOUBLE_EQ(retrieved.rates[0].value, 420.0);
    EXPECT_DOUBLE_EQ(retrieved.rates[1].value, 14.0);
    EXPECT_DOUBLE_EQ(retrieved.rates[2].value, 16.0);
}

// Тест тарифов с разными ставками НДС
TEST_F(TariffTest, TariffWithDifferentVatRates)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    auto executor = CreateTestExecutor("TC_FAST", "ТК Быстрый", true);

    // Тариф с НДС 18%
    auto tariff18 = CreateTestTariff(serviceType.id, executor.id, "2024-01-01", "2024-12-31", true, 18.0);
    EXPECT_DOUBLE_EQ(tariff18.vatRate, 18.0);

    // Тариф с НДС 20%
    auto tariff20 = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31", true, 20.0);
    EXPECT_DOUBLE_EQ(tariff20.vatRate, 20.0);

    // Тариф без НДС
    auto tariffNoVat = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31", false, 0.0);
    EXPECT_FALSE(tariffNoVat.isWithVat);
}

// Тест периода действия тарифа
TEST_F(TariffTest, TariffValidityPeriod)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    auto executor = CreateTestExecutor("TC_FAST", "ТК Быстрый", true);

    auto tariff = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31");

    EXPECT_EQ(tariff.dateBegin, "2025-01-01");
    EXPECT_EQ(tariff.dateEnd, "2025-12-31");
}

// Тест обновления тарифа
TEST_F(TariffTest, UpdateTariff)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    auto executor = CreateTestExecutor("TC_FAST", "ТК Быстрый", true);
    auto tariff = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31");

    tariff.name = "Тариф (обновлённый)";
    tariff.isActive = false;

    service_->UpdateTariff(tariff);

    auto retrieved = service_->GetTariff(tariff.id);

    EXPECT_EQ(retrieved.name, "Тариф (обновлённый)");
    EXPECT_EQ(retrieved.isActive, false);
}

// Тест удаления тарифа
TEST_F(TariffTest, DeleteTariff)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    auto executor = CreateTestExecutor("TC_FAST", "ТК Быстрый", true);
    auto tariff = CreateTestTariff(serviceType.id, executor.id, "2025-01-01", "2025-12-31");

    service_->DeleteTariff(tariff.id);

    auto tariffs = service_->GetAllTariffs();
    auto found = std::find_if(tariffs.begin(), tariffs.end(),
        [&tariff](const Tariff& t) { return t.id == tariff.id; });

    EXPECT_EQ(found, tariffs.end());
}

} // namespace core::test
