// ============================================================================
// Тесты для классов модели
// Фреймворк: Google Test
// Описание: Юнит-тесты для классов Parameter, Classifier, Service
// ============================================================================

#include <gtest/gtest.h>
#include "model/Parameter.hpp"
#include "model/Classifier.hpp"
#include "model/Service.hpp"
#include "model/Tariff.hpp"
#include "model/Order.hpp"

using namespace TariffSystem;
using namespace TariffSystem::Model;

// ============================================================================
// Тесты для Parameter
// ============================================================================

class ParameterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создание тестовых параметров
        intParam = std::make_shared<Parameter>(1, "TEST_INT", "Целочисленный параметр", ParameterType::INTEGER);
        doubleParam = std::make_shared<Parameter>(2, "TEST_DOUBLE", "Вещественный параметр", ParameterType::DOUBLE);
        stringParam = std::make_shared<Parameter>(3, "TEST_STRING", "Строковый параметр", ParameterType::STRING);
        boolParam = std::make_shared<Parameter>(4, "TEST_BOOL", "Логический параметр", ParameterType::BOOLEAN);
    }

    std::shared_ptr<Parameter> intParam;
    std::shared_ptr<Parameter> doubleParam;
    std::shared_ptr<Parameter> stringParam;
    std::shared_ptr<Parameter> boolParam;
};

TEST_F(ParameterTest, BasicProperties) {
    EXPECT_EQ(intParam->getId(), 1);
    EXPECT_EQ(intParam->getCode(), "TEST_INT");
    EXPECT_EQ(intParam->getName(), "Целочисленный параметр");
    EXPECT_EQ(intParam->getType(), ParameterType::INTEGER);
}

TEST_F(ParameterTest, SetAndGetIntegerValue) {
    intParam->setValue(Integer{42});
    EXPECT_TRUE(intParam->hasValue());
    
    auto value = intParam->getDoubleValue();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, 42.0);
}

TEST_F(ParameterTest, SetAndGetDoubleValue) {
    doubleParam->setValue(Double{3.14159});
    EXPECT_TRUE(doubleParam->hasValue());
    
    auto value = doubleParam->getDoubleValue();
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 3.14159);
}

TEST_F(ParameterTest, SetAndGetStringValue) {
    stringParam->setValue(String{"Тестовая строка"});
    EXPECT_TRUE(stringParam->hasValue());
    
    auto value = stringParam->getStringValue();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "Тестовая строка");
}

TEST_F(ParameterTest, SetAndGetBooleanValue) {
    boolParam->setValue(Boolean{true});
    EXPECT_TRUE(boolParam->hasValue());
    
    auto value = boolParam->getBooleanValue();
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(*value);
}

TEST_F(ParameterTest, RequiredParameterValidation) {
    intParam->setRequired(true);
    EXPECT_FALSE(intParam->validate()); // Нет значения
    
    intParam->setValue(Integer{100});
    EXPECT_TRUE(intParam->validate()); // Есть значение
}

TEST_F(ParameterTest, OptionalParameterValidation) {
    doubleParam->setRequired(false);
    EXPECT_TRUE(doubleParam->validate()); // Необязательный параметр всегда валиден
}

TEST_F(ParameterTest, InvalidValueType) {
    // Попытка установить неправильный тип должна вызвать исключение
    EXPECT_THROW(intParam->setValue(String{"строка"}), ParameterException);
}

TEST_F(ParameterTest, UnitOfMeasurement) {
    doubleParam->setUnit("кг");
    EXPECT_TRUE(doubleParam->getUnit().has_value());
    EXPECT_EQ(*doubleParam->getUnit(), "кг");
}

// ============================================================================
// Тесты для Classifier
// ============================================================================

class ClassifierTest : public ::testing::Test {
protected:
    void SetUp() override {
        root = std::make_shared<Classifier>(1, "ROOT", "Корень", 0);
        service = std::make_shared<Classifier>(2, "SERVICE", "Услуги", 1);
        cargo = std::make_shared<Classifier>(3, "CARGO_SERVICE", "Грузоперевозки", 2);
    }

    std::shared_ptr<Classifier> root;
    std::shared_ptr<Classifier> service;
    std::shared_ptr<Classifier> cargo;
};

TEST_F(ClassifierTest, BasicProperties) {
    EXPECT_EQ(root->getId(), 1);
    EXPECT_EQ(root->getCode(), "ROOT");
    EXPECT_EQ(root->getName(), "Корень");
    EXPECT_EQ(root->getLevel(), 0);
}

TEST_F(ClassifierTest, HierarchyStructure) {
    root->addChild(service);
    service->addChild(cargo);
    
    EXPECT_TRUE(root->isRoot());
    EXPECT_FALSE(service->isRoot());
    EXPECT_FALSE(cargo->isRoot());
    
    EXPECT_FALSE(root->isLeaf());
    EXPECT_FALSE(service->isLeaf());
    EXPECT_TRUE(cargo->isLeaf());
}

TEST_F(ClassifierTest, ParentChildRelationship) {
    root->addChild(service);
    
    EXPECT_EQ(root->getChildren().size(), 1);
    EXPECT_EQ(root->getChildren()[0], service);
    
    ASSERT_TRUE(service->getParentId().has_value());
    EXPECT_EQ(*service->getParentId(), root->getId());
}

TEST_F(ClassifierTest, MultipleChildren) {
    auto tariff = std::make_shared<Classifier>(4, "TARIFF", "Тарифы", 1);
    auto order = std::make_shared<Classifier>(5, "ORDER", "Заказы", 1);
    
    root->addChild(service);
    root->addChild(tariff);
    root->addChild(order);
    
    EXPECT_EQ(root->getChildren().size(), 3);
}

// ============================================================================
// Тесты для Service
// ============================================================================

class ServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        cargoService = std::make_shared<CargoService>(1, "CARGO_001", "Грузоперевозка по городу", 10);
        storageService = std::make_shared<StorageService>(2, "STORAGE_001", "Ответственное хранение", 11);
    }

    std::shared_ptr<CargoService> cargoService;
    std::shared_ptr<StorageService> storageService;
};

TEST_F(ServiceTest, CargoServiceBasicProperties) {
    EXPECT_EQ(cargoService->getId(), 1);
    EXPECT_EQ(cargoService->getCode(), "CARGO_001");
    EXPECT_EQ(cargoService->getName(), "Грузоперевозка по городу");
    EXPECT_EQ(cargoService->getClassId(), 10);
}

TEST_F(ServiceTest, CargoServiceParameters) {
    cargoService->setCargoWeight(1.5);
    cargoService->setCargoVolume(3.0);
    cargoService->setVehicleClass(VehicleClass::CLOSED);
    
    ASSERT_TRUE(cargoService->getCargoWeight().has_value());
    EXPECT_DOUBLE_EQ(*cargoService->getCargoWeight(), 1.5);
    
    ASSERT_TRUE(cargoService->getCargoVolume().has_value());
    EXPECT_DOUBLE_EQ(*cargoService->getCargoVolume(), 3.0);
    
    ASSERT_TRUE(cargoService->getVehicleClass().has_value());
    EXPECT_EQ(*cargoService->getVehicleClass(), VehicleClass::CLOSED);
}

TEST_F(ServiceTest, CargoServiceBaseCostCalculation) {
    cargoService->setCargoWeight(2.0);  // 2 тонны * 1000 = 2000
    cargoService->setCargoVolume(5.0);  // 5 м³ * 500 = 2500
    
    Double cost = cargoService->calculateBaseCost();
    EXPECT_DOUBLE_EQ(cost, 4500.0);  // 2000 + 2500
}

TEST_F(ServiceTest, StorageServiceBasicProperties) {
    EXPECT_EQ(storageService->getId(), 2);
    EXPECT_EQ(storageService->getCode(), "STORAGE_001");
}

TEST_F(ServiceTest, StorageServiceParameters) {
    storageService->setAvgPalletPlaces(100.0);
    storageService->setTurnover(0.5);
    storageService->setOperationType(OperationType::MECHANIZED);
    
    ASSERT_TRUE(storageService->getAvgPalletPlaces().has_value());
    EXPECT_DOUBLE_EQ(*storageService->getAvgPalletPlaces(), 100.0);
    
    ASSERT_TRUE(storageService->getTurnover().has_value());
    EXPECT_DOUBLE_EQ(*storageService->getTurnover(), 0.5);
}

TEST_F(ServiceTest, StorageServiceBaseCostCalculation) {
    storageService->setAvgPalletPlaces(50.0);  // 50 паллет * 100 = 5000
    
    Double cost = storageService->calculateBaseCost();
    EXPECT_DOUBLE_EQ(cost, 5000.0);
}

TEST_F(ServiceTest, StorageServiceMonthlyBudget) {
    storageService->setAvgPalletPlaces(100.0);
    
    Double budget = storageService->calculateMonthlyBudget();
    EXPECT_DOUBLE_EQ(budget, 300000.0);  // 100 * 100 * 30
}

TEST_F(ServiceTest, ServiceWithParameters) {
    auto param1 = std::make_shared<Parameter>(1, "WEIGHT", "Вес", ParameterType::DOUBLE);
    auto param2 = std::make_shared<Parameter>(2, "VOLUME", "Объем", ParameterType::DOUBLE);
    
    cargoService->addParameter(param1);
    cargoService->addParameter(param2);
    
    EXPECT_EQ(cargoService->getParameters().size(), 2);
    
    auto retrievedParam = cargoService->getParameter(1);
    ASSERT_NE(retrievedParam, nullptr);
    EXPECT_EQ(retrievedParam->getCode(), "WEIGHT");
}

// ============================================================================
// Тесты для Tariff
// ============================================================================

class TariffTest : public ::testing::Test {
protected:
    void SetUp() override {
        tariff = std::make_shared<Tariff>(1, "TARIFF_001", "Тариф стандарт", 10);
    }

    std::shared_ptr<Tariff> tariff;
};

TEST_F(TariffTest, BasicProperties) {
    EXPECT_EQ(tariff->getId(), 1);
    EXPECT_EQ(tariff->getCode(), "TARIFF_001");
    EXPECT_EQ(tariff->getName(), "Тариф стандарт");
    EXPECT_TRUE(tariff->isActive());
}

TEST_F(TariffTest, ActiveStateManagement) {
    tariff->setActive(false);
    EXPECT_FALSE(tariff->isActive());
    EXPECT_FALSE(tariff->isValidNow());
}

TEST_F(TariffTest, AddRulesToTariff) {
    auto rule1 = std::make_shared<Rule>(1, "RULE_001", "Правило 1");
    auto rule2 = std::make_shared<Rule>(2, "RULE_002", "Правило 2");
    
    tariff->addRule(rule1);
    tariff->addRule(rule2);
    
    EXPECT_EQ(tariff->getRules().size(), 2);
}

// ============================================================================
// Тесты для Order
// ============================================================================

class OrderTest : public ::testing::Test {
protected:
    void SetUp() override {
        order = std::make_unique<Order>(1, "ORDER_001", "Заказ на грузоперевозку", 100);
    }

    std::unique_ptr<Order> order;
};

TEST_F(OrderTest, BasicProperties) {
    EXPECT_EQ(order->getId(), 1);
    EXPECT_EQ(order->getCode(), "ORDER_001");
    EXPECT_EQ(order->getName(), "Заказ на грузоперевозку");
    EXPECT_EQ(order->getStatus(), OrderStatus::DRAFT);
}

TEST_F(OrderTest, StatusTransitions) {
    EXPECT_EQ(order->getStatus(), OrderStatus::DRAFT);
    
    order->setCalculatedCost(1500.0);
    EXPECT_EQ(order->getStatus(), OrderStatus::CALCULATED);
    EXPECT_DOUBLE_EQ(order->getCalculatedCost(), 1500.0);
    
    order->confirm();
    EXPECT_EQ(order->getStatus(), OrderStatus::CONFIRMED);
    
    order->complete();
    EXPECT_EQ(order->getStatus(), OrderStatus::COMPLETED);
}

TEST_F(OrderTest, InvalidStatusTransition) {
    // Попытка подтвердить неоплаченный заказ
    EXPECT_THROW(order->confirm(), TariffSystemException);
}

TEST_F(OrderTest, CancelOrder) {
    order->setCalculatedCost(1000.0);
    order->cancel();
    EXPECT_EQ(order->getStatus(), OrderStatus::CANCELLED);
}

TEST_F(OrderTest, CannotCancelCompletedOrder) {
    order->setCalculatedCost(1000.0);
    order->confirm();
    order->complete();
    EXPECT_THROW(order->cancel(), TariffSystemException);
}

TEST_F(OrderTest, AddParametersToOrder) {
    order->addParameter(1, Double{1.5});
    order->addParameter(2, String{"Тестовое значение"});
    order->addParameter(3, Integer{42});
    
    auto param1 = order->getParameter(1);
    ASSERT_TRUE(param1.has_value());
    auto doubleVal = GetDoubleValue(*param1);
    ASSERT_TRUE(doubleVal.has_value());
    EXPECT_DOUBLE_EQ(*doubleVal, 1.5);
    
    auto param2 = order->getParameter(2);
    ASSERT_TRUE(param2.has_value());
    auto stringVal = GetStringValue(*param2);
    ASSERT_TRUE(stringVal.has_value());
    EXPECT_EQ(*stringVal, "Тестовое значение");
}

TEST_F(OrderTest, GetAllParameters) {
    order->addParameter(1, Double{1.0});
    order->addParameter(2, Double{2.0});
    order->addParameter(3, Double{3.0});
    
    const auto& params = order->getParameters();
    EXPECT_EQ(params.size(), 3);
}

TEST_F(OrderTest, SetTariff) {
    order->setTariffId(500);
    ASSERT_TRUE(order->getTariffId().has_value());
    EXPECT_EQ(*order->getTariffId(), 500);
}

TEST_F(OrderTest, StatusString) {
    EXPECT_EQ(order->getStatusString(), "Черновик");
    
    order->setCalculatedCost(1000.0);
    EXPECT_EQ(order->getStatusString(), "Рассчитан");
    
    order->confirm();
    EXPECT_EQ(order->getStatusString(), "Подтвержден");
}

// ============================================================================
// Главная функция тестов
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
