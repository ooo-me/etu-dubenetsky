# Система работы с тарифами на услуги

## Описание проекта

Информационная система для управления тарифами на услуги с поддержкой правил и ограничений. Система обеспечивает:

- Ведение справочника услуг различных типов
- Управление тарифами на услуги
- Формирование заказов с расчетом стоимости
- Поиск оптимального исполнителя по критерию стоимости

## Предметная область

Система поддерживает два варианта тарифов:

### 1. Грузоперевозки по городу
- Грузоподъемность, объем груза, количество паллет
- Класс автомобиля (закрытый/открытый/рефрижератор)
- Временной интервал и вариант маршрута
- Расценки: стоимость часа и километра

### 2. Ответственное хранение
- Средний объем паллето-мест в день
- Модель паллеты и оборачиваемость
- Операции приемки/отгрузки
- Дополнительные услуги и коэффициенты

## Структура проекта

```
.
├── docs/                   # Документация
│   ├── diagrams/          # UML и ER диаграммы (PlantUML, Mermaid)
│   ├── REPORT.md          # Основной отчет курсовой работы
│   └── ARCHITECTURE.md    # Архитектура и диаграмма зависимостей
├── database/              # База данных PostgreSQL
│   ├── schema/            # DDL скрипты (16 таблиц)
│   ├── procedures/        # SQL процедуры (22 процедуры)
│   ├── test-data/         # Тестовые данные
│   └── README.md          # Руководство по настройке БД
├── src/                   # Исходный код C++20
│   ├── model/             # TariffModel: модель предметной области
│   ├── core/              # TariffCore: бизнес-логика
│   ├── utils/             # TariffUtils: утилиты и работа с БД
│   ├── gui/               # TariffGUI: Qt 6 интерфейс
│   ├── main.cpp           # Точка входа приложения
│   └── README.md          # Документация структуры кода
└── tests/                 # Тесты (100+ тестов на GTest)
    ├── model/             # Тесты модели
    ├── core/              # Тесты бизнес-логики
    ├── database/          # Тесты SQL процедур
    ├── integration/       # Интеграционные тесты
    └── README.md          # Руководство по тестированию
```

## Технологии

- **СУБД:** PostgreSQL 15+
- **Язык:** C++20 + STL
- **GUI:** Qt 6.8.3
- **Сборка:** CMake 3.16+ + Conan
- **Тестирование:** Google Test 1.15.0
- **Диаграммы:** PlantUML, Mermaid

## Архитектура

Проект организован в виде модульной архитектуры с четкими зависимостями:

### Библиотеки (STATIC)

1. **TariffModel** - модель предметной области (Classifier, Service, Tariff, Order)
2. **TariffCore** - бизнес-логика (RuleEngine, CostCalculator, OptimalSearcher)
3. **TariffUtils** - утилиты и БД (DatabaseManager, Repository pattern)
4. **TariffGUI** - GUI компоненты (MainWindow)
5. **TariffSystemCore** - интерфейсная библиотека (объединяет Model + Core + Utils)

### Исполняемые файлы

- **TariffSystem** - главное GUI приложение
- **model_test, core_test, database_test, integration_test** - тесты

Подробнее см. `docs/ARCHITECTURE.md`

## Требования

- PostgreSQL 15+ (для работы с БД)
- Qt 6.8.3 (для GUI)
- CMake 3.16+ (для сборки)
- Conan 2.0+ (для управления зависимостями)
- C++20 совместимый компилятор (MSVC 2019+, GCC 10+, Clang 12+)
- Google Test 1.15.0 (устанавливается через Conan)

## Сборка

### 1. Установка зависимостей через Conan

```bash
# Установить зависимости (Qt6, GTest, PostgreSQL, Boost, nlohmann_json)
conan install . --output-folder=build --build=missing -pr=profiles/x64-debug
```

### 2. Конфигурация CMake

```bash
# Использовать preset для автоматической конфигурации
cmake --preset x64-debug
```

### 3. Сборка проекта

```bash
# Собрать все библиотеки и приложение
cmake --build build

# Или для Release
cmake --preset x64-release
cmake --build build --config Release
```

### Структура сборки

После сборки будут созданы:
- `build/libTariffModel.a` - библиотека модели
- `build/libTariffCore.a` - библиотека бизнес-логики
- `build/libTariffUtils.a` - библиотека утилит
- `build/libTariffGUI.a` - библиотека GUI
- `build/TariffSystem` (или `TariffSystem.exe`) - главное приложение
- `build/tests/*_test` - исполняемые файлы тестов

## Использование

### Создание схемы БД

```bash
# Создать базу данных
psql -U postgres -c "CREATE DATABASE tariff_system ENCODING 'UTF8';"

# Создать таблицы
psql -U postgres -d tariff_system -f database/schema/01_tables.sql
psql -U postgres -d tariff_system -f database/schema/02_indexes.sql

# Создать процедуры
psql -U postgres -d tariff_system -f database/procedures/constructor/constructor.sql
psql -U postgres -d tariff_system -f database/procedures/calculator/calculator.sql
psql -U postgres -d tariff_system -f database/procedures/utils/utils.sql
```

### Загрузка тестовых данных

```bash
psql -U postgres -d tariff_system -f database/test-data/01_classifiers.sql
psql -U postgres -d tariff_system -f database/test-data/02_tariffs.sql
```

### Запуск приложения

```bash
# Linux/macOS
./build/TariffSystem

# Windows
.\build\Debug\TariffSystem.exe
# или для Release
.\build\Release\TariffSystem.exe
```

При запуске приложение отобразит главное окно с меню:
- **Файл** → Подключиться к БД
- **Справочники** → Услуги, Тарифы
- **Заказы** → Новый заказ, Поиск оптимального тарифа

### Запуск тестов

```bash
# Настроить переменные окружения для database_test
export DB_HOST=localhost
export DB_PORT=5432
export DB_NAME=tariff_system
export DB_USER=postgres
export DB_PASSWORD=your_password

# Запустить все тесты
cd build
ctest --output-on-failure --verbose

# Или запустить отдельные тесты
./tests/model_test
./tests/core_test
./tests/database_test  # требует PostgreSQL
./tests/integration_test
```

## Ключевые возможности

### 1. Метамодель правил
- Предикаты (<, <=, =, >=, >)
- Арифметические выражения (+, -, *, /)
- Логические выражения (AND, OR, NOT)
- Функции выбора (CASE/IF-THEN-ELSE)
- Рекурсивные вычисления

### 2. Конструктор тарифов
- Динамическое создание классов услуг
- Описание параметров и их типов
- Формирование правил через SQL
- Композиция функций

### 3. Исполнитель правил
- Вычисление сложных выражений
- Подстановка фактических параметров
- Обработка цепочек правил
- Обработка ошибок

### 4. Поиск оптимального тарифа
- Сравнение всех доступных тарифов
- Ранжирование по стоимости
- Анализ экономии
- Топ-N лучших предложений

## Документация

- **[docs/REPORT.md](docs/REPORT.md)** - Полный отчет курсовой работы
- **[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)** - Архитектура и диаграмма зависимостей
- **[src/README.md](src/README.md)** - Структура исходного кода
- **[database/README.md](database/README.md)** - Настройка PostgreSQL
- **[tests/README.md](tests/README.md)** - Руководство по тестированию

## Статистика проекта

- **Строк кода:** ~8000+
- **Файлов:** 40+
- **Библиотек:** 5 (Model, Core, Utils, GUI, SystemCore)
- **Таблиц БД:** 16
- **SQL процедур:** 22
- **Тестов:** 100+
- **Покрытие:** ~90%

## Автор

Дубенецкий - Курсовая работа по проектированию информационных систем, ЭТУ "ЛЭТИ"

## Лицензия

Образовательный проект
