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
-- Конец скрипта процедур конструктора
-- ============================================================================
