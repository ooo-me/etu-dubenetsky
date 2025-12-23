#pragma once

#include "Database.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace db
{

// Result structures for queries
struct UnitOfMeasure
{
    int id;
    std::string code;
    std::string name;
    std::string note;
};

struct EnumInfo
{
    int id;
    std::string code;
    std::string name;
    std::string note;
};

struct EnumValue
{
    int id;
    std::string code;
    std::string name;
    int position;
    std::string note;
};

struct ClassInfo
{
    int id;
    std::string code;
    std::string name;
    std::optional<int> parentId;
    int level;
    std::string note;
};

struct ParameterInfo
{
    int id;
    std::string code;
    std::string name;
    std::optional<int> classId;
    int type;
    std::string typeName;
    std::optional<int> unitId;
    std::string unitName;
    std::string note;
};

struct ServiceTypeInfo
{
    int id;
    std::string code;
    std::string name;
    int classId;
    std::string className;
    std::string note;
};

struct ServiceTypeParamInfo
{
    int parId;
    std::string code;
    std::string name;
    int type;
    bool isRequired;
    std::optional<double> defaultValNum;
    std::string defaultValStr;
    std::optional<double> minVal;
    std::optional<double> maxVal;
    std::string unitName;
};

struct ExecutorInfo
{
    int id;
    std::string code;
    std::string name;
    std::string address;
    std::string phone;
    std::string email;
    bool isActive;
    std::string note;
};

struct TariffInfo
{
    int id;
    std::string code;
    std::string name;
    int serviceTypeId;
    std::string serviceName;
    std::optional<int> executorId;
    std::string executorName;
    std::string dateBegin;
    std::string dateEnd;
    bool isWithVat;
    double vatRate;
    bool isActive;
    std::string note;
};

struct TariffRateInfo
{
    int id;
    std::string code;
    std::string name;
    double value;
    std::optional<int> unitId;
    std::string unitName;
    std::string note;
};

struct OrderInfo
{
    int id;
    std::string code;
    int serviceTypeId;
    std::string serviceName;
    std::string orderDate;
    std::string executionDate;
    int status;
    std::string statusName;
    std::optional<int> executorId;
    std::string executorName;
    std::optional<int> tariffId;
    std::string tariffName;
    std::optional<double> totalCost;
    std::string note;
};

struct OrderParamInfo
{
    int parId;
    std::string code;
    std::string name;
    int type;
    std::optional<double> valNum;
    std::string valStr;
    std::string valDate;
    std::optional<int> enumId;
    std::string enumName;
    std::string unitName;
};

struct CoefficientInfo
{
    int id;
    std::string code;
    std::string name;
    double valueMin;
    double valueMax;
    double valueDefault;
    std::string note;
};

struct OptimalExecutorInfo
{
    int executorId;
    std::string executorName;
    int tariffId;
    std::string tariffName;
    double estimatedCost;
};

struct ValidationResult
{
    bool isValid;
    std::string errorMessage;
};

class DbApi
{
public:
    explicit DbApi(std::shared_ptr<DatabaseManager> db);
    ~DbApi();

    // Initialize database schema
    void InitializeSchema();

    // ==================== Units of Measure ====================
    int CreateUnit(const std::string& code, const std::string& name, const std::string& note = "");
    void UpdateUnit(int id, const std::string& code, const std::string& name, const std::string& note = "");
    void DeleteUnit(int id);
    std::vector<UnitOfMeasure> GetAllUnits();

    // ==================== Enumerations ====================
    int CreateEnum(const std::string& code, const std::string& name, const std::string& note = "");
    void DeleteEnum(int id);
    std::vector<EnumInfo> GetAllEnums();
    
    int CreateEnumValue(int enumId, const std::string& code, const std::string& name, int position, const std::string& note = "");
    void DeleteEnumValue(int id);
    std::vector<EnumValue> GetEnumValues(int enumId);

    // ==================== Classes ====================
    int CreateClass(const std::string& code,
                    const std::string& name,
                    std::optional<int> parentId,
                    const std::string& note = "");
    void UpdateClass(int id, const std::string& code, const std::string& name, const std::string& note = "");
    void DeleteClass(int id);
    std::vector<ClassInfo> GetAllClasses();

    // ==================== Parameters ====================
    int CreateParameter(const std::string& code, const std::string& name, 
                        std::optional<int> classId, int type, 
                        std::optional<int> unitId, const std::string& note = "");
    void UpdateParameter(int id, const std::string& code, const std::string& name,
                         int type, std::optional<int> unitId, const std::string& note = "");
    void DeleteParameter(int id);
    std::vector<ParameterInfo> GetAllParameters();

    // ==================== Service Types ====================
    int CreateServiceType(const std::string& code, const std::string& name, 
                          int classId, const std::string& note = "");
    void UpdateServiceType(int id, const std::string& code, const std::string& name, 
                           const std::string& note = "");
    void DeleteServiceType(int id);
    std::vector<ServiceTypeInfo> GetAllServiceTypes();

    void AddServiceTypeParam(int serviceTypeId, int parId, bool isRequired,
                             std::optional<double> defaultNum, const std::string& defaultStr,
                             std::optional<double> minVal, std::optional<double> maxVal);
    void RemoveServiceTypeParam(int serviceTypeId, int parId);
    std::vector<ServiceTypeParamInfo> GetServiceTypeParams(int serviceTypeId);

    // ==================== Executors ====================
    int CreateExecutor(const std::string& code, const std::string& name,
                       const std::string& address = "", const std::string& phone = "",
                       const std::string& email = "", bool isActive = true,
                       const std::string& note = "");
    void UpdateExecutor(int id, const std::string& code, const std::string& name,
                        const std::string& address, const std::string& phone,
                        const std::string& email, bool isActive, const std::string& note = "");
    void DeleteExecutor(int id);
    std::vector<ExecutorInfo> GetAllExecutors();

    // ==================== Tariffs ====================
    int CreateTariff(int serviceTypeId, const std::string& code, const std::string& name,
                     std::optional<int> executorId = std::nullopt,
                     const std::string& dateBegin = "", const std::string& dateEnd = "",
                     bool isWithVat = true, double vatRate = 20.0,
                     bool isActive = true, const std::string& note = "");
    void UpdateTariff(int id, const std::string& code, const std::string& name,
                      std::optional<int> executorId,
                      const std::string& dateBegin, const std::string& dateEnd,
                      bool isWithVat, double vatRate, bool isActive,
                      const std::string& note = "");
    void DeleteTariff(int id);
    std::vector<TariffInfo> GetAllTariffs();

    int CreateTariffRate(int tariffId, const std::string& code, const std::string& name,
                         double value, std::optional<int> unitId = std::nullopt,
                         const std::string& note = "");
    void UpdateTariffRate(int id, const std::string& code, const std::string& name,
                          double value, std::optional<int> unitId, const std::string& note = "");
    void DeleteTariffRate(int id);
    std::vector<TariffRateInfo> GetTariffRates(int tariffId);

    void AddTariffCoefficient(int tariffId, int coeffId, double value);
    void RemoveTariffCoefficient(int tariffId, int coeffId);

    // ==================== Orders ====================
    int CreateOrder(const std::string& code, int serviceTypeId,
                    const std::string& orderDate = "", const std::string& executionDate = "",
                    int status = 0, std::optional<int> executorId = std::nullopt,
                    std::optional<int> tariffId = std::nullopt, const std::string& note = "");
    void UpdateOrder(int id, const std::string& code,
                     const std::string& executionDate, int status,
                     std::optional<int> executorId, std::optional<int> tariffId,
                     std::optional<double> totalCost, const std::string& note = "");
    void DeleteOrder(int id);
    std::vector<OrderInfo> GetAllOrders();

    void SetOrderParam(int orderId, int parId,
                       std::optional<double> valNum, const std::string& valStr = "",
                       const std::string& valDate = "", std::optional<int> enumId = std::nullopt);
    void RemoveOrderParam(int orderId, int parId);
    std::vector<OrderParamInfo> GetOrderParams(int orderId);

    // ==================== Coefficients ====================
    int CreateCoefficient(const std::string& code, const std::string& name,
                          double valueMin, double valueMax, double valueDefault,
                          const std::string& note = "");
    void UpdateCoefficient(int id, const std::string& code, const std::string& name,
                           double valueMin, double valueMax, double valueDefault,
                           const std::string& note = "");
    void DeleteCoefficient(int id);
    std::vector<CoefficientInfo> GetAllCoefficients();

    // ==================== Functions ====================
    int CreateFunction(const std::string& code,
                       const std::string& name,
                       int type,
                       const std::string& operation = "",
                       const std::string& note = "");
    void UpdateFunction(int id, const std::string& code, const std::string& name,
                        int type, const std::string& operation, const std::string& note = "");
    void DeleteFunction(int id);

    int AddArgument(int functionId,
                    int argNumber,
                    std::optional<int> classArg,
                    const std::string& name = "",
                    const std::string& note = "");

    int CreateObject(int classId, const std::string& code, const std::string& name, const std::string& note = "");
    void UpdateObject(int id, const std::string& code, const std::string& name, const std::string& note = "");
    void DeleteObject(int id);

    void UpdateRoleValue(int functionId, int objectId, std::optional<double> numValue = std::nullopt);

    // ==================== Calculations ====================
    double CalculateValue(int functionId, int objectId, std::optional<int> tariffId = std::nullopt);
    double CalculateOrderCost(int orderId, std::optional<int> tariffId = std::nullopt);
    double CalculateOrderItemCost(int orderItemId, int tariffId);

    ValidationResult ValidateOrder(int orderId);

    std::vector<OptimalExecutorInfo> FindOptimalExecutor(int serviceTypeId, const std::string& targetDate = "");
    std::vector<OptimalExecutorInfo> FindOptimalTariff(int orderId);

private:
    std::shared_ptr<DatabaseManager> db_;
    
    void ExecuteSchemaFile(const std::string& filename);
};

} // namespace db

