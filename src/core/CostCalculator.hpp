// ============================================================================
// Калькулятор стоимости
// Язык: C++20
// Описание: Класс для расчета стоимости заказов
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include "../model/Order.hpp"
#include "../model/Tariff.hpp"
#include "RuleEngine.hpp"
#include <memory>
#include <vector>
#include <map>

namespace TariffSystem::Core {

using namespace TariffSystem::Model;

/**
 * Результат расчета стоимости
 */
struct CalculationResult {
    Double cost;                    // Рассчитанная стоимость
    Integer tariffId;               // ID использованного тарифа
    String tariffName;              // Название тарифа
    bool success;                   // Успешность расчета
    String errorMessage;            // Сообщение об ошибке (если не успешно)
};

/**
 * Калькулятор стоимости заказов
 */
class CostCalculator {
public:
    /**
     * Конструктор
     * @param ruleEngine Движок правил
     */
    explicit CostCalculator(std::shared_ptr<RuleEngine> ruleEngine)
        : ruleEngine_(std::move(ruleEngine))
    {}

    /**
     * Расчет стоимости заказа по тарифу
     * @param order Заказ
     * @param tariff Тариф
     * @return Рассчитанная стоимость
     */
    Double calculateCost(Order& order, const Tariff& tariff) const {
        // Проверка действительности тарифа
        if (!tariff.isValidNow()) {
            throw CalculationException("Тариф недействителен");
        }

        try {
            // Применяем правила тарифа
            auto result = ruleEngine_->applyTariffRules(tariff, order);
            
            // Извлекаем числовое значение
            auto cost = GetDoubleValue(result);
            if (!cost) {
                throw CalculationException("Результат расчета не является числом");
            }

            // Обновляем заказ
            order.setTariffId(tariff.getId());
            order.setCalculatedCost(*cost);

            return *cost;
        } catch (const std::exception& e) {
            throw CalculationException(
                "Ошибка при расчете стоимости по тарифу '" + 
                tariff.getName() + "': " + e.what()
            );
        }
    }

    /**
     * Расчет стоимости заказа по всем тарифам
     * @param order Заказ
     * @param tariffs Список тарифов
     * @return Карта: ID тарифа -> стоимость
     */
    std::map<Integer, CalculationResult> calculateWithAllTariffs(
        const Order& order,
        const std::vector<std::shared_ptr<Tariff>>& tariffs
    ) const {
        std::map<Integer, CalculationResult> results;

        for (const auto& tariff : tariffs) {
            CalculationResult result;
            result.tariffId = tariff->getId();
            result.tariffName = tariff->getName();

            try {
                // Проверка действительности тарифа
                if (!tariff->isValidNow()) {
                    result.success = false;
                    result.errorMessage = "Тариф недействителен";
                    result.cost = 0.0;
                } else {
                    // Расчет стоимости
                    auto calcResult = ruleEngine_->applyTariffRules(*tariff, order);
                    auto cost = GetDoubleValue(calcResult);
                    
                    if (cost) {
                        result.cost = *cost;
                        result.success = true;
                    } else {
                        result.success = false;
                        result.errorMessage = "Результат расчета не является числом";
                        result.cost = 0.0;
                    }
                }
            } catch (const std::exception& e) {
                result.success = false;
                result.errorMessage = e.what();
                result.cost = 0.0;
            }

            results[tariff->getId()] = result;
        }

        return results;
    }

    /**
     * Получение списка тарифов, применимых к заказу
     * @param order Заказ
     * @param tariffs Список всех тарифов
     * @return Список применимых тарифов
     */
    std::vector<std::shared_ptr<Tariff>> getApplicableTariffs(
        const Order& order,
        const std::vector<std::shared_ptr<Tariff>>& tariffs
    ) const {
        std::vector<std::shared_ptr<Tariff>> applicable;

        for (const auto& tariff : tariffs) {
            // Проверка действительности
            if (!tariff->isValidNow()) {
                continue;
            }

            // Проверка соответствия классу услуги
            // (в реальной реализации нужно получить класс услуги из БД)
            
            applicable.push_back(tariff);
        }

        return applicable;
    }

private:
    std::shared_ptr<RuleEngine> ruleEngine_;  // Движок правил
};

} // namespace TariffSystem::Core
