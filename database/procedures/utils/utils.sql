-- ============================================================================
-- Утилиты и вспомогательные процедуры
-- СУБД: PostgreSQL 12+
-- Описание: Дополнительные процедуры для работы с системой
-- ============================================================================

-- ============================================================================
-- GET_ALL_EI - Получение всех единиц измерения
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_EI()
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    name VARCHAR,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT ID_EI, COD_EI, NAME_EI, EI.NOTE
    FROM EI
    ORDER BY NAME_EI;
END;
$$;

COMMENT ON FUNCTION GET_ALL_EI IS 'Получение всех единиц измерения';

-- ============================================================================
-- GET_ALL_ENUMS - Получение всех перечислений
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_ENUMS()
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    name VARCHAR,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT ID_ENUM, COD_ENUM, NAME_ENUM, ENUM_VAL_R.NOTE
    FROM ENUM_VAL_R
    ORDER BY NAME_ENUM;
END;
$$;

COMMENT ON FUNCTION GET_ALL_ENUMS IS 'Получение всех перечислений';

-- ============================================================================
-- GET_ENUM_VALUES - Получение значений перечисления
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ENUM_VALUES(p_id_enum INTEGER)
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    name VARCHAR,
    position INTEGER,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT ID_POS_ENUM, COD_POS, NAME_POS, NUM_POS, POS_ENUM.NOTE
    FROM POS_ENUM
    WHERE ID_ENUM = p_id_enum
    ORDER BY NUM_POS;
END;
$$;

COMMENT ON FUNCTION GET_ENUM_VALUES IS 'Получение значений перечисления';

-- ============================================================================
-- GET_ALL_CLASSES - Получение всех классов
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_CLASSES()
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    name VARCHAR,
    parent_id INTEGER,
    level INTEGER,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT ID_CHEM, COD_CHEM, NAME_CHEM, PARENT_ID, LEV, CHEM_CLASS.NOTE
    FROM CHEM_CLASS
    ORDER BY LEV, NAME_CHEM;
END;
$$;

COMMENT ON FUNCTION GET_ALL_CLASSES IS 'Получение всех классов';

-- ============================================================================
-- GET_ALL_PARAMETERS - Получение всех параметров
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_PARAMETERS()
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    name VARCHAR,
    class_id INTEGER,
    type INTEGER,
    type_name VARCHAR,
    unit_id INTEGER,
    unit_name VARCHAR,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT 
        p.ID_PAR,
        p.COD_PAR,
        p.NAME_PAR,
        p.CLASS_PAR,
        p.TYPE_PAR,
        CASE p.TYPE_PAR
            WHEN 0 THEN 'Число'::VARCHAR
            WHEN 1 THEN 'Строка'::VARCHAR
            WHEN 2 THEN 'Дата'::VARCHAR
            WHEN 3 THEN 'Перечисление'::VARCHAR
            ELSE 'Неизвестно'::VARCHAR
        END,
        p.EI,
        e.NAME_EI,
        p.NOTE
    FROM PARAMETR1 p
    LEFT JOIN EI e ON p.EI = e.ID_EI
    ORDER BY p.NAME_PAR;
END;
$$;

COMMENT ON FUNCTION GET_ALL_PARAMETERS IS 'Получение всех параметров';

-- ============================================================================
-- DEL_PARAMETR - Удаление параметра
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_PARAMETR(p_id_par INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM PARAMETR1 WHERE ID_PAR = p_id_par;
END;
$$;

COMMENT ON FUNCTION DEL_PARAMETR IS 'Удаление параметра';

-- ============================================================================
-- GET_ALL_SERVICE_TYPES - Получение всех типов услуг
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_SERVICE_TYPES()
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    name VARCHAR,
    class_id INTEGER,
    class_name VARCHAR,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT 
        st.ID_SERVICE_TYPE,
        st.COD_SERVICE,
        st.NAME_SERVICE,
        st.ID_CLASS,
        c.NAME_CHEM,
        st.NOTE
    FROM SERVICE_TYPE st
    LEFT JOIN CHEM_CLASS c ON st.ID_CLASS = c.ID_CHEM
    ORDER BY st.NAME_SERVICE;
END;
$$;

COMMENT ON FUNCTION GET_ALL_SERVICE_TYPES IS 'Получение всех типов услуг';

-- ============================================================================
-- INS_SERVICE_TYPE - Создание типа услуги
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_SERVICE_TYPE(
    p_cod_service VARCHAR,
    p_name_service VARCHAR,
    p_id_class INTEGER,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id INTEGER;
BEGIN
    INSERT INTO SERVICE_TYPE (COD_SERVICE, NAME_SERVICE, ID_CLASS, NOTE)
    VALUES (p_cod_service, p_name_service, p_id_class, p_note)
    RETURNING ID_SERVICE_TYPE INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_SERVICE_TYPE IS 'Создание типа услуги';

-- ============================================================================
-- UPD_SERVICE_TYPE - Обновление типа услуги
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_SERVICE_TYPE(
    p_id INTEGER,
    p_cod_service VARCHAR DEFAULT NULL,
    p_name_service VARCHAR DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE SERVICE_TYPE
    SET COD_SERVICE = COALESCE(p_cod_service, COD_SERVICE),
        NAME_SERVICE = COALESCE(p_name_service, NAME_SERVICE),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_SERVICE_TYPE = p_id;
END;
$$;

COMMENT ON FUNCTION UPD_SERVICE_TYPE IS 'Обновление типа услуги';

-- ============================================================================
-- DEL_SERVICE_TYPE - Удаление типа услуги
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_SERVICE_TYPE(p_id INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM SERVICE_TYPE WHERE ID_SERVICE_TYPE = p_id;
END;
$$;

COMMENT ON FUNCTION DEL_SERVICE_TYPE IS 'Удаление типа услуги';

-- ============================================================================
-- INS_SERVICE_TYPE_PARAM - Добавление параметра к типу услуги
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_SERVICE_TYPE_PARAM(
    p_id_service_type INTEGER,
    p_id_par INTEGER,
    p_is_required INTEGER DEFAULT 0,
    p_default_val_num DOUBLE PRECISION DEFAULT NULL,
    p_default_val_str TEXT DEFAULT NULL,
    p_min_val DOUBLE PRECISION DEFAULT NULL,
    p_max_val DOUBLE PRECISION DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    INSERT INTO SERVICE_TYPE_PARAM (ID_SERVICE_TYPE, ID_PAR, IS_REQUIRED, 
                                     DEFAULT_VAL_NUM, DEFAULT_VAL_STR, MIN_VAL, MAX_VAL)
    VALUES (p_id_service_type, p_id_par, p_is_required, 
            p_default_val_num, p_default_val_str, p_min_val, p_max_val)
    ON CONFLICT (ID_SERVICE_TYPE, ID_PAR) DO UPDATE
    SET IS_REQUIRED = p_is_required,
        DEFAULT_VAL_NUM = p_default_val_num,
        DEFAULT_VAL_STR = p_default_val_str,
        MIN_VAL = p_min_val,
        MAX_VAL = p_max_val;
END;
$$;

COMMENT ON FUNCTION INS_SERVICE_TYPE_PARAM IS 'Добавление параметра к типу услуги';

-- ============================================================================
-- GET_SERVICE_TYPE_PARAMS - Получение параметров типа услуги
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_SERVICE_TYPE_PARAMS(p_id_service_type INTEGER)
RETURNS TABLE (
    par_id INTEGER,
    code VARCHAR,
    name VARCHAR,
    type INTEGER,
    is_required INTEGER,
    default_val_num DOUBLE PRECISION,
    default_val_str TEXT,
    min_val DOUBLE PRECISION,
    max_val DOUBLE PRECISION,
    unit_name VARCHAR
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT 
        p.ID_PAR,
        p.COD_PAR,
        p.NAME_PAR,
        p.TYPE_PAR,
        stp.IS_REQUIRED,
        stp.DEFAULT_VAL_NUM,
        stp.DEFAULT_VAL_STR,
        stp.MIN_VAL,
        stp.MAX_VAL,
        e.NAME_EI
    FROM SERVICE_TYPE_PARAM stp
    JOIN PARAMETR1 p ON stp.ID_PAR = p.ID_PAR
    LEFT JOIN EI e ON p.EI = e.ID_EI
    WHERE stp.ID_SERVICE_TYPE = p_id_service_type
    ORDER BY p.NAME_PAR;
END;
$$;

COMMENT ON FUNCTION GET_SERVICE_TYPE_PARAMS IS 'Получение параметров типа услуги';

-- ============================================================================
-- GET_ALL_EXECUTORS - Получение всех исполнителей
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_EXECUTORS()
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    name VARCHAR,
    address TEXT,
    phone VARCHAR,
    email VARCHAR,
    is_active INTEGER,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT ID_EXECUTOR, COD_EXECUTOR, NAME_EXECUTOR, ADDRESS, PHONE, EMAIL, IS_ACTIVE, EXECUTOR.NOTE
    FROM EXECUTOR
    ORDER BY NAME_EXECUTOR;
END;
$$;

COMMENT ON FUNCTION GET_ALL_EXECUTORS IS 'Получение всех исполнителей';

-- ============================================================================
-- INS_EXECUTOR - Создание исполнителя
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_EXECUTOR(
    p_cod_executor VARCHAR,
    p_name_executor VARCHAR,
    p_address TEXT DEFAULT NULL,
    p_phone VARCHAR DEFAULT NULL,
    p_email VARCHAR DEFAULT NULL,
    p_is_active INTEGER DEFAULT 1,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id INTEGER;
BEGIN
    INSERT INTO EXECUTOR (COD_EXECUTOR, NAME_EXECUTOR, ADDRESS, PHONE, EMAIL, IS_ACTIVE, NOTE)
    VALUES (p_cod_executor, p_name_executor, p_address, p_phone, p_email, p_is_active, p_note)
    RETURNING ID_EXECUTOR INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_EXECUTOR IS 'Создание исполнителя';

-- ============================================================================
-- UPD_EXECUTOR - Обновление исполнителя
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_EXECUTOR(
    p_id INTEGER,
    p_cod_executor VARCHAR DEFAULT NULL,
    p_name_executor VARCHAR DEFAULT NULL,
    p_address TEXT DEFAULT NULL,
    p_phone VARCHAR DEFAULT NULL,
    p_email VARCHAR DEFAULT NULL,
    p_is_active INTEGER DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE EXECUTOR
    SET COD_EXECUTOR = COALESCE(p_cod_executor, COD_EXECUTOR),
        NAME_EXECUTOR = COALESCE(p_name_executor, NAME_EXECUTOR),
        ADDRESS = COALESCE(p_address, ADDRESS),
        PHONE = COALESCE(p_phone, PHONE),
        EMAIL = COALESCE(p_email, EMAIL),
        IS_ACTIVE = COALESCE(p_is_active, IS_ACTIVE),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_EXECUTOR = p_id;
END;
$$;

COMMENT ON FUNCTION UPD_EXECUTOR IS 'Обновление исполнителя';

-- ============================================================================
-- DEL_EXECUTOR - Удаление исполнителя
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_EXECUTOR(p_id INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM EXECUTOR WHERE ID_EXECUTOR = p_id;
END;
$$;

COMMENT ON FUNCTION DEL_EXECUTOR IS 'Удаление исполнителя';

-- ============================================================================
-- GET_ALL_TARIFFS - Получение всех тарифов
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_TARIFFS()
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    name VARCHAR,
    service_type_id INTEGER,
    service_name VARCHAR,
    executor_id INTEGER,
    executor_name VARCHAR,
    date_begin TEXT,
    date_end TEXT,
    is_with_vat INTEGER,
    vat_rate DOUBLE PRECISION,
    is_active INTEGER,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT 
        t.ID_TARIFF,
        t.COD_TARIFF,
        t.NAME_TARIFF,
        t.ID_SERVICE_TYPE,
        st.NAME_SERVICE,
        t.ID_EXECUTOR,
        e.NAME_EXECUTOR,
        t.DATE_BEGIN::TEXT,
        t.DATE_END::TEXT,
        t.IS_WITH_VAT,
        t.VAT_RATE,
        t.IS_ACTIVE,
        t.NOTE
    FROM TARIFF t
    LEFT JOIN SERVICE_TYPE st ON t.ID_SERVICE_TYPE = st.ID_SERVICE_TYPE
    LEFT JOIN EXECUTOR e ON t.ID_EXECUTOR = e.ID_EXECUTOR
    ORDER BY t.NAME_TARIFF;
END;
$$;

COMMENT ON FUNCTION GET_ALL_TARIFFS IS 'Получение всех тарифов';

-- ============================================================================
-- INS_TARIFF - Создание тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_TARIFF(
    p_id_service_type INTEGER,
    p_cod_tariff VARCHAR,
    p_name_tariff VARCHAR,
    p_id_executor INTEGER DEFAULT NULL,
    p_date_begin TEXT DEFAULT NULL,
    p_date_end TEXT DEFAULT NULL,
    p_is_with_vat INTEGER DEFAULT 1,
    p_vat_rate DOUBLE PRECISION DEFAULT 20.0,
    p_is_active INTEGER DEFAULT 1,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id INTEGER;
    v_date_begin DATE;
    v_date_end DATE;
BEGIN
    v_date_begin := CASE WHEN p_date_begin IS NULL OR p_date_begin = 'NULL' THEN CURRENT_DATE ELSE p_date_begin::DATE END;
    v_date_end := CASE WHEN p_date_end IS NULL OR p_date_end = 'NULL' THEN NULL ELSE p_date_end::DATE END;
    
    INSERT INTO TARIFF (ID_SERVICE_TYPE, COD_TARIFF, NAME_TARIFF, ID_EXECUTOR, 
                        DATE_BEGIN, DATE_END, IS_WITH_VAT, VAT_RATE, IS_ACTIVE, NOTE)
    VALUES (p_id_service_type, p_cod_tariff, p_name_tariff, 
            CASE WHEN p_id_executor = 0 THEN NULL ELSE p_id_executor END,
            v_date_begin, v_date_end, p_is_with_vat, p_vat_rate, p_is_active, p_note)
    RETURNING ID_TARIFF INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_TARIFF IS 'Создание тарифа';

-- ============================================================================
-- UPD_TARIFF - Обновление тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_TARIFF(
    p_id INTEGER,
    p_cod_tariff VARCHAR DEFAULT NULL,
    p_name_tariff VARCHAR DEFAULT NULL,
    p_id_executor INTEGER DEFAULT NULL,
    p_date_begin TEXT DEFAULT NULL,
    p_date_end TEXT DEFAULT NULL,
    p_is_with_vat INTEGER DEFAULT NULL,
    p_vat_rate DOUBLE PRECISION DEFAULT NULL,
    p_is_active INTEGER DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE TARIFF
    SET COD_TARIFF = COALESCE(p_cod_tariff, COD_TАРИФ),
        NAME_TARIFF = COALESCE(p_name_tariff, NAME_TАРИФ),
        ID_EXECUTOR = CASE WHEN p_id_executor = 0 THEN NULL ELSE COALESCE(p_id_executor, ID_EXECUTOR) END,
        DATE_BEGIN = CASE WHEN p_date_begin IS NULL OR p_date_begin = 'NULL' THEN DATE_BEGIN ELSE p_date_begin::DATE END,
        DATE_END = CASE WHEN p_date_end IS NULL OR p_date_end = 'NULL' THEN DATE_END ELSE p_date_end::DATE END,
        IS_WITH_VAT = COALESCE(p_is_with_vat, IS_WITH_VAT),
        VAT_RATE = COALESCE(p_vat_rate, VAT_RATE),
        IS_ACTIVE = COALESCE(p_is_active, IS_ACTIVE),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_TARIFF = p_id;
END;
$$;

COMMENT ON FUNCTION UPD_TARIFF IS 'Обновление тарифа';

-- ============================================================================
-- DEL_TARIFF - Удаление тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_TARIFF(p_id INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM TARIFF WHERE ID_TARIFF = p_id;
END;
$$;

COMMENT ON FUNCTION DEL_TARIFF IS 'Удаление тарифа';

-- ============================================================================
-- INS_TARIFF_RATE - Создание ставки тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_TARIFF_RATE(
    p_id_tariff INTEGER,
    p_cod_rate VARCHAR,
    p_name_rate VARCHAR,
    p_rate_value DOUBLE PRECISION,
    p_id_ei INTEGER DEFAULT NULL,
    p_id_condition_enum INTEGER DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id INTEGER;
BEGIN
    INSERT INTO TARIFF_RATE (ID_TARIFF, COD_RATE, NAME_RATE, RATE_VALUE, ID_EI, ID_CONDITION_ENUM, NOTE)
    VALUES (p_id_tariff, p_cod_rate, p_name_rate, p_rate_value, 
            CASE WHEN p_id_ei = 0 THEN NULL ELSE p_id_ei END,
            CASE WHEN p_id_condition_enum = 0 THEN NULL ELSE p_id_condition_enum END,
            p_note)
    RETURNING ID_TARIFF_RATE INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_TARIFF_RATE IS 'Создание ставки тарифа';

-- ============================================================================
-- UPD_TARIFF_RATE - Обновление ставки тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_TARIFF_RATE(
    p_id INTEGER,
    p_cod_rate VARCHAR DEFAULT NULL,
    p_name_rate VARCHAR DEFAULT NULL,
    p_rate_value DOUBLE PRECISION DEFAULT NULL,
    p_id_ei INTEGER DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE TARIFF_RATE
    SET COD_RATE = COALESCE(p_cod_rate, COD_RATE),
        NAME_RATE = COALESCE(p_name_rate, NAME_RATE),
        RATE_VALUE = COALESCE(p_rate_value, RATE_VALUE),
        ID_EI = CASE WHEN p_id_ei = 0 THEN NULL ELSE COALESCE(p_id_ei, ID_EI) END,
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_TARIFF_RATE = p_id;
END;
$$;

COMMENT ON FUNCTION UPD_TARIFF_RATE IS 'Обновление ставки тарифа';

-- ============================================================================
-- DEL_TARIFF_RATE - Удаление ставки тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_TARIFF_RATE(p_id INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM TARIFF_RATE WHERE ID_TARIFF_RATE = p_id;
END;
$$;

COMMENT ON FUNCTION DEL_TARIFF_RATE IS 'Удаление ставки тарифа';

-- ============================================================================
-- GET_TARIFF_RATES - Получение ставок тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_TARIFF_RATES(p_id_tariff INTEGER)
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    name VARCHAR,
    value DOUBLE PRECISION,
    unit_id INTEGER,
    unit_name VARCHAR,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT 
        tr.ID_TARIFF_RATE,
        tr.COD_RATE,
        tr.NAME_RATE,
        tr.RATE_VALUE,
        tr.ID_EI,
        e.NAME_EI,
        tr.NOTE
    FROM TARIFF_RATE tr
    LEFT JOIN EI e ON tr.ID_EI = e.ID_EI
    WHERE tr.ID_TARIFF = p_id_tariff
    ORDER BY tr.NAME_RATE;
END;
$$;

COMMENT ON FUNCTION GET_TARIFF_RATES IS 'Получение ставок тарифа';

-- ============================================================================
-- INS_TARIFF_COEFFICIENT - Добавление коэффициента к тарифу
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_TARIFF_COEFFICIENT(
    p_id_tariff INTEGER,
    p_id_coefficient INTEGER,
    p_coeff_value DOUBLE PRECISION
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    INSERT INTO TARIFF_COEFFICIENT (ID_TARIFF, ID_COEFFICIENT, COEFF_VALUE)
    VALUES (p_id_tariff, p_id_coefficient, p_coeff_value)
    ON CONFLICT (ID_TARIFF, ID_COEFFICIENT) DO UPDATE
    SET COEFF_VALUE = p_coeff_value;
END;
$$;

COMMENT ON FUNCTION INS_TARIFF_COEFFICIENT IS 'Добавление коэффициента к тарифу';

-- ============================================================================
-- GET_ALL_ORDERS - Получение всех заказов
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_ORDERS()
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    service_type_id INTEGER,
    service_name VARCHAR,
    order_date TEXT,
    execution_date TEXT,
    status INTEGER,
    status_name VARCHAR,
    executor_id INTEGER,
    executor_name VARCHAR,
    tariff_id INTEGER,
    tariff_name VARCHAR,
    total_cost DOUBLE PRECISION,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT 
        so.ID_ORDER,
        so.COD_ORDER,
        so.ID_SERVICE_TYPE,
        st.NAME_SERVICE,
        so.ORDER_DATE::TEXT,
        so.EXECUTION_DATE::TEXT,
        so.STATUS,
        CASE so.STATUS
            WHEN 0 THEN 'Новый'::VARCHAR
            WHEN 1 THEN 'В работе'::VARCHAR
            WHEN 2 THEN 'Выполнен'::VARCHAR
            WHEN 3 THEN 'Отменен'::VARCHAR
            ELSE 'Неизвестно'::VARCHAR
        END,
        so.ID_EXECUTOR,
        e.NAME_EXECUTOR,
        so.ID_TARIFF,
        t.NAME_TARIFF,
        so.TOTAL_COST,
        so.NOTE
    FROM SERVICE_ORDER so
    LEFT JOIN SERVICE_TYPE st ON so.ID_SERVICE_TYPE = st.ID_SERVICE_TYPE
    LEFT JOIN EXECUTOR e ON so.ID_EXECUTOR = e.ID_EXECUTOR
    LEFT JOIN TARIFF t ON so.ID_TARIFF = t.ID_TARIFF
    ORDER BY so.ORDER_DATE DESC, so.ID_ORDER DESC;
END;
$$;

COMMENT ON FUNCTION GET_ALL_ORDERS IS 'Получение всех заказов';

-- ============================================================================
-- INS_ORDER - Создание заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_ORDER(
    p_cod_order VARCHAR,
    p_id_service_type INTEGER,
    p_order_date TEXT DEFAULT NULL,
    p_execution_date TEXT DEFAULT NULL,
    p_status INTEGER DEFAULT 0,
    p_id_executor INTEGER DEFAULT NULL,
    p_id_tariff INTEGER DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id INTEGER;
    v_order_date DATE;
    v_execution_date DATE;
BEGIN
    v_order_date := CASE WHEN p_order_date IS NULL OR p_order_date = 'NULL' THEN CURRENT_DATE ELSE p_order_date::DATE END;
    v_execution_date := CASE WHEN p_execution_date IS NULL OR p_execution_date = 'NULL' THEN NULL ELSE p_execution_date::DATE END;
    
    INSERT INTO SERVICE_ORDER (COD_ORDER, ID_SERVICE_TYPE, ORDER_DATE, EXECUTION_DATE, STATUS, 
                               ID_EXECUTOR, ID_TARIFF, NOTE)
    VALUES (p_cod_order, p_id_service_type, v_order_date, v_execution_date, p_status,
            CASE WHEN p_id_executor = 0 THEN NULL ELSE p_id_executor END,
            CASE WHEN p_id_tariff = 0 THEN NULL ELSE p_id_tariff END,
            p_note)
    RETURNING ID_ORDER INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_ORDER IS 'Создание заказа';

-- ============================================================================
-- UPD_ORDER - Обновление заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_ORDER(
    p_id INTEGER,
    p_cod_order VARCHAR DEFAULT NULL,
    p_execution_date TEXT DEFAULT NULL,
    p_status INTEGER DEFAULT NULL,
    p_id_executor INTEGER DEFAULT NULL,
    p_id_tariff INTEGER DEFAULT NULL,
    p_total_cost DOUBLE PRECISION DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE SERVICE_ORDER
    SET COD_ORDER = COALESCE(p_cod_order, COD_ORDER),
        EXECUTION_DATE = CASE WHEN p_execution_date IS NULL OR p_execution_date = 'NULL' THEN EXECUTION_DATE ELSE p_execution_date::DATE END,
        STATUS = COALESCE(p_status, STATUS),
        ID_EXECUTOR = CASE WHEN p_id_executor = 0 THEN NULL ELSE COALESCE(p_id_executor, ID_EXECUTOR) END,
        ID_TARIFF = CASE WHEN p_id_tariff = 0 THEN NULL ELSE COALESCE(p_id_tariff, ID_TARIFF) END,
        TOTAL_COST = COALESCE(p_total_cost, TOTAL_COST),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_ORDER = p_id;
END;
$$;

COMMENT ON FUNCTION UPD_ORDER IS 'Обновление заказа';

-- ============================================================================
-- DEL_ORDER - Удаление заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_ORDER(p_id INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM SERVICE_ORDER WHERE ID_ORDER = p_id;
END;
$$;

COMMENT ON FUNCTION DEL_ORDER IS 'Удаление заказа';

-- ============================================================================
-- INS_ORDER_PARAM - Установка параметра заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_ORDER_PARAM(
    p_id_order INTEGER,
    p_id_par INTEGER,
    p_val_num DOUBLE PRECISION DEFAULT NULL,
    p_val_str TEXT DEFAULT NULL,
    p_val_date TEXT DEFAULT NULL,
    p_id_val_enum INTEGER DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
DECLARE
    v_val_date DATE;
BEGIN
    v_val_date := CASE WHEN p_val_date IS NULL OR p_val_date = 'NULL' THEN NULL ELSE p_val_date::DATE END;
    
    INSERT INTO ORDER_PARAM (ID_ORDER, ID_PAR, VAL_NUM, VAL_STR, VAL_DATE, ID_VAL_ENUM)
    VALUES (p_id_order, p_id_par, p_val_num, 
            CASE WHEN p_val_str = 'NULL' THEN NULL ELSE p_val_str END, 
            v_val_date,
            CASE WHEN p_id_val_enum = 0 THEN NULL ELSE p_id_val_enum END)
    ON CONFLICT (ID_ORDER, ID_PAR) DO UPDATE
    SET VAL_NUM = p_val_num,
        VAL_STR = CASE WHEN p_val_str = 'NULL' THEN NULL ELSE p_val_str END,
        VAL_DATE = v_val_date,
        ID_VAL_ENUM = CASE WHEN p_id_val_enum = 0 THEN NULL ELSE p_id_val_enum END;
END;
$$;

COMMENT ON FUNCTION INS_ORDER_PARAM IS 'Установка параметра заказа';

-- ============================================================================
-- GET_ORDER_PARAMS - Получение параметров заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ORDER_PARAMS(p_id_order INTEGER)
RETURNS TABLE (
    par_id INTEGER,
    code VARCHAR,
    name VARCHAR,
    type INTEGER,
    val_num DOUBLE PRECISION,
    val_str TEXT,
    val_date TEXT,
    enum_id INTEGER,
    enum_name VARCHAR,
    unit_name VARCHAR
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT 
        p.ID_PAR,
        p.COD_PAR,
        p.NAME_PAR,
        p.TYPE_PAR,
        op.VAL_NUM,
        op.VAL_STR,
        op.VAL_DATE::TEXT,
        op.ID_VAL_ENUM,
        pe.NAME_POS,
        e.NAME_EI
    FROM ORDER_PARAM op
    JOIN PARAMETR1 p ON op.ID_PAR = p.ID_PAR
    LEFT JOIN EI e ON p.EI = e.ID_EI
    LEFT JOIN POS_ENUM pe ON op.ID_VAL_ENUM = pe.ID_POS_ENUM
    WHERE op.ID_ORDER = p_id_order
    ORDER BY p.NAME_PAR;
END;
$$;

COMMENT ON FUNCTION GET_ORDER_PARAMS IS 'Получение параметров заказа';

-- ============================================================================
-- GET_ALL_COEFFICIENTS - Получение всех коэффициентов
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_COEFFICIENTS()
RETURNS TABLE (
    id INTEGER,
    code VARCHAR,
    name VARCHAR,
    value_min DOUBLE PRECISION,
    value_max DOUBLE PRECISION,
    value_default DOUBLE PRECISION,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT ID_COEFFICIENT, COD_COEFF, NAME_COEFF, VALUE_MIN, VALUE_MAX, VALUE_DEFAULT, COEFFICIENT.NOTE
    FROM COEFFICIENT
    ORDER BY NAME_COEFF;
END;
$$;

COMMENT ON FUNCTION GET_ALL_COEFFICIENTS IS 'Получение всех коэффициентов';

-- ============================================================================
-- INS_COEFFICIENT - Создание коэффициента
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_COEFFICIENT(
    p_cod_coeff VARCHAR,
    p_name_coeff VARCHAR,
    p_value_min DOUBLE PRECISION,
    p_value_max DOUBLE PRECISION,
    p_value_default DOUBLE PRECISION,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id INTEGER;
BEGIN
    INSERT INTO COEFFICIENT (COD_COEFF, NAME_COEFF, VALUE_MIN, VALUE_MAX, VALUE_DEFAULT, NOTE)
    VALUES (p_cod_coeff, p_name_coeff, p_value_min, p_value_max, p_value_default, p_note)
    RETURNING ID_COEFFICIENT INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_COEFFICIENT IS 'Создание коэффициента';

-- ============================================================================
-- UPD_COEFFICIENT - Обновление коэффициента
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_COEFFICIENT(
    p_id INTEGER,
    p_cod_coeff VARCHAR DEFAULT NULL,
    p_name_coeff VARCHAR DEFAULT NULL,
    p_value_min DOUBLE PRECISION DEFAULT NULL,
    p_value_max DOUBLE PRECISION DEFAULT NULL,
    p_value_default DOUBLE PRECISION DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE COEFFICIENT
    SET COD_COEFF = COALESCE(p_cod_coeff, COD_COEFF),
        NAME_COEFF = COALESCE(p_name_coeff, NAME_COEFF),
        VALUE_MIN = COALESCE(p_value_min, VALUE_MIN),
        VALUE_MAX = COALESCE(p_value_max, VALUE_MAX),
        VALUE_DEFAULT = COALESCE(p_value_default, VALUE_DEFAULT),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_COEFFICIENT = p_id;
END;
$$;

COMMENT ON FUNCTION UPD_COEFFICIENT IS 'Обновление коэффициента';

-- ============================================================================
-- DEL_COEFFICIENT - Удаление коэффициента
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_COEFFICIENT(p_id INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM COEFFICIENT WHERE ID_COEFFICIENT = p_id;
END;
$$;

COMMENT ON FUNCTION DEL_COEFFICIENT IS 'Удаление коэффициента';

-- ============================================================================
-- CALC_ORDER_COST - Расчет стоимости заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION CALC_ORDER_COST(
    p_id_order INTEGER,
    p_id_tariff INTEGER DEFAULT NULL
)
RETURNS DOUBLE PRECISION
LANGUAGE plpgsql
AS $$
DECLARE
    v_tariff_id INTEGER;
    v_total_cost DOUBLE PRECISION := 0;
    v_service_type_id INTEGER;
    v_is_with_vat INTEGER;
    v_vat_rate DOUBLE PRECISION;
    rec_param RECORD;
    rec_rate RECORD;
    v_param_value DOUBLE PRECISION;
BEGIN
    -- Получаем ID тарифа (из параметра или из заказа)
    IF p_id_tariff IS NOT NULL THEN
        v_tariff_id := p_id_tariff;
    ELSE
        SELECT ID_TARIFF, ID_SERVICE_TYPE INTO v_tariff_id, v_service_type_id
        FROM SERVICE_ORDER
        WHERE ID_ORDER = p_id_order;
    END IF;
    
    IF v_tariff_id IS NULL THEN
        RAISE EXCEPTION 'Тариф не указан для заказа %', p_id_order;
    END IF;
    
    -- Получаем информацию о тарифе
    SELECT IS_WITH_VAT, VAT_RATE INTO v_is_with_vat, v_vat_rate
    FROM TARIFF
    WHERE ID_TARIFF = v_tariff_id;
    
    -- Базовый расчет на основе ставок тарифа и параметров заказа
    FOR rec_rate IN
        SELECT tr.COD_RATE, tr.RATE_VALUE, tr.ID_CONDITION_ENUM
        FROM TARIFF_RATE tr
        WHERE tr.ID_TARIFF = v_tariff_id
    LOOP
        -- Ищем соответствующий параметр в заказе
        SELECT VAL_NUM INTO v_param_value
        FROM ORDER_PARAM op
        JOIN PARAMETR1 p ON op.ID_PAR = p.ID_PAR
        WHERE op.ID_ORDER = p_id_order
          AND LOWER(p.COD_PAR) = LOWER(rec_rate.COD_RATE);
        
        IF v_param_value IS NOT NULL THEN
            v_total_cost := v_total_cost + (rec_rate.RATE_VALUE * v_param_value);
        ELSE
            -- Если параметр не найден, добавляем базовую ставку
            v_total_cost := v_total_cost + rec_rate.RATE_VALUE;
        END IF;
    END LOOP;
    
    -- Применяем коэффициенты тарифа
    FOR rec_param IN
        SELECT tc.COEFF_VALUE
        FROM TARIFF_COEFFICIENT tc
        WHERE tc.ID_TARIFF = v_tariff_id
    LOOP
        v_total_cost := v_total_cost * rec_param.COEFF_VALUE;
    END LOOP;
    
    -- Применяем НДС если требуется
    IF v_is_with_vat = 1 AND v_vat_rate > 0 THEN
        v_total_cost := v_total_cost * (1 + v_vat_rate / 100);
    END IF;
    
    -- Обновляем стоимость в заказе
    UPDATE SERVICE_ORDER
    SET TOTAL_COST = v_total_cost
    WHERE ID_ORDER = p_id_order;
    
    RETURN v_total_cost;
END;
$$;

COMMENT ON FUNCTION CALC_ORDER_COST IS 'Расчет стоимости заказа по тарифу';

-- ============================================================================
-- CALC_ORDER_ITEM_COST - Расчет стоимости позиции заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION CALC_ORDER_ITEM_COST(
    p_id_order_item INTEGER,
    p_id_tariff INTEGER
)
RETURNS DOUBLE PRECISION
LANGUAGE plpgsql
AS $$
DECLARE
    v_total_cost DOUBLE PRECISION := 0;
    v_quantity DOUBLE PRECISION;
    v_is_with_vat INTEGER;
    v_vat_rate DOUBLE PRECISION;
    rec_rate RECORD;
    v_param_value DOUBLE PRECISION;
BEGIN
    -- Получаем количество
    SELECT QUANTITY INTO v_quantity
    FROM ORDER_ITEM
    WHERE ID_ORDER_ITEM = p_id_order_item;
    
    IF v_quantity IS NULL THEN
        v_quantity := 1;
    END IF;
    
    -- Получаем информацию о тарифе
    SELECT IS_WITH_VAT, VAT_RATE INTO v_is_with_vat, v_vat_rate
    FROM TARIFF
    WHERE ID_TARIFF = p_id_tariff;
    
    -- Расчет на основе ставок
    FOR rec_rate IN
        SELECT tr.COD_RATE, tr.RATE_VALUE
        FROM TARIFF_RATE tr
        WHERE tr.ID_TARIFF = p_id_tariff
    LOOP
        SELECT VAL_NUM INTO v_param_value
        FROM ORDER_ITEM_PARAM oip
        JOIN PARAMETR1 p ON oip.ID_PAR = p.ID_PAR
        WHERE oip.ID_ORDER_ITEM = p_id_order_item
          AND LOWER(p.COD_PAR) = LOWER(rec_rate.COD_RATE);
        
        IF v_param_value IS NOT NULL THEN
            v_total_cost := v_total_cost + (rec_rate.RATE_VALUE * v_param_value);
        ELSE
            v_total_cost := v_total_cost + rec_rate.RATE_VALUE;
        END IF;
    END LOOP;
    
    v_total_cost := v_total_cost * v_quantity;
    
    -- Применяем НДС
    IF v_is_with_vat = 1 AND v_vat_rate > 0 THEN
        v_total_cost := v_total_cost * (1 + v_vat_rate / 100);
    END IF;
    
    -- Обновляем стоимость позиции
    UPDATE ORDER_ITEM
    SET UNIT_COST = v_total_cost / v_quantity,
        TOTAL_COST = v_total_cost
    WHERE ID_ORDER_ITEM = p_id_order_item;
    
    RETURN v_total_cost;
END;
$$;

COMMENT ON FUNCTION CALC_ORDER_ITEM_COST IS 'Расчет стоимости позиции заказа';

-- ============================================================================
-- FIND_OPTIMAL_EXECUTOR - Поиск оптимального исполнителя
-- ============================================================================

CREATE OR REPLACE FUNCTION FIND_OPTIMAL_EXECUTOR(
    p_id_service_type INTEGER,
    p_target_date TEXT DEFAULT NULL
)
RETURNS TABLE (
    executor_id INTEGER,
    executor_name VARCHAR,
    tariff_id INTEGER,
    tariff_name VARCHAR,
    estimated_cost DOUBLE PRECISION
)
LANGUAGE plpgsql
AS $$
DECLARE
    v_target_date DATE;
BEGIN
    v_target_date := CASE WHEN p_target_date IS NULL OR p_target_date = 'NULL' THEN CURRENT_DATE ELSE p_target_date::DATE END;
    
    RETURN QUERY
    SELECT 
        e.ID_EXECUTOR,
        e.NAME_EXECUTOR,
        t.ID_TARIFF,
        t.NAME_TARIFF,
        COALESCE(
            (SELECT SUM(tr.RATE_VALUE) FROM TARIFF_RATE tr WHERE tr.ID_TARIFF = t.ID_TARIFF),
            0.0
        ) AS estimated_cost
    FROM TARIFF t
    JOIN EXECUTOR e ON t.ID_EXECUTOR = e.ID_EXECUTOR
    WHERE t.ID_SERVICE_TYPE = p_id_service_type
      AND t.IS_ACTIVE = 1
      AND e.IS_ACTIVE = 1
      AND t.DATE_BEGIN <= v_target_date
      AND (t.DATE_END IS NULL OR t.DATE_END >= v_target_date)
    ORDER BY estimated_cost ASC;
END;
$$;

COMMENT ON FUNCTION FIND_OPTIMAL_EXECUTOR IS 'Поиск оптимального исполнителя по стоимости';

-- ============================================================================
-- FIND_OPTIMAL_TARIFF - Поиск оптимального тарифа для заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION FIND_OPTIMAL_TARIFF(
    p_id_order INTEGER
)
RETURNS TABLE (
    tariff_id INTEGER,
    tariff_name VARCHAR,
    executor_name VARCHAR,
    estimated_cost DOUBLE PRECISION
)
LANGUAGE plpgsql
AS $$
DECLARE
    v_service_type_id INTEGER;
    v_order_date DATE;
BEGIN
    -- Получаем информацию о заказе
    SELECT ID_SERVICE_TYPE, ORDER_DATE INTO v_service_type_id, v_order_date
    FROM SERVICE_ORDER
    WHERE ID_ORDER = p_id_order;
    
    RETURN QUERY
    SELECT 
        t.ID_TARIFF,
        t.NAME_TARIFF,
        e.NAME_EXECUTOR,
        COALESCE(
            (SELECT SUM(tr.RATE_VALUE) FROM TARIFF_RATE tr WHERE tr.ID_TARIFF = t.ID_TАРИФ),
            0.0
        ) AS estimated_cost
    FROM TARIFF t
    LEFT JOIN EXECUTOR e ON t.ID_EXECUTOR = e.ID_EXECUTOR
    WHERE t.ID_SERVICE_TYPE = v_service_type_id
      AND t.IS_ACTIVE = 1
      AND t.DATE_BEGIN <= v_order_date
      AND (t.DATE_END IS NULL OR t.DATE_END >= v_order_date)
    ORDER BY estimated_cost ASC;
END;
$$;

COMMENT ON FUNCTION FIND_OPTIMAL_TARIFF IS 'Поиск оптимального тарифа для заказа';

-- ============================================================================
-- Утилиты и вспомогательные процедуры
-- СУБД: PostgreSQL 12+
-- Описание: Дополнительные процедуры для работы с системой
-- ============================================================================

-- ============================================================================
-- COPY_FUNCT_PROD - Копирование функций для продукта
-- ============================================================================

CREATE OR REPLACE FUNCTION COPY_FUNCT_PROD(
    p_id_pr_from INTEGER,    -- ID исходного объекта
    p_id_pr_to INTEGER       -- ID целевого объекта
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_count INTEGER := 0;    -- Счетчик скопированных функций
    rec_role RECORD;         -- Запись роли
BEGIN
    -- Проверка существования объектов
    IF NOT EXISTS (SELECT 1 FROM PROD WHERE ID_PR = p_id_pr_from) THEN
        RAISE EXCEPTION 'Исходный объект с ID % не найден', p_id_pr_from;
    END IF;
    
    IF NOT EXISTS (SELECT 1 FROM PROD WHERE ID_PR = p_id_pr_to) THEN
        RAISE EXCEPTION 'Целевой объект с ID % не найден', p_id_pr_to;
    END IF;
    
    -- Копирование значений функций
    FOR rec_role IN
        SELECT ID_FUNCT, ID_VAL_CONST, ID_VAL_FUNCT, 
               VAL_NUM, VAL_STR, VAL_DATE, ID_VAL_ENUM, NOTE
        FROM ROLE_VAL
        WHERE ID_PR = p_id_pr_from
    LOOP
        INSERT INTO ROLE_VAL (ID_FUNCT, ID_PR, ID_VAL_CONST, ID_VAL_FUNCT,
                              VAL_NUM, VAL_STR, VAL_DATE, ID_VAL_ENUM, NOTE)
        VALUES (rec_role.ID_FUNCT, p_id_pr_to, rec_role.ID_VAL_CONST, 
                rec_role.ID_VAL_FUNCT, rec_role.VAL_NUM, rec_role.VAL_STR,
                rec_role.VAL_DATE, rec_role.ID_VAL_ENUM, rec_role.NOTE)
        ON CONFLICT (ID_FUNCT, ID_PR) DO NOTHING;
        
        v_count := v_count + 1;
    END LOOP;
    
    RETURN v_count;
END;
$$;

COMMENT ON FUNCTION COPY_FUNCT_PROD IS 'Копирование функций и их значений от одного объекта к другому';

-- ============================================================================
-- FIND_VAL_ALL_PAR - Поиск всех параметров объекта
-- ============================================================================

CREATE OR REPLACE FUNCTION FIND_VAL_ALL_PAR(
    p_id_pr INTEGER          -- ID объекта
)
RETURNS TABLE (
    par_id INTEGER,
    par_code VARCHAR,
    par_name VARCHAR,
    par_type INTEGER,
    val_num DOUBLE PRECISION,
    val_str TEXT,
    val_date DATE,
    val_enum_name VARCHAR,
    unit VARCHAR
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT 
        p.ID_PAR,
        p.COD_PAR,
        p.NAME_PAR,
        p.TYPE_PAR,
        pp.VAL_NUM,
        pp.VAL_STR,
        pp.VAL_DATE,
        pe.NAME_POS as val_enum_name,
        e.NAME_EI as unit
    FROM PAR_PROD2 pp
    JOIN PARAMETR1 p ON pp.ID_PAR = p.ID_PAR
    LEFT JOIN EI e ON p.EI = e.ID_EI
    LEFT JOIN POS_ENUM pe ON pp.ID_VAL_ENUM = pe.ID_POS_ENUM
    WHERE pp.ID_PR = p_id_pr
    ORDER BY p.ID_PAR;
END;
$$;

COMMENT ON FUNCTION FIND_VAL_ALL_PAR IS 'Получение всех параметров объекта с их значениями';

-- ============================================================================
-- GET_HIERARCHY_PATH - Получение пути в иерархии
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_HIERARCHY_PATH(
    p_id_chem INTEGER        -- ID класса
)
RETURNS TEXT
LANGUAGE plpgsql
AS $$
DECLARE
    v_path TEXT := '';       -- Путь в иерархии
    v_current_id INTEGER;    -- Текущий ID
    v_name VARCHAR;          -- Имя класса
BEGIN
    v_current_id := p_id_chem;
    
    -- Построение пути снизу вверх
    WHILE v_current_id IS NOT NULL LOOP
        SELECT NAME_CHEM, PARENT_ID 
        INTO v_name, v_current_id
        FROM CHEM_CLASS
        WHERE ID_CHEM = v_current_id;
        
        IF NOT FOUND THEN
            EXIT;
        END IF;
        
        IF v_path = '' THEN
            v_path := v_name;
        ELSE
            v_path := v_name || ' / ' || v_path;
        END IF;
        
        v_current_id := v_current_id;
    END LOOP;
    
    RETURN v_path;
END;
$$;

COMMENT ON FUNCTION GET_HIERARCHY_PATH IS 'Получение полного пути класса в иерархии';

-- ============================================================================
-- VALIDATE_ORDER - Валидация заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION VALIDATE_ORDER(
    p_id_pr INTEGER          -- ID заказа
)
RETURNS TABLE (
    is_valid BOOLEAN,
    error_message TEXT
)
LANGUAGE plpgsql
AS $$
DECLARE
    v_class_id INTEGER;      -- ID класса заказа
    v_required_count INTEGER;-- Количество обязательных параметров
    v_provided_count INTEGER;-- Количество заполненных обязательных параметров
    v_missing_params TEXT;   -- Список отсутствующих параметров
BEGIN
    -- Получаем класс объекта
    SELECT CLASS_PR INTO v_class_id
    FROM PROD
    WHERE ID_PR = p_id_pr;
    
    IF NOT FOUND THEN
        RETURN QUERY SELECT FALSE, 'Заказ не найден';
        RETURN;
    END IF;
    
    -- Проверяем обязательные параметры
    SELECT COUNT(*) INTO v_required_count
    FROM PAR_CLASS1 pc
    JOIN PARAMETR1 p ON pc.ID_PAR = p.ID_PAR
    WHERE pc.ID_CHEM = v_class_id;
    
    SELECT COUNT(*) INTO v_provided_count
    FROM PAR_PROD2 pp
    JOIN PARAMETR1 p ON pp.ID_PAR = p.ID_PAR
    JOIN PAR_CLASS1 pc ON p.ID_PAR = pc.ID_PAR
    WHERE pp.ID_PR = p_id_pr 
      AND pc.ID_CHEM = v_class_id
      AND pp.IS_REQUIRE = 1
      AND (pp.VAL_NUM IS NOT NULL OR pp.VAL_STR IS NOT NULL OR 
           pp.VAL_DATE IS NOT NULL OR pp.ID_VAL_ENUM IS NOT NULL);
    
    IF v_provided_count < v_required_count THEN
        -- Формируем список отсутствующих параметров
        SELECT STRING_AGG(p.NAME_PAR, ', ') INTO v_missing_params
        FROM PARAMETR1 p
        JOIN PAR_CLASS1 pc ON p.ID_PAR = pc.ID_PAR
        WHERE pc.ID_CHEM = v_class_id
          AND NOT EXISTS (
              SELECT 1 FROM PAR_PROD2 pp
              WHERE pp.ID_PAR = p.ID_PAR 
                AND pp.ID_PR = p_id_pr
                AND pp.IS_REQUIRE = 1
                AND (pp.VAL_NUM IS NOT NULL OR pp.VAL_STR IS NOT NULL OR 
                     pp.VAL_DATE IS NOT NULL OR pp.ID_VAL_ENUM IS NOT NULL)
          );
        
        RETURN QUERY SELECT FALSE, 
            'Отсутствуют обязательные параметры: ' || COALESCE(v_missing_params, 'неизвестно');
        RETURN;
    END IF;
    
    RETURN QUERY SELECT TRUE, 'Заказ валиден';
END;
$$;

COMMENT ON FUNCTION VALIDATE_ORDER IS 'Проверка корректности заполнения заказа';

-- ============================================================================
-- GET_TARIFF_SUMMARY - Получение сводки по тарифу
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_TARIFF_SUMMARY(
    p_id_tariff INTEGER      -- ID тарифа
)
RETURNS TABLE (
    tariff_code VARCHAR,
    tariff_name VARCHAR,
    class_name VARCHAR,
    rules_count INTEGER,
    functions_count INTEGER,
    description TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY
    SELECT 
        p.COD_PR,
        p.NAME_PR,
        c.NAME_CHEM,
        (SELECT COUNT(*) FROM FACT_FUN WHERE ID_PR = p_id_tariff)::INTEGER,
        (SELECT COUNT(DISTINCT ID_FUNCT) FROM ROLE_VAL WHERE ID_PR = p_id_tariff)::INTEGER,
        p.NOTE
    FROM PROD p
    JOIN CHEM_CLASS c ON p.CLASS_PR = c.ID_CHEM
    WHERE p.ID_PR = p_id_tariff;
END;
$$;

COMMENT ON FUNCTION GET_TARIFF_SUMMARY IS 'Получение сводной информации о тарифе';

-- ============================================================================
-- CLEAR_CALCULATED_VALUES - Очистка рассчитанных значений
-- ============================================================================

CREATE OR REPLACE FUNCTION CLEAR_CALCULATED_VALUES(
    p_id_pr INTEGER          -- ID объекта
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_deleted_count INTEGER; -- Количество удаленных записей
BEGIN
    -- Удаление всех рассчитанных значений функций для объекта
    DELETE FROM ROLE_VAL
    WHERE ID_PR = p_id_pr;
    
    GET DIAGNOSTICS v_deleted_count = ROW_COUNT;
    
    RETURN v_deleted_count;
END;
$$;

COMMENT ON FUNCTION CLEAR_CALCULATED_VALUES IS 'Очистка рассчитанных значений для пересчета';

-- ============================================================================
-- Примеры использования утилит
-- ============================================================================

-- Пример 1: Копирование функций от одного тарифа к другому
/*
SELECT COPY_FUNCT_PROD(
    100,  -- ID исходного тарифа
    101   -- ID нового тарифа
);
*/

-- Пример 2: Получение всех параметров заказа
/*
SELECT * FROM FIND_VAL_ALL_PAR(200);
*/

-- Пример 3: Получение пути класса в иерархии
/*
SELECT GET_HIERARCHY_PATH(
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'CARGO_SERVICE')
);
-- Результат: "Корень / Услуги / Грузоперевозки"
*/

-- Пример 4: Валидация заказа
/*
SELECT * FROM VALIDATE_ORDER(200);
*/

-- Пример 5: Сводка по тарифу
/*
SELECT * FROM GET_TARIFF_SUMMARY(100);
*/

-- Пример 6: Очистка рассчитанных значений
/*
SELECT CLEAR_CALCULATED_VALUES(200);
*/

-- ============================================================================
-- Конец скрипта утилит
-- ============================================================================
