-- ============================================================================
-- Индексы и дополнительные ограничения
-- СУБД: PostgreSQL 12+
-- Описание: Создание индексов для оптимизации запросов
-- ============================================================================

-- ============================================================================
-- Индексы для CHEM_CLASS
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_CHEM_CLASS_PARENT 
    ON CHEM_CLASS(PARENT_ID);

CREATE INDEX IF NOT EXISTS IDX_CHEM_CLASS_LEV 
    ON CHEM_CLASS(LEV);

CREATE INDEX IF NOT EXISTS IDX_CHEM_CLASS_COD 
    ON CHEM_CLASS(COD_CHEM);

-- ============================================================================
-- Индексы для FUNCT_R
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_FUNCT_R_TYPE 
    ON FUNCT_R(TYPE_F);

CREATE INDEX IF NOT EXISTS IDX_FUNCT_R_COD 
    ON FUNCT_R(COD_FUNCT);

-- ============================================================================
-- Индексы для ARG_FUNCT
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_ARG_FUNCT_FUNCT 
    ON ARG_FUNCT(ID_FUNCT);

CREATE INDEX IF NOT EXISTS IDX_ARG_FUNCT_CLASS 
    ON ARG_FUNCT(CLASS_ARG);

-- ============================================================================
-- Индексы для PROD
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_PROD_CLASS 
    ON PROD(CLASS_PR);

CREATE INDEX IF NOT EXISTS IDX_PROD_PARENT 
    ON PROD(PAR_PR);

CREATE INDEX IF NOT EXISTS IDX_PROD_COD 
    ON PROD(COD_PR);

-- ============================================================================
-- Индексы для ROLE_VAL
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_ROLE_VAL_PROD 
    ON ROLE_VAL(ID_PR);

CREATE INDEX IF NOT EXISTS IDX_ROLE_VAL_FUNCT 
    ON ROLE_VAL(ID_FUNCT);

-- ============================================================================
-- Индексы для PARAMETR1
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_PARAMETR1_CLASS 
    ON PARAMETR1(CLASS_PAR);

CREATE INDEX IF NOT EXISTS IDX_PARAMETR1_TYPE 
    ON PARAMETR1(TYPE_PAR);

CREATE INDEX IF NOT EXISTS IDX_PARAMETR1_COD 
    ON PARAMETR1(COD_PAR);

-- ============================================================================
-- Индексы для PAR_CLASS1
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_PAR_CLASS1_PAR 
    ON PAR_CLASS1(ID_PAR);

CREATE INDEX IF NOT EXISTS IDX_PAR_CLASS1_CHEM 
    ON PAR_CLASS1(ID_CHEM);

-- ============================================================================
-- Индексы для PAR_PROD2
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_PAR_PROD2_PROD 
    ON PAR_PROD2(ID_PR);

CREATE INDEX IF NOT EXISTS IDX_PAR_PROD2_PAR 
    ON PAR_PROD2(ID_PAR);

-- ============================================================================
-- Индексы для CONST
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_CONST_COD 
    ON CONST(COD_CONST);

-- ============================================================================
-- Индексы для FACT_FUN
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_FACT_FUN_FUNCT 
    ON FACT_FUN(ID_FUNCT);

CREATE INDEX IF NOT EXISTS IDX_FACT_FUN_PROD 
    ON FACT_FUN(ID_PR);

CREATE INDEX IF NOT EXISTS IDX_FACT_FUN_FUNCT_PROD 
    ON FACT_FUN(ID_FUNCT, ID_PR);

-- ============================================================================
-- Индексы для FACT_PAR
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_FACT_PAR_FACT_FUN 
    ON FACT_PAR(ID_FACT_FUN);

CREATE INDEX IF NOT EXISTS IDX_FACT_PAR_ARG 
    ON FACT_PAR(ID_ARG);

-- ============================================================================
-- Индексы для DECISION_RULE
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_DECISION_RULE_FUNCT 
    ON DECISION_RULE(ID_FUNCT);

CREATE INDEX IF NOT EXISTS IDX_DECISION_RULE_PROD 
    ON DECISION_RULE(ID_PR);

CREATE INDEX IF NOT EXISTS IDX_DECISION_RULE_FUNCT_DEC 
    ON DECISION_RULE(ID_FUNCT_DEC);

CREATE INDEX IF NOT EXISTS IDX_DECISION_RULE_PRIORITET 
    ON DECISION_RULE(PRIORITET);

-- ============================================================================
-- Индексы для POS_ENUM
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_POS_ENUM_ENUM 
    ON POS_ENUM(ID_ENUM);

CREATE INDEX IF NOT EXISTS IDX_POS_ENUM_NUM 
    ON POS_ENUM(NUM_POS);

-- ============================================================================
-- Дополнительные ограничения целостности
-- ============================================================================

-- Проверка типа функции
ALTER TABLE FUNCT_R 
    ADD CONSTRAINT CHK_FUNCT_R_TYPE 
    CHECK (TYPE_F IN (0, 1, 2, 3));

-- Проверка типа параметра
ALTER TABLE PARAMETR1 
    ADD CONSTRAINT CHK_PARAMETR1_TYPE 
    CHECK (TYPE_PAR IN (0, 1, 2, 3));

-- Проверка уровня иерархии
ALTER TABLE CHEM_CLASS 
    ADD CONSTRAINT CHK_CHEM_CLASS_LEV 
    CHECK (LEV >= 0);

-- Проверка номера аргумента
ALTER TABLE ARG_FUNCT 
    ADD CONSTRAINT CHK_ARG_FUNCT_NUM 
    CHECK (NUM_ARG > 0);

-- Проверка номера вызова
ALTER TABLE FACT_FUN 
    ADD CONSTRAINT CHK_FACT_FUN_NUM 
    CHECK (NUM_CALL > 0);

-- Проверка приоритета
ALTER TABLE DECISION_RULE 
    ADD CONSTRAINT CHK_DECISION_RULE_PRIORITET 
    CHECK (PRIORITET >= 0);

-- ============================================================================
-- Конец скрипта создания индексов
-- ============================================================================
