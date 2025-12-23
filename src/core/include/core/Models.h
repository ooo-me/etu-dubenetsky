#pragma once

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace core
{

// Единица измерения
struct Unit
{
    int id = 0;
    std::string code;
    std::string name;
    std::string note;
};

// Перечисление
struct Enumeration
{
    int id = 0;
    std::string code;
    std::string name;
    std::string note;
};

// Значение перечисления
struct EnumValue
{
    int id = 0;
    int enumId = 0;
    std::string code;
    std::string name;
    int position = 0;
    std::string note;
};

// Класс в классификаторе
struct Class
{
    int id = 0;
    std::string code;
    std::string name;
    std::optional<int> parentId;
    int level = 0;
    std::string note;
};

// Параметр
struct Parameter
{
    int id = 0;
    std::string code;
    std::string name;
    std::optional<int> classId;
    int type = 0;  // 0-число, 1-строка, 2-дата, 3-перечисление
    std::optional<int> unitId;
    std::string unitName;
    std::string note;
    
    static std::string TypeName(int type)
    {
        switch (type)
        {
            case 0: return "Число";
            case 1: return "Строка";
            case 2: return "Дата";
            case 3: return "Перечисление";
            default: return "Неизвестно";
        }
    }
};

// Параметр типа услуги
struct ServiceTypeParameter
{
    int parameterId = 0;
    std::string code;
    std::string name;
    int type = 0;
    bool isRequired = false;
    std::optional<double> defaultValue;
    std::string defaultValueStr;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::string unitName;
};

// Тип услуги
struct ServiceType
{
    int id = 0;
    std::string code;
    std::string name;
    int classId = 0;
    std::string className;
    std::string note;
    std::vector<ServiceTypeParameter> parameters;
};

// Исполнитель
struct Executor
{
    int id = 0;
    std::string code;
    std::string name;
    std::string address;
    std::string phone;
    std::string email;
    bool isActive = true;
    std::string note;
};

// Ставка тарифа
struct TariffRate
{
    int id = 0;
    int tariffId = 0;
    std::string code;
    std::string name;
    double value = 0.0;
    std::optional<int> unitId;
    std::string unitName;
    std::string note;
};

// Тариф
struct Tariff
{
    int id = 0;
    std::string code;
    std::string name;
    int serviceTypeId = 0;
    std::string serviceName;
    std::optional<int> executorId;
    std::string executorName;
    std::string dateBegin;
    std::string dateEnd;
    bool isWithVat = true;
    double vatRate = 20.0;
    bool isActive = true;
    std::string note;
    std::vector<TariffRate> rates;
};

// Значение параметра заказа
struct OrderParameterValue
{
    int parameterId = 0;
    std::string code;
    std::string name;
    int type = 0;
    std::optional<double> numValue;
    std::string strValue;
    std::string dateValue;
    std::optional<int> enumId;
    std::string enumName;
    std::string unitName;
};

// Статус заказа
enum class OrderStatus
{
    New = 0,
    InProgress = 1,
    Completed = 2,
    Cancelled = 3
};

inline std::string OrderStatusName(OrderStatus status)
{
    switch (status)
    {
        case OrderStatus::New: return "Новый";
        case OrderStatus::InProgress: return "В работе";
        case OrderStatus::Completed: return "Выполнен";
        case OrderStatus::Cancelled: return "Отменен";
        default: return "Неизвестно";
    }
}

// Заказ
struct Order
{
    int id = 0;
    std::string code;
    int serviceTypeId = 0;
    std::string serviceName;
    std::string orderDate;
    std::string executionDate;
    OrderStatus status = OrderStatus::New;
    std::optional<int> executorId;
    std::string executorName;
    std::optional<int> tariffId;
    std::string tariffName;
    std::optional<double> totalCost;
    std::string note;
    std::vector<OrderParameterValue> parameters;
};

// Коэффициент
struct Coefficient
{
    int id = 0;
    std::string code;
    std::string name;
    double valueMin = 1.0;
    double valueMax = 1.0;
    double valueDefault = 1.0;
    std::string note;
};

// Результат поиска оптимального исполнителя
struct OptimalExecutor
{
    int executorId = 0;
    std::string executorName;
    int tariffId = 0;
    std::string tariffName;
    double estimatedCost = 0.0;
};

// Результат валидации
struct ValidationResult
{
    bool isValid = false;
    std::string errorMessage;
};

} // namespace core

