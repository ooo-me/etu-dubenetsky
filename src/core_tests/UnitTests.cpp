// ============================================================================
// UnitTests.cpp
// Описание: Тесты единиц измерения
// ============================================================================

#include "TestFixture.h"

namespace core::test
{

class UnitTest : public TariffServiceTestFixture
{
};

// Тест создания единицы измерения
TEST_F(UnitTest, CreateUnit)
{
    Unit unit;
    unit.code = "kg";
    unit.name = "Килограмм";
    unit.note = "Единица измерения массы";

    auto created = service_->CreateUnit(unit);

    EXPECT_GT(created.id, 0);
    EXPECT_EQ(created.code, unit.code);
    EXPECT_EQ(created.name, unit.name);
    EXPECT_EQ(created.note, unit.note);
}

// Тест получения всех единиц измерения
TEST_F(UnitTest, GetAllUnits)
{
    // Создаём несколько единиц
    CreateTestUnit("kg", "Килограмм");
    CreateTestUnit("t", "Тонна");
    CreateTestUnit("m3", "Кубический метр");

    auto units = service_->GetAllUnits();

    EXPECT_GE(units.size(), 3);
}

// Тест обновления единицы измерения
TEST_F(UnitTest, UpdateUnit)
{
    auto unit = CreateTestUnit("kg", "Килограмм");

    unit.name = "Килограмм (обновлённый)";
    unit.note = "Обновлённое описание";

    service_->UpdateUnit(unit);

    auto units = service_->GetAllUnits();
    auto found = std::find_if(units.begin(), units.end(),
        [&unit](const Unit& u) { return u.id == unit.id; });

    ASSERT_NE(found, units.end());
    EXPECT_EQ(found->name, "Килограмм (обновлённый)");
    EXPECT_EQ(found->note, "Обновлённое описание");
}

// Тест удаления единицы измерения
TEST_F(UnitTest, DeleteUnit)
{
    auto unit = CreateTestUnit("kg", "Килограмм");

    service_->DeleteUnit(unit.id);

    auto units = service_->GetAllUnits();
    auto found = std::find_if(units.begin(), units.end(),
        [&unit](const Unit& u) { return u.id == unit.id; });

    EXPECT_EQ(found, units.end());
}

} // namespace core::test
