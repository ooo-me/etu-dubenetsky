// ============================================================================
// Движок правил
// Язык: C++20
// Описание: Класс для выполнения правил и вычисления выражений
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include "../model/Rule.hpp"
#include "../model/Order.hpp"
#include "../model/Tariff.hpp"
#include <memory>
#include <vector>

namespace TariffSystem::Core {

using namespace TariffSystem::Model;

/**
 * Движок правил
 * 
 * Выполняет вычисление правил и выражений в контексте заказа
 */
class RuleEngine {
public:
    RuleEngine() = default;

    /**
     * Вычисление правила
     * @param rule Правило для вычисления
     * @param context Контекст вычисления
     * @return Результат вычисления правила
     */
    ParameterValue evaluateRule(const Rule& rule, const Context& context) const {
        try {
            return rule.evaluate(context);
        } catch (const std::exception& e) {
            throw CalculationException(
                "Ошибка при вычислении правила '" + rule.getName() + "': " + e.what()
            );
        }
    }

    /**
     * Вычисление выражения
     * @param expression Выражение для вычисления
     * @param context Контекст вычисления
     * @return Результат вычисления выражения
     */
    ParameterValue evaluateExpression(
        const Expression& expression, 
        const Context& context
    ) const {
        try {
            return expression.evaluate(context);
        } catch (const std::exception& e) {
            throw CalculationException(
                "Ошибка при вычислении выражения: " + e.what()
            );
        }
    }

    /**
     * Проверка условия
     * @param condition Выражение-условие
     * @param context Контекст вычисления
     * @return true, если условие выполнено
     */
    bool checkCondition(const Expression& condition, const Context& context) const {
        try {
            auto result = condition.evaluate(context);
            auto boolValue = GetBooleanValue(result);
            return boolValue && *boolValue;
        } catch (const std::exception& e) {
            throw CalculationException(
                "Ошибка при проверке условия: " + e.what()
            );
        }
    }

    /**
     * Создание контекста из заказа
     * @param order Заказ
     * @return Контекст вычисления
     */
    Context createContext(const Order& order) const {
        Context context;
        
        // Добавляем параметры заказа в контекст
        for (const auto& [parameterId, value] : order.getParameters()) {
            context.setParameter(parameterId, value);
        }
        
        return context;
    }

    /**
     * Применение всех правил тарифа к заказу
     * @param tariff Тариф
     * @param order Заказ
     * @return Результат применения правил
     */
    ParameterValue applyTariffRules(
        const Tariff& tariff,
        const Order& order
    ) const {
        Context context = createContext(order);
        
        const auto& rules = tariff.getRules();
        if (rules.empty()) {
            throw CalculationException("Тариф не содержит правил расчета");
        }
        
        // Применяем правила по приоритету
        std::vector<std::shared_ptr<Rule>> sortedRules = rules;
        std::sort(sortedRules.begin(), sortedRules.end(),
            [](const auto& a, const auto& b) {
                return a->getPriority() < b->getPriority();
            });
        
        ParameterValue result;
        for (const auto& rule : sortedRules) {
            try {
                result = evaluateRule(*rule, context);
                
                // Если правило вернуло результат, используем его
                if (!std::holds_alternative<std::monostate>(result)) {
                    break;
                }
            } catch (const std::exception& e) {
                // Пропускаем правила с ошибками
                continue;
            }
        }
        
        return result;
    }

private:
    // Кэш результатов вычислений (для оптимизации)
    // Можно добавить при необходимости
};

} // namespace TariffSystem::Core
