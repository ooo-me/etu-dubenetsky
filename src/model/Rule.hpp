// ============================================================================
// Правило
// Язык: C++20
// Описание: Классы для работы с правилами и выражениями
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include <memory>
#include <vector>
#include <map>

namespace TariffSystem::Model {

// Предварительное объявление
class Expression;
class Context;

/**
 * Правило
 * 
 * Представляет правило расчета с условием и действием
 */
class Rule {
public:
    /**
     * Конструктор
     * @param id Уникальный идентификатор
     * @param code Код правила
     * @param name Наименование правила
     */
    Rule(Integer id, String code, String name)
        : id_(id)
        , code_(std::move(code))
        , name_(std::move(name))
        , priority_(0)
    {}

    // Геттеры
    Integer getId() const { return id_; }
    const String& getCode() const { return code_; }
    const String& getName() const { return name_; }
    Integer getPriority() const { return priority_; }
    const std::optional<String>& getNote() const { return note_; }

    // Сеттеры
    void setPriority(Integer priority) { priority_ = priority; }
    void setNote(String note) { note_ = std::move(note); }
    void setCondition(std::shared_ptr<Expression> condition) { 
        condition_ = std::move(condition); 
    }
    void setAction(std::shared_ptr<Expression> action) { 
        action_ = std::move(action); 
    }

    /**
     * Получение условия правила
     * @return Выражение-условие
     */
    std::shared_ptr<Expression> getCondition() const { 
        return condition_; 
    }

    /**
     * Получение действия правила
     * @return Выражение-действие
     */
    std::shared_ptr<Expression> getAction() const { 
        return action_; 
    }

    /**
     * Вычисление правила
     * @param context Контекст вычисления
     * @return Результат вычисления
     */
    ParameterValue evaluate(const Context& context) const;

private:
    Integer id_;                              // Уникальный идентификатор
    String code_;                             // Код правила
    String name_;                             // Наименование
    Integer priority_;                        // Приоритет выполнения
    std::optional<String> note_;              // Примечание
    std::shared_ptr<Expression> condition_;   // Условие
    std::shared_ptr<Expression> action_;      // Действие
};

/**
 * Базовый класс для выражений
 */
class Expression {
public:
    virtual ~Expression() = default;

    /**
     * Вычисление выражения
     * @param context Контекст вычисления
     * @return Результат вычисления
     */
    virtual ParameterValue evaluate(const Context& context) const = 0;

    /**
     * Преобразование в строку
     * @return Строковое представление выражения
     */
    virtual String toString() const = 0;

    /**
     * Получение типа выражения
     * @return Тип выражения
     */
    virtual FunctionType getType() const = 0;
};

/**
 * Контекст вычисления
 * 
 * Содержит значения параметров и констант для вычисления выражений
 */
class Context {
public:
    /**
     * Установка значения параметра
     * @param parameterId ID параметра
     * @param value Значение параметра
     */
    void setParameter(Integer parameterId, ParameterValue value) {
        parameters_[parameterId] = std::move(value);
    }

    /**
     * Получение значения параметра
     * @param parameterId ID параметра
     * @return Значение параметра или std::nullopt
     */
    std::optional<ParameterValue> getParameter(Integer parameterId) const {
        auto it = parameters_.find(parameterId);
        if (it != parameters_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    /**
     * Установка значения константы
     * @param name Имя константы
     * @param value Значение константы
     */
    void setConstant(const String& name, ParameterValue value) {
        constants_[name] = std::move(value);
    }

    /**
     * Получение значения константы
     * @param name Имя константы
     * @return Значение константы или std::nullopt
     */
    std::optional<ParameterValue> getConstant(const String& name) const {
        auto it = constants_.find(name);
        if (it != constants_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

private:
    std::map<Integer, ParameterValue> parameters_;  // Параметры
    std::map<String, ParameterValue> constants_;    // Константы
};

/**
 * Константное выражение
 */
class ConstantExpression : public Expression {
public:
    explicit ConstantExpression(ParameterValue value)
        : value_(std::move(value)) {}

    ParameterValue evaluate(const Context& context) const override {
        return value_;
    }

    String toString() const override {
        if (auto val = GetDoubleValue(value_)) {
            return std::to_string(*val);
        }
        if (auto val = GetStringValue(value_)) {
            return "\"" + *val + "\"";
        }
        return "null";
    }

    FunctionType getType() const override {
        return FunctionType::ARITHMETIC;
    }

private:
    ParameterValue value_;
};

/**
 * Выражение-параметр
 */
class ParameterExpression : public Expression {
public:
    explicit ParameterExpression(Integer parameterId)
        : parameterId_(parameterId) {}

    ParameterValue evaluate(const Context& context) const override {
        auto value = context.getParameter(parameterId_);
        if (!value) {
            throw CalculationException("Параметр " + std::to_string(parameterId_) + " не найден в контексте");
        }
        return *value;
    }

    String toString() const override {
        return "param[" + std::to_string(parameterId_) + "]";
    }

    FunctionType getType() const override {
        return FunctionType::ARITHMETIC;
    }

private:
    Integer parameterId_;
};

/**
 * Арифметическое выражение
 */
class ArithmeticExpression : public Expression {
public:
    ArithmeticExpression(ArithmeticOperator op, 
                        std::shared_ptr<Expression> left,
                        std::shared_ptr<Expression> right)
        : operator_(op)
        , left_(std::move(left))
        , right_(std::move(right))
    {}

    ParameterValue evaluate(const Context& context) const override {
        auto leftVal = GetDoubleValue(left_->evaluate(context));
        auto rightVal = GetDoubleValue(right_->evaluate(context));

        if (!leftVal || !rightVal) {
            throw CalculationException("Некорректные аргументы арифметической операции");
        }

        switch (operator_) {
            case ArithmeticOperator::ADD:
                return *leftVal + *rightVal;
            case ArithmeticOperator::SUBTRACT:
                return *leftVal - *rightVal;
            case ArithmeticOperator::MULTIPLY:
                return *leftVal * *rightVal;
            case ArithmeticOperator::DIVIDE:
                if (*rightVal == 0.0) {
                    throw CalculationException("Деление на ноль");
                }
                return *leftVal / *rightVal;
            default:
                throw CalculationException("Неизвестная арифметическая операция");
        }
    }

    String toString() const override {
        String opStr;
        switch (operator_) {
            case ArithmeticOperator::ADD: opStr = "+"; break;
            case ArithmeticOperator::SUBTRACT: opStr = "-"; break;
            case ArithmeticOperator::MULTIPLY: opStr = "*"; break;
            case ArithmeticOperator::DIVIDE: opStr = "/"; break;
        }
        return "(" + left_->toString() + " " + opStr + " " + right_->toString() + ")";
    }

    FunctionType getType() const override {
        return FunctionType::ARITHMETIC;
    }

private:
    ArithmeticOperator operator_;
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};

/**
 * Выражение сравнения (предикат)
 */
class ComparisonExpression : public Expression {
public:
    ComparisonExpression(ComparisonOperator op,
                        std::shared_ptr<Expression> left,
                        std::shared_ptr<Expression> right)
        : operator_(op)
        , left_(std::move(left))
        , right_(std::move(right))
    {}

    ParameterValue evaluate(const Context& context) const override {
        auto leftVal = GetDoubleValue(left_->evaluate(context));
        auto rightVal = GetDoubleValue(right_->evaluate(context));

        if (!leftVal || !rightVal) {
            throw CalculationException("Некорректные аргументы операции сравнения");
        }

        bool result;
        switch (operator_) {
            case ComparisonOperator::LESS_THAN:
                result = *leftVal < *rightVal;
                break;
            case ComparisonOperator::LESS_EQUAL:
                result = *leftVal <= *rightVal;
                break;
            case ComparisonOperator::EQUAL:
                result = *leftVal == *rightVal;
                break;
            case ComparisonOperator::GREATER_EQUAL:
                result = *leftVal >= *rightVal;
                break;
            case ComparisonOperator::GREATER_THAN:
                result = *leftVal > *rightVal;
                break;
            case ComparisonOperator::NOT_EQUAL:
                result = *leftVal != *rightVal;
                break;
            default:
                throw CalculationException("Неизвестная операция сравнения");
        }

        return result;
    }

    String toString() const override {
        String opStr;
        switch (operator_) {
            case ComparisonOperator::LESS_THAN: opStr = "<"; break;
            case ComparisonOperator::LESS_EQUAL: opStr = "<="; break;
            case ComparisonOperator::EQUAL: opStr = "="; break;
            case ComparisonOperator::GREATER_EQUAL: opStr = ">="; break;
            case ComparisonOperator::GREATER_THAN: opStr = ">"; break;
            case ComparisonOperator::NOT_EQUAL: opStr = "<>"; break;
        }
        return "(" + left_->toString() + " " + opStr + " " + right_->toString() + ")";
    }

    FunctionType getType() const override {
        return FunctionType::PREDICATE;
    }

private:
    ComparisonOperator operator_;
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};

/**
 * Логическое выражение
 */
class LogicalExpression : public Expression {
public:
    LogicalExpression(LogicalOperator op, std::vector<std::shared_ptr<Expression>> operands)
        : operator_(op)
        , operands_(std::move(operands))
    {}

    ParameterValue evaluate(const Context& context) const override {
        if (operator_ == LogicalOperator::NOT) {
            if (operands_.size() != 1) {
                throw CalculationException("Операция NOT требует один операнд");
            }
            auto val = GetBooleanValue(operands_[0]->evaluate(context));
            if (!val) {
                throw CalculationException("Некорректный аргумент логической операции NOT");
            }
            return !(*val);
        }

        bool result = (operator_ == LogicalOperator::AND);
        for (const auto& operand : operands_) {
            auto val = GetBooleanValue(operand->evaluate(context));
            if (!val) {
                throw CalculationException("Некорректный аргумент логической операции");
            }
            
            if (operator_ == LogicalOperator::AND) {
                result = result && *val;
                if (!result) break; // Короткое вычисление
            } else { // OR
                result = result || *val;
                if (result) break; // Короткое вычисление
            }
        }

        return result;
    }

    String toString() const override {
        String opStr;
        switch (operator_) {
            case LogicalOperator::AND: opStr = " AND "; break;
            case LogicalOperator::OR: opStr = " OR "; break;
            case LogicalOperator::NOT: return "NOT " + operands_[0]->toString();
        }

        String result = "(";
        for (size_t i = 0; i < operands_.size(); ++i) {
            if (i > 0) result += opStr;
            result += operands_[i]->toString();
        }
        result += ")";
        return result;
    }

    FunctionType getType() const override {
        return FunctionType::LOGICAL;
    }

private:
    LogicalOperator operator_;
    std::vector<std::shared_ptr<Expression>> operands_;
};

// Реализация метода evaluate для Rule
inline ParameterValue Rule::evaluate(const Context& context) const {
    // Если есть условие, проверяем его
    if (condition_) {
        auto conditionResult = GetBooleanValue(condition_->evaluate(context));
        if (!conditionResult || !(*conditionResult)) {
            // Условие не выполнено
            return std::monostate{};
        }
    }

    // Выполняем действие
    if (action_) {
        return action_->evaluate(context);
    }

    return std::monostate{};
}

} // namespace TariffSystem::Model
