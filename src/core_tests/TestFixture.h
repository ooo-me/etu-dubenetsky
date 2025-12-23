// ============================================================================
// TestFixture.h
// Описание: Базовый fixture для unit тестов библиотеки core
// ============================================================================

#pragma once

#include <core/TariffService.h>
#include <db/DbApi.h>
#include <db/Database.h>

#include <gtest/gtest.h>

#include <memory>
#include <string>

namespace core::test
{

// Базовый класс для тестов с подготовкой данных
class TariffServiceTestFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Создание подключения к тестовой БД
        db::DatabaseManager::ConnectionParams params;
        params.host = "localhost";
        params.port = "5433"; // PostgreSQL 15
        params.database = "tariff_test"; // Отдельная БД для тестов
        params.user = "postgres";
        params.password = "postgres";

        dbManager_ = std::make_shared<db::DatabaseManager>();
        
        try
        {
            dbManager_->Connect(params);
            
            // Создаём API и сервис
            dbApi_ = std::make_shared<db::DbApi>(dbManager_);
            service_ = std::make_unique<TariffService>(dbApi_);
            
            // Пересоздаём схему БД для каждого теста (изоляция)
            service_->InitializeDatabase();
        }
        catch (const std::exception& e)
        {
            // Если не удалось подключиться, пропускаем тест
            GTEST_SKIP() << "Не удалось подключиться к тестовой БД: " << e.what();
        }
    }

    void TearDown() override
    {
        // Закрываем соединение
        if (dbManager_)
        {
            dbManager_->Disconnect();
        }
    }

    // Вспомогательные методы для создания тестовых данных

    // Создание единицы измерения
    Unit CreateTestUnit(const std::string& code, const std::string& name)
    {
        Unit unit;
        unit.code = code;
        unit.name = name;
        unit.note = "Тестовая единица измерения";
        return service_->CreateUnit(unit);
    }

    // Создание класса услуг
    Class CreateTestClass(const std::string& code, const std::string& name)
    {
        Class cls;
        cls.code = code;
        cls.name = name;
        cls.level = 0;
        cls.note = "Тестовый класс";
        return service_->CreateClass(cls);
    }

    // Создание параметра
    Parameter CreateTestParameter(const std::string& code, const std::string& name, 
                                  int type, std::optional<int> unitId = std::nullopt)
    {
        Parameter param;
        param.code = code;
        param.name = name;
        param.type = type; // 0-число, 1-строка, 2-дата, 3-перечисление
        param.unitId = unitId;
        param.note = "Тестовый параметр";
        return service_->CreateParameter(param);
    }

    // Создание типа услуги
    ServiceType CreateTestServiceType(const std::string& code, const std::string& name, int classId)
    {
        ServiceType serviceType;
        serviceType.code = code;
        serviceType.name = name;
        serviceType.classId = classId;
        serviceType.note = "Тестовый тип услуги";
        return service_->CreateServiceType(serviceType);
    }

    // Добавление параметра к типу услуги
    void AddServiceTypeParameter(int serviceTypeId, int parameterId, bool isRequired = true, 
                                std::optional<double> defaultValue = std::nullopt)
    {
        ServiceTypeParameter param;
        param.parameterId = parameterId;
        param.isRequired = isRequired;
        param.defaultValue = defaultValue;
        service_->AddServiceTypeParameter(serviceTypeId, param);
    }

    // Создание исполнителя
    Executor CreateTestExecutor(const std::string& code, const std::string& name, bool isActive = true)
    {
        Executor executor;
        executor.code = code;
        executor.name = name;
        executor.address = "Тестовый адрес";
        executor.phone = "+7 (123) 456-78-90";
        executor.email = "test@example.com";
        executor.isActive = isActive;
        executor.note = "Тестовый исполнитель";
        return service_->CreateExecutor(executor);
    }

    // Создание тарифа
    Tariff CreateTestTariff(int serviceTypeId, std::optional<int> executorId,
                           const std::string& dateBegin, const std::string& dateEnd,
                           bool isWithVat = true, double vatRate = 20.0)
    {
        Tariff tariff;
        tariff.code = "TARIFF_" + std::to_string(serviceTypeId);
        tariff.name = "Тестовый тариф";
        tariff.serviceTypeId = serviceTypeId;
        tariff.executorId = executorId;
        tariff.dateBegin = dateBegin;
        tariff.dateEnd = dateEnd;
        tariff.isWithVat = isWithVat;
        tariff.vatRate = vatRate;
        tariff.isActive = true;
        tariff.note = "Тестовый тариф";
        return service_->CreateTariff(tariff);
    }

    // Создание ставки тарифа
    TariffRate CreateTestTariffRate(int tariffId, const std::string& code, 
                                   const std::string& name, double value,
                                   std::optional<int> unitId = std::nullopt)
    {
        TariffRate rate;
        rate.code = code;
        rate.name = name;
        rate.value = value;
        rate.unitId = unitId;
        rate.note = "Тестовая ставка";
        return service_->CreateTariffRate(tariffId, rate);
    }

    // Создание заказа
    Order CreateTestOrder(int serviceTypeId, const std::string& orderDate,
                         const std::string& executionDate)
    {
        Order order;
        order.code = "ORDER_" + orderDate;
        order.serviceTypeId = serviceTypeId;
        order.orderDate = orderDate;
        order.executionDate = executionDate;
        order.status = OrderStatus::New;
        order.note = "Тестовый заказ";
        return service_->CreateOrder(order);
    }

    // Установка значения параметра заказа
    void SetOrderParameter(int orderId, int parameterId, std::optional<double> numValue)
    {
        OrderParameterValue param;
        param.parameterId = parameterId;
        param.numValue = numValue;
        service_->SetOrderParameter(orderId, param);
    }

    // Члены класса для доступа в тестах
    std::shared_ptr<db::DatabaseManager> dbManager_;
    std::shared_ptr<db::DbApi> dbApi_;
    std::unique_ptr<TariffService> service_;
};

} // namespace core::test
