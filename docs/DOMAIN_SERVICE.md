# DomainService - Слой оркестрации системы тарифов

## Описание

**DomainService** — центральный компонент системы, обеспечивающий:
- Оркестрацию всех бизнес-операций
- Персистентность данных в PostgreSQL
- Транзакционность операций
- Целостность между C++ моделью и БД
- Кэширование для производительности

## Архитектура

```
┌─────────────────────────────────────────────────┐
│              GUI (Qt6 MainWindow)               │
│        Взаимодействие через DomainService       │
└────────────────┬────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────┐
│            DomainService                        │
│  • Оркестрация бизнес-логики                    │
│  • Управление транзакциями                      │
│  • Валидация данных                             │
│  • Кэширование                                  │
└─────┬──────────┬──────────────┬─────────────────┘
      │          │              │
┌─────▼────┐ ┌──▼─────┐ ┌──────▼──────┐
│  Model   │ │  Core  │ │  Repository │
│(Entities)│ │(Logic) │ │  (Data)     │
└──────────┘ └────────┘ └──────┬──────┘
                                │
                    ┌───────────▼───────────┐
                    │    DatabaseManager    │
                    │      (libpq)          │
                    └───────────┬───────────┘
                                │
                    ┌───────────▼───────────┐
                    │    PostgreSQL         │
                    │ 16 Tables + 22 Procs  │
                    └───────────────────────┘
```

## Основные возможности

### 1. Управление классификаторами

```cpp
// Создание корневого классификатора
auto classifier = domainService->createClassifier(
    "SERVICE_CLASS",
    "Класс услуг",
    1,  // уровень
    std::nullopt,  // нет родителя
    "Примечание"
);

// Создание дочернего классификатора
auto childClassifier = domainService->createClassifier(
    "CARGO_CLASS",
    "Грузоперевозки",
    2,  // уровень
    classifier->getId(),  // родитель
    ""
);

// Получение классификатора (с кэшированием)
auto retrieved = domainService->getClassifier(classifier->getId());

// Получение дочерних классификаторов
auto children = domainService->getChildClassifiers(classifier->getId());
```

### 2. Управление услугами

```cpp
// Создание услуги
auto service = domainService->createService(
    classifierId,
    "CARGO_001",
    "Грузоперевозка по городу",
    "Описание услуги"
);

// Добавление параметров к услуге
auto result = domainService->addServiceParameter(
    service->getId(),
    1,  // ID параметра
    "Вес груза (т)",
    ParameterType::DOUBLE,
    true  // обязательный
);

if (result.success) {
    std::cout << "Параметр добавлен" << std::endl;
}

// Получение услуги
auto retrieved = domainService->getService(service->getId());
```

### 3. Управление тарифами

```cpp
// Создание тарифа
auto tariff = domainService->createTariff(
    serviceId,
    "TARIFF_001",
    "Тариф Стандарт",
    "Транспорт-А ООО",  // поставщик
    "Базовый тариф"
);

// Добавление правила к тарифу
auto rule = std::make_shared<Rule>(/* ... */);
auto result = domainService->addTariffRule(tariff->getId(), rule);

// Получение всех активных тарифов для услуги
auto tariffs = domainService->getActiveTariffsForService(serviceId);
```

### 4. Управление заказами

```cpp
// Создание заказа
auto order = domainService->createOrder(
    serviceId,
    tariffId,
    "ООО Клиент",  // заказчик
    "Срочный заказ"
);

// Установка параметров заказа
domainService->setOrderParameter(order->getId(), 1, Double{100.5});
domainService->setOrderParameter(order->getId(), 2, String{"Москва"});
domainService->setOrderParameter(order->getId(), 3, Integer{5});

// Расчет стоимости
Double cost = domainService->calculateOrderCost(order->getId(), tariffId);
std::cout << "Стоимость заказа: " << cost << " руб." << std::endl;

// Подтверждение заказа
auto result = domainService->confirmOrder(order->getId(), tariffId);
if (result.success) {
    std::cout << result.message << std::endl;
}

// Отмена заказа
auto cancelResult = domainService->cancelOrder(
    order->getId(), 
    "Клиент передумал"
);
```

### 5. Поиск оптимальных тарифов

```cpp
// Поиск топ-N оптимальных тарифов
auto results = domainService->findOptimalTariffs(
    orderId,
    serviceId,
    5  // топ-5
);

// Результаты отсортированы по возрастанию стоимости
for (const auto& result : results) {
    std::cout << "Тариф: " << result.tariff->getName() << std::endl;
    std::cout << "Провайдер: " << result.tariff->getProvider() << std::endl;
    std::cout << "Стоимость: " << result.cost << " руб." << std::endl;
    std::cout << "Экономия: " << result.savings << " руб." << std::endl;
    std::cout << "---" << std::endl;
}

// Использование оптимального тарифа
if (!results.empty()) {
    auto optimalTariff = results[0].tariff;
    domainService->confirmOrder(orderId, optimalTariff->getId());
}
```

## Гарантии и особенности

### Транзакционность

Все операции выполняются в транзакциях:

```cpp
try {
    // Начало транзакции (автоматически)
    auto classifier = domainService->createClassifier(/* ... */);
    auto service = domainService->createService(/* ... */);
    // Commit (автоматически при успехе)
} catch (const DomainServiceException& e) {
    // Rollback (автоматически при ошибке)
    std::cerr << "Ошибка: " << e.what() << std::endl;
}
```

### Валидация данных

DomainService проверяет данные перед сохранением:

```cpp
// Валидация обязательных полей
domainService->createService("", "Название", 1);  // ❌ Исключение: пустой код

// Валидация диапазонов
domainService->createClassifier("CODE", "Name", 0);  // ❌ Исключение: уровень < 1

// Валидация ссылочной целостности
domainService->createService(999999, "CODE", "Name");  // ❌ Исключение: класс не существует

// Валидация бизнес-правил
order->setStatus(OrderStatus::COMPLETED);
domainService->setOrderParameter(order->getId(), 1, 100.0);  // ❌ Исключение: нельзя менять завершенный заказ
```

### Кэширование

DomainService кэширует объекты для производительности:

```cpp
// Первый запрос - из БД (медленно)
auto service1 = domainService->getService(serviceId);

// Второй запрос - из кэша (быстро)
auto service2 = domainService->getService(serviceId);

// Проверка: это один и тот же объект в памяти
assert(service1.get() == service2.get());

// Очистка кэша (для тестирования)
domainService->clearCache();

// Статистика кэша
auto stats = domainService->getCacheStats();
std::cout << "Кэш классификаторов: " << stats.classifiers << std::endl;
std::cout << "Кэш услуг: " << stats.services << std::endl;
std::cout << "Кэш тарифов: " << stats.tariffs << std::endl;
std::cout << "Кэш заказов: " << stats.orders << std::endl;
```

### Персистентность

Все данные сохраняются в PostgreSQL:

```cpp
// Создание данных
auto service = domainService->createService(/* ... */);
domainService->setOrderParameter(orderId, 1, 100.0);

// Перезапуск приложения...

// Данные сохранены в БД
auto retrieved = domainService->getService(service->getId());
assert(retrieved != nullptr);
assert(retrieved->getCode() == service->getCode());
```

## Обработка ошибок

DomainService генерирует исключение `DomainServiceException`:

```cpp
try {
    auto service = domainService->createService(/* ... */);
} catch (const DomainServiceException& e) {
    std::cerr << "Ошибка DomainService: " << e.what() << std::endl;
} catch (const DatabaseException& e) {
    std::cerr << "Ошибка БД: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Общая ошибка: " << e.what() << std::endl;
}
```

Для операций, которые могут частично завершиться с ошибкой, используется `OperationResult`:

```cpp
auto result = domainService->setOrderParameter(orderId, parameterId, value);

if (result.success) {
    std::cout << "Успех: " << result.message << std::endl;
    if (result.entityId.has_value()) {
        std::cout << "ID сущности: " << *result.entityId << std::endl;
    }
} else {
    std::cerr << "Ошибка: " << result.message << std::endl;
}
```

## Тестирование

Полный набор тестов находится в `tests/domain/domain_service_test.cpp`:

```bash
# Запуск тестов DomainService
cd build
./tests/domain_service_test

# Или через CTest
ctest -R domain_service_test --output-on-failure
```

### Типы тестов

1. **Unit тесты** - каждый метод DomainService
2. **Интеграционные тесты** - взаимодействие с PostgreSQL
3. **Каверзные тесты** (edge cases):
   - Очень длинные строки
   - Специальные символы
   - Конкурентный доступ
   - Глубокие иерархии
   - Большие объемы данных
4. **Real-world сценарии** - полные бизнес-процессы

## Использование в GUI

Пример из MainWindow:

```cpp
class MainWindow : public QMainWindow {
private:
    std::shared_ptr<DomainService> domainService_;
    
    void connectToDatabase() {
        auto db = std::make_shared<DatabaseManager>();
        db->connect(params);
        
        // Создание DomainService
        domainService_ = std::make_shared<DomainService>(db);
        
        statusBar()->showMessage("DomainService готов к работе");
    }
    
    void createOrder() {
        if (!domainService_) {
            QMessageBox::warning(this, "Ошибка", "Подключитесь к БД");
            return;
        }
        
        try {
            auto order = domainService_->createOrder(/* ... */);
            QMessageBox::information(this, "Успех", 
                QString("Заказ создан: %1").arg(order->getCode().c_str()));
        } catch (const DomainServiceException& e) {
            QMessageBox::critical(this, "Ошибка", e.what());
        }
    }
};
```

## Best Practices

### 1. Всегда используйте DomainService

❌ **Неправильно:**
```cpp
// Прямой доступ к Repository
auto repo = std::make_unique<ClassifierRepository>(db);
auto id = repo->createClass(/* ... */);
```

✅ **Правильно:**
```cpp
// Через DomainService
auto domainService = std::make_unique<DomainService>(db);
auto classifier = domainService->createClassifier(/* ... */);
```

### 2. Обрабатывайте исключения

❌ **Неправильно:**
```cpp
auto service = domainService->createService(/* ... */);  // Может упасть
```

✅ **Правильно:**
```cpp
try {
    auto service = domainService->createService(/* ... */);
} catch (const DomainServiceException& e) {
    // Обработка ошибки
}
```

### 3. Проверяйте OperationResult

❌ **Неправильно:**
```cpp
auto result = domainService->setOrderParameter(/* ... */);
// Не проверяем result.success
```

✅ **Правильно:**
```cpp
auto result = domainService->setOrderParameter(/* ... */);
if (!result.success) {
    std::cerr << "Ошибка: " << result.message << std::endl;
    return;
}
```

### 4. Используйте транзакции

```cpp
// DomainService автоматически управляет транзакциями
// Но для сложных операций можно использовать явные транзакции

try {
    domainService->beginTransaction();  // Если такой метод добавлен
    
    auto service = domainService->createService(/* ... */);
    domainService->addServiceParameter(/* ... */);
    auto tariff = domainService->createTariff(/* ... */);
    
    domainService->commit();
} catch (...) {
    domainService->rollback();
    throw;
}
```

## Заключение

**DomainService** - это единая точка входа для всех операций с данными в системе. Он:

✅ Обеспечивает оркестрацию бизнес-логики  
✅ Гарантирует персистентность данных  
✅ Управляет транзакциями  
✅ Валидирует данные  
✅ Кэширует для производительности  
✅ Предоставляет единый API для GUI  

**Используйте DomainService везде, где нужно работать с данными системы!**
