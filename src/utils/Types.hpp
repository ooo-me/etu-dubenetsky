// ============================================================================
// Общие типы и определения
// Язык: C++20
// Описание: Базовые типы данных, используемые в системе
// ============================================================================

#pragma once

#include <string>
#include <variant>
#include <optional>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <stdexcept>

namespace TariffSystem {

// ============================================================================
// Базовые типы
// ============================================================================

using Date = std::chrono::year_month_day;
using String = std::string;
using Integer = int;
using Double = double;
using Boolean = bool;

// ============================================================================
// Вариантный тип для значений параметров
// ============================================================================

using ParameterValue = std::variant<
    std::monostate,    // Пустое значение
    Integer,           // Целочисленное
    Double,            // Вещественное
    String,            // Строковое
    Boolean,           // Логическое
    Date               // Дата
>;

// ============================================================================
// Перечисления
// ============================================================================

/**
 * Тип параметра
 */
enum class ParameterType {
    INTEGER = 0,       // Целое число
    DOUBLE = 1,        // Вещественное число
    STRING = 2,        // Строка
    BOOLEAN = 3,       // Логическое значение
    ENUMERATION = 4,   // Перечисление
    DATE = 5           // Дата
};

/**
 * Тип функции/выражения
 */
enum class FunctionType {
    PREDICATE = 0,     // Предикат (сравнение)
    ARITHMETIC = 1,    // Арифметическое выражение
    LOGICAL = 2,       // Логическое выражение
    CHOICE = 3         // Функция выбора (CASE)
};

/**
 * Арифметические операции
 */
enum class ArithmeticOperator {
    ADD,               // Сложение (+)
    SUBTRACT,          // Вычитание (-)
    MULTIPLY,          // Умножение (*)
    DIVIDE             // Деление (/)
};

/**
 * Операции сравнения
 */
enum class ComparisonOperator {
    LESS_THAN,         // Меньше (<)
    LESS_EQUAL,        // Меньше или равно (<=)
    EQUAL,             // Равно (=)
    GREATER_EQUAL,     // Больше или равно (>=)
    GREATER_THAN,      // Больше (>)
    NOT_EQUAL          // Не равно (<>)
};

/**
 * Логические операции
 */
enum class LogicalOperator {
    AND,               // Логическое И
    OR,                // Логическое ИЛИ
    NOT                // Логическое НЕ
};

/**
 * Статус заказа
 */
enum class OrderStatus {
    DRAFT,             // Черновик
    CALCULATED,        // Рассчитан
    CONFIRMED,         // Подтвержден
    COMPLETED,         // Выполнен
    CANCELLED          // Отменен
};

/**
 * Класс автомобиля для грузоперевозок
 */
enum class VehicleClass {
    CLOSED,            // Закрытый
    OPEN,              // Открытый
    REFRIGERATOR       // Рефрижератор
};

/**
 * Временной интервал
 */
enum class TimeInterval {
    HOURS_4,           // 4 часа
    HOURS_8,           // 8 часов
    HOURS_4_PLUS_1,    // 4+1 час
    HOURS_5_PLUS_1,    // 5+1 час
    HOURS_6_PLUS_1     // 6+1 час
};

/**
 * Тип маршрута
 */
enum class RouteType {
    CITY,              // По городу
    REGION,            // По области
    INTERCITY          // Межгород
};

/**
 * Тип операции приемки/отгрузки
 */
enum class OperationType {
    MECHANIZED,        // Механизированная
    MANUAL,            // Ручная
    PIECE_GOODS        // Штучный товар
};

// ============================================================================
// Вспомогательные функции для работы с типами
// ============================================================================

/**
 * Преобразование строки в перечисление VehicleClass
 */
inline VehicleClass StringToVehicleClass(const String& str) {
    if (str == "CLOSED") return VehicleClass::CLOSED;
    if (str == "OPEN") return VehicleClass::OPEN;
    if (str == "REFRIGERATOR") return VehicleClass::REFRIGERATOR;
    throw std::invalid_argument("Неизвестный класс автомобиля: " + str);
}

/**
 * Преобразование перечисления VehicleClass в строку
 */
inline String VehicleClassToString(VehicleClass vehicleClass) {
    switch (vehicleClass) {
        case VehicleClass::CLOSED: return "CLOSED";
        case VehicleClass::OPEN: return "OPEN";
        case VehicleClass::REFRIGERATOR: return "REFRIGERATOR";
        default: throw std::invalid_argument("Неизвестный класс автомобиля");
    }
}

/**
 * Преобразование строки в перечисление RouteType
 */
inline RouteType StringToRouteType(const String& str) {
    if (str == "CITY") return RouteType::CITY;
    if (str == "REGION") return RouteType::REGION;
    if (str == "INTERCITY") return RouteType::INTERCITY;
    throw std::invalid_argument("Неизвестный тип маршрута: " + str);
}

/**
 * Преобразование перечисления RouteType в строку
 */
inline String RouteTypeToString(RouteType routeType) {
    switch (routeType) {
        case RouteType::CITY: return "CITY";
        case RouteType::REGION: return "REGION";
        case RouteType::INTERCITY: return "INTERCITY";
        default: throw std::invalid_argument("Неизвестный тип маршрута");
    }
}

/**
 * Получение числового значения из ParameterValue
 */
inline std::optional<Double> GetDoubleValue(const ParameterValue& value) {
    if (std::holds_alternative<Double>(value)) {
        return std::get<Double>(value);
    }
    if (std::holds_alternative<Integer>(value)) {
        return static_cast<Double>(std::get<Integer>(value));
    }
    return std::nullopt;
}

/**
 * Получение строкового значения из ParameterValue
 */
inline std::optional<String> GetStringValue(const ParameterValue& value) {
    if (std::holds_alternative<String>(value)) {
        return std::get<String>(value);
    }
    return std::nullopt;
}

/**
 * Получение логического значения из ParameterValue
 */
inline std::optional<Boolean> GetBooleanValue(const ParameterValue& value) {
    if (std::holds_alternative<Boolean>(value)) {
        return std::get<Boolean>(value);
    }
    return std::nullopt;
}

// ============================================================================
// Исключения
// ============================================================================

/**
 * Базовый класс для исключений системы
 */
class TariffSystemException : public std::runtime_error {
public:
    explicit TariffSystemException(const String& message)
        : std::runtime_error(message) {}
};

/**
 * Исключение при работе с базой данных
 */
class DatabaseException : public TariffSystemException {
public:
    explicit DatabaseException(const String& message)
        : TariffSystemException("Ошибка БД: " + message) {}
};

/**
 * Исключение при вычислении выражений
 */
class CalculationException : public TariffSystemException {
public:
    explicit CalculationException(const String& message)
        : TariffSystemException("Ошибка вычисления: " + message) {}
};

/**
 * Исключение при работе с параметрами
 */
class ParameterException : public TariffSystemException {
public:
    explicit ParameterException(const String& message)
        : TariffSystemException("Ошибка параметра: " + message) {}
};

} // namespace TariffSystem
