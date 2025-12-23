// ============================================================================
// ServiceTypeTests.cpp
// Описание: Тесты типов услуг и их параметров
// ============================================================================

#include "TestFixture.h"

namespace core::test
{

class ServiceTypeTest : public TariffServiceTestFixture
{
};

// Тест создания типа услуги
TEST_F(ServiceTypeTest, CreateServiceType)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    
    ServiceType serviceType;
    serviceType.code = "CARGO_TRANSPORT";
    serviceType.name = "Грузоперевозка";
    serviceType.classId = cls.id;
    serviceType.note = "Транспортировка грузов";

    auto created = service_->CreateServiceType(serviceType);

    EXPECT_GT(created.id, 0);
    EXPECT_EQ(created.code, serviceType.code);
    EXPECT_EQ(created.name, serviceType.name);
    EXPECT_EQ(created.classId, cls.id);
}

// Тест добавления параметров к типу услуги
TEST_F(ServiceTypeTest, AddServiceTypeParameters)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);

    // Создаём единицы измерения
    auto tonUnit = CreateTestUnit("t", "Тонна");
    auto m3Unit = CreateTestUnit("m3", "Кубический метр");
    auto hourUnit = CreateTestUnit("hour", "Час");

    // Создаём параметры
    auto weightParam = CreateTestParameter("WEIGHT", "Вес груза", 0, tonUnit.id);
    auto volumeParam = CreateTestParameter("VOLUME", "Объём груза", 0, m3Unit.id);
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);

    // Добавляем параметры к типу услуги
    AddServiceTypeParameter(serviceType.id, weightParam.id, true);
    AddServiceTypeParameter(serviceType.id, volumeParam.id, true);
    AddServiceTypeParameter(serviceType.id, timeParam.id, true);

    // Получаем тип услуги с параметрами
    auto retrieved = service_->GetServiceType(serviceType.id);

    EXPECT_EQ(retrieved.parameters.size(), 3);
    EXPECT_EQ(retrieved.parameters[0].name, "Вес груза");
    EXPECT_EQ(retrieved.parameters[1].name, "Объём груза");
    EXPECT_EQ(retrieved.parameters[2].name, "Время аренды");
}

// Тест получения всех типов услуг
TEST_F(ServiceTypeTest, GetAllServiceTypes)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");

    CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    CreateTestServiceType("STORAGE", "Хранение", cls.id);

    auto serviceTypes = service_->GetAllServiceTypes();

    EXPECT_GE(serviceTypes.size(), 2);
}

// Тест обновления типа услуги
TEST_F(ServiceTypeTest, UpdateServiceType)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);

    serviceType.name = "Грузоперевозка (обновлённая)";
    serviceType.note = "Обновлённое описание";

    service_->UpdateServiceType(serviceType);

    auto retrieved = service_->GetServiceType(serviceType.id);

    EXPECT_EQ(retrieved.name, "Грузоперевозка (обновлённая)");
    EXPECT_EQ(retrieved.note, "Обновлённое описание");
}

// Тест удаления типа услуги
TEST_F(ServiceTypeTest, DeleteServiceType)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);

    service_->DeleteServiceType(serviceType.id);

    auto serviceTypes = service_->GetAllServiceTypes();
    auto found = std::find_if(serviceTypes.begin(), serviceTypes.end(),
        [&serviceType](const ServiceType& st) { return st.id == serviceType.id; });

    EXPECT_EQ(found, serviceTypes.end());
}

} // namespace core::test
