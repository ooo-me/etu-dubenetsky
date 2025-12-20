// ============================================================================
// Услуга
// Язык: C++20
// Описание: Базовый класс для услуг и специализированные классы
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include "Parameter.hpp"
#include "Classifier.hpp"
#include <vector>
#include <memory>
#include <map>

namespace TariffSystem::Model {

/**
 * Базовый класс услуги
 */
class Service {
public:
    /**
     * Конструктор
     * @param id Уникальный идентификатор
     * @param code Код услуги
     * @param name Наименование услуги
     * @param classId ID класса услуги
     */
    Service(Integer id, String code, String name, Integer classId)
        : id_(id)
        , code_(std::move(code))
        , name_(std::move(name))
        , classId_(classId)
    {}

    virtual ~Service() = default;

    // Геттеры
    Integer getId() const { return id_; }
    const String& getCode() const { return code_; }
    const String& getName() const { return name_; }
    Integer getClassId() const { return classId_; }
    const std::optional<String>& getDescription() const { return description_; }
    const std::optional<String>& getNote() const { return note_; }

    // Сеттеры
    void setDescription(String description) { description_ = std::move(description); }
    void setNote(String note) { note_ = std::move(note); }

    /**
     * Добавление параметра к услуге
     * @param parameter Параметр
     */
    void addParameter(std::shared_ptr<Parameter> parameter) {
        parameters_.push_back(parameter);
        parameterMap_[parameter->getId()] = parameter;
    }

    /**
     * Получение всех параметров услуги
     * @return Вектор параметров
     */
    const std::vector<std::shared_ptr<Parameter>>& getParameters() const {
        return parameters_;
    }

    /**
     * Получение параметра по ID
     * @param parameterId ID параметра
     * @return Параметр или nullptr
     */
    std::shared_ptr<Parameter> getParameter(Integer parameterId) const {
        auto it = parameterMap_.find(parameterId);
        if (it != parameterMap_.end()) {
            return it->second;
        }
        return nullptr;
    }

    /**
     * Расчет базовой стоимости услуги
     * @return Базовая стоимость
     */
    virtual Double calculateBaseCost() const = 0;

protected:
    Integer id_;                                              // Уникальный идентификатор
    String code_;                                             // Код услуги
    String name_;                                             // Наименование
    Integer classId_;                                         // ID класса услуги
    std::optional<String> description_;                       // Описание
    std::optional<String> note_;                              // Примечание
    std::vector<std::shared_ptr<Parameter>> parameters_;      // Параметры услуги
    std::map<Integer, std::shared_ptr<Parameter>> parameterMap_;  // Карта параметров
};

/**
 * Услуга грузоперевозки
 */
class CargoService : public Service {
public:
    using Service::Service;

    /**
     * Установка веса груза
     * @param weight Вес в тоннах
     */
    void setCargoWeight(Double weight) { cargoWeight_ = weight; }

    /**
     * Установка объема груза
     * @param volume Объем в м³
     */
    void setCargoVolume(Double volume) { cargoVolume_ = volume; }

    /**
     * Установка количества паллет
     * @param count Количество паллет
     */
    void setPalletCount(Integer count) { palletCount_ = count; }

    /**
     * Установка класса автомобиля
     * @param vehicleClass Класс автомобиля
     */
    void setVehicleClass(VehicleClass vehicleClass) { vehicleClass_ = vehicleClass; }

    /**
     * Установка временного интервала
     * @param interval Временной интервал
     */
    void setTimeInterval(TimeInterval interval) { timeInterval_ = interval; }

    /**
     * Установка типа маршрута
     * @param routeType Тип маршрута
     */
    void setRouteType(RouteType routeType) { routeType_ = routeType; }

    // Геттеры
    std::optional<Double> getCargoWeight() const { return cargoWeight_; }
    std::optional<Double> getCargoVolume() const { return cargoVolume_; }
    std::optional<Integer> getPalletCount() const { return palletCount_; }
    std::optional<VehicleClass> getVehicleClass() const { return vehicleClass_; }
    std::optional<TimeInterval> getTimeInterval() const { return timeInterval_; }
    std::optional<RouteType> getRouteType() const { return routeType_; }

    /**
     * Расчет базовой стоимости грузоперевозки
     * @return Базовая стоимость
     */
    Double calculateBaseCost() const override {
        // Базовая реализация - расчет на основе веса и объема
        Double cost = 0.0;
        
        if (cargoWeight_) {
            cost += *cargoWeight_ * 1000.0; // 1000 руб за тонну
        }
        
        if (cargoVolume_) {
            cost += *cargoVolume_ * 500.0; // 500 руб за м³
        }

        return cost;
    }

private:
    std::optional<Double> cargoWeight_;              // Вес груза (т)
    std::optional<Double> cargoVolume_;              // Объем груза (м³)
    std::optional<Integer> palletCount_;             // Количество паллет
    std::optional<VehicleClass> vehicleClass_;       // Класс автомобиля
    std::optional<TimeInterval> timeInterval_;       // Временной интервал
    std::optional<RouteType> routeType_;             // Тип маршрута
};

/**
 * Услуга ответственного хранения
 */
class StorageService : public Service {
public:
    using Service::Service;

    /**
     * Установка среднего объема паллето-мест в день
     * @param avgPlaces Средний объем
     */
    void setAvgPalletPlaces(Double avgPlaces) { avgPalletPlaces_ = avgPlaces; }

    /**
     * Установка модели паллеты
     * @param model Модель паллеты
     */
    void setPalletModel(String model) { palletModel_ = std::move(model); }

    /**
     * Установка оборачиваемости
     * @param turnover Оборачиваемость
     */
    void setTurnover(Double turnover) { turnover_ = turnover; }

    /**
     * Установка типа операции
     * @param operationType Тип операции
     */
    void setOperationType(OperationType operationType) { operationType_ = operationType; }

    /**
     * Добавление дополнительной услуги
     * @param service Название услуги
     */
    void addAdditionalService(String service) {
        additionalServices_.push_back(std::move(service));
    }

    // Геттеры
    std::optional<Double> getAvgPalletPlaces() const { return avgPalletPlaces_; }
    const std::optional<String>& getPalletModel() const { return palletModel_; }
    std::optional<Double> getTurnover() const { return turnover_; }
    std::optional<OperationType> getOperationType() const { return operationType_; }
    const std::vector<String>& getAdditionalServices() const { return additionalServices_; }

    /**
     * Расчет базовой стоимости хранения
     * @return Базовая стоимость
     */
    Double calculateBaseCost() const override {
        // Базовая реализация - расчет на основе паллето-мест
        Double cost = 0.0;
        
        if (avgPalletPlaces_) {
            cost += *avgPalletPlaces_ * 100.0; // 100 руб за паллето-место в день
        }

        return cost;
    }

    /**
     * Расчет месячного бюджета
     * @return Месячный бюджет
     */
    Double calculateMonthlyBudget() const {
        return calculateBaseCost() * 30.0; // 30 дней в месяце
    }

private:
    std::optional<Double> avgPalletPlaces_;          // Средний объем паллето-мест
    std::optional<String> palletModel_;              // Модель паллеты
    std::optional<Double> turnover_;                 // Оборачиваемость
    std::optional<OperationType> operationType_;     // Тип операции
    std::vector<String> additionalServices_;         // Дополнительные услуги
};

} // namespace TariffSystem::Model
