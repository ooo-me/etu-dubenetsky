// ============================================================================
// Параметр
// Язык: C++20
// Описание: Класс для работы с параметрами объектов
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include <optional>
#include <memory>

namespace TariffSystem::Model {

/**
 * Параметр объекта
 * 
 * Представляет параметр услуги, тарифа или заказа
 */
class Parameter {
public:
    /**
     * Конструктор
     * @param id Уникальный идентификатор
     * @param code Код параметра
     * @param name Наименование параметра
     * @param type Тип параметра
     */
    Parameter(Integer id, String code, String name, ParameterType type)
        : id_(id)
        , code_(std::move(code))
        , name_(std::move(name))
        , type_(type)
        , isRequired_(false)
    {}

    // Геттеры
    Integer getId() const { return id_; }
    const String& getCode() const { return code_; }
    const String& getName() const { return name_; }
    ParameterType getType() const { return type_; }
    const std::optional<String>& getUnit() const { return unit_; }
    bool isRequired() const { return isRequired_; }
    const ParameterValue& getValue() const { return value_; }
    const std::optional<String>& getNote() const { return note_; }

    // Сеттеры
    void setUnit(String unit) { unit_ = std::move(unit); }
    void setRequired(bool required) { isRequired_ = required; }
    void setNote(String note) { note_ = std::move(note); }

    /**
     * Установка значения параметра
     * @param value Значение параметра
     */
    void setValue(ParameterValue value) {
        // Проверка типа значения
        if (!validateValue(value)) {
            throw ParameterException("Некорректный тип значения для параметра " + name_);
        }
        value_ = std::move(value);
    }

    /**
     * Проверка наличия значения
     * @return true, если значение установлено
     */
    bool hasValue() const {
        return !std::holds_alternative<std::monostate>(value_);
    }

    /**
     * Валидация параметра
     * @return true, если параметр валиден
     */
    bool validate() const {
        // Обязательный параметр должен иметь значение
        if (isRequired_ && !hasValue()) {
            return false;
        }
        return true;
    }

    /**
     * Получение числового значения
     * @return Числовое значение или std::nullopt
     */
    std::optional<Double> getDoubleValue() const {
        return GetDoubleValue(value_);
    }

    /**
     * Получение строкового значения
     * @return Строковое значение или std::nullopt
     */
    std::optional<String> getStringValue() const {
        return GetStringValue(value_);
    }

    /**
     * Получение логического значения
     * @return Логическое значение или std::nullopt
     */
    std::optional<Boolean> getBooleanValue() const {
        return GetBooleanValue(value_);
    }

private:
    /**
     * Валидация типа значения
     * @param value Значение для проверки
     * @return true, если тип значения соответствует типу параметра
     */
    bool validateValue(const ParameterValue& value) const {
        // Пустое значение всегда допустимо
        if (std::holds_alternative<std::monostate>(value)) {
            return true;
        }

        switch (type_) {
            case ParameterType::INTEGER:
                return std::holds_alternative<Integer>(value);
            case ParameterType::DOUBLE:
                return std::holds_alternative<Double>(value) || 
                       std::holds_alternative<Integer>(value);
            case ParameterType::STRING:
            case ParameterType::ENUMERATION:
                return std::holds_alternative<String>(value);
            case ParameterType::BOOLEAN:
                return std::holds_alternative<Boolean>(value);
            case ParameterType::DATE:
                return std::holds_alternative<Date>(value);
            default:
                return false;
        }
    }

    Integer id_;                      // Уникальный идентификатор
    String code_;                     // Код параметра
    String name_;                     // Наименование параметра
    ParameterType type_;              // Тип параметра
    std::optional<String> unit_;      // Единица измерения
    bool isRequired_;                 // Признак обязательности
    ParameterValue value_;            // Значение параметра
    std::optional<String> note_;      // Примечание
};

} // namespace TariffSystem::Model
