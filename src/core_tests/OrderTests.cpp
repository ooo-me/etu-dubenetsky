// ============================================================================
// OrderTests.cpp
// Описание: Тесты заказов
// ============================================================================

#include "TestFixture.h"

namespace core::test
{

class OrderTest : public TariffServiceTestFixture
{
};

// Тест создания заказа
TEST_F(OrderTest, CreateOrder)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);

    Order order;
    order.code = "ORD-2025-001";
    order.serviceTypeId = serviceType.id;
    order.orderDate = "2025-01-15";
    order.executionDate = "2025-01-16";
    order.status = OrderStatus::New;
    order.note = "Тестовый заказ на перевозку";

    auto created = service_->CreateOrder(order);

    EXPECT_GT(created.id, 0);
    EXPECT_EQ(created.code, order.code);
    EXPECT_EQ(created.status, OrderStatus::New);
}

// Тест создания заказа с параметрами
TEST_F(OrderTest, CreateOrderWithParameters)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);

    // Создаём параметры
    auto tonUnit = CreateTestUnit("t", "Тонна");
    auto hourUnit = CreateTestUnit("hour", "Час");
    
    auto weightParam = CreateTestParameter("WEIGHT", "Вес груза", 0, tonUnit.id);
    auto timeParam = CreateTestParameter("TIME", "Время аренды", 0, hourUnit.id);

    AddServiceTypeParameter(serviceType.id, weightParam.id, true);
    AddServiceTypeParameter(serviceType.id, timeParam.id, true);

    // Создаём заказ
    auto order = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");

    // Устанавливаем значения параметров
    SetOrderParameter(order.id, weightParam.id, 1.5);
    SetOrderParameter(order.id, timeParam.id, 4.0);

    // Получаем заказ с параметрами
    auto retrieved = service_->GetOrder(order.id);

    EXPECT_EQ(retrieved.parameters.size(), 2);
    EXPECT_DOUBLE_EQ(retrieved.parameters[0].numValue.value(), 1.5);
    EXPECT_DOUBLE_EQ(retrieved.parameters[1].numValue.value(), 4.0);
}

// Тест получения всех заказов
TEST_F(OrderTest, GetAllOrders)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);

    CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");
    CreateTestOrder(serviceType.id, "2025-01-16", "2025-01-17");
    CreateTestOrder(serviceType.id, "2025-01-17", "2025-01-18");

    auto orders = service_->GetAllOrders();

    EXPECT_GE(orders.size(), 3);
}

// Тест обновления заказа
TEST_F(OrderTest, UpdateOrder)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    auto order = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");

    order.status = OrderStatus::InProgress;
    order.note = "Заказ в работе";

    service_->UpdateOrder(order);

    auto retrieved = service_->GetOrder(order.id);

    EXPECT_EQ(retrieved.status, OrderStatus::InProgress);
    EXPECT_EQ(retrieved.note, "Заказ в работе");
}

// Тест присвоения исполнителя к заказу
TEST_F(OrderTest, AssignExecutorToOrder)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    auto executor = CreateTestExecutor("TC_FAST", "ТК Быстрый", true);
    auto order = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");

    order.executorId = executor.id;
    service_->UpdateOrder(order);

    auto retrieved = service_->GetOrder(order.id);

    EXPECT_EQ(retrieved.executorId, executor.id);
}

// Тест удаления заказа
TEST_F(OrderTest, DeleteOrder)
{
    auto cls = CreateTestClass("TRANSPORT", "Транспортные услуги");
    auto serviceType = CreateTestServiceType("CARGO_TRANSPORT", "Грузоперевозка", cls.id);
    auto order = CreateTestOrder(serviceType.id, "2025-01-15", "2025-01-16");

    service_->DeleteOrder(order.id);

    auto orders = service_->GetAllOrders();
    auto found = std::find_if(orders.begin(), orders.end(),
        [&order](const Order& o) { return o.id == order.id; });

    EXPECT_EQ(found, orders.end());
}

} // namespace core::test
