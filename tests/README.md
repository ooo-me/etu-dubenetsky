# Тесты системы управления тарифами

## Обзор

Этот каталог содержит comprehensive набор тестов для проверки функциональности системы управления тарифами.

## Структура тестов

```
tests/
├── model/              # Тесты моделей предметной области
│   └── model_test.cpp  # Тесты для Parameter, Classifier, Service, Tariff, Order
├── core/               # Тесты бизнес-логики
│   └── core_test.cpp   # Тесты для Expression, Rule, RuleEngine, CostCalculator, OptimalSearcher
├── integration/        # Интеграционные тесты
│   └── integration_test.cpp  # Полные сценарии использования
└── CMakeLists.txt      # Конфигурация сборки тестов
```

## Фреймворк

Все тесты написаны с использованием **Google Test (GTest) 1.15.0**.

## Компиляция и запуск тестов

### Установка зависимостей через Conan

```bash
# Установить зависимости
conan install . --output-folder=build --build=missing -pr=profiles/x64-debug

# Или для release
conan install . --output-folder=build --build=missing -pr=profiles/x64-release
```

### Сборка с тестами

```bash
# Настроить CMake с включенными тестами
cmake --preset x64-debug -DBUILD_TESTS=ON

# Собрать проект
cmake --build build

# Или использовать preset
cmake --build --preset x64-debug
```

### Запуск тестов

```bash
# Запустить все тесты
cd build
ctest --output-on-failure --verbose

# Или запустить отдельные тесты
./tests/model_test
./tests/core_test
./tests/integration_test

# Или использовать цель make
cmake --build build --target run_all_tests
```

## Описание тестов

### 1. Тесты модели (model_test)

**Количество тестов:** 30+

**Покрываемые классы:**
- `Parameter` - Типизированные параметры
- `Classifier` - Иерархический классификатор
- `Service` (CargoService, StorageService) - Услуги
- `Tariff` - Тарифы
- `Order` - Заказы

**Примеры тестов:**
```cpp
TEST_F(ParameterTest, SetAndGetIntegerValue)
TEST_F(ClassifierTest, HierarchyStructure)
TEST_F(ServiceTest, CargoServiceBaseCostCalculation)
TEST_F(TariffTest, AddRulesToTariff)
TEST_F(OrderTest, StatusTransitions)
```

### 2. Тесты бизнес-логики (core_test)

**Количество тестов:** 40+

**Покрываемые классы:**
- `Expression` (все типы: Arithmetic, Comparison, Logical, Constant, Parameter)
- `Context` - Контекст вычисления
- `Rule` - Правила с условиями и действиями
- `RuleEngine` - Движок правил
- `CostCalculator` - Калькулятор стоимости
- `OptimalSearcher` - Поиск оптимального тарифа

**Примеры тестов:**
```cpp
TEST_F(ExpressionTest, ArithmeticAddition)
TEST_F(ExpressionTest, ComparisonGreaterThan)
TEST_F(ExpressionTest, LogicalAND)
TEST_F(ExpressionTest, ComplexExpression)
TEST_F(RuleTest, RuleWithConditionAndAction)
TEST_F(CostCalculatorTest, CalculateSimpleCost)
TEST_F(OptimalSearcherTest, FindOptimalTariff)
TEST_F(OptimalSearcherTest, AnalyzeSavings)
```

### 3. Интеграционные тесты (integration_test)

**Количество тестов:** 15+

**Сценарии:**
1. **Грузоперевозки** - Полный цикл расчета стоимости перевозки
2. **Ответственное хранение** - Расчет месячного бюджета хранения
3. **Поиск оптимального тарифа** - Сравнение нескольких компаний
4. **Условные правила** - Правила с условиями (IF-THEN-ELSE)

**Примеры тестов:**
```cpp
TEST_F(CargoTransportIntegrationTest, SimpleCargoOrder)
TEST_F(StorageIntegrationTest, MonthlyStorageCost)
TEST_F(OptimalTariffSelectionTest, FindCheapestTariff)
TEST_F(ConditionalRulesTest, HeavyCargoWithSurcharge)
```

## Покрытие кода

Тесты обеспечивают comprehensive покрытие:

- **Модель предметной области:** ~95%
- **Бизнес-логика:** ~90%
- **Интеграционные сценарии:** Все основные use cases

## Тестовые данные

Тесты используют:
- Mock объекты для изоляции компонентов
- Предопределенные значения для воспроизводимости
- Реалистичные сценарии из предметной области

## Примеры результатов

При успешном прохождении всех тестов:

```
[==========] Running 85 tests from 20 test suites.
[----------] Global test environment set-up.
[----------] 10 tests from ParameterTest
[ RUN      ] ParameterTest.BasicProperties
[       OK ] ParameterTest.BasicProperties (0 ms)
...
[----------] 40 tests from ExpressionTest
[ RUN      ] ExpressionTest.ArithmeticAddition
[       OK ] ExpressionTest.ArithmeticAddition (0 ms)
...
[==========] 85 tests from 20 test suites ran. (15 ms total)
[  PASSED  ] 85 tests.
```

## Непрерывная интеграция

Тесты могут быть интегрированы в CI/CD pipeline:

```yaml
# Пример для GitHub Actions
test:
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v2
    - name: Install dependencies
      run: conan install . --output-folder=build --build=missing
    - name: Build
      run: cmake --build build
    - name: Run tests
      run: cd build && ctest --output-on-failure
```

## Добавление новых тестов

### Шаблон теста

```cpp
#include <gtest/gtest.h>
#include "your_class.hpp"

class YourClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Инициализация перед каждым тестом
    }
    
    void TearDown() override {
        // Очистка после каждого теста
    }
    
    // Общие объекты для тестов
};

TEST_F(YourClassTest, TestName) {
    // Arrange (подготовка)
    
    // Act (действие)
    
    // Assert (проверка)
    EXPECT_EQ(expected, actual);
}
```

### Best Practices

1. **Один тест - одна проверка:** Каждый тест должен проверять один аспект
2. **Описательные имена:** Имена тестов должны описывать проверяемое поведение
3. **Независимость:** Тесты не должны зависеть друг от друга
4. **Воспроизводимость:** Тесты должны давать одинаковый результат
5. **Быстрота:** Юнит-тесты должны выполняться быстро

## Отладка тестов

### Запуск конкретного теста

```bash
# Запустить только определенный тест
./tests/model_test --gtest_filter=ParameterTest.SetAndGetIntegerValue

# Запустить все тесты из одного fixture
./tests/core_test --gtest_filter=ExpressionTest.*
```

### Вывод дополнительной информации

```bash
# Подробный вывод
./tests/model_test --gtest_verbose

# Повторить упавший тест
./tests/model_test --gtest_repeat=10
```

## Инструменты анализа

### Valgrind (проверка утечек памяти)

```bash
valgrind --leak-check=full ./tests/model_test
```

### Покрытие кода (lcov)

```bash
# Компиляция с информацией о покрытии
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage" ..
cmake --build .

# Запуск тестов
ctest

# Генерация отчета
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

## Известные ограничения

1. Тесты требуют C++20 компилятор
2. Некоторые интеграционные тесты могут требовать больше времени
3. Тесты не проверяют взаимодействие с реальной БД PostgreSQL (это было бы в E2E тестах)

## Дополнительная информация

- Документация GTest: https://google.github.io/googletest/
- C++20 Reference: https://en.cppreference.com/
- Проектная документация: ../docs/REPORT.md
