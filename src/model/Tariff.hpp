// ============================================================================
// Тариф
// Язык: C++20
// Описание: Класс для работы с тарифами
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include "Rule.hpp"
#include <vector>
#include <memory>
#include <chrono>

namespace TariffSystem::Model {

/**
 * Тариф на услугу
 */
class Tariff {
public:
    /**
     * Конструктор
     * @param id Уникальный идентификатор
     * @param code Код тарифа
     * @param name Наименование тарифа
     * @param serviceClassId ID класса услуги
     */
    Tariff(Integer id, String code, String name, Integer serviceClassId)
        : id_(id)
        , code_(std::move(code))
        , name_(std::move(name))
        , serviceClassId_(serviceClassId)
        , isActive_(true)
    {}
    
    // Конструктор с 3 параметрами для совместимости
    Tariff(Integer id, String code, String name)
        : id_(id)
        , code_(std::move(code))
        , name_(std::move(name))
        , serviceClassId_(0)
        , isActive_(true)
    {}

    // Геттеры
    Integer getId() const { return id_; }
    const String& getCode() const { return code_; }
    const String& getName() const { return name_; }
    Integer getServiceClassId() const { return serviceClassId_; }
    bool isActive() const { return isActive_; }
    const std::optional<String>& getDescription() const { return description_; }
    const std::optional<Date>& getValidFrom() const { return validFrom_; }
    const std::optional<Date>& getValidTo() const { return validTo_; }
    const std::optional<String>& getNote() const { return note_; }
    const std::optional<String>& getProvider() const { return provider_; }

    // Сеттеры
    void setDescription(String description) { description_ = std::move(description); }
    void setActive(bool active) { isActive_ = active; }
    void setValidFrom(Date validFrom) { validFrom_ = validFrom; }
    void setValidTo(Date validTo) { validTo_ = validTo; }
    void setNote(String note) { note_ = std::move(note); }
    void setProvider(String provider) { provider_ = std::move(provider); }
    
    /**
     * Активация тарифа
     */
    void activate() { isActive_ = true; }
    
    /**
     * Деактивация тарифа
     */
    void deactivate() { isActive_ = false; }

    /**
     * Добавление правила к тарифу
     * @param rule Правило
     */
    void addRule(std::shared_ptr<Rule> rule) {
        rules_.push_back(rule);
    }

    /**
     * Получение всех правил тарифа
     * @return Вектор правил
     */
    const std::vector<std::shared_ptr<Rule>>& getRules() const {
        return rules_;
    }

    /**
     * Проверка действительности тарифа на заданную дату
     * @param date Дата для проверки
     * @return true, если тариф действителен
     */
    bool isValid(const Date& date) const {
        if (!isActive_) {
            return false;
        }

        if (validFrom_ && date < *validFrom_) {
            return false;
        }

        if (validTo_ && date > *validTo_) {
            return false;
        }

        return true;
    }

    /**
     * Проверка действительности тарифа на текущую дату
     * @return true, если тариф действителен
     */
    bool isValidNow() const {
        auto now = std::chrono::system_clock::now();
        auto days = std::chrono::floor<std::chrono::days>(now);
        auto ymd = std::chrono::year_month_day{days};
        return isValid(ymd);
    }

private:
    Integer id_;                                    // Уникальный идентификатор
    String code_;                                   // Код тарифа
    String name_;                                   // Наименование
    Integer serviceClassId_;                        // ID класса услуги
    bool isActive_;                                 // Признак активности
    std::optional<String> description_;             // Описание
    std::optional<String> provider_;                // Поставщик тарифа
    std::optional<Date> validFrom_;                 // Дата начала действия
    std::optional<Date> validTo_;                   // Дата окончания действия
    std::optional<String> note_;                    // Примечание
    std::vector<std::shared_ptr<Rule>> rules_;      // Правила тарифа
};

} // namespace TariffSystem::Model
