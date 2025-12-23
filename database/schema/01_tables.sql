-- ============================================================================
-- Схема базы данных для системы работы с тарифами на услуги
-- СУБД: PostgreSQL 12+
-- Описание: Основные таблицы системы
-- ============================================================================

-- Создание базы данных (выполнить отдельно от имени superuser)
-- CREATE DATABASE tariff_system ENCODING 'UTF8' LC_COLLATE='ru_RU.UTF-8' LC_CTYPE='ru_RU.UTF-8';

SET client_encoding = 'UTF8';

-- ============================================================================
-- 1. Единицы измерения
-- ============================================================================

COMMENT ON TABLE EI IS 'Справочник единиц измерения';

CREATE TABLE IF NOT EXISTS EI (
    ID_EI SERIAL PRIMARY KEY,
    COD_EI VARCHAR(20) NOT NULL UNIQUE,
    NAME_EI VARCHAR(100) NOT NULL,
    NOTE TEXT
);

COMMENT ON COLUMN EI.ID_EI IS 'Уникальный идентификатор единицы измерения';
COMMENT ON COLUMN EI.COD_EI IS 'Код единицы измерения (например, кг, м, ч)';
COMMENT ON COLUMN EI.NAME_EI IS 'Полное наименование единицы измерения';
COMMENT ON COLUMN EI.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 2. Перечисления
-- ============================================================================

COMMENT ON TABLE ENUM_VAL_R IS 'Справочник перечислений для параметров';

CREATE TABLE IF NOT EXISTS ENUM_VAL_R (
    ID_ENUM SERIAL PRIMARY KEY,
    COD_ENUM VARCHAR(50) NOT NULL UNIQUE,
    NAME_ENUM VARCHAR(200) NOT NULL,
    NOTE TEXT
);

COMMENT ON COLUMN ENUM_VAL_R.ID_ENUM IS 'Уникальный идентификатор перечисления';
COMMENT ON COLUMN ENUM_VAL_R.COD_ENUM IS 'Код перечисления (например, VEHICLE_CLASS)';
COMMENT ON COLUMN ENUM_VAL_R.NAME_ENUM IS 'Наименование перечисления';
COMMENT ON COLUMN ENUM_VAL_R.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 3. Позиции перечислений
-- ============================================================================

COMMENT ON TABLE POS_ENUM IS 'Значения (позиции) перечислений';

CREATE TABLE IF NOT EXISTS POS_ENUM (
    ID_POS_ENUM SERIAL PRIMARY KEY,
    ID_ENUM INTEGER NOT NULL,
    COD_POS VARCHAR(50) NOT NULL,
    NAME_POS VARCHAR(200) NOT NULL,
    NUM_POS INTEGER NOT NULL DEFAULT 0,
    NOTE TEXT,
    CONSTRAINT UK_POS_ENUM UNIQUE (ID_ENUM, COD_POS),
    CONSTRAINT FK_POS_ENUM_ENUM FOREIGN KEY (ID_ENUM) 
        REFERENCES ENUM_VAL_R(ID_ENUM) ON DELETE CASCADE
);

COMMENT ON COLUMN POS_ENUM.ID_POS_ENUM IS 'Уникальный идентификатор позиции';
COMMENT ON COLUMN POS_ENUM.ID_ENUM IS 'Ссылка на перечисление';
COMMENT ON COLUMN POS_ENUM.COD_POS IS 'Код позиции (например, CLOSED, OPEN)';
COMMENT ON COLUMN POS_ENUM.NAME_POS IS 'Наименование позиции';
COMMENT ON COLUMN POS_ENUM.NUM_POS IS 'Порядковый номер позиции';
COMMENT ON COLUMN POS_ENUM.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 4. Классификатор компонентов системы
-- ============================================================================

COMMENT ON TABLE CHEM_CLASS IS 'Иерархический классификатор компонентов системы';

CREATE TABLE IF NOT EXISTS CHEM_CLASS (
    ID_CHEM SERIAL PRIMARY KEY,
    COD_CHEM VARCHAR(50) NOT NULL UNIQUE,
    NAME_CHEM VARCHAR(200) NOT NULL,
    PARENT_ID INTEGER,
    LEV INTEGER NOT NULL DEFAULT 0,
    NOTE TEXT,
    CONSTRAINT FK_CHEM_CLASS_PARENT FOREIGN KEY (PARENT_ID) 
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE CASCADE
);

COMMENT ON COLUMN CHEM_CLASS.ID_CHEM IS 'Уникальный идентификатор класса';
COMMENT ON COLUMN CHEM_CLASS.COD_CHEM IS 'Код класса (например, SERVICE, TARIFF, ORDER)';
COMMENT ON COLUMN CHEM_CLASS.NAME_CHEM IS 'Наименование класса';
COMMENT ON COLUMN CHEM_CLASS.PARENT_ID IS 'Ссылка на родительский класс';
COMMENT ON COLUMN CHEM_CLASS.LEV IS 'Уровень в иерархии (0 - корень)';
COMMENT ON COLUMN CHEM_CLASS.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 5. Функции и правила
-- ============================================================================

COMMENT ON TABLE FUNCT_R IS 'Функции и правила для вычислений';

CREATE TABLE IF NOT EXISTS FUNCT_R (
    ID_FUNCT SERIAL PRIMARY KEY,
    COD_FUNCT VARCHAR(50) NOT NULL UNIQUE,
    NAME_FUNCT VARCHAR(200) NOT NULL,
    TYPE_F INTEGER NOT NULL DEFAULT 0,
    OPERATION VARCHAR(20),
    NOTE TEXT
);

COMMENT ON COLUMN FUNCT_R.ID_FUNCT IS 'Уникальный идентификатор функции';
COMMENT ON COLUMN FUNCT_R.COD_FUNCT IS 'Код функции';
COMMENT ON COLUMN FUNCT_R.NAME_FUNCT IS 'Наименование функции';
COMMENT ON COLUMN FUNCT_R.TYPE_F IS 'Тип функции: 0-предикат, 1-арифметическое, 2-логическое, 3-выбор';
COMMENT ON COLUMN FUNCT_R.OPERATION IS 'Операция: +, -, *, /, <, <=, =, >=, >, AND, OR, NOT, IF';
COMMENT ON COLUMN FUNCT_R.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 6. Аргументы функций
-- ============================================================================

COMMENT ON TABLE ARG_FUNCT IS 'Аргументы функций';

CREATE TABLE IF NOT EXISTS ARG_FUNCT (
    ID_ARG SERIAL PRIMARY KEY,
    ID_FUNCT INTEGER NOT NULL,
    NUM_ARG INTEGER NOT NULL DEFAULT 1,
    CLASS_ARG INTEGER,
    NAME_ARG VARCHAR(200),
    NOTE TEXT,
    CONSTRAINT UK_ARG_FUNCT UNIQUE (ID_FUNCT, NUM_ARG),
    CONSTRAINT FK_ARG_FUNCT_FUNCT FOREIGN KEY (ID_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE,
    CONSTRAINT FK_ARG_FUNCT_CLASS FOREIGN KEY (CLASS_ARG) 
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE SET NULL
);

COMMENT ON COLUMN ARG_FUNCT.ID_ARG IS 'Уникальный идентификатор аргумента';
COMMENT ON COLUMN ARG_FUNCT.ID_FUNCT IS 'Ссылка на функцию';
COMMENT ON COLUMN ARG_FUNCT.NUM_ARG IS 'Порядковый номер аргумента';
COMMENT ON COLUMN ARG_FUNCT.CLASS_ARG IS 'Класс аргумента (тип)';
COMMENT ON COLUMN ARG_FUNCT.NAME_ARG IS 'Наименование аргумента';
COMMENT ON COLUMN ARG_FUNCT.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 7. Объекты предметной области
-- ============================================================================

COMMENT ON TABLE PROD IS 'Объекты предметной области (услуги, тарифы, заказы)';

CREATE TABLE IF NOT EXISTS PROD (
    ID_PR SERIAL PRIMARY KEY,
    CLASS_PR INTEGER NOT NULL,
    COD_PR VARCHAR(50) NOT NULL UNIQUE,
    NAME_PR VARCHAR(200) NOT NULL,
    PAR_PR INTEGER,
    NOTE TEXT,
    CONSTRAINT FK_PROD_CLASS FOREIGN KEY (CLASS_PR) 
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE RESTRICT,
    CONSTRAINT FK_PROD_PARENT FOREIGN KEY (PAR_PR) 
        REFERENCES PROD(ID_PR) ON DELETE CASCADE
);

COMMENT ON COLUMN PROD.ID_PR IS 'Уникальный идентификатор объекта';
COMMENT ON COLUMN PROD.CLASS_PR IS 'Класс объекта';
COMMENT ON COLUMN PROD.COD_PR IS 'Код объекта';
COMMENT ON COLUMN PROD.NAME_PR IS 'Наименование объекта';
COMMENT ON COLUMN PROD.PAR_PR IS 'Родительский объект';
COMMENT ON COLUMN PROD.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 8. Константы
-- ============================================================================

COMMENT ON TABLE CONST IS 'Константы для использования в функциях';

CREATE TABLE IF NOT EXISTS CONST (
    ID_CONST SERIAL PRIMARY KEY,
    COD_CONST VARCHAR(50) NOT NULL UNIQUE,
    NAME_CONST VARCHAR(200) NOT NULL,
    VAL_NUM DOUBLE PRECISION,
    VAL_STR TEXT,
    VAL_DATE DATE,
    EI INTEGER,
    NOTE TEXT,
    CONSTRAINT FK_CONST_EI FOREIGN KEY (EI) 
        REFERENCES EI(ID_EI) ON DELETE SET NULL
);

COMMENT ON COLUMN CONST.ID_CONST IS 'Уникальный идентификатор константы';
COMMENT ON COLUMN CONST.COD_CONST IS 'Код константы';
COMMENT ON COLUMN CONST.NAME_CONST IS 'Наименование константы';
COMMENT ON COLUMN CONST.VAL_NUM IS 'Числовое значение';
COMMENT ON COLUMN CONST.VAL_STR IS 'Строковое значение';
COMMENT ON COLUMN CONST.VAL_DATE IS 'Значение даты';
COMMENT ON COLUMN CONST.EI IS 'Единица измерения';
COMMENT ON COLUMN CONST.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 9. Определения параметров
-- ============================================================================

COMMENT ON TABLE PARAMETR1 IS 'Определения параметров для объектов';

CREATE TABLE IF NOT EXISTS PARAMETR1 (
    ID_PAR SERIAL PRIMARY KEY,
    COD_PAR VARCHAR(50) NOT NULL UNIQUE,
    NAME_PAR VARCHAR(200) NOT NULL,
    CLASS_PAR INTEGER,
    TYPE_PAR INTEGER NOT NULL DEFAULT 0,
    EI INTEGER,
    NOTE TEXT,
    CONSTRAINT FK_PARAMETR1_CLASS FOREIGN KEY (CLASS_PAR) 
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE SET NULL,
    CONSTRAINT FK_PARAMETR1_EI FOREIGN KEY (EI) 
        REFERENCES EI(ID_EI) ON DELETE SET NULL
);

COMMENT ON COLUMN PARAMETR1.ID_PAR IS 'Уникальный идентификатор параметра';
COMMENT ON COLUMN PARAMETR1.COD_PAR IS 'Код параметра';
COMMENT ON COLUMN PARAMETR1.NAME_PAR IS 'Наименование параметра';
COMMENT ON COLUMN PARAMETR1.CLASS_PAR IS 'Класс параметра';
COMMENT ON COLUMN PARAMETR1.TYPE_PAR IS 'Тип параметра: 0-число, 1-строка, 2-дата, 3-перечисление';
COMMENT ON COLUMN PARAMETR1.EI IS 'Единица измерения';
COMMENT ON COLUMN PARAMETR1.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 10. Классы параметров
-- ============================================================================

COMMENT ON TABLE PAR_CLASS1 IS 'Связь параметров с классами';

CREATE TABLE IF NOT EXISTS PAR_CLASS1 (
    ID_PAR INTEGER NOT NULL,
    ID_CHEM INTEGER NOT NULL,
    CONSTRAINT PK_PAR_CLASS1 PRIMARY KEY (ID_PAR, ID_CHEM),
    CONSTRAINT FK_PAR_CLASS1_PAR FOREIGN KEY (ID_PAR) 
        REFERENCES PARAMETR1(ID_PAR) ON DELETE CASCADE,
    CONSTRAINT FK_PAR_CLASS1_CHEM FOREIGN KEY (ID_CHEM) 
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE CASCADE
);

COMMENT ON COLUMN PAR_CLASS1.ID_PAR IS 'Ссылка на параметр';
COMMENT ON COLUMN PAR_CLASS1.ID_CHEM IS 'Ссылка на класс';

-- ============================================================================
-- 11. Значения параметров объектов
-- ============================================================================

COMMENT ON TABLE PAR_PROD2 IS 'Значения параметров для конкретных объектов';

CREATE TABLE IF NOT EXISTS PAR_PROD2 (
    ID_PAR INTEGER NOT NULL,
    ID_PR INTEGER NOT NULL,
    VAL_NUM DOUBLE PRECISION,
    VAL_STR TEXT,
    VAL_DATE DATE,
    ID_VAL_ENUM INTEGER,
    IS_REQUIRE INTEGER DEFAULT 0,
    NOTE TEXT,
    CONSTRAINT PK_PAR_PROD2 PRIMARY KEY (ID_PAR, ID_PR),
    CONSTRAINT FK_PAR_PROD2_PAR FOREIGN KEY (ID_PAR) 
        REFERENCES PARAMETR1(ID_PAR) ON DELETE CASCADE,
    CONSTRAINT FK_PAR_PROD2_PROD FOREIGN KEY (ID_PR) 
        REFERENCES PROD(ID_PR) ON DELETE CASCADE,
    CONSTRAINT FK_PAR_PROD2_ENUM FOREIGN KEY (ID_VAL_ENUM) 
        REFERENCES POS_ENUM(ID_POS_ENUM) ON DELETE SET NULL
);

COMMENT ON COLUMN PAR_PROD2.ID_PAR IS 'Ссылка на параметр';
COMMENT ON COLUMN PAR_PROD2.ID_PR IS 'Ссылка на объект';
COMMENT ON COLUMN PAR_PROD2.VAL_NUM IS 'Числовое значение';
COMMENT ON COLUMN PAR_PROD2.VAL_STR IS 'Строковое значение';
COMMENT ON COLUMN PAR_PROD2.VAL_DATE IS 'Значение даты';
COMMENT ON COLUMN PAR_PROD2.ID_VAL_ENUM IS 'Значение перечисления';
COMMENT ON COLUMN PAR_PROD2.IS_REQUIRE IS 'Признак обязательности (0-нет, 1-да)';
COMMENT ON COLUMN PAR_PROD2.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 12. Значения функций для объектов (роли)
-- ============================================================================

COMMENT ON TABLE ROLE_VAL IS 'Значения функций для объектов';

CREATE TABLE IF NOT EXISTS ROLE_VAL (
    ID_FUNCT INTEGER NOT NULL,
    ID_PR INTEGER NOT NULL,
    ID_VAL_CONST INTEGER,
    ID_VAL_FUNCT INTEGER,
    VAL_NUM DOUBLE PRECISION,
    VAL_STR TEXT,
    VAL_DATE DATE,
    ID_VAL_ENUM INTEGER,
    NOTE TEXT,
    CONSTRAINT PK_ROLE_VAL PRIMARY KEY (ID_FUNCT, ID_PR),
    CONSTRAINT FK_ROLE_VAL_FUNCT FOREIGN KEY (ID_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE,
    CONSTRAINT FK_ROLE_VAL_PROD FOREIGN KEY (ID_PR) 
        REFERENCES PROD(ID_PR) ON DELETE CASCADE,
    CONSTRAINT FK_ROLE_VAL_CONST FOREIGN KEY (ID_VAL_CONST) 
        REFERENCES CONST(ID_CONST) ON DELETE SET NULL,
    CONSTRAINT FK_ROLE_VAL_FUNCT_VAL FOREIGN KEY (ID_VAL_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE SET NULL,
    CONSTRAINT FK_ROLE_VAL_ENUM FOREIGN KEY (ID_VAL_ENUM) 
        REFERENCES POS_ENUM(ID_POS_ENUM) ON DELETE SET NULL
);

COMMENT ON COLUMN ROLE_VAL.ID_FUNCT IS 'Ссылка на функцию';
COMMENT ON COLUMN ROLE_VAL.ID_PR IS 'Ссылка на объект';
COMMENT ON COLUMN ROLE_VAL.ID_VAL_CONST IS 'Значение - константа';
COMMENT ON COLUMN ROLE_VAL.ID_VAL_FUNCT IS 'Значение - функция';
COMMENT ON COLUMN ROLE_VAL.VAL_NUM IS 'Числовое значение';
COMMENT ON COLUMN ROLE_VAL.VAL_STR IS 'Строковое значение';
COMMENT ON COLUMN ROLE_VAL.VAL_DATE IS 'Значение даты';
COMMENT ON COLUMN ROLE_VAL.ID_VAL_ENUM IS 'Значение перечисления';
COMMENT ON COLUMN ROLE_VAL.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 13. Композитные функции
-- ============================================================================

COMMENT ON TABLE FUN_COMP IS 'Композиция функций';

CREATE TABLE IF NOT EXISTS FUN_COMP (
    ID_FUNCT INTEGER NOT NULL,
    ID_FUNCT_COMP INTEGER NOT NULL,
    CONSTRAINT PK_FUN_COMP PRIMARY KEY (ID_FUNCT, ID_FUNCT_COMP),
    CONSTRAINT FK_FUN_COMP_FUNCT FOREIGN KEY (ID_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE,
    CONSTRAINT FK_FUN_COMP_FUNCT_COMP FOREIGN KEY (ID_FUNCT_COMP) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE
);

COMMENT ON COLUMN FUN_COMP.ID_FUNCT IS 'Композитная функция';
COMMENT ON COLUMN FUN_COMP.ID_FUNCT_COMP IS 'Функция-компонент';

-- ============================================================================
-- 14. Вызовы функций
-- ============================================================================

COMMENT ON TABLE FACT_FUN IS 'Фактические вызовы функций';

CREATE TABLE IF NOT EXISTS FACT_FUN (
    ID_FACT_FUN SERIAL PRIMARY KEY,
    ID_FUNCT INTEGER NOT NULL,
    ID_PR INTEGER NOT NULL,
    NUM_CALL INTEGER NOT NULL DEFAULT 1,
    NOTE TEXT,
    CONSTRAINT UK_FACT_FUN UNIQUE (ID_FUNCT, ID_PR, NUM_CALL),
    CONSTRAINT FK_FACT_FUN_FUNCT FOREIGN KEY (ID_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE,
    CONSTRAINT FK_FACT_FUN_PROD FOREIGN KEY (ID_PR) 
        REFERENCES PROD(ID_PR) ON DELETE CASCADE
);

COMMENT ON COLUMN FACT_FUN.ID_FACT_FUN IS 'Уникальный идентификатор вызова';
COMMENT ON COLUMN FACT_FUN.ID_FUNCT IS 'Вызываемая функция';
COMMENT ON COLUMN FACT_FUN.ID_PR IS 'Объект-контекст вызова';
COMMENT ON COLUMN FACT_FUN.NUM_CALL IS 'Номер вызова';
COMMENT ON COLUMN FACT_FUN.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 15. Фактические параметры вызовов
-- ============================================================================

COMMENT ON TABLE FACT_PAR IS 'Фактические параметры вызовов функций';

CREATE TABLE IF NOT EXISTS FACT_PAR (
    ID_FACT_FUN INTEGER NOT NULL,
    ID_ARG INTEGER NOT NULL,
    ID_VAL_CONST INTEGER,
    ID_VAL_FUNCT INTEGER,
    ID_VAL_FACT_FUN INTEGER,
    VAL_NUM DOUBLE PRECISION,
    VAL_STR TEXT,
    VAL_DATE DATE,
    ID_VAL_ENUM INTEGER,
    NOTE TEXT,
    CONSTRAINT PK_FACT_PAR PRIMARY KEY (ID_FACT_FUN, ID_ARG),
    CONSTRAINT FK_FACT_PAR_FACT_FUN FOREIGN KEY (ID_FACT_FUN) 
        REFERENCES FACT_FUN(ID_FACT_FUN) ON DELETE CASCADE,
    CONSTRAINT FK_FACT_PAR_ARG FOREIGN KEY (ID_ARG) 
        REFERENCES ARG_FUNCT(ID_ARG) ON DELETE CASCADE,
    CONSTRAINT FK_FACT_PAR_CONST FOREIGN KEY (ID_VAL_CONST) 
        REFERENCES CONST(ID_CONST) ON DELETE SET NULL,
    CONSTRAINT FK_FACT_PAR_FUNCT FOREIGN KEY (ID_VAL_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE SET NULL,
    CONSTRAINT FK_FACT_PAR_FACT_FUN_VAL FOREIGN KEY (ID_VAL_FACT_FUN) 
        REFERENCES FACT_FUN(ID_FACT_FUN) ON DELETE SET NULL,
    CONSTRAINT FK_FACT_PAR_ENUM FOREIGN KEY (ID_VAL_ENUM) 
        REFERENCES POS_ENUM(ID_POS_ENUM) ON DELETE SET NULL
);

COMMENT ON COLUMN FACT_PAR.ID_FACT_FUN IS 'Ссылка на вызов функции';
COMMENT ON COLUMN FACT_PAR.ID_ARG IS 'Ссылка на аргумент функции';
COMMENT ON COLUMN FACT_PAR.ID_VAL_CONST IS 'Значение - константа';
COMMENT ON COLUMN FACT_PAR.ID_VAL_FUNCT IS 'Значение - функция';
COMMENT ON COLUMN FACT_PAR.ID_VAL_FACT_FUN IS 'Значение - результат другого вызова';
COMMENT ON COLUMN FACT_PAR.VAL_NUM IS 'Числовое значение';
COMMENT ON COLUMN FACT_PAR.VAL_STR IS 'Строковое значение';
COMMENT ON COLUMN FACT_PAR.VAL_DATE IS 'Значение даты';
COMMENT ON COLUMN FACT_PAR.ID_VAL_ENUM IS 'Значение перечисления';
COMMENT ON COLUMN FACT_PAR.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 16. Решения для правил
-- ============================================================================

COMMENT ON TABLE DECISION_RULE IS 'Решения для правил';

CREATE TABLE IF NOT EXISTS DECISION_RULE (
    ID_FUNCT INTEGER NOT NULL,
    ID_PR INTEGER NOT NULL,
    NUM_CALL INTEGER NOT NULL,
    ID_FUNCT_DEC INTEGER NOT NULL,
    PRIORITET INTEGER DEFAULT 0,
    NOTE TEXT,
    CONSTRAINT PK_DECISION_RULE PRIMARY KEY (ID_FUNCT, ID_PR, NUM_CALL, ID_FUNCT_DEC),
    CONSTRAINT FK_DECISION_RULE_FUNCT FOREIGN KEY (ID_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE,
    CONSTRAINT FK_DECISION_RULE_PROD FOREIGN KEY (ID_PR) 
        REFERENCES PROD(ID_PR) ON DELETE CASCADE,
    CONSTRAINT FK_DECISION_RULE_FUNCT_DEC FOREIGN KEY (ID_FUNCT_DEC) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE
);

COMMENT ON COLUMN DECISION_RULE.ID_FUNCT IS 'Функция-правило';
COMMENT ON COLUMN DECISION_RULE.ID_PR IS 'Объект применения правила';
COMMENT ON COLUMN DECISION_RULE.NUM_CALL IS 'Номер вызова';
COMMENT ON COLUMN DECISION_RULE.ID_FUNCT_DEC IS 'Функция-решение';
COMMENT ON COLUMN DECISION_RULE.PRIORITET IS 'Приоритет выполнения';
COMMENT ON COLUMN DECISION_RULE.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- Конец скрипта создания таблиц
-- ============================================================================

-- ============================================================================
-- Схема базы данных для системы работы с тарифами на услуги
-- СУБД: PostgreSQL 12+
-- Описание: Основные таблицы системы
-- ============================================================================

-- Создание базы данных (выполнить отдельно от имени superuser)
-- CREATE DATABASE tariff_system ENCODING 'UTF8' LC_COLLATE='ru_RU.UTF-8' LC_CTYPE='ru_RU.UTF-8';

SET client_encoding = 'UTF8';

-- ============================================================================
-- 1. Единицы измерения
-- ============================================================================

CREATE TABLE IF NOT EXISTS EI (
    ID_EI SERIAL PRIMARY KEY,
    COD_EI VARCHAR(20) NOT NULL UNIQUE,
    NAME_EI VARCHAR(100) NOT NULL,
    NOTE TEXT
);

COMMENT ON TABLE EI IS 'Справочник единиц измерения';
COMMENT ON COLUMN EI.ID_EI IS 'Уникальный идентификатор единицы измерения';
COMMENT ON COLUMN EI.COD_EI IS 'Код единицы измерения (например, кг, м, ч)';
COMMENT ON COLUMN EI.NAME_EI IS 'Полное наименование единицы измерения';
COMMENT ON COLUMN EI.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 2. Перечисления
-- ============================================================================

CREATE TABLE IF NOT EXISTS ENUM_VAL_R (
    ID_ENUM SERIAL PRIMARY KEY,
    COD_ENUM VARCHAR(50) NOT NULL UNIQUE,
    NAME_ENUM VARCHAR(200) NOT NULL,
    NOTE TEXT
);

COMMENT ON TABLE ENUM_VAL_R IS 'Справочник перечислений для параметров';
COMMENT ON COLUMN ENUM_VAL_R.ID_ENUM IS 'Уникальный идентификатор перечисления';
COMMENT ON COLUMN ENUM_VAL_R.COD_ENUM IS 'Код перечисления (например, VEHICLE_CLASS)';
COMMENT ON COLUMN ENUM_VAL_R.NAME_ENUM IS 'Наименование перечисления';
COMMENT ON COLUMN ENUM_VAL_R.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 3. Позиции перечислений
-- ============================================================================

CREATE TABLE IF NOT EXISTS POS_ENUM (
    ID_POS_ENUM SERIAL PRIMARY KEY,
    ID_ENUM INTEGER NOT NULL,
    COD_POS VARCHAR(50) NOT NULL,
    NAME_POS VARCHAR(200) NOT NULL,
    NUM_POS INTEGER NOT NULL DEFAULT 0,
    NOTE TEXT,
    CONSTRAINT UK_POS_ENUM UNIQUE (ID_ENUM, COD_POS),
    CONSTRAINT FK_POS_ENUM_ENUM FOREIGN KEY (ID_ENUM) 
        REFERENCES ENUM_VAL_R(ID_ENUM) ON DELETE CASCADE
);

COMMENT ON TABLE POS_ENUM IS 'Значения (позиции) перечислений';
COMMENT ON COLUMN POS_ENUM.ID_POS_ENUM IS 'Уникальный идентификатор позиции';
COMMENT ON COLUMN POS_ENUM.ID_ENUM IS 'Ссылка на перечисление';
COMMENT ON COLUMN POS_ENUM.COD_POS IS 'Код позиции (например, CLOSED, OPEN)';
COMMENT ON COLUMN POS_ENUM.NAME_POS IS 'Наименование позиции';
COMMENT ON COLUMN POS_ENUM.NUM_POS IS 'Порядковый номер позиции';
COMMENT ON COLUMN POS_ENUM.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 4. Классификатор компонентов системы
-- ============================================================================

CREATE TABLE IF NOT EXISTS CHEM_CLASS (
    ID_CHEM SERIAL PRIMARY KEY,
    COD_CHEM VARCHAR(50) NOT NULL UNIQUE,
    NAME_CHEM VARCHAR(200) NOT NULL,
    PARENT_ID INTEGER,
    LEV INTEGER NOT NULL DEFAULT 0,
    NOTE TEXT,
    CONSTRAINT FK_CHEM_CLASS_PARENT FOREIGN KEY (PARENT_ID) 
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE CASCADE
);

COMMENT ON TABLE CHEM_CLASS IS 'Иерархический классификатор компонентов системы';
COMMENT ON COLUMN CHEM_CLASS.ID_CHEM IS 'Уникальный идентификатор класса';
COMMENT ON COLUMN CHEM_CLASS.COD_CHEM IS 'Код класса (например, SERVICE, TARIFF, ORDER)';
COMMENT ON COLUMN CHEM_CLASS.NAME_CHEM IS 'Наименование класса';
COMMENT ON COLUMN CHEM_CLASS.PARENT_ID IS 'Ссылка на родительский класс';
COMMENT ON COLUMN CHEM_CLASS.LEV IS 'Уровень в иерархии (0 - корень)';
COMMENT ON COLUMN CHEM_CLASS.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 5. Функции и правила
-- ============================================================================

CREATE TABLE IF NOT EXISTS FUNCT_R (
    ID_FUNCT SERIAL PRIMARY KEY,
    COD_FUNCT VARCHAR(50) NOT NULL UNIQUE,
    NAME_FUNCT VARCHAR(200) NOT NULL,
    TYPE_F INTEGER NOT NULL DEFAULT 0,
    OPERATION VARCHAR(20),
    NOTE TEXT
);

COMMENT ON TABLE FUNCT_R IS 'Функции и правила для вычислений';
COMMENT ON COLUMN FUNCT_R.ID_FUNCT IS 'Уникальный идентификатор функции';
COMMENT ON COLUMN FUNCT_R.COD_FUNCT IS 'Код функции';
COMMENT ON COLUMN FUNCT_R.NAME_FUNCT IS 'Наименование функции';
COMMENT ON COLUMN FUNCT_R.TYPE_F IS 'Тип функции: 0-предикат, 1-арифметическое, 2-логическое, 3-выбор';
COMMENT ON COLUMN FUNCT_R.OPERATION IS 'Операция: +, -, *, /, <, <=, =, >=, >, AND, OR, NOT, IF';
COMMENT ON COLUMN FUNCT_R.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 6. Аргументы функций
-- ============================================================================

CREATE TABLE IF NOT EXISTS ARG_FUNCT (
    ID_ARG SERIAL PRIMARY KEY,
    ID_FUNCT INTEGER NOT NULL,
    NUM_ARG INTEGER NOT NULL DEFAULT 1,
    CLASS_ARG INTEGER,
    NAME_ARG VARCHAR(200),
    NOTE TEXT,
    CONSTRAINT UK_ARG_FUNCT UNIQUE (ID_FUNCT, NUM_ARG),
    CONSTRAINT FK_ARG_FUNCT_FUNCT FOREIGN KEY (ID_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE,
    CONSTRAINT FK_ARG_FUNCT_CLASS FOREIGN KEY (CLASS_ARG) 
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE SET NULL
);

COMMENT ON TABLE ARG_FUNCT IS 'Аргументы функций';
COMMENT ON COLUMN ARG_FUNCT.ID_ARG IS 'Уникальный идентификатор аргумента';
COMMENT ON COLUMN ARG_FUNCT.ID_FUNCT IS 'Ссылка на функцию';
COMMENT ON COLUMN ARG_FUNCT.NUM_ARG IS 'Порядковый номер аргумента';
COMMENT ON COLUMN ARG_FUNCT.CLASS_ARG IS 'Класс аргумента (тип)';
COMMENT ON COLUMN ARG_FUNCT.NAME_ARG IS 'Наименование аргумента';
COMMENT ON COLUMN ARG_FUNCT.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 7. Объекты предметной области
-- ============================================================================

CREATE TABLE IF NOT EXISTS PROD (
    ID_PR SERIAL PRIMARY KEY,
    CLASS_PR INTEGER NOT NULL,
    COD_PR VARCHAR(50) NOT NULL UNIQUE,
    NAME_PR VARCHAR(200) NOT NULL,
    PAR_PR INTEGER,
    NOTE TEXT,
    CONSTRAINT FK_PROD_CLASS FOREIGN KEY (CLASS_PR) 
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE RESTRICT,
    CONSTRAINT FK_PROD_PARENT FOREIGN KEY (PAR_PR) 
        REFERENCES PROD(ID_PR) ON DELETE CASCADE
);

COMMENT ON TABLE PROD IS 'Объекты предметной области (услуги, тарифы, заказы)';
COMMENT ON COLUMN PROD.ID_PR IS 'Уникальный идентификатор объекта';
COMMENT ON COLUMN PROD.CLASS_PR IS 'Класс объекта';
COMMENT ON COLUMN PROD.COD_PR IS 'Код объекта';
COMMENT ON COLUMN PROD.NAME_PR IS 'Наименование объекта';
COMMENT ON COLUMN PROD.PAR_PR IS 'Родительский объект';
COMMENT ON COLUMN PROD.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 8. Константы
-- ============================================================================

CREATE TABLE IF NOT EXISTS CONST (
    ID_CONST SERIAL PRIMARY KEY,
    COD_CONST VARCHAR(50) NOT NULL UNIQUE,
    NAME_CONST VARCHAR(200) NOT NULL,
    VAL_NUM DOUBLE PRECISION,
    VAL_STR TEXT,
    VAL_DATE DATE,
    EI INTEGER,
    NOTE TEXT,
    CONSTRAINT FK_CONST_EI FOREIGN KEY (EI) 
        REFERENCES EI(ID_EI) ON DELETE SET NULL
);

COMMENT ON TABLE CONST IS 'Константы для использования в функциях';
COMMENT ON COLUMN CONST.ID_CONST IS 'Уникальный идентификатор константы';
COMMENT ON COLUMN CONST.COD_CONST IS 'Код константы';
COMMENT ON COLUMN CONST.NAME_CONST IS 'Наименование константы';
COMMENT ON COLUMN CONST.VAL_NUM IS 'Числовое значение';
COMMENT ON COLUMN CONST.VAL_STR IS 'Строковое значение';
COMMENT ON COLUMN CONST.VAL_DATE IS 'Значение даты';
COMMENT ON COLUMN CONST.EI IS 'Единица измерения';
COMMENT ON COLUMN CONST.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 9. Определения параметров
-- ============================================================================

CREATE TABLE IF NOT EXISTS PARAMETR1 (
    ID_PAR SERIAL PRIMARY KEY,
    COD_PAR VARCHAR(50) NOT NULL UNIQUE,
    NAME_PAR VARCHAR(200) NOT NULL,
    CLASS_PAR INTEGER,
    TYPE_PAR INTEGER NOT NULL DEFAULT 0,
    EI INTEGER,
    NOTE TEXT,
    CONSTRAINT FK_PARAMETR1_CLASS FOREIGN KEY (CLASS_PAR) 
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE SET NULL,
    CONSTRAINT FK_PARAMETR1_EI FOREIGN KEY (EI) 
        REFERENCES EI(ID_EI) ON DELETE SET NULL
);

COMMENT ON TABLE PARAMETR1 IS 'Определения параметров для объектов';
COMMENT ON COLUMN PARAMETR1.ID_PAR IS 'Уникальный идентификатор параметра';
COMMENT ON COLUMN PARAMETR1.COD_PAR IS 'Код параметра';
COMMENT ON COLUMN PARAMETR1.NAME_PAR IS 'Наименование параметра';
COMMENT ON COLUMN PARAMETR1.CLASS_PAR IS 'Класс параметра';
COMMENT ON COLUMN PARAMETR1.TYPE_PAR IS 'Тип параметра: 0-число, 1-строка, 2-дата, 3-перечисление';
COMMENT ON COLUMN PARAMETR1.EI IS 'Единица измерения';
COMMENT ON COLUMN PARAMETR1.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 10. Классы параметров
-- ============================================================================

COMMENT ON TABLE PAR_CLASS1 IS 'Связь параметров с классами';

CREATE TABLE IF NOT EXISTS PAR_CLASS1 (
    ID_PAR INTEGER NOT NULL,
    ID_CHEM INTEGER NOT NULL,
    CONSTRAINT PK_PAR_CLASS1 PRIMARY KEY (ID_PAR, ID_CHEM),
    CONSTRAINT FK_PAR_CLASS1_PAR FOREIGN KEY (ID_PAR) 
        REFERENCES PARAMETR1(ID_PAR) ON DELETE CASCADE,
    CONSTRAINT FK_PAR_CLASS1_CHEM FOREIGN KEY (ID_CHEM) 
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE CASCADE
);

COMMENT ON COLUMN PAR_CLASS1.ID_PAR IS 'Ссылка на параметр';
COMMENT ON COLUMN PAR_CLASS1.ID_CHEM IS 'Ссылка на класс';

-- ============================================================================
-- 11. Значения параметров объектов
-- ============================================================================

COMMENT ON TABLE PAR_PROD2 IS 'Значения параметров для конкретных объектов';

CREATE TABLE IF NOT EXISTS PAR_PROD2 (
    ID_PAR INTEGER NOT NULL,
    ID_PR INTEGER NOT NULL,
    VAL_NUM DOUBLE PRECISION,
    VAL_STR TEXT,
    VAL_DATE DATE,
    ID_VAL_ENUM INTEGER,
    IS_REQUIRE INTEGER DEFAULT 0,
    NOTE TEXT,
    CONSTRAINT PK_PAR_PROD2 PRIMARY KEY (ID_PAR, ID_PR),
    CONSTRAINT FK_PAR_PROD2_PAR FOREIGN KEY (ID_PAR) 
        REFERENCES PARAMETR1(ID_PAR) ON DELETE CASCADE,
    CONSTRAINT FK_PAR_PROD2_PROD FOREIGN KEY (ID_PR) 
        REFERENCES PROD(ID_PR) ON DELETE CASCADE,
    CONSTRAINT FK_PAR_PROD2_ENUM FOREIGN KEY (ID_VAL_ENUM) 
        REFERENCES POS_ENUM(ID_POS_ENUM) ON DELETE SET NULL
);

COMMENT ON COLUMN PAR_PROD2.ID_PAR IS 'Ссылка на параметр';
COMMENT ON COLUMN PAR_PROD2.ID_PR IS 'Ссылка на объект';
COMMENT ON COLUMN PAR_PROD2.VAL_NUM IS 'Числовое значение';
COMMENT ON COLUMN PAR_PROD2.VAL_STR IS 'Строковое значение';
COMMENT ON COLUMN PAR_PROD2.VAL_DATE IS 'Значение даты';
COMMENT ON COLUMN PAR_PROD2.ID_VAL_ENUM IS 'Значение перечисления';
COMMENT ON COLUMN PAR_PROD2.IS_REQUIRE IS 'Признак обязательности (0-нет, 1-да)';
COMMENT ON COLUMN PAR_PROD2.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 12. Значения функций для объектов (роли)
-- ============================================================================

COMMENT ON TABLE ROLE_VAL IS 'Значения функций для объектов';

CREATE TABLE IF NOT EXISTS ROLE_VAL (
    ID_FUNCT INTEGER NOT NULL,
    ID_PR INTEGER NOT NULL,
    ID_VAL_CONST INTEGER,
    ID_VAL_FUNCT INTEGER,
    VAL_NUM DOUBLE PRECISION,
    VAL_STR TEXT,
    VAL_DATE DATE,
    ID_VAL_ENUM INTEGER,
    NOTE TEXT,
    CONSTRAINT PK_ROLE_VAL PRIMARY KEY (ID_FUNCT, ID_PR),
    CONSTRAINT FK_ROLE_VAL_FUNCT FOREIGN KEY (ID_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE,
    CONSTRAINT FK_ROLE_VAL_PROD FOREIGN KEY (ID_PR) 
        REFERENCES PROD(ID_PR) ON DELETE CASCADE,
    CONSTRAINT FK_ROLE_VAL_CONST FOREIGN KEY (ID_VAL_CONST) 
        REFERENCES CONST(ID_CONST) ON DELETE SET NULL,
    CONSTRAINT FK_ROLE_VAL_FUNCT_VAL FOREIGN KEY (ID_VAL_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE SET NULL,
    CONSTRAINT FK_ROLE_VAL_ENUM FOREIGN KEY (ID_VAL_ENUM) 
        REFERENCES POS_ENUM(ID_POS_ENUM) ON DELETE SET NULL
);

COMMENT ON COLUMN ROLE_VAL.ID_FUNCT IS 'Ссылка на функцию';
COMMENT ON COLUMN ROLE_VAL.ID_PR IS 'Ссылка на объект';
COMMENT ON COLUMN ROLE_VAL.ID_VAL_CONST IS 'Значение - константа';
COMMENT ON COLUMN ROLE_VAL.ID_VAL_FUNCT IS 'Значение - функция';
COMMENT ON COLUMN ROLE_VAL.VAL_NUM IS 'Числовое значение';
COMMENT ON COLUMN ROLE_VAL.VAL_STR IS 'Строковое значение';
COMMENT ON COLUMN ROLE_VAL.VAL_DATE IS 'Значение даты';
COMMENT ON COLUMN ROLE_VAL.ID_VAL_ENUM IS 'Значение перечисления';
COMMENT ON COLUMN ROLE_VAL.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 13. Композитные функции
-- ============================================================================

COMMENT ON TABLE FUN_COMP IS 'Композиция функций';

CREATE TABLE IF NOT EXISTS FUN_COMP (
    ID_FUNCT INTEGER NOT NULL,
    ID_FUNCT_COMP INTEGER NOT NULL,
    CONSTRAINT PK_FUN_COMP PRIMARY KEY (ID_FUNCT, ID_FUNCT_COMP),
    CONSTRAINT FK_FUN_COMP_FUNCT FOREIGN KEY (ID_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE,
    CONSTRAINT FK_FUN_COMP_FUNCT_COMP FOREIGN KEY (ID_FUNCT_COMP) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE
);

COMMENT ON COLUMN FUN_COMP.ID_FUNCT IS 'Композитная функция';
COMMENT ON COLUMN FUN_COMP.ID_FUNCT_COMP IS 'Функция-компонент';

-- ============================================================================
-- 14. Вызовы функций
-- ============================================================================

COMMENT ON TABLE FACT_FUN IS 'Фактические вызовы функций';

CREATE TABLE IF NOT EXISTS FACT_FUN (
    ID_FACT_FUN SERIAL PRIMARY KEY,
    ID_FUNCT INTEGER NOT NULL,
    ID_PR INTEGER NOT NULL,
    NUM_CALL INTEGER NOT NULL DEFAULT 1,
    NOTE TEXT,
    CONSTRAINT UK_FACT_FUN UNIQUE (ID_FUNCT, ID_PR, NUM_CALL),
    CONSTRAINT FK_FACT_FUN_FUNCT FOREIGN KEY (ID_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE,
    CONSTRAINT FK_FACT_FUN_PROD FOREIGN KEY (ID_PR) 
        REFERENCES PROD(ID_PR) ON DELETE CASCADE
);

COMMENT ON COLUMN FACT_FUN.ID_FACT_FUN IS 'Уникальный идентификатор вызова';
COMMENT ON COLUMN FACT_FUN.ID_FUNCT IS 'Вызываемая функция';
COMMENT ON COLUMN FACT_FUN.ID_PR IS 'Объект-контекст вызова';
COMMENT ON COLUMN FACT_FUN.NUM_CALL IS 'Номер вызова';
COMMENT ON COLUMN FACT_FUN.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 15. Фактические параметры вызовов
-- ============================================================================

COMMENT ON TABLE FACT_PAR IS 'Фактические параметры вызовов функций';

CREATE TABLE IF NOT EXISTS FACT_PAR (
    ID_FACT_FUN INTEGER NOT NULL,
    ID_ARG INTEGER NOT NULL,
    ID_VAL_CONST INTEGER,
    ID_VAL_FUNCT INTEGER,
    ID_VAL_FACT_FUN INTEGER,
    VAL_NUM DOUBLE PRECISION,
    VAL_STR TEXT,
    VAL_DATE DATE,
    ID_VAL_ENUM INTEGER,
    NOTE TEXT,
    CONSTRAINT PK_FACT_PAR PRIMARY KEY (ID_FACT_FUN, ID_ARG),
    CONSTRAINT FK_FACT_PAR_FACT_FUN FOREIGN KEY (ID_FACT_FUN) 
        REFERENCES FACT_FUN(ID_FACT_FUN) ON DELETE CASCADE,
    CONSTRAINT FK_FACT_PAR_ARG FOREIGN KEY (ID_ARG) 
        REFERENCES ARG_FUNCT(ID_ARG) ON DELETE CASCADE,
    CONSTRAINT FK_FACT_PAR_CONST FOREIGN KEY (ID_VAL_CONST) 
        REFERENCES CONST(ID_CONST) ON DELETE SET NULL,
    CONSTRAINT FK_FACT_PAR_FUNCT FOREIGN KEY (ID_VAL_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE SET NULL,
    CONSTRAINT FK_FACT_PAR_FACT_FUN_VAL FOREIGN KEY (ID_VAL_FACT_FUN) 
        REFERENCES FACT_FUN(ID_FACT_FUN) ON DELETE SET NULL,
    CONSTRAINT FK_FACT_PAR_ENUM FOREIGN KEY (ID_VAL_ENUM) 
        REFERENCES POS_ENUM(ID_POS_ENUM) ON DELETE SET NULL
);

COMMENT ON COLUMN FACT_PAR.ID_FACT_FUN IS 'Ссылка на вызов функции';
COMMENT ON COLUMN FACT_PAR.ID_ARG IS 'Ссылка на аргумент функции';
COMMENT ON COLUMN FACT_PAR.ID_VAL_CONST IS 'Значение - константа';
COMMENT ON COLUMN FACT_PAR.ID_VAL_FUNCT IS 'Значение - функция';
COMMENT ON COLUMN FACT_PAR.ID_VAL_FACT_FUN IS 'Значение - результат другого вызова';
COMMENT ON COLUMN FACT_PAR.VAL_NUM IS 'Числовое значение';
COMMENT ON COLUMN FACT_PAR.VAL_STR IS 'Строковое значение';
COMMENT ON COLUMN FACT_PAR.VAL_DATE IS 'Значение даты';
COMMENT ON COLUMN FACT_PAR.ID_VAL_ENUM IS 'Значение перечисления';
COMMENT ON COLUMN FACT_PAR.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 16. Решения для правил
-- ============================================================================

COMMENT ON TABLE DECISION_RULE IS 'Решения для правил';

CREATE TABLE IF NOT EXISTS DECISION_RULE (
    ID_FUNCT INTEGER NOT NULL,
    ID_PR INTEGER NOT NULL,
    NUM_CALL INTEGER NOT NULL,
    ID_FUNCT_DEC INTEGER NOT NULL,
    PRIORITET INTEGER DEFAULT 0,
    NOTE TEXT,
    CONSTRAINT PK_DECISION_RULE PRIMARY KEY (ID_FUNCT, ID_PR, NUM_CALL, ID_FUNCT_DEC),
    CONSTRAINT FK_DECISION_RULE_FUNCT FOREIGN KEY (ID_FUNCT) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE,
    CONSTRAINT FK_DECISION_RULE_PROD FOREIGN KEY (ID_PR) 
        REFERENCES PROD(ID_PR) ON DELETE CASCADE,
    CONSTRAINT FK_DECISION_RULE_FUNCT_DEC FOREIGN KEY (ID_FUNCT_DEC) 
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE
);

COMMENT ON COLUMN DECISION_RULE.ID_FUNCT IS 'Функция-правило';
COMMENT ON COLUMN DECISION_RULE.ID_PR IS 'Объект применения правила';
COMMENT ON COLUMN DECISION_RULE.NUM_CALL IS 'Номер вызова';
COMMENT ON COLUMN DECISION_RULE.ID_FUNCT_DEC IS 'Функция-решение';
COMMENT ON COLUMN DECISION_RULE.PRIORITET IS 'Приоритет выполнения';
COMMENT ON COLUMN DECISION_RULE.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 17. Типы услуг
-- ============================================================================

COMMENT ON TABLE SERVICE_TYPE IS 'Справочник типов услуг';

CREATE TABLE IF NOT EXISTS SERVICE_TYPE (
    ID_SERVICE_TYPE SERIAL PRIMARY KEY,
    COD_SERVICE VARCHAR(50) NOT NULL UNIQUE,
    NAME_SERVICE VARCHAR(200) NOT NULL,
    ID_CLASS INTEGER NOT NULL,
    NOTE TEXT,
    CONSTRAINT FK_SERVICE_TYPE_CLASS FOREIGN KEY (ID_CLASS)
        REFERENCES CHEM_CLASS(ID_CHEM) ON DELETE RESTRICT
);

COMMENT ON COLUMN SERVICE_TYPE.ID_SERVICE_TYPE IS 'Уникальный идентификатор типа услуги';
COMMENT ON COLUMN SERVICE_TYPE.COD_SERVICE IS 'Код типа услуги';
COMMENT ON COLUMN SERVICE_TYPE.NAME_SERVICE IS 'Наименование типа услуги';
COMMENT ON COLUMN SERVICE_TYPE.ID_CLASS IS 'Класс услуги в классификаторе';
COMMENT ON COLUMN SERVICE_TYPE.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 18. Параметры типов услуг (шаблон)
-- ============================================================================

COMMENT ON TABLE SERVICE_TYPE_PARAM IS 'Параметры типов услуг';

CREATE TABLE IF NOT EXISTS SERVICE_TYPE_PARAM (
    ID_SERVICE_TYPE INTEGER NOT NULL,
    ID_PAR INTEGER NOT NULL,
    IS_REQUIRED INTEGER DEFAULT 0,
    DEFAULT_VAL_NUM DOUBLE PRECISION,
    DEFAULT_VAL_STR TEXT,
    MIN_VAL DOUBLE PRECISION,
    MAX_VAL DOUBLE PRECISION,
    CONSTRAINT PK_SERVICE_TYPE_PARAM PRIMARY KEY (ID_SERVICE_TYPE, ID_PAR),
    CONSTRAINT FK_SERVICE_TYPE_PARAM_ST FOREIGN KEY (ID_SERVICE_TYPE)
        REFERENCES SERVICE_TYPE(ID_SERVICE_TYPE) ON DELETE CASCADE,
    CONSTRAINT FK_SERVICE_TYPE_PARAM_PAR FOREIGN KEY (ID_PAR)
        REFERENCES PARAMETR1(ID_PAR) ON DELETE CASCADE
);

COMMENT ON COLUMN SERVICE_TYPE_PARAM.ID_SERVICE_TYPE IS 'Ссылка на тип услуги';
COMMENT ON COLUMN SERVICE_TYPE_PARAM.ID_PAR IS 'Ссылка на параметр';
COMMENT ON COLUMN SERVICE_TYPE_PARAM.IS_REQUIRED IS 'Обязательность: 0-нет, 1-да';
COMMENT ON COLUMN SERVICE_TYPE_PARAM.DEFAULT_VAL_NUM IS 'Значение по умолчанию (числовое)';
COMMENT ON COLUMN SERVICE_TYPE_PARAM.DEFAULT_VAL_STR IS 'Значение по умолчанию (строковое)';
COMMENT ON COLUMN SERVICE_TYPE_PARAM.MIN_VAL IS 'Минимальное допустимое значение';
COMMENT ON COLUMN SERVICE_TYPE_PARAM.MAX_VAL IS 'Максимальное допустимое значение';

-- ============================================================================
-- 19. Исполнители услуг
-- ============================================================================

COMMENT ON TABLE EXECUTOR IS 'Справочник исполнителей услуг';

CREATE TABLE IF NOT EXISTS EXECUTOR (
    ID_EXECUTOR SERIAL PRIMARY KEY,
    COD_EXECUTOR VARCHAR(50) NOT NULL UNIQUE,
    NAME_EXECUTOR VARCHAR(200) NOT NULL,
    ADDRESS TEXT,
    PHONE VARCHAR(50),
    EMAIL VARCHAR(100),
    IS_ACTIVE INTEGER DEFAULT 1,
    NOTE TEXT
);

COMMENT ON COLUMN EXECUTOR.ID_EXECUTOR IS 'Уникальный идентификатор исполнителя';
COMMENT ON COLUMN EXECUTOR.COD_EXECUTOR IS 'Код исполнителя';
COMMENT ON COLUMN EXECUTOR.NAME_EXECUTOR IS 'Наименование исполнителя';
COMMENT ON COLUMN EXECUTOR.ADDRESS IS 'Адрес';
COMMENT ON COLUMN EXECUTOR.PHONE IS 'Телефон';
COMMENT ON COLUMN EXECUTOR.EMAIL IS 'Электронная почта';
COMMENT ON COLUMN EXECUTOR.IS_ACTIVE IS 'Активен: 0-нет, 1-да';
COMMENT ON COLUMN EXECUTOR.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 20. Тарифы на услуги
-- ============================================================================

COMMENT ON TABLE TARIFF IS 'Тарифы на услуги';

CREATE TABLE IF NOT EXISTS TARIFF (
    ID_TARIFF SERIAL PRIMARY KEY,
    ID_SERVICE_TYPE INTEGER NOT NULL,
    ID_EXECUTOR INTEGER,
    COD_TARIFF VARCHAR(50) NOT NULL UNIQUE,
    NAME_TARIFF VARCHAR(200) NOT NULL,
    DATE_BEGIN DATE NOT NULL DEFAULT CURRENT_DATE,
    DATE_END DATE,
    IS_WITH_VAT INTEGER DEFAULT 1,
    VAT_RATE DOUBLE PRECISION DEFAULT 20.0,
    IS_ACTIVE INTEGER DEFAULT 1,
    NOTE TEXT,
    CONSTRAINT FK_TARIFF_SERVICE_TYPE FOREIGN KEY (ID_SERVICE_TYPE)
        REFERENCES SERVICE_TYPE(ID_SERVICE_TYPE) ON DELETE RESTRICT,
    CONSTRAINT FK_TARIFF_EXECUTOR FOREIGN KEY (ID_EXECUTOR)
        REFERENCES EXECUTOR(ID_EXECUTOR) ON DELETE SET NULL
);

COMMENT ON COLUMN TARIFF.ID_TARIFF IS 'Уникальный идентификатор тарифа';
COMMENT ON COLUMN TARIFF.ID_SERVICE_TYPE IS 'Тип услуги';
COMMENT ON COLUMN TARIFF.ID_EXECUTOR IS 'Исполнитель (владелец тарифа)';
COMMENT ON COLUMN TARIFF.COD_TARIFF IS 'Код тарифа';
COMMENT ON COLUMN TARIFF.NAME_TARIFF IS 'Наименование тарифа';
COMMENT ON COLUMN TARIFF.DATE_BEGIN IS 'Дата начала действия';
COMMENT ON COLUMN TARIFF.DATE_END IS 'Дата окончания действия';
COMMENT ON COLUMN TARIFF.IS_WITH_VAT IS 'С НДС: 0-нет, 1-да';
COMMENT ON COLUMN TARIFF.VAT_RATE IS 'Ставка НДС в процентах';
COMMENT ON COLUMN TARIFF.IS_ACTIVE IS 'Активен: 0-нет, 1-да';
COMMENT ON COLUMN TARIFF.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 21. Ставки тарифов
-- ============================================================================

COMMENT ON TABLE TARIFF_RATE IS 'Ставки тарифов';

CREATE TABLE IF NOT EXISTS TARIFF_RATE (
    ID_TARIFF_RATE SERIAL PRIMARY KEY,
    ID_TARIFF INTEGER NOT NULL,
    COD_RATE VARCHAR(50) NOT NULL,
    NAME_RATE VARCHAR(200) NOT NULL,
    RATE_VALUE DOUBLE PRECISION NOT NULL,
    ID_EI INTEGER,
    ID_CONDITION_ENUM INTEGER,
    NOTE TEXT,
    CONSTRAINT FK_TARIFF_RATE_TARIFF FOREIGN KEY (ID_TARIFF)
        REFERENCES TARIFF(ID_TARIFF) ON DELETE CASCADE,
    CONSTRAINT FK_TARIFF_RATE_EI FOREIGN KEY (ID_EI)
        REFERENCES EI(ID_EI) ON DELETE SET NULL,
    CONSTRAINT FK_TARIFF_RATE_ENUM FOREIGN KEY (ID_CONDITION_ENUM)
        REFERENCES POS_ENUM(ID_POS_ENUM) ON DELETE SET NULL
);

COMMENT ON COLUMN TARIFF_RATE.ID_TARIFF_RATE IS 'Уникальный идентификатор ставки';
COMMENT ON COLUMN TARIFF_RATE.ID_TARIFF IS 'Ссылка на тариф';
COMMENT ON COLUMN TARIFF_RATE.COD_RATE IS 'Код ставки';
COMMENT ON COLUMN TARIFF_RATE.NAME_RATE IS 'Наименование ставки';
COMMENT ON COLUMN TARIFF_RATE.RATE_VALUE IS 'Значение ставки';
COMMENT ON COLUMN TARIFF_RATE.ID_EI IS 'Единица измерения';
COMMENT ON COLUMN TARIFF_RATE.ID_CONDITION_ENUM IS 'Условие применения (перечисление)';
COMMENT ON COLUMN TARIFF_RATE.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 22. Правила расчета тарифов
-- ============================================================================

COMMENT ON TABLE TARIFF_RULE IS 'Правила расчета тарифов';

CREATE TABLE IF NOT EXISTS TARIFF_RULE (
    ID_TARIFF_RULE SERIAL PRIMARY KEY,
    ID_TARIFF INTEGER NOT NULL,
    ID_FUNCT INTEGER NOT NULL,
    PRIORITY INTEGER DEFAULT 0,
    IS_ACTIVE INTEGER DEFAULT 1,
    NOTE TEXT,
    CONSTRAINT FK_TARIFF_RULE_TARIFF FOREIGN KEY (ID_TARIFF)
        REFERENCES TARIFF(ID_TARIFF) ON DELETE CASCADE,
    CONSTRAINT FK_TARIFF_RULE_FUNCT FOREIGN KEY (ID_FUNCT)
        REFERENCES FUNCT_R(ID_FUNCT) ON DELETE CASCADE
);

COMMENT ON COLUMN TARIFF_RULE.ID_TARIFF_RULE IS 'Уникальный идентификатор правила';
COMMENT ON COLUMN TARIFF_RULE.ID_TARIFF IS 'Ссылка на тариф';
COMMENT ON COLUMN TARIFF_RULE.ID_FUNCT IS 'Ссылка на функцию расчета';
COMMENT ON COLUMN TARIFF_RULE.PRIORITY IS 'Приоритет применения';
COMMENT ON COLUMN TARIFF_RULE.IS_ACTIVE IS 'Активен: 0-нет, 1-да';
COMMENT ON COLUMN TARIFF_RULE.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 23. Заказы на услуги
-- ============================================================================

COMMENT ON TABLE SERVICE_ORDER IS 'Заказы на услуги';

CREATE TABLE IF NOT EXISTS SERVICE_ORDER (
    ID_ORDER SERIAL PRIMARY KEY,
    COD_ORDER VARCHAR(50) NOT NULL UNIQUE,
    ID_SERVICE_TYPE INTEGER NOT NULL,
    ORDER_DATE DATE NOT NULL DEFAULT CURRENT_DATE,
    EXECUTION_DATE DATE,
    STATUS INTEGER DEFAULT 0,
    ID_EXECUTOR INTEGER,
    ID_TARIFF INTEGER,
    TOTAL_COST DOUBLE PRECISION,
    NOTE TEXT,
    CONSTRAINT FK_ORDER_SERVICE_TYPE FOREIGN KEY (ID_SERVICE_TYPE)
        REFERENCES SERVICE_TYPE(ID_SERVICE_TYPE) ON DELETE RESTRICT,
    CONSTRAINT FK_ORDER_EXECUTOR FOREIGN KEY (ID_EXECUTOR)
        REFERENCES EXECUTOR(ID_EXECUTOR) ON DELETE SET NULL,
    CONSTRAINT FK_ORDER_TARIFF FOREIGN KEY (ID_TARIFF)
        REFERENCES TARIFF(ID_TARIFF) ON DELETE SET NULL
);

COMMENT ON COLUMN SERVICE_ORDER.ID_ORDER IS 'Уникальный идентификатор заказа';
COMMENT ON COLUMN SERVICE_ORDER.COD_ORDER IS 'Код (номер) заказа';
COMMENT ON COLUMN SERVICE_ORDER.ID_SERVICE_TYPE IS 'Тип услуги';
COMMENT ON COLUMN SERVICE_ORDER.ORDER_DATE IS 'Дата создания заказа';
COMMENT ON COLUMN SERVICE_ORDER.EXECUTION_DATE IS 'Дата исполнения';
COMMENT ON COLUMN SERVICE_ORDER.STATUS IS 'Статус: 0-новый, 1-в работе, 2-выполнен, 3-отменен';
COMMENT ON COLUMN SERVICE_ORDER.ID_EXECUTOR IS 'Исполнитель';
COMMENT ON COLUMN SERVICE_ORDER.ID_TARIFF IS 'Примененный тариф';
COMMENT ON COLUMN SERVICE_ORDER.TOTAL_COST IS 'Итоговая стоимость';
COMMENT ON COLUMN SERVICE_ORDER.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 24. Параметры заказа
-- ============================================================================

COMMENT ON TABLE ORDER_PARAM IS 'Параметры заказа';

CREATE TABLE IF NOT EXISTS ORDER_PARAM (
    ID_ORDER INTEGER NOT NULL,
    ID_PAR INTEGER NOT NULL,
    VAL_NUM DOUBLE PRECISION,
    VAL_STR TEXT,
    VAL_DATE DATE,
    ID_VAL_ENUM INTEGER,
    CONSTRAINT PK_ORDER_PARAM PRIMARY KEY (ID_ORDER, ID_PAR),
    CONSTRAINT FK_ORDER_PARAM_ORDER FOREIGN KEY (ID_ORDER)
        REFERENCES SERVICE_ORDER(ID_ORDER) ON DELETE CASCADE,
    CONSTRAINT FK_ORDER_PARAM_PAR FOREIGN KEY (ID_PAR)
        REFERENCES PARAMETR1(ID_PAR) ON DELETE CASCADE,
    CONSTRAINT FK_ORDER_PARAM_ENUM FOREIGN KEY (ID_VAL_ENUM)
        REFERENCES POS_ENUM(ID_POS_ENUM) ON DELETE SET NULL
);

COMMENT ON COLUMN ORDER_PARAM.ID_ORDER IS 'Ссылка на заказ';
COMMENT ON COLUMN ORDER_PARAM.ID_PAR IS 'Ссылка на параметр';
COMMENT ON COLUMN ORDER_PARAM.VAL_NUM IS 'Числовое значение';
COMMENT ON COLUMN ORDER_PARAM.VAL_STR IS 'Строковое значение';
COMMENT ON COLUMN ORDER_PARAM.VAL_DATE IS 'Значение даты';
COMMENT ON COLUMN ORDER_PARAM.ID_VAL_ENUM IS 'Значение перечисления';

-- ============================================================================
-- 25. Позиции заказа (для нескольких услуг в одном заказе)
-- ============================================================================

COMMENT ON TABLE ORDER_ITEM IS 'Позиции заказа';

CREATE TABLE IF NOT EXISTS ORDER_ITEM (
    ID_ORDER_ITEM SERIAL PRIMARY KEY,
    ID_ORDER INTEGER NOT NULL,
    ID_SERVICE_TYPE INTEGER NOT NULL,
    QUANTITY DOUBLE PRECISION DEFAULT 1,
    UNIT_COST DOUBLE PRECISION,
    TOTAL_COST DOUBLE PRECISION,
    NOTE TEXT,
    CONSTRAINT FK_ORDER_ITEM_ORDER FOREIGN KEY (ID_ORDER)
        REFERENCES SERVICE_ORDER(ID_ORDER) ON DELETE CASCADE,
    CONSTRAINT FK_ORDER_ITEM_SERVICE_TYPE FOREIGN KEY (ID_SERVICE_TYPE)
        REFERENCES SERVICE_TYPE(ID_SERVICE_TYPE) ON DELETE RESTRICT
);

COMMENT ON COLUMN ORDER_ITEM.ID_ORDER_ITEM IS 'Уникальный идентификатор позиции';
COMMENT ON COLUMN ORDER_ITEM.ID_ORDER IS 'Ссылка на заказ';
COMMENT ON COLUMN ORDER_ITEM.ID_SERVICE_TYPE IS 'Тип услуги';
COMMENT ON COLUMN ORDER_ITEM.QUANTITY IS 'Количество';
COMMENT ON COLUMN ORDER_ITEM.UNIT_COST IS 'Стоимость за единицу';
COMMENT ON COLUMN ORDER_ITEM.TOTAL_COST IS 'Общая стоимость';
COMMENT ON COLUMN ORDER_ITEM.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 26. Параметры позиции заказа
-- ============================================================================

COMMENT ON TABLE ORDER_ITEM_PARAM IS 'Параметры позиции заказа';

CREATE TABLE IF NOT EXISTS ORDER_ITEM_PARAM (
    ID_ORDER_ITEM INTEGER NOT NULL,
    ID_PAR INTEGER NOT NULL,
    VAL_NUM DOUBLE PRECISION,
    VAL_STR TEXT,
    VAL_DATE DATE,
    ID_VAL_ENUM INTEGER,
    CONSTRAINT PK_ORDER_ITEM_PARAM PRIMARY KEY (ID_ORDER_ITEM, ID_PAR),
    CONSTRAINT FK_ORDER_ITEM_PARAM_ITEM FOREIGN KEY (ID_ORDER_ITEM)
        REFERENCES ORDER_ITEM(ID_ORDER_ITEM) ON DELETE CASCADE,
    CONSTRAINT FK_ORDER_ITEM_PARAM_PAR FOREIGN KEY (ID_PAR)
        REFERENCES PARAMETR1(ID_PAR) ON DELETE CASCADE,
    CONSTRAINT FK_ORDER_ITEM_PARAM_ENUM FOREIGN KEY (ID_VAL_ENUM)
        REFERENCES POS_ENUM(ID_POS_ENUM) ON DELETE SET NULL
);

COMMENT ON COLUMN ORDER_ITEM_PARAM.ID_ORDER_ITEM IS 'Ссылка на позицию заказа';
COMMENT ON COLUMN ORDER_ITEM_PARAM.ID_PAR IS 'Ссылка на параметр';
COMMENT ON COLUMN ORDER_ITEM_PARAM.VAL_NUM IS 'Числовое значение';
COMMENT ON COLUMN ORDER_ITEM_PARAM.VAL_STR IS 'Строковое значение';
COMMENT ON COLUMN ORDER_ITEM_PARAM.VAL_DATE IS 'Значение даты';
COMMENT ON COLUMN ORDER_ITEM_PARAM.ID_VAL_ENUM IS 'Значение перечисления';

-- ============================================================================
-- 27. Повышающие коэффициенты
-- ============================================================================

COMMENT ON TABLE COEFFICIENT IS 'Повышающие коэффициенты';

CREATE TABLE IF NOT EXISTS COEFFICIENT (
    ID_COEFFICIENT SERIAL PRIMARY KEY,
    COD_COEFF VARCHAR(50) NOT NULL UNIQUE,
    NAME_COEFF VARCHAR(200) NOT NULL,
    VALUE_MIN DOUBLE PRECISION NOT NULL,
    VALUE_MAX DOUBLE PRECISION NOT NULL,
    VALUE_DEFAULT DOUBLE PRECISION NOT NULL,
    NOTE TEXT
);

COMMENT ON COLUMN COEFFICIENT.ID_COEFFICIENT IS 'Уникальный идентификатор';
COMMENT ON COLUMN COEFFICIENT.COD_COEFF IS 'Код коэффициента';
COMMENT ON COLUMN COEFFICIENT.NAME_COEFF IS 'Наименование';
COMMENT ON COLUMN COEFFICIENT.VALUE_MIN IS 'Минимальное значение';
COMMENT ON COLUMN COEFFICIENT.VALUE_MAX IS 'Максимальное значение';
COMMENT ON COLUMN COEFFICIENT.VALUE_DEFAULT IS 'Значение по умолчанию';
COMMENT ON COLUMN COEFFICIENT.NOTE IS 'Дополнительные примечания';

-- ============================================================================
-- 28. Связь тарифов с коэффициентами
-- ============================================================================

COMMENT ON TABLE TARIFF_COEFFICIENT IS 'Коэффициенты тарифов';

CREATE TABLE IF NOT EXISTS TARIFF_COEFFICIENT (
    ID_TARIFF INTEGER NOT NULL,
    ID_COEFFICIENT INTEGER NOT NULL,
    COEFF_VALUE DOUBLE PRECISION NOT NULL,
    CONSTRAINT PK_TARIFF_COEFFICIENT PRIMARY KEY (ID_TARIFF, ID_COEFFICIENT),
    CONSTRAINT FK_TARIFF_COEFF_TARIFF FOREIGN KEY (ID_TARIFF)
        REFERENCES TARIFF(ID_TARIFF) ON DELETE CASCADE,
    CONSTRAINT FK_TARIFF_COEFF_COEFF FOREIGN KEY (ID_COEFFICIENT)
        REFERENCES COEFFICIENT(ID_COEFFICIENT) ON DELETE CASCADE
);

COMMENT ON COLUMN TARIFF_COEFFICIENT.ID_TARIFF IS 'Ссылка на тариф';
COMMENT ON COLUMN TARIFF_COEFFICIENT.ID_COEFFICIENT IS 'Ссылка на коэффициент';
COMMENT ON COLUMN TARIFF_COEFFICIENT.COEFF_VALUE IS 'Значение коэффициента для данного тарифа';

-- ============================================================================
-- Конец скрипта создания таблиц
-- ============================================================================
