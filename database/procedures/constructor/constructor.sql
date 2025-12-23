-- ============================================================================
-- Процедуры конструктора тарифов
-- СУБД: PostgreSQL 12+
-- Описание: Процедуры для создания классов, функций, объектов
-- ============================================================================

-- ============================================================================
-- INS_CLASS - Создание класса в классификаторе
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_CLASS(
    p_cod_chem VARCHAR,      -- Код класса
    p_name_chem VARCHAR,     -- Наименование класса
    p_parent_id INTEGER,     -- ID родительского класса (NULL для корневого)
    p_note TEXT DEFAULT NULL -- Примечание
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_chem INTEGER;       -- ID созданного класса
    v_lev INTEGER := 0;      -- Уровень в иерархии
BEGIN
    -- Определяем уровень иерархии
    IF p_parent_id IS NOT NULL THEN
        SELECT LEV + 1 INTO v_lev
        FROM CHEM_CLASS
        WHERE ID_CHEM = p_parent_id;
        
        IF NOT FOUND THEN
            RAISE EXCEPTION 'Родительский класс с ID % не найден', p_parent_id;
        END IF;
    END IF;
    
    -- Создаем новый класс
    INSERT INTO CHEM_CLASS (COD_CHEM, NAME_CHEM, PARENT_ID, LEV, NOTE)
    VALUES (p_cod_chem, p_name_chem, p_parent_id, v_lev, p_note)
    RETURNING ID_CHEM INTO v_id_chem;
    
    RETURN v_id_chem;
END;
$$;

COMMENT ON FUNCTION INS_CLASS IS 'Создание класса в классификаторе с автоматическим определением уровня';

-- ============================================================================
-- UPD_CLASS - Обновление класса
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_CLASS(
    p_id_chem INTEGER,
    p_cod_chem VARCHAR DEFAULT NULL,
    p_name_chem VARCHAR DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE CHEM_CLASS
    SET COD_CHEM = COALESCE(p_cod_chem, COD_CHEM),
        NAME_CHEM = COALESCE(p_name_chem, NAME_CHEM),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_CHEM = p_id_chem;
END;
$$;

COMMENT ON FUNCTION UPD_CLASS IS 'Обновление класса в классификаторе';

-- ============================================================================
-- DEL_CLASS - Удаление класса
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_CLASS(p_id_chem INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM CHEM_CLASS WHERE ID_CHEM = p_id_chem;
END;
$$;

COMMENT ON FUNCTION DEL_CLASS IS 'Удаление класса из классификатора';

-- ============================================================================
-- INS_FUNCT - Создание функции
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_FUNCT(
    p_cod_funct VARCHAR,     -- Код функции
    p_name_funct VARCHAR,    -- Наименование функции
    p_type_f INTEGER,        -- Тип: 0-предикат, 1-арифм, 2-лог, 3-выбор
    p_operation VARCHAR DEFAULT NULL,  -- Операция
    p_note TEXT DEFAULT NULL -- Примечание
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_funct INTEGER;      -- ID созданной функции
BEGIN
    -- Проверка типа функции
    IF p_type_f NOT IN (0, 1, 2, 3) THEN
        RAISE EXCEPTION 'Недопустимый тип функции: %. Допустимые значения: 0-предикат, 1-арифм, 2-лог, 3-выбор', p_type_f;
    END IF;
    
    -- Создаем функцию
    INSERT INTO FUNCT_R (COD_FUNCT, NAME_FUNCT, TYPE_F, OPERATION, NOTE)
    VALUES (p_cod_funct, p_name_funct, p_type_f, p_operation, p_note)
    RETURNING ID_FUNCT INTO v_id_funct;
    
    RETURN v_id_funct;
END;
$$;

COMMENT ON FUNCTION INS_FUNCT IS 'Создание функции с указанием типа и операции';

-- ============================================================================
-- UPD_FUNCT - Обновление функции
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_FUNCT(
    p_id_funct INTEGER,
    p_cod_funct VARCHAR DEFAULT NULL,
    p_name_funct VARCHAR DEFAULT NULL,
    p_type_f INTEGER DEFAULT NULL,
    p_operation VARCHAR DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE FUNCT_R
    SET COD_FUNCT = COALESCE(p_cod_funct, COD_FUNCT),
        NAME_FUNCT = COALESCE(p_name_funct, NAME_FUNCT),
        TYPE_F = COALESCE(p_type_f, TYPE_F),
        OPERATION = COALESCE(p_operation, OPERATION),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_FUNCT = p_id_funct;
END;
$$;

COMMENT ON FUNCTION UPD_FUNCT IS 'Обновление функции';

-- ============================================================================
-- DEL_FUNCT - Удаление функции
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_FUNCT(p_id_funct INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM FUNCT_R WHERE ID_FUNCT = p_id_funct;
END;
$$;

COMMENT ON FUNCTION DEL_FUNCT IS 'Удаление функции';

-- ============================================================================
-- INS_ARG_FUN - Добавление аргумента к функции
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_ARG_FUN(
    p_id_funct INTEGER,      -- ID функции
    p_num_arg INTEGER,       -- Номер аргумента
    p_class_arg INTEGER DEFAULT NULL,  -- Класс аргумента
    p_name_arg VARCHAR DEFAULT NULL,   -- Название аргумента
    p_note TEXT DEFAULT NULL -- Примечание
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_arg INTEGER;        -- ID созданного аргумента
BEGIN
    -- Проверка существования функции
    IF NOT EXISTS (SELECT 1 FROM FUNCT_R WHERE ID_FUNCT = p_id_funct) THEN
        RAISE EXCEPTION 'Функция с ID % не найдена', p_id_funct;
    END IF;
    
    -- Создаем аргумент
    INSERT INTO ARG_FUNCT (ID_FUNCT, NUM_ARG, CLASS_ARG, NAME_ARG, NOTE)
    VALUES (p_id_funct, p_num_arg, p_class_arg, p_name_arg, p_note)
    RETURNING ID_ARG INTO v_id_arg;
    
    RETURN v_id_arg;
END;
$$;

COMMENT ON FUNCTION INS_ARG_FUN IS 'Добавление аргумента к функции';

-- ============================================================================
-- INS_CONST - Создание константы
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_CONST(
    p_cod_const VARCHAR,     -- Код константы
    p_name_const VARCHAR,    -- Наименование константы
    p_val_num DOUBLE PRECISION DEFAULT NULL,  -- Числовое значение
    p_val_str TEXT DEFAULT NULL,              -- Строковое значение
    p_val_date DATE DEFAULT NULL,             -- Значение даты
    p_ei INTEGER DEFAULT NULL,                -- Единица измерения
    p_note TEXT DEFAULT NULL                  -- Примечание
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_const INTEGER;      -- ID созданной константы
BEGIN
    -- Создаем константу
    INSERT INTO CONST (COD_CONST, NAME_CONST, VAL_NUM, VAL_STR, VAL_DATE, EI, NOTE)
    VALUES (p_cod_const, p_name_const, p_val_num, p_val_str, p_val_date, p_ei, p_note)
    RETURNING ID_CONST INTO v_id_const;
    
    RETURN v_id_const;
END;
$$;

COMMENT ON FUNCTION INS_CONST IS 'Создание константы с поддержкой различных типов значений';

-- ============================================================================
-- INS_VAL_ENUM - Добавление значения в перечисление
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_VAL_ENUM(
    p_id_enum INTEGER,       -- ID перечисления
    p_cod_pos VARCHAR,       -- Код позиции
    p_name_pos VARCHAR,      -- Название позиции
    p_num_pos INTEGER,       -- Номер позиции
    p_note TEXT DEFAULT NULL -- Примечание
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_pos_enum INTEGER;   -- ID созданной позиции
BEGIN
    -- Проверка существования перечисления
    IF NOT EXISTS (SELECT 1 FROM ENUM_VAL_R WHERE ID_ENUM = p_id_enum) THEN
        RAISE EXCEPTION 'Перечисление с ID % не найдено', p_id_enum;
    END IF;
    
    -- Создаем позицию перечисления
    INSERT INTO POS_ENUM (ID_ENUM, COD_POS, NAME_POS, NUM_POS, NOTE)
    VALUES (p_id_enum, p_cod_pos, p_name_pos, p_num_pos, p_note)
    RETURNING ID_POS_ENUM INTO v_id_pos_enum;
    
    RETURN v_id_pos_enum;
END;
$$;

COMMENT ON FUNCTION INS_VAL_ENUM IS 'Добавление позиции в перечисление';

-- ============================================================================
-- INS_ENUM - Создание перечисления
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_ENUM(
    p_cod_enum VARCHAR,
    p_name_enum VARCHAR,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_enum INTEGER;
BEGIN
    INSERT INTO ENUM_VAL_R (COD_ENUM, NAME_ENUM, NOTE)
    VALUES (p_cod_enum, p_name_enum, p_note)
    RETURNING ID_ENUM INTO v_id_enum;
    
    RETURN v_id_enum;
END;
$$;

COMMENT ON FUNCTION INS_ENUM IS 'Создание нового перечисления';

-- ============================================================================
-- INS_OB - Создание объекта предметной области
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_OB(
    p_class_pr INTEGER,      -- Класс объекта
    p_cod_pr VARCHAR,        -- Код объекта
    p_name_pr VARCHAR,       -- Наименование объекта
    p_par_pr INTEGER DEFAULT NULL,  -- Родительский объект
    p_note TEXT DEFAULT NULL -- Примечание
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_pr INTEGER;         -- ID созданного объекта
BEGIN
    -- Проверка существования класса
    IF NOT EXISTS (SELECT 1 FROM CHEM_CLASS WHERE ID_CHEM = p_class_pr) THEN
        RAISE EXCEPTION 'Класс с ID % не найден', p_class_pr;
    END IF;
    
    -- Проверка существования родительского объекта
    IF p_par_pr IS NOT NULL THEN
        IF NOT EXISTS (SELECT 1 FROM PROD WHERE ID_PR = p_par_pr) THEN
            RAISE EXCEPTION 'Родительский объект с ID % не найден', p_par_pr;
        END IF;
    END IF;
    
    -- Создаем объект
    INSERT INTO PROD (CLASS_PR, COD_PR, NAME_PR, PAR_PR, NOTE)
    VALUES (p_class_pr, p_cod_pr, p_name_pr, p_par_pr, p_note)
    RETURNING ID_PR INTO v_id_pr;
    
    RETURN v_id_pr;
END;
$$;

COMMENT ON FUNCTION INS_OB IS 'Создание объекта предметной области (услуги, тарифа, заказа)';

-- ============================================================================
-- UPD_OB - Обновление объекта
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_OB(
    p_id_pr INTEGER,
    p_cod_pr VARCHAR DEFAULT NULL,
    p_name_pr VARCHAR DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE PROD
    SET COD_PR = COALESCE(p_cod_pr, COD_PR),
        NAME_PR = COALESCE(p_name_pr, NAME_PR),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_PR = p_id_pr;
END;
$$;

COMMENT ON FUNCTION UPD_OB IS 'Обновление объекта';

-- ============================================================================
-- DEL_OB - Удаление объекта
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_OB(p_id_pr INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM PROD WHERE ID_PR = p_id_pr;
END;
$$;

COMMENT ON FUNCTION DEL_OB IS 'Удаление объекта';

-- ============================================================================
-- INS_FACT_FUN - Создание вызова функции
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_FACT_FUN(
    p_id_funct INTEGER,      -- ID функции
    p_id_pr INTEGER,         -- ID объекта
    p_num_call INTEGER DEFAULT 1,     -- Номер вызова
    p_note TEXT DEFAULT NULL -- Примечание
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_fact_fun INTEGER;   -- ID созданного вызова
BEGIN
    -- Проверка существования функции
    IF NOT EXISTS (SELECT 1 FROM FUNCT_R WHERE ID_FUNCT = p_id_funct) THEN
        RAISE EXCEPTION 'Функция с ID % не найдена', p_id_funct;
    END IF;
    
    -- Проверка существования объекта
    IF NOT EXISTS (SELECT 1 FROM PROD WHERE ID_PR = p_id_pr) THEN
        RAISE EXCEPTION 'Объект с ID % не найден', p_id_pr;
    END IF;
    
    -- Создаем вызов функции
    INSERT INTO FACT_FUN (ID_FUNCT, ID_PR, NUM_CALL, NOTE)
    VALUES (p_id_funct, p_id_pr, p_num_call, p_note)
    RETURNING ID_FACT_FUN INTO v_id_fact_fun;
    
    RETURN v_id_fact_fun;
END;
$$;

COMMENT ON FUNCTION INS_FACT_FUN IS 'Создание вызова функции в контексте объекта';

-- ============================================================================
-- WRITE_FACT_PAR - Запись фактического параметра
-- ============================================================================

CREATE OR REPLACE FUNCTION WRITE_FACT_PAR(
    p_id_fact_fun INTEGER,   -- ID вызова функции
    p_id_arg INTEGER,        -- ID аргумента
    p_id_val_const INTEGER DEFAULT NULL,      -- Значение-константа
    p_id_val_funct INTEGER DEFAULT NULL,      -- Значение-функция
    p_id_val_fact_fun INTEGER DEFAULT NULL,   -- Значение-вызов функции
    p_val_num DOUBLE PRECISION DEFAULT NULL,  -- Числовое значение
    p_val_str TEXT DEFAULT NULL,              -- Строковое значение
    p_val_date DATE DEFAULT NULL,             -- Значение даты
    p_id_val_enum INTEGER DEFAULT NULL,       -- Значение перечисления
    p_note TEXT DEFAULT NULL                  -- Примечание
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    -- Проверка существования вызова функции
    IF NOT EXISTS (SELECT 1 FROM FACT_FUN WHERE ID_FACT_FUN = p_id_fact_fun) THEN
        RAISE EXCEPTION 'Вызов функции с ID % не найден', p_id_fact_fun;
    END IF;
    
    -- Проверка существования аргумента
    IF NOT EXISTS (SELECT 1 FROM ARG_FUNCT WHERE ID_ARG = p_id_arg) THEN
        RAISE EXCEPTION 'Аргумент с ID % не найден', p_id_arg;
    END IF;
    
    -- Вставляем или обновляем фактический параметр
    INSERT INTO FACT_PAR (ID_FACT_FUN, ID_ARG, ID_VAL_CONST, ID_VAL_FUNCT, ID_VAL_FACT_FUN,
                          VAL_NUM, VAL_STR, VAL_DATE, ID_VAL_ENUM, NOTE)
    VALUES (p_id_fact_fun, p_id_arg, p_id_val_const, p_id_val_funct, p_id_val_fact_fun,
            p_val_num, p_val_str, p_val_date, p_id_val_enum, p_note)
    ON CONFLICT (ID_FACT_FUN, ID_ARG) DO UPDATE
    SET ID_VAL_CONST = p_id_val_const,
        ID_VAL_FUNCT = p_id_val_funct,
        ID_VAL_FACT_FUN = p_id_val_fact_fun,
        VAL_NUM = p_val_num,
        VAL_STR = p_val_str,
        VAL_DATE = p_val_date,
        ID_VAL_ENUM = p_id_val_enum,
        NOTE = p_note;
END;
$$;

COMMENT ON FUNCTION WRITE_FACT_PAR IS 'Запись фактического параметра вызова функции';

-- ============================================================================
-- INS_DEC_F - Добавление решения в правило
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_DEC_F(
    p_id_funct INTEGER,      -- ID функции-правила
    p_id_pr INTEGER,         -- ID объекта
    p_num_call INTEGER,      -- Номер вызова
    p_id_funct_dec INTEGER,  -- ID функции-решения
    p_prioritet INTEGER DEFAULT 0,    -- Приоритет
    p_note TEXT DEFAULT NULL -- Примечание
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    -- Проверка существования функции-правила
    IF NOT EXISTS (SELECT 1 FROM FUNCT_R WHERE ID_FUNCT = p_id_funct) THEN
        RAISE EXCEPTION 'Функция-правило с ID % не найдена', p_id_funct;
    END IF;
    
    -- Проверка существования объекта
    IF NOT EXISTS (SELECT 1 FROM PROD WHERE ID_PR = p_id_pr) THEN
        RAISE EXCEPTION 'Объект с ID % не найден', p_id_pr;
    END IF;
    
    -- Проверка существования функции-решения
    IF NOT EXISTS (SELECT 1 FROM FUNCT_R WHERE ID_FUNCT = p_id_funct_dec) THEN
        RAISE EXCEPTION 'Функция-решение с ID % не найдена', p_id_funct_dec;
    END IF;
    
    -- Добавляем решение
    INSERT INTO DECISION_RULE (ID_FUNCT, ID_PR, NUM_CALL, ID_FUNCT_DEC, PRIORITET, NOTE)
    VALUES (p_id_funct, p_id_pr, p_num_call, p_id_funct_dec, p_prioritet, p_note)
    ON CONFLICT (ID_FUNCT, ID_PR, NUM_CALL, ID_FUNCT_DEC) DO UPDATE
    SET PRIORITET = p_prioritet,
        NOTE = p_note;
END;
$$;

COMMENT ON FUNCTION INS_DEC_F IS 'Добавление решения к правилу с указанием приоритета';

-- ============================================================================
-- INS_EI - Добавление единицы измерения
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_EI(
    p_cod_ei VARCHAR,
    p_name_ei VARCHAR,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_ei INTEGER;
BEGIN
    INSERT INTO EI (COD_EI, NAME_EI, NOTE)
    VALUES (p_cod_ei, p_name_ei, p_note)
    RETURNING ID_EI INTO v_id_ei;
    
    RETURN v_id_ei;
END;
$$;

COMMENT ON FUNCTION INS_EI IS 'Добавление единицы измерения';

-- ============================================================================
-- UPD_EI - Обновление единицы измерения
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_EI(
    p_id_ei INTEGER,
    p_cod_ei VARCHAR DEFAULT NULL,
    p_name_ei VARCHAR DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE EI
    SET COD_EI = COALESCE(p_cod_ei, COD_EI),
        NAME_EI = COALESCE(p_name_ei, NAME_EI),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_EI = p_id_ei;
END;
$$;

COMMENT ON FUNCTION UPD_EI IS 'Обновление единицы измерения';

-- ============================================================================
-- DEL_EI - Удаление единицы измерения
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_EI(p_id_ei INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM EI WHERE ID_EI = p_id_ei;
END;
$$;

COMMENT ON FUNCTION DEL_EI IS 'Удаление единицы измерения';

-- ============================================================================
-- INS_PARAMETR - Добавление параметра
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_PARAMETR(
    p_cod_par VARCHAR,
    p_name_par VARCHAR,
    p_class_par INTEGER DEFAULT NULL,
    p_type_par INTEGER DEFAULT 0,
    p_ei INTEGER DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_par INTEGER;
BEGIN
    INSERT INTO PARAMETR1 (COD_PAR, NAME_PAR, CLASS_PAR, TYPE_PAR, EI, NOTE)
    VALUES (p_cod_par, p_name_par, p_class_par, p_type_par, p_ei, p_note)
    RETURNING ID_PAR INTO v_id_par;
    
    RETURN v_id_par;
END;
$$;

COMMENT ON FUNCTION INS_PARAMETR IS 'Добавление параметра';

-- ============================================================================
-- UPD_PARAMETR - Обновление параметра
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_PARAMETR(
    p_id_par INTEGER,
    p_cod_par VARCHAR DEFAULT NULL,
    p_name_par VARCHAR DEFAULT NULL,
    p_type_par INTEGER DEFAULT NULL,
    p_ei INTEGER DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE PARAMETR1
    SET COD_PAR = COALESCE(p_cod_par, COD_PAR),
        NAME_PAR = COALESCE(p_name_par, NAME_PAR),
        TYPE_PAR = COALESCE(p_type_par, TYPE_PAR),
        EI = COALESCE(p_ei, EI),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_PAR = p_id_par;
END;
$$;

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
    p_id_service_type INTEGER,
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
    WHERE ID_SERVICE_TYPE = p_id_service_type;
END;
$$;

COMMENT ON FUNCTION UPD_SERVICE_TYPE IS 'Обновление типа услуги';

-- ============================================================================
-- DEL_SERVICE_TYPE - Удаление типа услуги
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_SERVICE_TYPE(p_id_service_type INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM SERVICE_TYPE WHERE ID_SERVICE_TYPE = p_id_service_type;
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

COMMENT ON FUNCTION INS_EXECUTOR IS 'Создание исполнителя услуг';

-- ============================================================================
-- UPD_EXECUTOR - Обновление исполнителя
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_EXECUTOR(
    p_id_executor INTEGER,
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
    WHERE ID_EXECUTOR = p_id_executor;
END;
$$;

COMMENT ON FUNCTION UPD_EXECUTOR IS 'Обновление исполнителя';

-- ============================================================================
-- DEL_EXECUTOR - Удаление исполнителя
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_EXECUTOR(p_id_executor INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM EXECUTOR WHERE ID_EXECUTOR = p_id_executor;
END;
$$;

COMMENT ON FUNCTION DEL_EXECUTOR IS 'Удаление исполнителя';

-- ============================================================================
-- INS_TARIFF - Создание тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_TARIFF(
    p_id_service_type INTEGER,
    p_cod_tariff VARCHAR,
    p_name_tariff VARCHAR,
    p_id_executor INTEGER DEFAULT NULL,
    p_date_begin DATE DEFAULT CURRENT_DATE,
    p_date_end DATE DEFAULT NULL,
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
BEGIN
    INSERT INTO TARIFF (ID_SERVICE_TYPE, COD_TARIFF, NAME_TARIFF, ID_EXECUTOR,
                        DATE_BEGIN, DATE_END, IS_WITH_VAT, VAT_RATE, IS_ACTIVE, NOTE)
    VALUES (p_id_service_type, p_cod_tariff, p_name_tariff, p_id_executor,
            p_date_begin, p_date_end, p_is_with_vat, p_vat_rate, p_is_active, p_note)
    RETURNING ID_TARIFF INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_TARIFF IS 'Создание тарифа на услугу';

-- ============================================================================
-- UPD_TARIFF - Обновление тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_TARIFF(
    p_id_tariff INTEGER,
    p_cod_tariff VARCHAR DEFAULT NULL,
    p_name_tariff VARCHAR DEFAULT NULL,
    p_id_executor INTEGER DEFAULT NULL,
    p_date_begin DATE DEFAULT NULL,
    p_date_end DATE DEFAULT NULL,
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
    SET COD_TARIFF = COALESCE(p_cod_tariff, COD_TARIFF),
        NAME_TARIFF = COALESCE(p_name_tariff, NAME_TARIFF),
        ID_EXECUTOR = COALESCE(p_id_executor, ID_EXECUTOR),
        DATE_BEGIN = COALESCE(p_date_begin, DATE_BEGIN),
        DATE_END = COALESCE(p_date_end, DATE_END),
        IS_WITH_VAT = COALESCE(p_is_with_vat, IS_WITH_VAT),
        VAT_RATE = COALESCE(p_vat_rate, VAT_RATE),
        IS_ACTIVE = COALESCE(p_is_active, IS_ACTIVE),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_TARIFF = p_id_tariff;
END;
$$;

COMMENT ON FUNCTION UPD_TARIFF IS 'Обновление тарифа';

-- ============================================================================
-- DEL_TARIFF - Удаление тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_TARIFF(p_id_tariff INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM TARIFF WHERE ID_TARIFF = p_id_tariff;
END;
$$;

COMMENT ON FUNCTION DEL_TARIFF IS 'Удаление тарифа';

-- ============================================================================
-- INS_TARIFF_RATE - Добавление ставки тарифа
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
    INSERT INTO TARIFF_RATE (ID_TARIFF, COD_RATE, NAME_RATE, RATE_VALUE, 
                              ID_EI, ID_CONDITION_ENUM, NOTE)
    VALUES (p_id_tariff, p_cod_rate, p_name_rate, p_rate_value,
            p_id_ei, p_id_condition_enum, p_note)
    RETURNING ID_TARIFF_RATE INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_TARIFF_RATE IS 'Добавление ставки к тарифу';

-- ============================================================================
-- UPD_TARIFF_RATE - Обновление ставки тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_TARIFF_RATE(
    p_id_tariff_rate INTEGER,
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
        ID_EI = COALESCE(p_id_ei, ID_EI),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_TARIFF_RATE = p_id_tariff_rate;
END;
$$;

COMMENT ON FUNCTION UPD_TARIFF_RATE IS 'Обновление ставки тарифа';

-- ============================================================================
-- DEL_TARIFF_RATE - Удаление ставки тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_TARIFF_RATE(p_id_tariff_rate INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM TARIFF_RATE WHERE ID_TARIFF_RATE = p_id_tariff_rate;
END;
$$;

COMMENT ON FUNCTION DEL_TARIFF_RATE IS 'Удаление ставки тарифа';

-- ============================================================================
-- INS_ORDER - Создание заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_ORDER(
    p_cod_order VARCHAR,
    p_id_service_type INTEGER,
    p_order_date DATE DEFAULT CURRENT_DATE,
    p_execution_date DATE DEFAULT NULL,
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
BEGIN
    INSERT INTO SERVICE_ORDER (COD_ORDER, ID_SERVICE_TYPE, ORDER_DATE, EXECUTION_DATE,
                                STATUS, ID_EXECUTOR, ID_TARIFF, NOTE)
    VALUES (p_cod_order, p_id_service_type, p_order_date, p_execution_date,
            p_status, p_id_executor, p_id_tariff, p_note)
    RETURNING ID_ORDER INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_ORDER IS 'Создание заказа на услугу';

-- ============================================================================
-- UPD_ORDER - Обновление заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_ORDER(
    p_id_order INTEGER,
    p_cod_order VARCHAR DEFAULT NULL,
    p_execution_date DATE DEFAULT NULL,
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
        EXECUTION_DATE = COALESCE(p_execution_date, EXECUTION_DATE),
        STATUS = COALESCE(p_status, STATUS),
        ID_EXECUTOR = COALESCE(p_id_executor, ID_EXECUTOR),
        ID_TARIFF = COALESCE(p_id_tariff, ID_TARIFF),
        TOTAL_COST = COALESCE(p_total_cost, TOTAL_COST),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_ORDER = p_id_order;
END;
$$;

COMMENT ON FUNCTION UPD_ORDER IS 'Обновление заказа';

-- ============================================================================
-- DEL_ORDER - Удаление заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_ORDER(p_id_order INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM SERVICE_ORDER WHERE ID_ORDER = p_id_order;
END;
$$;

COMMENT ON FUNCTION DEL_ORDER IS 'Удаление заказа';

-- ============================================================================
-- INS_ORDER_PARAM - Добавление параметра заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_ORDER_PARAM(
    p_id_order INTEGER,
    p_id_par INTEGER,
    p_val_num DOUBLE PRECISION DEFAULT NULL,
    p_val_str TEXT DEFAULT NULL,
    p_val_date DATE DEFAULT NULL,
    p_id_val_enum INTEGER DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    INSERT INTO ORDER_PARAM (ID_ORDER, ID_PAR, VAL_NUM, VAL_STR, VAL_DATE, ID_VAL_ENUM)
    VALUES (p_id_order, p_id_par, p_val_num, p_val_str, p_val_date, p_id_val_enum)
    ON CONFLICT (ID_ORDER, ID_PAR) DO UPDATE
    SET VAL_NUM = p_val_num,
        VAL_STR = p_val_str,
        VAL_DATE = p_val_date,
        ID_VAL_ENUM = p_id_val_enum;
END;
$$;

COMMENT ON FUNCTION INS_ORDER_PARAM IS 'Добавление параметра к заказу';

-- ============================================================================
-- INS_ORDER_ITEM - Добавление позиции заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_ORDER_ITEM(
    p_id_order INTEGER,
    p_id_service_type INTEGER,
    p_quantity DOUBLE PRECISION DEFAULT 1,
    p_unit_cost DOUBLE PRECISION DEFAULT NULL,
    p_total_cost DOUBLE PRECISION DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id INTEGER;
BEGIN
    INSERT INTO ORDER_ITEM (ID_ORDER, ID_SERVICE_TYPE, QUANTITY, UNIT_COST, TOTAL_COST, NOTE)
    VALUES (p_id_order, p_id_service_type, p_quantity, p_unit_cost, p_total_cost, p_note)
    RETURNING ID_ORDER_ITEM INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_ORDER_ITEM IS 'Добавление позиции к заказу';

-- ============================================================================
-- UPD_ORDER_ITEM - Обновление позиции заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_ORDER_ITEM(
    p_id_order_item INTEGER,
    p_quantity DOUBLE PRECISION DEFAULT NULL,
    p_unit_cost DOUBLE PRECISION DEFAULT NULL,
    p_total_cost DOUBLE PRECISION DEFAULT NULL,
    p_note TEXT DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE ORDER_ITEM
    SET QUANTITY = COALESCE(p_quantity, QUANTITY),
        UNIT_COST = COALESCE(p_unit_cost, UNIT_COST),
        TOTAL_COST = COALESCE(p_total_cost, TOTAL_COST),
        NOTE = COALESCE(p_note, NOTE)
    WHERE ID_ORDER_ITEM = p_id_order_item;
END;
$$;

COMMENT ON FUNCTION UPD_ORDER_ITEM IS 'Обновление позиции заказа';

-- ============================================================================
-- DEL_ORDER_ITEM - Удаление позиции заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_ORDER_ITEM(p_id_order_item INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM ORDER_ITEM WHERE ID_ORDER_ITEM = p_id_order_item;
END;
$$;

COMMENT ON FUNCTION DEL_ORDER_ITEM IS 'Удаление позиции заказа';

-- ============================================================================
-- INS_ORDER_ITEM_PARAM - Добавление параметра позиции заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_ORDER_ITEM_PARAM(
    p_id_order_item INTEGER,
    p_id_par INTEGER,
    p_val_num DOUBLE PRECISION DEFAULT NULL,
    p_val_str TEXT DEFAULT NULL,
    p_val_date DATE DEFAULT NULL,
    p_id_val_enum INTEGER DEFAULT NULL
)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    INSERT INTO ORDER_ITEM_PARAM (ID_ORDER_ITEM, ID_PAR, VAL_NUM, VAL_STR, VAL_DATE, ID_VAL_ENUM)
    VALUES (p_id_order_item, p_id_par, p_val_num, p_val_str, p_val_date, p_id_val_enum)
    ON CONFLICT (ID_ORDER_ITEM, ID_PAR) DO UPDATE
    SET VAL_NUM = p_val_num,
        VAL_STR = p_val_str,
        VAL_DATE = p_val_date,
        ID_VAL_ENUM = p_id_val_enum;
END;
$$;

COMMENT ON FUNCTION INS_ORDER_ITEM_PARAM IS 'Добавление параметра к позиции заказа';

-- ============================================================================
-- INS_COEFFICIENT - Добавление коэффициента
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

COMMENT ON FUNCTION INS_COEFFICIENT IS 'Добавление повышающего коэффициента';

-- ============================================================================
-- UPD_COEFFICIENT - Обновление коэффициента
-- ============================================================================

CREATE OR REPLACE FUNCTION UPD_COEFFICIENT(
    p_id_coefficient INTEGER,
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
    WHERE ID_COEFFICIENT = p_id_coefficient;
END;
$$;

-- ============================================================================
-- DEL_COEFFICIENT - Удаление коэффициента
-- ============================================================================

CREATE OR REPLACE FUNCTION DEL_COEFFICIENT(p_id_coefficient INTEGER)
RETURNS VOID
LANGUAGE plpgsql
AS $$
BEGIN
    DELETE FROM COEFFICIENT WHERE ID_COEFFICIENT = p_id_coefficient;
END;
$$;

COMMENT ON FUNCTION DEL_COEFFICIENT IS 'Удаление коэффициента';

-- ============================================================================
-- INS_TARIFF_COEFFICIENT - Связь тарифа с коэффициентом
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
-- INS_TARIFF_RULE - Добавление правила к тарифу
-- ============================================================================

CREATE OR REPLACE FUNCTION INS_TARIFF_RULE(
    p_id_tariff INTEGER,
    p_id_funct INTEGER,
    p_priority INTEGER DEFAULT 0,
    p_is_active INTEGER DEFAULT 1,
    p_note TEXT DEFAULT NULL
)
RETURNS INTEGER
LANGUAGE plpgsql
AS $$
DECLARE
    v_id INTEGER;
BEGIN
    INSERT INTO TARIFF_RULE (ID_TARIFF, ID_FUNCT, PRIORITY, IS_ACTIVE, NOTE)
    VALUES (p_id_tariff, p_id_funct, p_priority, p_is_active, p_note)
    RETURNING ID_TARIFF_RULE INTO v_id;
    
    RETURN v_id;
END;
$$;

COMMENT ON FUNCTION INS_TARIFF_RULE IS 'Добавление правила расчета к тарифу';

-- ============================================================================
-- Конец скрипта процедур конструктора
-- ============================================================================
