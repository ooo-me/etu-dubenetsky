// ============================================================================
// Интеграционные тесты
// Фреймворк: Google Test
// Описание: Тесты полного цикла работы системы
// ============================================================================

#include <gtest/gtest.h>
#include "model/Service.hpp"
#include "model/Tariff.hpp"
#include "model/Order.hpp"
#include "model/Rule.hpp"
#include "core/RuleEngine.hpp"
#include "core/CostCalculator.hpp"
#include "core/OptimalSearch.hpp"

using namespace TariffSystem;
using namespace TariffSystem::Core;
using namespace TariffSystem::Model;

// ============================================================================
// Интеграционный тест: Расчет стоимости грузоперевозки
// ============================================================================

class CargoTransportIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ruleEngine = std::make_shared<RuleEngine>();
        calculator = std::make_shared<CostCalculator>(ruleEngine);
        searcher = std::make_unique<OptimalSearcher>(calculator);
        
        // Создаем тариф на грузоперевозки
        // Формула: (часы * 800) + ((км_город - 50) * 50) + ((км_область - 50) * 75)
        tariff = createCargoTariff();
        
        // Создаем заказ
        order = std::make_unique<Order>(1, "ORDER_001", "Заказ на грузоперевозку", 100);
    }
    
    std::shared_ptr<Tariff> createCargoTariff() {
        auto tariff = std::make_shared<Tariff>(1, "CARGO_TARIFF", "Тариф на грузоперевозки", 100);
        
        // Правило расчета: базовая формула
        auto rule = std::make_shared<Rule>(1, "CARGO_COST", "Расчет стоимости грузоперевозки");
        
        // Параметры: 1=hours, 2=city_km, 3=region_km
        // Формула: (hours * 800) + max(0, city_km - 50) * 50 + max(0, region_km - 50) * 75
        
        // Упрощенная версия: hours * 800 + city_km * 50 + region_km * 75
        auto hours = std::make_shared<ParameterExpression>(1);
        auto cityKm = std::make_shared<ParameterExpression>(2);
        auto regionKm = std::make_shared<ParameterExpression>(3);
        
        auto hourRate = std::make_shared<ConstantExpression>(Double{800.0});
        auto cityRate = std::make_shared<ConstantExpression>(Double{50.0});
        auto regionRate = std::make_shared<ConstantExpression>(Double{75.0});
        
        // hours * 800
        auto hoursCost = std::make_shared<ArithmeticExpression>(
            ArithmeticOperator::MULTIPLY, hours, hourRate);
        
        // city_km * 50
        auto cityCost = std::make_shared<ArithmeticExpression>(
            ArithmeticOperator::MULTIPLY, cityKm, cityRate);
        
        // region_km * 75
        auto regionCost = std::make_shared<ArithmeticExpression>(
            ArithmeticOperator::MULTIPLY, regionKm, regionRate);
        
        // hoursCost + cityCost
        auto temp = std::make_shared<ArithmeticExpression>(
            ArithmeticOperator::ADD, hoursCost, cityCost);
        
        // temp + regionCost
        auto totalCost = std::make_shared<ArithmeticExpression>(
            ArithmeticOperator::ADD, temp, regionCost);
        
        rule->setAction(totalCost);
        tariff->addRule(rule);
        
        return tariff;
    }

    std::shared_ptr<RuleEngine> ruleEngine;
    std::shared_ptr<CostCalculator> calculator;
    std::unique_ptr<OptimalSearcher> searcher;
    std::shared_ptr<Tariff> tariff;
    std::unique_ptr<Order> order;
};

TEST_F(CargoTransportIntegrationTest, SimpleCargoOrder) {
    // Заказ: 6 часов, 105 км по городу, 200 км по области
    order->addParameter(1, Double{6.0});    // hours
    order->addParameter(2, Double{105.0});  // city_km
    order->addParameter(3, Double{200.0});  // region_km
    
    Double cost = calculator->calculateCost(*order, *tariff);
    
    // Ожидаемый расчет:
    // 6 * 800 = 4800
    // 105 * 50 = 5250
    // 200 * 75 = 15000
    // Итого: 25050
    EXPECT_DOUBLE_EQ(cost, 25050.0);
}

TEST_F(CargoTransportIntegrationTest, MinimalCargoOrder) {
    // Минимальный заказ
    order->addParameter(1, Double{4.0});   // 4 часа
    order->addParameter(2, Double{20.0});  // 20 км город
    order->addParameter(3, Double{0.0});   // 0 км область
    
    Double cost = calculator->calculateCost(*order, *tariff);
    
    // 4 * 800 + 20 * 50 + 0 * 75 = 3200 + 1000 = 4200
    EXPECT_DOUBLE_EQ(cost, 4200.0);
}

// ============================================================================
// Интеграционный тест: Расчет стоимости хранения
// ============================================================================

class StorageIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ruleEngine = std::make_shared<RuleEngine>();
        calculator = std::make_shared<CostCalculator>(ruleEngine);
        
        // Тариф на хранение: pallets * rate * days * (1 - turnover * 0.1)
        tariff = createStorageTariff();
        
        order = std::make_unique<Order>(1, "STORAGE_ORDER", "Заказ на хранение", 200);
    }
    
    std::shared_ptr<Tariff> createStorageTariff() {
        auto tariff = std::make_shared<Tariff>(1, "STORAGE_TARIFF", "Тариф на хранение", 200);
        
        auto rule = std::make_shared<Rule>(1, "STORAGE_COST", "Расчет стоимости хранения");
        
        // Параметры: 1=pallets, 2=days, 3=turnover
        // Формула: pallets * 15 * days * (1 - turnover * 0.1)
        // Упрощенная: pallets * 15 * days (без учета оборачиваемости для простоты)
        
        auto pallets = std::make_shared<ParameterExpression>(1);
        auto days = std::make_shared<ParameterExpression>(2);
        auto rate = std::make_shared<ConstantExpression>(Double{15.0});
        
        // pallets * rate
        auto temp = std::make_shared<ArithmeticExpression>(
            ArithmeticOperator::MULTIPLY, pallets, rate);
        
        // temp * days
        auto totalCost = std::make_shared<ArithmeticExpression>(
            ArithmeticOperator::MULTIPLY, temp, days);
        
        rule->setAction(totalCost);
        tariff->addRule(rule);
        
        return tariff;
    }

    std::shared_ptr<RuleEngine> ruleEngine;
    std::shared_ptr<CostCalculator> calculator;
    std::shared_ptr<Tariff> tariff;
    std::unique_ptr<Order> order;
};

TEST_F(StorageIntegrationTest, MonthlyStorageCost) {
    // 100 паллет на 30 дней
    order->addParameter(1, Double{100.0});  // pallets
    order->addParameter(2, Double{30.0});   // days
    
    Double cost = calculator->calculateCost(*order, *tariff);
    
    // 100 * 15 * 30 = 45000
    EXPECT_DOUBLE_EQ(cost, 45000.0);
}

TEST_F(StorageIntegrationTest, ShortTermStorage) {
    // 50 паллет на 7 дней
    order->addParameter(1, Double{50.0});   // pallets
    order->addParameter(2, Double{7.0});    // days
    
    Double cost = calculator->calculateCost(*order, *tariff);
    
    // 50 * 15 * 7 = 5250
    EXPECT_DOUBLE_EQ(cost, 5250.0);
}

// ============================================================================
// Интеграционный тест: Поиск оптимального тарифа
// ============================================================================

class OptimalTariffSelectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        ruleEngine = std::make_shared<RuleEngine>();
        calculator = std::make_shared<CostCalculator>(ruleEngine);
        searcher = std::make_unique<OptimalSearcher>(calculator);
        
        // Создаем три компании с разными тарифами
        tariff1 = createTariffForCompany(1, "COMPANY_A", 800.0);  // Дорогая
        tariff2 = createTariffForCompany(2, "COMPANY_B", 600.0);  // Средняя
        tariff3 = createTariffForCompany(3, "COMPANY_C", 700.0);  // Средняя
        
        tariffs = {tariff1, tariff2, tariff3};
        
        order = std::make_unique<Order>(1, "COMPARISON_ORDER", "Заказ для сравнения", 100);
        order->addParameter(1, Double{5.0});  // hours
    }
    
    std::shared_ptr<Tariff> createTariffForCompany(int id, const String& company, Double hourRate) {
        auto tariff = std::make_shared<Tariff>(id, company + "_TARIFF", 
                                                "Тариф компании " + company, 100);
        
        auto rule = std::make_shared<Rule>(id, company + "_RULE", "Правило " + company);
        
        auto hours = std::make_shared<ParameterExpression>(1);
        auto rate = std::make_shared<ConstantExpression>(Double{hourRate});
        
        auto cost = std::make_shared<ArithmeticExpression>(
            ArithmeticOperator::MULTIPLY, hours, rate);
        
        rule->setAction(cost);
        tariff->addRule(rule);
        
        return tariff;
    }

    std::shared_ptr<RuleEngine> ruleEngine;
    std::shared_ptr<CostCalculator> calculator;
    std::unique_ptr<OptimalSearcher> searcher;
    std::shared_ptr<Tariff> tariff1, tariff2, tariff3;
    std::vector<std::shared_ptr<Tariff>> tariffs;
    std::unique_ptr<Order> order;
};

TEST_F(OptimalTariffSelectionTest, FindCheapestTariff) {
    auto result = searcher->findOptimalTariff(*order, tariffs);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->first->getCode(), "COMPANY_B_TARIFF");  // Самый дешевый (600*5=3000)
    EXPECT_DOUBLE_EQ(result->second, 3000.0);
}

TEST_F(OptimalTariffSelectionTest, CompareAllCompanies) {
    auto results = searcher->compareAllTariffs(*order, tariffs);
    
    ASSERT_EQ(results.size(), 3);
    
    // Проверяем сортировку: COMPANY_B (3000), COMPANY_C (3500), COMPANY_A (4000)
    EXPECT_EQ(results[0].tariffCode, "COMPANY_B_TARIFF");
    EXPECT_DOUBLE_EQ(results[0].cost, 3000.0);
    EXPECT_TRUE(results[0].isOptimal);
    
    EXPECT_EQ(results[1].tariffCode, "COMPANY_C_TARIFF");
    EXPECT_DOUBLE_EQ(results[1].cost, 3500.0);
    EXPECT_FALSE(results[1].isOptimal);
    
    EXPECT_EQ(results[2].tariffCode, "COMPANY_A_TARIFF");
    EXPECT_DOUBLE_EQ(results[2].cost, 4000.0);
    EXPECT_FALSE(results[2].isOptimal);
}

TEST_F(OptimalTariffSelectionTest, SavingsAnalysis) {
    auto analysis = searcher->analyzeSavings(*order, tariffs);
    
    ASSERT_TRUE(analysis.has_value());
    
    EXPECT_DOUBLE_EQ(analysis->optimalCost, 3000.0);    // COMPANY_B
    EXPECT_DOUBLE_EQ(analysis->maximumCost, 4000.0);    // COMPANY_A
    EXPECT_DOUBLE_EQ(analysis->savings, 1000.0);        // 4000 - 3000
    EXPECT_DOUBLE_EQ(analysis->savingsPercent, 25.0);   // (1000/4000)*100
}

// ============================================================================
// Интеграционный тест: Условные правила
// ============================================================================

class ConditionalRulesTest : public ::testing::Test {
protected:
    void SetUp() override {
        ruleEngine = std::make_shared<RuleEngine>();
        calculator = std::make_shared<CostCalculator>(ruleEngine);
        
        // Тариф с условным правилом: если вес > 1т, то повышающий коэффициент 1.2
        tariff = createTariffWithCondition();
        
        order = std::make_unique<Order>(1, "CONDITIONAL_ORDER", "Заказ с условием", 100);
    }
    
    std::shared_ptr<Tariff> createTariffWithCondition() {
        auto tariff = std::make_shared<Tariff>(1, "CONDITIONAL_TARIFF", "Условный тариф", 100);
        
        // Правило 1: Если вес > 1, то базовая стоимость * 1.2, иначе базовая стоимость
        auto rule1 = std::make_shared<Rule>(1, "HEAVY_CARGO_RULE", "Правило для тяжелого груза");
        
        // Условие: weight > 1
        auto weight = std::make_shared<ParameterExpression>(1);
        auto threshold = std::make_shared<ConstantExpression>(Double{1.0});
        auto condition = std::make_shared<ComparisonExpression>(
            ComparisonOperator::GREATER_THAN, weight, threshold);
        
        // Действие: base_cost * 1.2
        auto baseCost = std::make_shared<ParameterExpression>(2);
        auto coefficient = std::make_shared<ConstantExpression>(Double{1.2});
        auto action = std::make_shared<ArithmeticExpression>(
            ArithmeticOperator::MULTIPLY, baseCost, coefficient);
        
        rule1->setCondition(condition);
        rule1->setAction(action);
        rule1->setPriority(1);
        
        // Правило 2: Базовая стоимость (без условия, низкий приоритет)
        auto rule2 = std::make_shared<Rule>(2, "STANDARD_RULE", "Стандартное правило");
        auto standardAction = std::make_shared<ParameterExpression>(2);
        rule2->setAction(standardAction);
        rule2->setPriority(2);
        
        tariff->addRule(rule1);
        tariff->addRule(rule2);
        
        return tariff;
    }

    std::shared_ptr<RuleEngine> ruleEngine;
    std::shared_ptr<CostCalculator> calculator;
    std::shared_ptr<Tariff> tariff;
    std::unique_ptr<Order> order;
};

TEST_F(ConditionalRulesTest, HeavyCargoWithSurcharge) {
    // Тяжелый груз: вес = 2т, базовая стоимость = 1000
    order->addParameter(1, Double{2.0});     // weight
    order->addParameter(2, Double{1000.0});  // base_cost
    
    Double cost = calculator->calculateCost(*order, *tariff);
    
    // Вес > 1, применяется коэффициент 1.2
    EXPECT_DOUBLE_EQ(cost, 1200.0);  // 1000 * 1.2
}

TEST_F(ConditionalRulesTest, LightCargoStandardRate) {
    // Легкий груз: вес = 0.5т, базовая стоимость = 1000
    order->addParameter(1, Double{0.5});     // weight
    order->addParameter(2, Double{1000.0});  // base_cost
    
    Double cost = calculator->calculateCost(*order, *tariff);
    
    // Вес <= 1, применяется стандартное правило
    EXPECT_DOUBLE_EQ(cost, 1000.0);
}

// ============================================================================
// Главная функция тестов
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
