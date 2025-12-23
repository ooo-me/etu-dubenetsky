// ============================================================================
// ExecutorTests.cpp
// Описание: Тесты исполнителей
// ============================================================================

#include "TestFixture.h"

namespace core::test
{

class ExecutorTest : public TariffServiceTestFixture
{
};

// Тест создания исполнителя
TEST_F(ExecutorTest, CreateExecutor)
{
    Executor executor;
    executor.code = "TC_FAST";
    executor.name = "ТК Быстрый";
    executor.address = "г. Санкт-Петербург, ул. Тестовая, д. 1";
    executor.phone = "+7 (812) 123-45-67";
    executor.email = "info@tc-fast.ru";
    executor.isActive = true;
    executor.note = "Транспортная компания";

    auto created = service_->CreateExecutor(executor);

    EXPECT_GT(created.id, 0);
    EXPECT_EQ(created.code, executor.code);
    EXPECT_EQ(created.name, executor.name);
    EXPECT_EQ(created.isActive, executor.isActive);
}

// Тест получения всех исполнителей
TEST_F(ExecutorTest, GetAllExecutors)
{
    CreateTestExecutor("TC_FAST", "ТК Быстрый", true);
    CreateTestExecutor("TC_ECON", "ТК Экономный", true);
    CreateTestExecutor("TC_RELIABLE", "ТК Надёжный", true);

    auto executors = service_->GetAllExecutors();

    EXPECT_GE(executors.size(), 3);
}

// Тест фильтрации активных исполнителей
TEST_F(ExecutorTest, FilterActiveExecutors)
{
    CreateTestExecutor("TC_ACTIVE", "ТК Активный", true);
    CreateTestExecutor("TC_INACTIVE", "ТК Неактивный", false);

    auto allExecutors = service_->GetAllExecutors();
    
    // Подсчитываем активных
    auto activeCount = std::count_if(allExecutors.begin(), allExecutors.end(),
        [](const Executor& e) { return e.isActive; });

    EXPECT_GE(activeCount, 1);
}

// Тест обновления исполнителя
TEST_F(ExecutorTest, UpdateExecutor)
{
    auto executor = CreateTestExecutor("TC_TEST", "ТК Тестовый", true);

    executor.name = "ТК Тестовый (обновлённый)";
    executor.isActive = false;
    executor.note = "Обновлённое описание";

    service_->UpdateExecutor(executor);

    auto executors = service_->GetAllExecutors();
    auto found = std::find_if(executors.begin(), executors.end(),
        [&executor](const Executor& e) { return e.id == executor.id; });

    ASSERT_NE(found, executors.end());
    EXPECT_EQ(found->name, "ТК Тестовый (обновлённый)");
    EXPECT_EQ(found->isActive, false);
}

// Тест удаления исполнителя
TEST_F(ExecutorTest, DeleteExecutor)
{
    auto executor = CreateTestExecutor("TC_TEST", "ТК Тестовый", true);

    service_->DeleteExecutor(executor.id);

    auto executors = service_->GetAllExecutors();
    auto found = std::find_if(executors.begin(), executors.end(),
        [&executor](const Executor& e) { return e.id == executor.id; });

    EXPECT_EQ(found, executors.end());
}

} // namespace core::test
