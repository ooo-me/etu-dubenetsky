// ============================================================================
// Поиск оптимального тарифа
// Язык: C++20
// Описание: Класс для поиска оптимального тарифа по критерию стоимости
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include "../model/Order.hpp"
#include "../model/Tariff.hpp"
#include "CostCalculator.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <optional>

namespace TariffSystem::Core {

using namespace TariffSystem::Model;

/**
 * Результат сравнения тарифов
 */
struct TariffComparisonResult {
    Integer tariffId;           // ID тарифа
    String tariffCode;          // Код тарифа
    String tariffName;          // Название тарифа
    Double cost;                // Стоимость по тарифу
    bool isOptimal;             // Является ли оптимальным
    Integer rank;               // Ранг (1 = лучший)
};

/**
 * Класс для поиска оптимального тарифа
 */
class OptimalSearcher {
public:
    /**
     * Конструктор
     * @param calculator Калькулятор стоимости
     */
    explicit OptimalSearcher(std::shared_ptr<CostCalculator> calculator)
        : calculator_(std::move(calculator))
    {}

    /**
     * Поиск оптимального тарифа по минимальной стоимости
     * @param order Заказ
     * @param tariffs Список тарифов для сравнения
     * @return Оптимальный тариф и его стоимость
     */
    std::optional<std::pair<std::shared_ptr<Tariff>, Double>> findOptimalTariff(
        const Order& order,
        const std::vector<std::shared_ptr<Tariff>>& tariffs
    ) const {
        if (tariffs.empty()) {
            return std::nullopt;
        }

        // Рассчитываем стоимость по всем тарифам
        auto results = calculator_->calculateWithAllTariffs(order, tariffs);

        // Находим тариф с минимальной стоимостью
        std::shared_ptr<Tariff> optimalTariff = nullptr;
        Double minCost = std::numeric_limits<Double>::max();

        for (const auto& tariff : tariffs) {
            auto it = results.find(tariff->getId());
            if (it != results.end() && it->second.success) {
                if (it->second.cost < minCost) {
                    minCost = it->second.cost;
                    optimalTariff = tariff;
                }
            }
        }

        if (optimalTariff) {
            return std::make_pair(optimalTariff, minCost);
        }

        return std::nullopt;
    }

    /**
     * Сравнение всех тарифов с ранжированием
     * @param order Заказ
     * @param tariffs Список тарифов
     * @return Список результатов сравнения, отсортированных по стоимости
     */
    std::vector<TariffComparisonResult> compareAllTariffs(
        const Order& order,
        const std::vector<std::shared_ptr<Tariff>>& tariffs
    ) const {
        std::vector<TariffComparisonResult> comparisonResults;

        // Рассчитываем стоимость по всем тарифам
        auto results = calculator_->calculateWithAllTariffs(order, tariffs);

        // Формируем результаты сравнения
        for (const auto& tariff : tariffs) {
            auto it = results.find(tariff->getId());
            if (it != results.end() && it->second.success) {
                TariffComparisonResult result;
                result.tariffId = tariff->getId();
                result.tariffCode = tariff->getCode();
                result.tariffName = tariff->getName();
                result.cost = it->second.cost;
                result.isOptimal = false; // Установим позже
                result.rank = 0;          // Установим позже

                comparisonResults.push_back(result);
            }
        }

        // Сортируем по стоимости (по возрастанию)
        std::sort(comparisonResults.begin(), comparisonResults.end(),
            [](const auto& a, const auto& b) {
                return a.cost < b.cost;
            });

        // Устанавливаем ранги и помечаем оптимальный
        for (size_t i = 0; i < comparisonResults.size(); ++i) {
            comparisonResults[i].rank = static_cast<Integer>(i + 1);
            if (i == 0) {
                comparisonResults[i].isOptimal = true;
            }
        }

        return comparisonResults;
    }

    /**
     * Поиск N лучших тарифов
     * @param order Заказ
     * @param tariffs Список тарифов
     * @param topN Количество лучших тарифов
     * @return Список лучших тарифов
     */
    std::vector<TariffComparisonResult> findTopNTariffs(
        const Order& order,
        const std::vector<std::shared_ptr<Tariff>>& tariffs,
        size_t topN
    ) const {
        auto allResults = compareAllTariffs(order, tariffs);

        // Возвращаем только топ-N
        if (allResults.size() > topN) {
            allResults.resize(topN);
        }

        return allResults;
    }

    /**
     * Анализ экономии при выборе оптимального тарифа
     * @param order Заказ
     * @param tariffs Список тарифов
     * @return Информация об экономии
     */
    struct SavingsAnalysis {
        Double optimalCost;         // Минимальная стоимость
        Double maximumCost;         // Максимальная стоимость
        Double averageCost;         // Средняя стоимость
        Double savings;             // Экономия по сравнению с максимумом
        Double savingsPercent;      // Процент экономии
        Integer optimalTariffId;    // ID оптимального тарифа
        Integer expensiveTariffId;  // ID самого дорогого тарифа
    };

    std::optional<SavingsAnalysis> analyzeSavings(
        const Order& order,
        const std::vector<std::shared_ptr<Tariff>>& tariffs
    ) const {
        auto results = compareAllTariffs(order, tariffs);
        
        if (results.empty()) {
            return std::nullopt;
        }

        SavingsAnalysis analysis;
        analysis.optimalCost = results.front().cost;
        analysis.maximumCost = results.back().cost;
        analysis.optimalTariffId = results.front().tariffId;
        analysis.expensiveTariffId = results.back().tariffId;

        // Рассчитываем среднюю стоимость
        Double totalCost = 0.0;
        for (const auto& result : results) {
            totalCost += result.cost;
        }
        analysis.averageCost = totalCost / results.size();

        // Рассчитываем экономию
        analysis.savings = analysis.maximumCost - analysis.optimalCost;
        analysis.savingsPercent = (analysis.savings / analysis.maximumCost) * 100.0;

        return analysis;
    }

private:
    std::shared_ptr<CostCalculator> calculator_;  // Калькулятор стоимости
};

} // namespace TariffSystem::Core
