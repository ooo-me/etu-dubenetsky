# Структура исходного кода

Данный каталог содержит исходный код системы управления тарифами, организованный в виде модульной архитектуры.

## Структура

```
src/
├── model/          # Модель предметной области
│   ├── CMakeLists.txt
│   ├── Model.cpp
│   ├── Classifier.hpp
│   ├── Parameter.hpp
│   ├── Rule.hpp
│   ├── Service.hpp
│   ├── Tariff.hpp
│   └── Order.hpp
├── core/           # Бизнес-логика
│   ├── CMakeLists.txt
│   ├── Core.cpp
│   ├── RuleEngine.hpp
│   ├── CostCalculator.hpp
│   └── OptimalSearch.hpp
├── utils/          # Утилиты и работа с БД
│   ├── CMakeLists.txt
│   ├── Utils.cpp
│   ├── Types.hpp
│   ├── Database.hpp
│   └── Repository.hpp
├── gui/            # GUI компоненты (Qt6)
│   ├── CMakeLists.txt
│   ├── MainWindow.cpp
│   └── MainWindow.hpp
└── main.cpp        # Точка входа приложения
```

## Библиотеки

### TariffModel
**Описание:** Модель предметной области  
**Файлы:** `src/model/*`  
**Зависимости:** нет  
**Содержит:**
- `Classifier` - иерархия классификаторов (услуги, тарифы, заказы)
- `Parameter` - типизированные параметры
- `Service` - базовый класс услуги и специализации (CargoService, StorageService)
- `Tariff` - тариф с правилами
- `Order` - заказ на услугу
- `Rule` - правила расчета стоимости

### TariffCore
**Описание:** Бизнес-логика системы  
**Файлы:** `src/core/*`  
**Зависимости:** TariffModel  
**Содержит:**
- `RuleEngine` - движок правил для вычисления выражений
- `CostCalculator` - калькулятор стоимости заказов по тарифам
- `OptimalSearcher` - поиск оптимального тарифа

### TariffUtils
**Описание:** Утилиты и работа с базой данных  
**Файлы:** `src/utils/*`  
**Зависимости:** TariffModel, PostgreSQL  
**Содержит:**
- `Types` - общие типы и утилиты (std::variant, std::optional)
- `DatabaseManager` - менеджер подключения к PostgreSQL (libpq)
- `Repository` - паттерн Repository для доступа к данным
  - ClassifierRepository
  - FunctionRepository
  - ObjectRepository
  - CalculationRepository
  - TariffSystemRepository (фасад)

### TariffGUI
**Описание:** Графический интерфейс  
**Файлы:** `src/gui/*`  
**Зависимости:** TariffSystemCore, Qt6  
**Содержит:**
- `MainWindow` - главное окно приложения с меню и панелями

### TariffSystemCore
**Описание:** Общая интерфейсная библиотека  
**Тип:** INTERFACE library  
**Зависимости:** TariffModel + TariffCore + TariffUtils  
**Использование:** Удобная библиотека для подключения всех компонентов системы

## Приложение

### TariffSystem
**Тип:** Исполняемый файл  
**Файлы:** `src/main.cpp`, `src/gui/*`  
**Зависимости:** TariffSystemCore, TariffGUI, Qt6, PostgreSQL  
**Описание:** Главное GUI приложение системы управления тарифами

## Сборка

Все библиотеки собираются как STATIC библиотеки для удобства развертывания.

```bash
# Основная сборка через корневой CMakeLists.txt
cmake --preset x64-debug
cmake --build build

# Исполняемый файл
./build/TariffSystem

# Альтернативно: использовать подкаталоги CMakeLists.txt
# (если раскомментировать add_subdirectory в корневом CMakeLists.txt)
```

## Принципы архитектуры

1. **Разделение ответственности**
   - Model: чистая модель предметной области
   - Core: бизнес-логика без привязки к БД
   - Utils: инфраструктурный код (БД, утилиты)
   - GUI: пользовательский интерфейс

2. **Минимизация зависимостей**
   - Model не зависит ни от чего
   - Core зависит только от Model
   - Utils зависит от Model и PostgreSQL
   - GUI зависит от всего через TariffSystemCore

3. **Современный C++20**
   - std::variant для полиморфизма без виртуальных функций
   - std::optional для необязательных значений
   - std::shared_ptr/unique_ptr для управления памятью
   - Concepts (готовность к использованию)

4. **Паттерны проектирования**
   - Repository: инкапсуляция доступа к данным
   - Strategy: различные типы выражений
   - Composite: дерево правил
   - Factory: создание объектов
   - RAII: управление ресурсами (подключения к БД)

## Тестирование

Каждая библиотека покрывается модульными тестами:
- `tests/model/` - тесты TariffModel
- `tests/core/` - тесты TariffCore
- `tests/database/` - тесты TariffUtils (интеграция с PostgreSQL)
- `tests/integration/` - интеграционные тесты

Смотрите `tests/README.md` для подробностей.
