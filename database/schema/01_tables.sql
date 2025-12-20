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
