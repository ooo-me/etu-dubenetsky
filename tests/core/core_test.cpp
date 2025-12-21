// ============================================================================
// Тесты для бизнес-логики
// Фреймворк: Google Test
// Описание: Юнит-тесты для RuleEngine, CostCalculator, OptimalSearcher
// ============================================================================

#include <gtest/gtest.h>
#include "core/RuleEngine.hpp"
#include "core/CostCalculator.hpp"
#include "core/OptimalSearch.hpp"
#include "model/Rule.hpp"
#include "model/Order.hpp"
#include "model/Tariff.hpp"
#include "model/Service.hpp"

using namespace TariffSystem;
using namespace TariffSystem::Core;
using namespace TariffSystem::Model;

// ============================================================================
// Тесты для Expression и Context
// ============================================================================

class ExpressionTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<Context>();
        context->setParameter(1, Double{10.0});
        context->setParameter(2, Double{5.0});
        context->setConstant("PI", Double{3.14159});
    }

    std::unique_ptr<Context> context;
};

TEST_F(ExpressionTest, ConstantExpression) {
    auto expr = std::make_shared<ConstantExpression>(Double{42.0});
    auto result = expr->evaluate(*context);
    
    auto value = GetDoubleValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 42.0);
}

TEST_F(ExpressionTest, ParameterExpression) {
    auto expr = std::make_shared<ParameterExpression>(1);
    auto result = expr->evaluate(*context);
    
    auto value = GetDoubleValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 10.0);
}

TEST_F(ExpressionTest, ArithmeticAddition) {
    auto left = std::make_shared<ParameterExpression>(1);
    auto right = std::make_shared<ParameterExpression>(2);
    auto expr = std::make_shared<ArithmeticExpression>(ArithmeticOperator::ADD, left, right);
    
    auto result = expr->evaluate(*context);
    auto value = GetDoubleValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 15.0);  // 10 + 5
}

TEST_F(ExpressionTest, ArithmeticSubtraction) {
    auto left = std::make_shared<ParameterExpression>(1);
    auto right = std::make_shared<ParameterExpression>(2);
    auto expr = std::make_shared<ArithmeticExpression>(ArithmeticOperator::SUBTRACT, left, right);
    
    auto result = expr->evaluate(*context);
    auto value = GetDoubleValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 5.0);  // 10 - 5
}

TEST_F(ExpressionTest, ArithmeticMultiplication) {
    auto left = std::make_shared<ParameterExpression>(1);
    auto right = std::make_shared<ParameterExpression>(2);
    auto expr = std::make_shared<ArithmeticExpression>(ArithmeticOperator::MULTIPLY, left, right);
    
    auto result = expr->evaluate(*context);
    auto value = GetDoubleValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 50.0);  // 10 * 5
}

TEST_F(ExpressionTest, ArithmeticDivision) {
    auto left = std::make_shared<ParameterExpression>(1);
    auto right = std::make_shared<ParameterExpression>(2);
    auto expr = std::make_shared<ArithmeticExpression>(ArithmeticOperator::DIVIDE, left, right);
    
    auto result = expr->evaluate(*context);
    auto value = GetDoubleValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 2.0);  // 10 / 5
}

TEST_F(ExpressionTest, DivisionByZero) {
    context->setParameter(3, Double{0.0});
    auto left = std::make_shared<ParameterExpression>(1);
    auto right = std::make_shared<ParameterExpression>(3);
    auto expr = std::make_shared<ArithmeticExpression>(ArithmeticOperator::DIVIDE, left, right);
    
    EXPECT_THROW(expr->evaluate(*context), CalculationException);
}

TEST_F(ExpressionTest, ComparisonLessThan) {
    auto left = std::make_shared<ParameterExpression>(2);   // 5
    auto right = std::make_shared<ParameterExpression>(1);  // 10
    auto expr = std::make_shared<ComparisonExpression>(ComparisonOperator::LESS_THAN, left, right);
    
    auto result = expr->evaluate(*context);
    auto value = GetBooleanValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(*value);  // 5 < 10
}

TEST_F(ExpressionTest, ComparisonEqual) {
    context->setParameter(3, Double{10.0});
    auto left = std::make_shared<ParameterExpression>(1);
    auto right = std::make_shared<ParameterExpression>(3);
    auto expr = std::make_shared<ComparisonExpression>(ComparisonOperator::EQUAL, left, right);
    
    auto result = expr->evaluate(*context);
    auto value = GetBooleanValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(*value);  // 10 == 10
}

TEST_F(ExpressionTest, ComparisonGreaterThan) {
    auto left = std::make_shared<ParameterExpression>(1);   // 10
    auto right = std::make_shared<ParameterExpression>(2);  // 5
    auto expr = std::make_shared<ComparisonExpression>(ComparisonOperator::GREATER_THAN, left, right);
    
    auto result = expr->evaluate(*context);
    auto value = GetBooleanValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(*value);  // 10 > 5
}

TEST_F(ExpressionTest, LogicalAND) {
    context->setParameter(10, Boolean{true});
    context->setParameter(11, Boolean{true});
    context->setParameter(12, Boolean{false});
    
    auto expr1 = std::make_shared<ParameterExpression>(10);
    auto expr2 = std::make_shared<ParameterExpression>(11);
    std::vector<std::shared_ptr<Expression>> operands = {expr1, expr2};
    auto andExpr = std::make_shared<LogicalExpression>(LogicalOperator::AND, operands);
    
    auto result = andExpr->evaluate(*context);
    auto value = GetBooleanValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(*value);  // true AND true
}

TEST_F(ExpressionTest, LogicalOR) {
    context->setParameter(10, Boolean{true});
    context->setParameter(12, Boolean{false});
    
    auto expr1 = std::make_shared<ParameterExpression>(10);
    auto expr2 = std::make_shared<ParameterExpression>(12);
    std::vector<std::shared_ptr<Expression>> operands = {expr1, expr2};
    auto orExpr = std::make_shared<LogicalExpression>(LogicalOperator::OR, operands);
    
    auto result = orExpr->evaluate(*context);
    auto value = GetBooleanValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(*value);  // true OR false
}

TEST_F(ExpressionTest, LogicalNOT) {
    context->setParameter(10, Boolean{false});
    
    auto expr = std::make_shared<ParameterExpression>(10);
    std::vector<std::shared_ptr<Expression>> operands = {expr};
    auto notExpr = std::make_shared<LogicalExpression>(LogicalOperator::NOT, operands);
    
    auto result = notExpr->evaluate(*context);
    auto value = GetBooleanValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(*value);  // NOT false
}

TEST_F(ExpressionTest, ComplexExpression) {
    // (10 + 5) * 2 = 30
    auto param1 = std::make_shared<ParameterExpression>(1);  // 10
    auto param2 = std::make_shared<ParameterExpression>(2);  // 5
    auto two = std::make_shared<ConstantExpression>(Double{2.0});
    
    auto sum = std::make_shared<ArithmeticExpression>(ArithmeticOperator::ADD, param1, param2);
    auto result = std::make_shared<ArithmeticExpression>(ArithmeticOperator::MULTIPLY, sum, two);
    
    auto value = GetDoubleValue(result->evaluate(*context));
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 30.0);
}

// ============================================================================
// Тесты для Rule
// ============================================================================

class RuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        context = std::make_unique<Context>();
        context->setParameter(1, Double{100.0});
        context->setParameter(2, Double{50.0});
    }

    std::unique_ptr<Context> context;
};

TEST_F(RuleTest, RuleWithConditionAndAction) {
    auto rule = std::make_shared<Rule>(1, "RULE_001", "Правило проверки");
    
    // Условие: param1 > param2 (100 > 50 = true)
    auto param1 = std::make_shared<ParameterExpression>(1);
    auto param2 = std::make_shared<ParameterExpression>(2);
    auto condition = std::make_shared<ComparisonExpression>(
        ComparisonOperator::GREATER_THAN, param1, param2);
    
    // Действие: param1 + param2 (100 + 50 = 150)
    auto action = std::make_shared<ArithmeticExpression>(
        ArithmeticOperator::ADD, param1, param2);
    
    rule->setCondition(condition);
    rule->setAction(action);
    
    auto result = rule->evaluate(*context);
    auto value = GetDoubleValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 150.0);
}

TEST_F(RuleTest, RuleWithFalseCondition) {
    auto rule = std::make_shared<Rule>(1, "RULE_001", "Правило проверки");
    
    // Условие: param2 > param1 (50 > 100 = false)
    auto param1 = std::make_shared<ParameterExpression>(1);
    auto param2 = std::make_shared<ParameterExpression>(2);
    auto condition = std::make_shared<ComparisonExpression>(
        ComparisonOperator::GREATER_THAN, param2, param1);
    
    auto action = std::make_shared<ConstantExpression>(Double{999.0});
    
    rule->setCondition(condition);
    rule->setAction(action);
    
    auto result = rule->evaluate(*context);
    // Условие не выполнено, должен вернуться monostate
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
}

// ============================================================================
// Тесты для RuleEngine
// ============================================================================

class RuleEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<RuleEngine>();
        order = std::make_unique<Order>(1, "ORDER_001", "Тестовый заказ", 100);
        order->addParameter(1, Double{100.0});
        order->addParameter(2, Double{5.0});
    }

    std::unique_ptr<RuleEngine> engine;
    std::unique_ptr<Order> order;
};

TEST_F(RuleEngineTest, CreateContextFromOrder) {
    auto context = engine->createContext(*order);
    
    auto param1 = context.getParameter(1);
    ASSERT_TRUE(param1.has_value());
    auto value = GetDoubleValue(*param1);
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 100.0);
}

TEST_F(RuleEngineTest, EvaluateSimpleExpression) {
    Context context;
    context.setParameter(1, Double{10.0});
    
    auto expr = std::make_shared<ParameterExpression>(1);
    auto result = engine->evaluateExpression(*expr, context);
    
    auto value = GetDoubleValue(result);
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 10.0);
}

TEST_F(RuleEngineTest, CheckTrueCondition) {
    Context context;
    context.setParameter(1, Double{10.0});
    context.setParameter(2, Double{5.0});
    
    auto left = std::make_shared<ParameterExpression>(1);
    auto right = std::make_shared<ParameterExpression>(2);
    auto condition = std::make_shared<ComparisonExpression>(
        ComparisonOperator::GREATER_THAN, left, right);
    
    EXPECT_TRUE(engine->checkCondition(*condition, context));
}

TEST_F(RuleEngineTest, CheckFalseCondition) {
    Context context;
    context.setParameter(1, Double{5.0});
    context.setParameter(2, Double{10.0});
    
    auto left = std::make_shared<ParameterExpression>(1);
    auto right = std::make_shared<ParameterExpression>(2);
    auto condition = std::make_shared<ComparisonExpression>(
        ComparisonOperator::GREATER_THAN, left, right);
    
    EXPECT_FALSE(engine->checkCondition(*condition, context));
}

// ============================================================================
// Тесты для CostCalculator
// ============================================================================

class CostCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        ruleEngine = std::make_shared<RuleEngine>();
        calculator = std::make_unique<CostCalculator>(ruleEngine);
        
        // Создаем тариф с простым правилом
        tariff = std::make_shared<Tariff>(1, "TARIFF_001", "Тестовый тариф", 100);
        
        // Правило: param1 * param2
        auto rule = std::make_shared<Rule>(1, "RULE_001", "Умножение параметров");
        auto param1 = std::make_shared<ParameterExpression>(1);
        auto param2 = std::make_shared<ParameterExpression>(2);
        auto action = std::make_shared<ArithmeticExpression>(
            ArithmeticOperator::MULTIPLY, param1, param2);
        rule->setAction(action);
        
        tariff->addRule(rule);
        
        // Создаем заказ
        order = std::make_unique<Order>(1, "ORDER_001", "Тестовый заказ", 100);
        order->addParameter(1, Double{10.0});
        order->addParameter(2, Double{5.0});
    }

    std::shared_ptr<RuleEngine> ruleEngine;
    std::unique_ptr<CostCalculator> calculator;
    std::shared_ptr<Tariff> tariff;
    std::unique_ptr<Order> order;
};

TEST_F(CostCalculatorTest, CalculateSimpleCost) {
    Double cost = calculator->calculateCost(*order, *tariff);
    EXPECT_DOUBLE_EQ(cost, 50.0);  // 10 * 5
    EXPECT_DOUBLE_EQ(order->getCalculatedCost(), 50.0);
}

TEST_F(CostCalculatorTest, InactiveTariffThrowsException) {
    tariff->setActive(false);
    EXPECT_THROW(calculator->calculateCost(*order, *tariff), CalculationException);
}

// ============================================================================
// Тесты для OptimalSearcher
// ============================================================================

class OptimalSearcherTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto ruleEngine = std::make_shared<RuleEngine>();
        auto costCalculator = std::make_shared<CostCalculator>(ruleEngine);
        searcher = std::make_unique<OptimalSearcher>(costCalculator);
        
        // Создаем несколько тарифов с разными стоимостями
        tariff1 = createTariffWithCost(1, "TARIFF_001", "Дешевый", 1000.0);
        tariff2 = createTariffWithCost(2, "TARIFF_002", "Средний", 1500.0);
        tariff3 = createTariffWithCost(3, "TARIFF_003", "Дорогой", 2000.0);
        
        tariffs = {tariff1, tariff2, tariff3};
        
        // Создаем заказ
        order = std::make_unique<Order>(1, "ORDER_001", "Тестовый заказ", 100);
        order->addParameter(1, Double{1.0});  // Этот параметр используется в правилах
    }
    
    std::shared_ptr<Tariff> createTariffWithCost(int id, const String& code, 
                                                   const String& name, Double cost) {
        auto tariff = std::make_shared<Tariff>(id, code, name, 100);
        auto rule = std::make_shared<Rule>(id, "RULE_" + code, "Правило для " + name);
        auto action = std::make_shared<ConstantExpression>(Double{cost});
        rule->setAction(action);
        tariff->addRule(rule);
        return tariff;
    }

    std::unique_ptr<OptimalSearcher> searcher;
    std::shared_ptr<Tariff> tariff1, tariff2, tariff3;
    std::vector<std::shared_ptr<Tariff>> tariffs;
    std::unique_ptr<Order> order;
};

TEST_F(OptimalSearcherTest, FindOptimalTariff) {
    auto result = searcher->findOptimalTariff(*order, tariffs);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->first->getId(), 1);  // Самый дешевый
    EXPECT_DOUBLE_EQ(result->second, 1000.0);
}

TEST_F(OptimalSearcherTest, CompareAllTariffs) {
    auto results = searcher->compareAllTariffs(*order, tariffs);
    
    ASSERT_EQ(results.size(), 3);
    
    // Проверяем сортировку по стоимости
    EXPECT_DOUBLE_EQ(results[0].cost, 1000.0);
    EXPECT_DOUBLE_EQ(results[1].cost, 1500.0);
    EXPECT_DOUBLE_EQ(results[2].cost, 2000.0);
    
    // Проверяем ранги
    EXPECT_EQ(results[0].rank, 1);
    EXPECT_EQ(results[1].rank, 2);
    EXPECT_EQ(results[2].rank, 3);
    
    // Проверяем отметку оптимального
    EXPECT_TRUE(results[0].isOptimal);
    EXPECT_FALSE(results[1].isOptimal);
    EXPECT_FALSE(results[2].isOptimal);
}

TEST_F(OptimalSearcherTest, FindTopNTariffs) {
    auto results = searcher->findTopNTariffs(*order, tariffs, 2);
    
    ASSERT_EQ(results.size(), 2);
    EXPECT_DOUBLE_EQ(results[0].cost, 1000.0);
    EXPECT_DOUBLE_EQ(results[1].cost, 1500.0);
}

TEST_F(OptimalSearcherTest, AnalyzeSavings) {
    auto analysis = searcher->analyzeSavings(*order, tariffs);
    
    ASSERT_TRUE(analysis.has_value());
    EXPECT_DOUBLE_EQ(analysis->optimalCost, 1000.0);
    EXPECT_DOUBLE_EQ(analysis->maximumCost, 2000.0);
    EXPECT_DOUBLE_EQ(analysis->averageCost, 1500.0);
    EXPECT_DOUBLE_EQ(analysis->savings, 1000.0);  // 2000 - 1000
    EXPECT_DOUBLE_EQ(analysis->savingsPercent, 50.0);  // (1000/2000) * 100
}

TEST_F(OptimalSearcherTest, EmptyTariffList) {
    std::vector<std::shared_ptr<Tariff>> emptyList;
    auto result = searcher->findOptimalTariff(*order, emptyList);
    
    EXPECT_FALSE(result.has_value());
}

// ============================================================================
// Главная функция тестов
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
