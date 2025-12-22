# Архитектура проекта - Диаграмма зависимостей

## Структура библиотек

```
┌─────────────────────────────────────────────────────────────┐
│                    TariffSystem (executable)                 │
│                         src/main.cpp                         │
└───────────────┬─────────────────────────────────────────────┘
                │
                │ links to
                ▼
┌─────────────────────────────────────────────────────────────┐
│                        TariffGUI                             │
│                  (STATIC library)                            │
│                      src/gui/                                │
│              MainWindow.cpp, MainWindow.hpp                  │
└───────────────┬─────────────────────────────────────────────┘
                │
                │ depends on
                ▼
┌─────────────────────────────────────────────────────────────┐
│                   TariffSystemCore                           │
│                  (INTERFACE library)                         │
│           Aggregates: Model + Core + Utils                   │
└───────────────┬─────────────────────────────────────────────┘
                │
        ┌───────┴───────┬──────────────┐
        │               │              │
        ▼               ▼              ▼
┌──────────────┐ ┌─────────────┐ ┌──────────────┐
│ TariffModel  │ │ TariffCore  │ │ TariffUtils  │
│   (STATIC)   │ │   (STATIC)  │ │   (STATIC)   │
│  src/model/  │ │  src/core/  │ │  src/utils/  │
│              │ │             │ │              │
│ • Classifier │ │ • RuleEngine│ │ • Types      │
│ • Parameter  │ │ • CostCalc  │ │ • Database   │
│ • Service    │ │ • Optimal   │ │ • Repository │
│ • Tariff     │ │             │ │              │
│ • Order      │ │             │ │              │
│ • Rule       │ │             │ │              │
└──────────────┘ └──────┬──────┘ └──────┬───────┘
                        │                │
                        │ depends on     │ depends on
                        └────────┬───────┴────────┐
                                 │                │
                                 ▼                ▼
                         ┌──────────────┐  ┌────────────┐
                         │ TariffModel  │  │ PostgreSQL │
                         └──────────────┘  └────────────┘
```

## Зависимости библиотек

### TariffModel (нулевой уровень)
- **Зависимости:** нет
- **Используется:** TariffCore, TariffUtils
- **Описание:** Чистая модель предметной области

### TariffCore (первый уровень)
- **Зависимости:** TariffModel
- **Используется:** TariffSystemCore
- **Описание:** Бизнес-логика без привязки к БД

### TariffUtils (первый уровень)
- **Зависимости:** TariffModel, PostgreSQL
- **Используется:** TariffSystemCore
- **Описание:** Инфраструктура и работа с БД

### TariffSystemCore (второй уровень)
- **Зависимости:** TariffModel + TariffCore + TariffUtils
- **Используется:** TariffGUI, тесты
- **Описание:** Интерфейсная библиотека для всех компонентов

### TariffGUI (третий уровень)
- **Зависимости:** TariffSystemCore, Qt6
- **Используется:** TariffSystem (executable)
- **Описание:** GUI компоненты

## Компиляция

### Порядок компиляции библиотек:

1. **TariffModel** ← компилируется первой (нет зависимостей)
2. **TariffCore** ← зависит от TariffModel
3. **TariffUtils** ← зависит от TariffModel
4. **TariffSystemCore** ← объединяет Model + Core + Utils
5. **TariffGUI** ← зависит от TariffSystemCore
6. **TariffSystem** ← финальная сборка приложения

### Тесты

Каждый уровень имеет свои тесты:
- `model_test` → TariffModel
- `core_test` → TariffCore
- `database_test` → TariffUtils
- `integration_test` → TariffSystemCore

## Файловая структура

```
project/
├── CMakeLists.txt          ← Главный файл сборки
├── src/
│   ├── model/
│   │   ├── CMakeLists.txt  ← Сборка TariffModel
│   │   ├── Model.cpp
│   │   └── *.hpp (6 файлов)
│   ├── core/
│   │   ├── CMakeLists.txt  ← Сборка TariffCore
│   │   ├── Core.cpp
│   │   └── *.hpp (3 файла)
│   ├── utils/
│   │   ├── CMakeLists.txt  ← Сборка TariffUtils
│   │   ├── Utils.cpp
│   │   └── *.hpp (3 файла)
│   ├── gui/
│   │   ├── CMakeLists.txt  ← Сборка TariffGUI
│   │   ├── MainWindow.cpp
│   │   └── MainWindow.hpp
│   └── main.cpp            ← Точка входа TariffSystem
├── tests/
│   ├── CMakeLists.txt      ← Сборка всех тестов
│   ├── model/
│   │   └── model_test.cpp
│   ├── core/
│   │   └── core_test.cpp
│   ├── database/
│   │   └── database_test.cpp
│   └── integration/
│       └── integration_test.cpp
└── database/
    ├── schema/             ← DDL скрипты
    ├── procedures/         ← SQL процедуры
    └── test-data/          ← Тестовые данные
```

## Принципы организации

### 1. Разделение на слои (Layered Architecture)
```
┌─────────────────────────────────────┐
│  Presentation Layer (GUI)           │  ← Qt6 интерфейс
├─────────────────────────────────────┤
│  Business Logic Layer (Core)        │  ← Правила, вычисления
├─────────────────────────────────────┤
│  Domain Model Layer (Model)         │  ← Сущности предметной области
├─────────────────────────────────────┤
│  Data Access Layer (Utils)          │  ← Repository, Database
├─────────────────────────────────────┤
│  Database Layer (PostgreSQL)        │  ← Хранение данных
└─────────────────────────────────────┘
```

### 2. Dependency Inversion Principle
- Высокоуровневые модули (GUI, Core) не зависят от низкоуровневых (БД)
- Зависимости направлены через абстракции (Repository pattern)

### 3. Single Responsibility Principle
- Model: только модель данных
- Core: только бизнес-логика
- Utils: только инфраструктура
- GUI: только представление

### 4. Open/Closed Principle
- Легко расширять (добавлять новые услуги, правила)
- Не нужно изменять существующий код

## CMake targets summary

| Target            | Type       | Dependencies                    | Output              |
|-------------------|------------|---------------------------------|---------------------|
| TariffModel       | STATIC     | -                               | libTariffModel.a    |
| TariffCore        | STATIC     | TariffModel                     | libTariffCore.a     |
| TariffUtils       | STATIC     | TariffModel, PostgreSQL         | libTariffUtils.a    |
| TariffGUI         | STATIC     | TariffSystemCore, Qt6           | libTariffGUI.a      |
| TariffSystemCore  | INTERFACE  | Model + Core + Utils            | (header-only)       |
| TariffSystem      | EXECUTABLE | TariffSystemCore, TariffGUI, Qt6| TariffSystem(.exe)  |
| model_test        | EXECUTABLE | TariffModel, GTest              | model_test(.exe)    |
| core_test         | EXECUTABLE | TariffCore, GTest               | core_test(.exe)     |
| database_test     | EXECUTABLE | TariffUtils, GTest              | database_test(.exe) |
| integration_test  | EXECUTABLE | TariffSystemCore, GTest         | integration_test    |

## Преимущества структуры

✅ **Модульность** - каждый компонент независим  
✅ **Тестируемость** - каждый слой тестируется отдельно  
✅ **Переиспользование** - библиотеки можно использовать в других проектах  
✅ **Масштабируемость** - легко добавлять новые модули  
✅ **Понятность** - четкая структура и зависимости  
✅ **Правильная компоновка** - реальные библиотеки с символами  
