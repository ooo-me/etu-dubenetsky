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
-- Индексы для POS_ENUM
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_POS_ENUM_ENUM 
    ON POS_ENUM(ID_ENUM);

-- ============================================================================
-- Индексы для FACT_FUN
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_FACT_FUN_FUNCT 
    ON FACT_FUN(ID_FUNCT);

CREATE INDEX IF NOT EXISTS IDX_FACT_FUN_PR 
    ON FACT_FUN(ID_PR);

-- ============================================================================
-- Индексы для FACT_PAR
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_FACT_PAR_ARG 
    ON FACT_PAR(ID_ARG);

-- ============================================================================
-- Индексы для SERVICE_TYPE
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_SERVICE_TYPE_CLASS 
    ON SERVICE_TYPE(ID_CLASS);

CREATE INDEX IF NOT EXISTS IDX_SERVICE_TYPE_COD 
    ON SERVICE_TYPE(COD_SERVICE);

-- ============================================================================
-- Индексы для EXECUTOR
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_EXECUTOR_ACTIVE 
    ON EXECUTOR(IS_ACTIVE);

CREATE INDEX IF NOT EXISTS IDX_EXECUTOR_COD 
    ON EXECUTOR(COD_EXECUTOR);

-- ============================================================================
-- Индексы для TARIFF
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_TARIFF_SERVICE_TYPE 
    ON TARIFF(ID_SERVICE_TYPE);

CREATE INDEX IF NOT EXISTS IDX_TARIFF_EXECUTOR 
    ON TARIFF(ID_EXECUTOR);

CREATE INDEX IF NOT EXISTS IDX_TARIFF_ACTIVE 
    ON TARIFF(IS_ACTIVE);

CREATE INDEX IF NOT EXISTS IDX_TARIFF_DATE_RANGE 
    ON TARIFF(DATE_BEGIN, DATE_END);

CREATE INDEX IF NOT EXISTS IDX_TARIFF_COD 
    ON TARIFF(COD_TARIFF);

-- ============================================================================
-- Индексы для TARIFF_RATE
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_TARIFF_RATE_TARIFF 
    ON TARIFF_RATE(ID_TARIFF);

CREATE INDEX IF NOT EXISTS IDX_TARIFF_RATE_COD 
    ON TARIFF_RATE(COD_RATE);

-- ============================================================================
-- Индексы для TARIFF_RULE
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_TARIFF_RULE_TARIFF 
    ON TARIFF_RULE(ID_TARIFF);

CREATE INDEX IF NOT EXISTS IDX_TARIFF_RULE_FUNCT 
    ON TARIFF_RULE(ID_FUNCT);

-- ============================================================================
-- Индексы для SERVICE_ORDER
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_ORDER_SERVICE_TYPE 
    ON SERVICE_ORDER(ID_SERVICE_TYPE);

CREATE INDEX IF NOT EXISTS IDX_ORDER_EXECUTOR 
    ON SERVICE_ORDER(ID_EXECUTOR);

CREATE INDEX IF NOT EXISTS IDX_ORDER_STATUS 
    ON SERVICE_ORDER(STATUS);

CREATE INDEX IF NOT EXISTS IDX_ORDER_DATE 
    ON SERVICE_ORDER(ORDER_DATE);

CREATE INDEX IF NOT EXISTS IDX_ORDER_COD 
    ON SERVICE_ORDER(COD_ORDER);

-- ============================================================================
-- Индексы для ORDER_PARAM
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_ORDER_PARAM_ORDER 
    ON ORDER_PARAM(ID_ORDER);

CREATE INDEX IF NOT EXISTS IDX_ORDER_PARAM_PAR 
    ON ORDER_PARAM(ID_PAR);

-- ============================================================================
-- Индексы для ORDER_ITEM
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_ORDER_ITEM_ORDER 
    ON ORDER_ITEM(ID_ORDER);

CREATE INDEX IF NOT EXISTS IDX_ORDER_ITEM_SERVICE_TYPE 
    ON ORDER_ITEM(ID_SERVICE_TYPE);

-- ============================================================================
-- Индексы для ORDER_ITEM_PARAM
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_ORDER_ITEM_PARAM_ITEM 
    ON ORDER_ITEM_PARAM(ID_ORDER_ITEM);

CREATE INDEX IF NOT EXISTS IDX_ORDER_ITEM_PARAM_PAR 
    ON ORDER_ITEM_PARAM(ID_PAR);

-- ============================================================================
-- Индексы для COEFFICIENT
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_COEFFICIENT_COD 
    ON COEFFICIENT(COD_COEFF);

-- ============================================================================
-- Индексы для TARIFF_COEFFICIENT
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_TARIFF_COEFF_TARIFF 
    ON TARIFF_COEFFICIENT(ID_TARIFF);

CREATE INDEX IF NOT EXISTS IDX_TARIFF_COEFF_COEFF 
    ON TARIFF_COEFFICIENT(ID_COEFFICIENT);

-- ============================================================================
-- Индексы для SERVICE_TYPE_PARAM
-- ============================================================================

CREATE INDEX IF NOT EXISTS IDX_SERVICE_TYPE_PARAM_ST 
    ON SERVICE_TYPE_PARAM(ID_SERVICE_TYPE);

CREATE INDEX IF NOT EXISTS IDX_SERVICE_TYPE_PARAM_PAR 
    ON SERVICE_TYPE_PARAM(ID_PAR);

-- ============================================================================
-- Конец скрипта создания индексов
-- ============================================================================
