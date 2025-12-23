// ============================================================================
// Заказ
// Язык: C++20
// Описание: Класс для работы с заказами на услуги
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include "Service.hpp"
#include "Tariff.hpp"
#include <map>
#include <memory>
#include <chrono>

namespace TariffSystem::Model {

/**
 * Заказ на услугу
 */
class Order {
public:
    /**
     * Конструктор
     * @param id Уникальный идентификатор
     * @param code Код заказа
     * @param name Наименование заказа
     * @param serviceId ID услуги
     */
    Order(Integer id, String code, String name, Integer serviceId)
        : id_(id)
        , code_(std::move(code))
        , name_(std::move(name))
        , serviceId_(serviceId)
        , status_(OrderStatus::DRAFT)
        , calculatedCost_(0.0)
    {
        auto now = std::chrono::system_clock::now();
        auto days = std::chrono::floor<std::chrono::days>(now);
        orderDate_ = std::chrono::year_month_day{days};
    }
    
    // Конструктор с 2 параметрами для совместимости  
    Order(Integer id, String code)
        : id_(id)
        , code_(std::move(code))
        , name_("")
        , serviceId_(0)
        , status_(OrderStatus::DRAFT)
        , calculatedCost_(0.0)
    {
        auto now = std::chrono::system_clock::now();
        auto days = std::chrono::floor<std::chrono::days>(now);
        orderDate_ = std::chrono::year_month_day{days};
    }

    // Геттеры
    Integer getId() const { return id_; }
    const String& getCode() const { return code_; }
    const String& getName() const { return name_; }
    Integer getServiceId() const { return serviceId_; }
    std::optional<Integer> getTariffId() const { return tariffId_; }
    OrderStatus getStatus() const { return status_; }
    const Date& getOrderDate() const { return orderDate_; }
    Double getCalculatedCost() const { return calculatedCost_; }
    Double getCost() const { return calculatedCost_; }  // Alias для совместимости
    const std::optional<String>& getNote() const { return note_; }
    const std::optional<String>& getCustomerName() const { return customerName_; }

    // Сеттеры
    void setTariffId(Integer tariffId) { tariffId_ = tariffId; }
    void setStatus(OrderStatus status) { status_ = status; }
    void setNote(String note) { note_ = std::move(note); }
    void setCustomerName(String name) { customerName_ = std::move(name); }
    void setCost(Double cost) { setCalculatedCost(cost); }  // Alias для совместимости

    /**
     * Добавление параметра к заказу
     * @param parameterId ID параметра
     * @param value Значение параметра
     */
    void addParameter(Integer parameterId, ParameterValue value) {
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
     * Получение всех параметров заказа
     * @return Карта параметров
     */
    const std::map<Integer, ParameterValue>& getParameters() const {
        return parameters_;
    }

    /**
     * Установка рассчитанной стоимости
     * @param cost Стоимость
     */
    void setCalculatedCost(Double cost) {
        calculatedCost_ = cost;
        if (status_ == OrderStatus::DRAFT) {
            status_ = OrderStatus::CALCULATED;
        }
    }

    /**
     * Подтверждение заказа
     */
    void confirm() {
        if (status_ == OrderStatus::CALCULATED) {
            status_ = OrderStatus::CONFIRMED;
        } else {
            throw TariffSystemException("Заказ должен быть рассчитан перед подтверждением");
        }
    }

    /**
     * Завершение заказа
     */
    void complete() {
        if (status_ == OrderStatus::CONFIRMED) {
            status_ = OrderStatus::COMPLETED;
        } else {
            throw TariffSystemException("Заказ должен быть подтвержден перед завершением");
        }
    }

    /**
     * Отмена заказа
     */
    void cancel() {
        if (status_ != OrderStatus::COMPLETED) {
            status_ = OrderStatus::CANCELLED;
        } else {
            throw TariffSystemException("Невозможно отменить завершенный заказ");
        }
    }

    /**
     * Получение статуса заказа в виде строки
     * @return Строковое представление статуса
     */
    String getStatusString() const {
        switch (status_) {
            case OrderStatus::DRAFT: return "Черновик";
            case OrderStatus::CALCULATED: return "Рассчитан";
            case OrderStatus::CONFIRMED: return "Подтвержден";
            case OrderStatus::COMPLETED: return "Выполнен";
            case OrderStatus::CANCELLED: return "Отменен";
            default: return "Неизвестный";
        }
    }

private:
    Integer id_;                                  // Уникальный идентификатор
    String code_;                                 // Код заказа
    String name_;                                 // Наименование
    Integer serviceId_;                           // ID услуги
    std::optional<Integer> tariffId_;             // ID тарифа
    OrderStatus status_;                          // Статус заказа
    Date orderDate_;                              // Дата заказа
    Double calculatedCost_;                       // Рассчитанная стоимость
    std::optional<String> note_;                  // Примечание
    std::optional<String> customerName_;          // Имя заказчика
    std::map<Integer, ParameterValue> parameters_; // Параметры заказа
};

} // namespace TariffSystem::Model
