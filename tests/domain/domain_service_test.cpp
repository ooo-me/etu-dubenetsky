// ============================================================================
// Тесты для DomainService - полная интеграция C++ и PostgreSQL
// Язык: C++20
// Описание: Комплексные тесты оркестрации бизнес-логики и персистентности
// ============================================================================

#include <gtest/gtest.h>
#include "../../src/core/DomainService.hpp"
#include "../../src/utils/Database.hpp"
#include "../../src/model/Service.hpp"
#include "../../src/model/Tariff.hpp"
#include "../../src/model/Order.hpp"
#include <cstdlib>
#include <memory>

using namespace TariffSystem::Core;
using namespace TariffSystem::Database;
using namespace TariffSystem::Model;

/**
 * Базовый класс для тестов DomainService
 * Настраивает подключение к БД для каждого теста
 */
class DomainServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Получение параметров подключения из переменных окружения
        DatabaseManager::ConnectionParams params;
        params.host = getEnvOrDefault("DB_HOST", "localhost");
        params.port = std::stoi(getEnvOrDefault("DB_PORT", "5432"));
        params.database = getEnvOrDefault("DB_NAME", "tariff_system");
        params.user = getEnvOrDefault("DB_USER", "postgres");
        params.password = getEnvOrDefault("DB_PASSWORD", "postgres");
        
        db = std::make_shared<DatabaseManager>();
        
        try {
            db->connect(params);
            
            // Создание DomainService
            service = std::make_unique<DomainService>(db);
            
            // Очистка тестовых данных
            cleanupTestData();
            
        } catch (const std::exception& e) {
            GTEST_SKIP() << "PostgreSQL недоступен: " << e.what();
        }
    }
    
    void TearDown() override {
        if (service) {
            cleanupTestData();
            service.reset();
        }
        if (db) {
            db->disconnect();
        }
    }
    
    /**
     * Очистка тестовых данных
     */
    void cleanupTestData() {
        try {
            // Удаление тестовых записей (если они помечены специальным префиксом)
            db->execute("DELETE FROM PROD WHERE COD LIKE 'TEST_%'");
            db->execute("DELETE FROM CHEM_CLASS WHERE COD_CHEM LIKE 'TEST_%'");
        } catch (...) {
            // Игнорируем ошибки при очистке
        }
    }
    
    /**
     * Получение переменной окружения с значением по умолчанию
     */
    static String getEnvOrDefault(const char* name, const char* defaultValue) {
        const char* value = std::getenv(name);
        return value ? String(value) : String(defaultValue);
    }
    
    std::shared_ptr<DatabaseManager> db;
    std::unique_ptr<DomainService> service;
};

// ============================================================================
// ТЕСТЫ УПРАВЛЕНИЯ КЛАССИФИКАТОРАМИ
// ============================================================================

TEST_F(DomainServiceTest, CreateClassifier_Success) {
    // Создание корневого классификатора
    auto classifier = service->createClassifier(
        "TEST_SERVICE_CLASS",
        "Тестовый класс услуг",
        1,
        std::nullopt,
        "Для тестирования"
    );
    
    ASSERT_NE(classifier, nullptr);
    EXPECT_GT(classifier->getId(), 0);
    EXPECT_EQ(classifier->getCode(), "TEST_SERVICE_CLASS");
    EXPECT_EQ(classifier->getName(), "Тестовый класс услуг");
    EXPECT_EQ(classifier->getLevel(), 1);
}

TEST_F(DomainServiceTest, CreateClassifier_WithParent) {
    // Создание родителя
    auto parent = service->createClassifier(
        "TEST_PARENT_CLASS", "Родительский класс", 1
    );
    
    // Создание дочернего класса
    auto child = service->createClassifier(
        "TEST_CHILD_CLASS",
        "Дочерний класс",
        2,
        parent->getId()
    );
    
    ASSERT_NE(child, nullptr);
    EXPECT_EQ(child->getLevel(), 2);
    EXPECT_TRUE(child->getParentId().has_value());
    EXPECT_EQ(*child->getParentId(), parent->getId());
}

TEST_F(DomainServiceTest, CreateClassifier_InvalidData) {
    // Пустой код
    EXPECT_THROW(
        service->createClassifier("", "Название", 1),
        DomainServiceException
    );
    
    // Пустое название
    EXPECT_THROW(
        service->createClassifier("CODE", "", 1),
        DomainServiceException
    );
    
    // Невалидный уровень
    EXPECT_THROW(
        service->createClassifier("CODE", "Название", 0),
        DomainServiceException
    );
    
    EXPECT_THROW(
        service->createClassifier("CODE", "Название", 11),
        DomainServiceException
    );
}

TEST_F(DomainServiceTest, GetClassifier_WithCaching) {
    // Создание
    auto created = service->createClassifier(
        "TEST_CACHED_CLASS", "Класс для кэша", 1
    );
    
    // Первое получение (из БД)
    auto retrieved1 = service->getClassifier(created->getId());
    ASSERT_NE(retrieved1, nullptr);
    EXPECT_EQ(retrieved1->getCode(), "TEST_CACHED_CLASS");
    
    // Второе получение (из кэша - должно быть быстрее)
    auto retrieved2 = service->getClassifier(created->getId());
    ASSERT_NE(retrieved2, nullptr);
    
    // Проверка, что это один и тот же объект в памяти (благодаря кэшу)
    EXPECT_EQ(retrieved1.get(), retrieved2.get());
}

// ============================================================================
// REAL WORLD SCENARIO - Полный цикл работы с системой
// ============================================================================

TEST_F(DomainServiceTest, RealWorldScenario_CompleteOrderFlow) {
    // ========================================================================
    // СЦЕНАРИЙ: Полный цикл создания и обработки заказа на грузоперевозку
    // ========================================================================
    
    // Шаг 1: Создание классификатора услуг
    auto serviceClass = service->createClassifier(
        "TEST_RW_CARGO_CLASS",
        "Грузоперевозки",
        1,
        std::nullopt,
        "Класс для грузоперевозок"
    );
    ASSERT_NE(serviceClass, nullptr);
    
    // Шаг 2: Создание услуги
    auto cargoService = service->createService(
        serviceClass->getId(),
        "TEST_RW_CARGO_SRV",
        "Грузоперевозка по городу",
        "Доставка грузов в пределах города"
    );
    ASSERT_NE(cargoService, nullptr);
    
    // Шаг 3: Создание тарифов от разных компаний
    auto tariffA = service->createTariff(
        cargoService->getId(),
        "TEST_RW_TARIFF_A",
        "Тариф Эконом",
        "Транспорт-А",
        "Базовый тариф с низкими ценами"
    );
    
    auto tariffB = service->createTariff(
        cargoService->getId(),
        "TEST_RW_TARIFF_B",
        "Тариф Стандарт",
        "Логистика-Б",
        "Стандартный тариф со средними ценами"
    );
    
    auto tariffC = service->createTariff(
        cargoService->getId(),
        "TEST_RW_TARIFF_C",
        "Тариф Премиум",
        "Экспресс-В",
        "Премиум тариф с быстрой доставкой"
    );
    
    ASSERT_NE(tariffA, nullptr);
    ASSERT_NE(tariffB, nullptr);
    ASSERT_NE(tariffC, nullptr);
    
    // Шаг 4: Создание заказа
    auto order = service->createOrder(
        cargoService->getId(),
        tariffA->getId(),
        "ООО Строй-Инвест",
        "Доставка стройматериалов"
    );
    ASSERT_NE(order, nullptr);
    EXPECT_EQ(order->getStatus(), OrderStatus::DRAFT);
    
    // Шаг 5: Заполнение параметров заказа
    auto result1 = service->setOrderParameter(order->getId(), 1, Double{2.5});
    EXPECT_TRUE(result1.success);
    
    auto result2 = service->setOrderParameter(order->getId(), 2, Double{45.0});
    EXPECT_TRUE(result2.success);
    
    auto result3 = service->setOrderParameter(order->getId(), 3, Double{3.0});
    EXPECT_TRUE(result3.success);
    
    // Шаг 6: Проверка персистентности - данные сохранены в БД
    service->clearCache();  // Очистка кэша
    auto retrievedOrder = service->getOrder(order->getId());
    ASSERT_NE(retrievedOrder, nullptr);
    EXPECT_EQ(retrievedOrder->getStatus(), OrderStatus::DRAFT);
    
    // Шаг 7: Подтверждение заказа
    auto confirmResult = service->confirmOrder(
        order->getId(),
        tariffA->getId()
    );
    
    EXPECT_TRUE(confirmResult.success);
    
    // Шаг 8: Проверка финального состояния
    auto finalOrder = service->getOrder(order->getId());
    ASSERT_NE(finalOrder, nullptr);
    EXPECT_EQ(finalOrder->getStatus(), OrderStatus::CONFIRMED);
    EXPECT_GT(finalOrder->getCost(), 0.0);
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
