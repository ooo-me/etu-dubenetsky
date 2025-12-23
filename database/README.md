# Настройка базы данных PostgreSQL

## Обзор

Этот каталог содержит SQL скрипты для создания схемы базы данных, хранимых процедур и тестовых данных для системы управления тарифами.

## Структура

```
database/
├── schema/                 # DDL скрипты для создания таблиц
│   ├── 01_tables.sql      # Создание 28 таблиц
│   └── 02_indexes.sql     # Создание 48 индексов и ограничений
├── procedures/            # Хранимые процедуры (118 процедур)
│   ├── constructor/       # Конструктор тарифов
│   │   └── constructor.sql  # INS_*, UPD_*, DEL_* процедуры
│   ├── calculator/        # Калькулятор стоимости
│   │   └── calculator.sql   # CALC_ORDER_COST, FIND_OPTIMAL_*, и др.
│   └── utils/             # Утилиты
│       └── utils.sql        # GET_ALL_*, VALIDATE_ORDER, и др.
└── test-data/             # Тестовые данные
    ├── 01_classifiers.sql # Заполнение классификаторов
    └── 02_tariffs.sql     # Примеры тарифов
```

## Установка PostgreSQL

### Windows

1. Скачать установщик с https://www.postgresql.org/download/windows/
2. Запустить установщик
3. Выбрать компоненты: PostgreSQL Server, pgAdmin 4
4. Установить пароль для пользователя postgres (по умолчанию: postgres)
5. Порт: 5433 (или 5432 по умолчанию)

### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install postgresql postgresql-contrib
sudo systemctl start postgresql
sudo systemctl enable postgresql
```

### macOS

```bash
brew install postgresql@15
brew services start postgresql@15
```

## Создание базы данных

### Вариант 1: Через psql (командная строка)

```bash
# Подключение к PostgreSQL
psql -U postgres

# Создание базы данных
CREATE DATABASE tariff_system
    WITH 
    ENCODING = 'UTF8'
    LC_COLLATE = 'ru_RU.UTF-8'
    LC_CTYPE = 'ru_RU.UTF-8';

# Подключение к созданной базе
\c tariff_system

# Выход
\q
```

### Вариант 2: Через pgAdmin

1. Открыть pgAdmin 4
2. Подключиться к серверу PostgreSQL
3. Правый клик на "Databases" → "Create" → "Database..."
4. Имя: `tariff_system`
5. Encoding: UTF8
6. Click "Save"

## Выполнение скриптов

### Порядок выполнения

Скрипты должны выполняться в следующем порядке:

```bash
cd /path/to/etu-dubenetsky

# 1. Создание таблиц
psql -U postgres -d tariff_system -f database/schema/01_tables.sql

# 2. Создание индексов
psql -U postgres -d tariff_system -f database/schema/02_indexes.sql

# 3. Создание процедур конструктора
psql -U postgres -d tariff_system -f database/procedures/constructor/constructor.sql

# 4. Создание процедур калькулятора
psql -U postgres -d tariff_system -f database/procedures/calculator/calculator.sql

# 5. Создание утилитных процедур
psql -U postgres -d tariff_system -f database/procedures/utils/utils.sql

# 6. (Опционально) Загрузка тестовых данных
psql -U postgres -d tariff_system -f database/test-data/01_classifiers.sql
psql -U postgres -d tariff_system -f database/test-data/02_tariffs.sql
```

### Скрипт автоматической установки

Создайте файл `setup_database.sh` (Linux/macOS) или `setup_database.bat` (Windows):

#### Linux/macOS (`setup_database.sh`)

```bash
#!/bin/bash

DB_USER="postgres"
DB_NAME="tariff_system"

echo "Создание базы данных $DB_NAME..."
psql -U $DB_USER -c "CREATE DATABASE $DB_NAME ENCODING 'UTF8';"

echo "Создание таблиц..."
psql -U $DB_USER -d $DB_NAME -f database/schema/01_tables.sql

echo "Создание индексов..."
psql -U $DB_USER -d $DB_NAME -f database/schema/02_indexes.sql

echo "Создание процедур конструктора..."
psql -U $DB_USER -d $DB_NAME -f database/procedures/constructor/constructor.sql

echo "Создание процедур калькулятора..."
psql -U $DB_USER -d $DB_NAME -f database/procedures/calculator/calculator.sql

echo "Создание утилитных процедур..."
psql -U $DB_USER -d $DB_NAME -f database/procedures/utils/utils.sql

echo "Загрузка тестовых данных..."
psql -U $DB_USER -d $DB_NAME -f database/test-data/01_classifiers.sql
psql -U $DB_USER -d $DB_NAME -f database/test-data/02_tariffs.sql

echo "База данных успешно настроена!"
```

Сделать исполняемым:
```bash
chmod +x setup_database.sh
./setup_database.sh
```

#### Windows (`setup_database.bat`)

```batch
@echo off
SET DB_USER=postgres
SET DB_NAME=tariff_system
SET PSQL="C:\Program Files\PostgreSQL\15\bin\psql.exe"

echo Creating database %DB_NAME%...
%PSQL% -U %DB_USER% -c "CREATE DATABASE %DB_NAME% ENCODING 'UTF8';"

echo Creating tables...
%PSQL% -U %DB_USER% -d %DB_NAME% -f database\schema\01_tables.sql

echo Creating indexes...
%PSQL% -U %DB_USER% -d %DB_NAME% -f database\schema\02_indexes.sql

echo Creating constructor procedures...
%PSQL% -U %DB_USER% -d %DB_NAME% -f database\procedures\constructor\constructor.sql

echo Creating calculator procedures...
%PSQL% -U %DB_USER% -d %DB_NAME% -f database\procedures\calculator\calculator.sql

echo Creating utility procedures...
%PSQL% -U %DB_USER% -d %DB_NAME% -f database\procedures\utils\utils.sql

echo Loading test data...
%PSQL% -U %DB_USER% -d %DB_NAME% -f database\test-data\01_classifiers.sql
%PSQL% -U %DB_USER% -d %DB_NAME% -f database\test-data\02_tariffs.sql

echo Database setup complete!
pause
```

## Проверка установки

После выполнения всех скриптов проверьте установку:

```sql
-- Подключение к БД
psql -U postgres -d tariff_system

-- Проверка таблиц
\dt

-- Ожидаемые таблицы (28 таблиц):
-- Метамодель: EI, ENUM_VAL_R, POS_ENUM, CHEM_CLASS, FUNCT_R, 
--   ARG_FUNCT, PROD, CONST, PARAMETR1, PAR_CLASS1
-- Предметная область: SERVICE_TYPE, SERVICE_TYPE_PARAM, EXECUTOR,
--   TARIFF, TARIFF_RATE, TARIFF_RULE, SERVICE_ORDER, ORDER_PARAM,
--   ORDER_ITEM, ORDER_ITEM_PARAM
-- Дополнительные: COEFFICIENT, TARIFF_COEFFICIENT, PAR_PROD2,
--   ROLE_VAL, FUN_COMP, FACT_FUN, FACT_PAR, DECISION_RULE

-- Проверка процедур (118 процедур)
\df

-- Примеры процедур:
-- Конструктор: INS_EI, INS_ENUM, INS_CLASS, INS_SERVICE_TYPE,
--   INS_EXECUTOR, INS_TARIFF, UPD_*, DEL_*
-- Калькулятор: CALC_ORDER_COST, CALC_ORDER_ITEM_COST,
--   FIND_OPTIMAL_EXECUTOR, FIND_OPTIMAL_TARIFF, VALIDATE_ORDER
-- Утилиты: GET_ALL_EI, GET_ALL_ENUMS, GET_ALL_SERVICE_TYPES,
--   GET_ALL_EXECUTORS, GET_ALL_TARIFFS, GET_ALL_ORDERS

-- Проверка данных
SELECT COUNT(*) FROM CHEM_CLASS;
SELECT COUNT(*) FROM FUNCT_R;
```

## Настройка для тестов C++

Для запуска database_test необходимо настроить переменные окружения:

### Linux/macOS

```bash
export DB_HOST=localhost
export DB_PORT=5433
export DB_NAME=tariff_system
export DB_USER=postgres
export DB_PASSWORD=postgres
```

### Windows (PowerShell)

```powershell
$env:DB_HOST="localhost"
$env:DB_PORT="5433"
$env:DB_NAME="tariff_system"
$env:DB_USER="postgres"
$env:DB_PASSWORD="postgres"
```

### Windows (CMD)

```batch
set DB_HOST=localhost
set DB_PORT=5433
set DB_NAME=tariff_system
set DB_USER=postgres
set DB_PASSWORD=postgres
```

## Запуск тестов

После настройки БД и переменных окружения:

```bash
cd build
ctest -R database_test --output-on-failure --verbose
```

Или напрямую:

```bash
./tests/database_test
```

## Сброс базы данных

Если нужно полностью пересоздать БД:

```sql
-- Подключение к postgres
psql -U postgres

-- Отключение всех соединений
SELECT pg_terminate_backend(pid) 
FROM pg_stat_activity 
WHERE datname = 'tariff_system';

-- Удаление БД
DROP DATABASE IF EXISTS tariff_system;

-- Создание заново
CREATE DATABASE tariff_system ENCODING 'UTF8';

\q
```

Затем повторно выполнить все скрипты.

## Резервное копирование

### Создание бэкапа

```bash
pg_dump -U postgres -d tariff_system -F c -b -v -f tariff_system_backup.dump
```

### Восстановление из бэкапа

```bash
pg_restore -U postgres -d tariff_system -v tariff_system_backup.dump
```

## Troubleshooting

### Проблема: "role does not exist"

```sql
CREATE USER your_user WITH PASSWORD 'your_password';
GRANT ALL PRIVILEGES ON DATABASE tariff_system TO your_user;
```

### Проблема: "peer authentication failed"

Отредактировать `/etc/postgresql/15/main/pg_hba.conf`:

```
# Изменить с peer на md5
local   all             all                                     md5
```

Перезапустить PostgreSQL:
```bash
sudo systemctl restart postgresql
```

### Проблема: "connection refused"

Проверить что PostgreSQL запущен:
```bash
sudo systemctl status postgresql
```

Проверить порт:
```bash
netstat -an | grep 5432
```

## Дополнительные ресурсы

- [PostgreSQL Documentation](https://www.postgresql.org/docs/)
- [pgAdmin Documentation](https://www.pgadmin.org/docs/)
- [psql Command Reference](https://www.postgresql.org/docs/current/app-psql.html)

## Поддержка

Для вопросов и проблем с БД:
- Проверьте логи PostgreSQL: `/var/log/postgresql/postgresql-15-main.log`
- Используйте pgAdmin для визуального управления
- Обратитесь к документации проекта
