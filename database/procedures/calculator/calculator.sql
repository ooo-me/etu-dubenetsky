-- ============================================================================
-- Процедуры исполнителя правил (калькулятора)
-- СУБД: PostgreSQL 12+
-- Описание: Процедуры для вычисления выражений и правил
-- ============================================================================

-- ============================================================================
-- FIND_VAL_PAR - Поиск значения параметра объекта
-- ============================================================================

CREATE OR REPLACE FUNCTION FIND_VAL_PAR(
    p_id_par INTEGER,        -- ID параметра
    p_id_pr INTEGER,         -- ID объекта
    OUT o_val_num DOUBLE PRECISION,    -- Числовое значение
    OUT o_val_str TEXT,                -- Строковое значение
    OUT o_val_date DATE,               -- Значение даты
    OUT o_id_val_enum INTEGER          -- Значение перечисления
)
LANGUAGE plpgsql
AS $$
BEGIN
    -- Поиск значения параметра для объекта
    SELECT VAL_NUM, VAL_STR, VAL_DATE, ID_VAL_ENUM
    INTO o_val_num, o_val_str, o_val_date, o_id_val_enum
    FROM PAR_PROD2
    WHERE ID_PAR = p_id_par AND ID_PR = p_id_pr;
    
    IF NOT FOUND THEN
        -- Параметр не найден, возвращаем NULL
        o_val_num := NULL;
        o_val_str := NULL;
        o_val_date := NULL;
        o_id_val_enum := NULL;
    END IF;
END;
$$;

COMMENT ON FUNCTION FIND_VAL_PAR IS 'Поиск значения параметра для объекта';

-- ============================================================================
-- UPDATE_VAL_ROLE - Обновление значения функции для объекта
-- ============================================================================

CREATE OR REPLACE FUNCTION UPDATE_VAL_ROLE(
    p_id_funct INTEGER,      -- ID функции
    p_id_pr INTEGER,         -- ID объекта
    p_id_val_const INTEGER DEFAULT NULL,      -- Значение-константа
    p_id_val_funct INTEGER DEFAULT NULL,      -- Значение-функция
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
    -- Вставка или обновление значения функции
    INSERT INTO ROLE_VAL (ID_FUNCT, ID_PR, ID_VAL_CONST, ID_VAL_FUNCT, 
                          VAL_NUM, VAL_STR, VAL_DATE, ID_VAL_ENUM, NOTE)
    VALUES (p_id_funct, p_id_pr, p_id_val_const, p_id_val_funct,
            p_val_num, p_val_str, p_val_date, p_id_val_enum, p_note)
    ON CONFLICT (ID_FUNCT, ID_PR) DO UPDATE
    SET ID_VAL_CONST = p_id_val_const,
        ID_VAL_FUNCT = p_id_val_funct,
        VAL_NUM = p_val_num,
        VAL_STR = p_val_str,
        VAL_DATE = p_val_date,
        ID_VAL_ENUM = p_id_val_enum,
        NOTE = p_note;
END;
$$;

COMMENT ON FUNCTION UPDATE_VAL_ROLE IS 'Обновление значения функции для объекта (роли)';

-- ============================================================================
-- CALC_PRED - Вычисление предиката (операции сравнения)
-- ============================================================================

CREATE OR REPLACE FUNCTION CALC_PRED(
    p_id_funct INTEGER,      -- ID функции-предиката
    p_id_pr INTEGER,         -- ID объекта
    p_num_call INTEGER DEFAULT 1  -- Номер вызова
)
RETURNS BOOLEAN
LANGUAGE plpgsql
AS $$
DECLARE
    v_operation VARCHAR;     -- Операция сравнения
    v_id_fact_fun INTEGER;   -- ID вызова функции
    v_arg1_num DOUBLE PRECISION;  -- Первый аргумент (число)
    v_arg2_num DOUBLE PRECISION;  -- Второй аргумент (число)
    v_arg1_str TEXT;         -- Первый аргумент (строка)
    v_arg2_str TEXT;         -- Второй аргумент (строка)
    v_result BOOLEAN;        -- Результат сравнения
    rec_arg RECORD;          -- Запись аргумента
    v_arg_count INTEGER := 0; -- Счетчик аргументов
BEGIN
    -- Получаем операцию предиката
    SELECT OPERATION INTO v_operation
    FROM FUNCT_R
    WHERE ID_FUNCT = p_id_funct AND TYPE_F = 0;
    
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Предикат с ID % не найден', p_id_funct;
    END IF;
    
    -- Получаем ID вызова функции
    SELECT ID_FACT_FUN INTO v_id_fact_fun
    FROM FACT_FUN
    WHERE ID_FUNCT = p_id_funct AND ID_PR = p_id_pr AND NUM_CALL = p_num_call;
    
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Вызов функции % для объекта % не найден', p_id_funct, p_id_pr;
    END IF;
    
    -- Получаем значения аргументов
    FOR rec_arg IN
        SELECT fp.VAL_NUM, fp.VAL_STR, af.NUM_ARG
        FROM FACT_PAR fp
        JOIN ARG_FUNCT af ON fp.ID_ARG = af.ID_ARG
        WHERE fp.ID_FACT_FUN = v_id_fact_fun
        ORDER BY af.NUM_ARG
    LOOP
        v_arg_count := v_arg_count + 1;
        IF v_arg_count = 1 THEN
            v_arg1_num := rec_arg.VAL_NUM;
            v_arg1_str := rec_arg.VAL_STR;
        ELSIF v_arg_count = 2 THEN
            v_arg2_num := rec_arg.VAL_NUM;
            v_arg2_str := rec_arg.VAL_STR;
        END IF;
    END LOOP;
    
    -- Выполняем сравнение
    IF v_arg1_num IS NOT NULL AND v_arg2_num IS NOT NULL THEN
        -- Числовое сравнение
        CASE v_operation
            WHEN '<' THEN v_result := v_arg1_num < v_arg2_num;
            WHEN '<=' THEN v_result := v_arg1_num <= v_arg2_num;
            WHEN '=' THEN v_result := v_arg1_num = v_arg2_num;
            WHEN '>=' THEN v_result := v_arg1_num >= v_arg2_num;
            WHEN '>' THEN v_result := v_arg1_num > v_arg2_num;
            ELSE RAISE EXCEPTION 'Неизвестная операция: %', v_operation;
        END CASE;
    ELSIF v_arg1_str IS NOT NULL AND v_arg2_str IS NOT NULL THEN
        -- Строковое сравнение
        CASE v_operation
            WHEN '=' THEN v_result := v_arg1_str = v_arg2_str;
            WHEN '<>' THEN v_result := v_arg1_str <> v_arg2_str;
            ELSE RAISE EXCEPTION 'Операция % не поддерживается для строк', v_operation;
        END CASE;
    ELSE
        RAISE EXCEPTION 'Недостаточно аргументов для сравнения';
    END IF;
    
    RETURN v_result;
END;
$$;

COMMENT ON FUNCTION CALC_PRED IS 'Вычисление предиката (операции сравнения <, <=, =, >=, >)';

-- ============================================================================
-- CALC_AR - Вычисление арифметического выражения
-- ============================================================================

CREATE OR REPLACE FUNCTION CALC_AR(
    p_id_funct INTEGER,      -- ID арифметической функции
    p_id_pr INTEGER,         -- ID объекта
    p_num_call INTEGER DEFAULT 1  -- Номер вызова
)
RETURNS DOUBLE PRECISION
LANGUAGE plpgsql
AS $$
DECLARE
    v_operation VARCHAR;     -- Арифметическая операция
    v_id_fact_fun INTEGER;   -- ID вызова функции
    v_result DOUBLE PRECISION := 0;  -- Результат вычисления
    v_arg_num DOUBLE PRECISION;      -- Значение аргумента
    v_first_arg BOOLEAN := TRUE;     -- Флаг первого аргумента
    rec_arg RECORD;          -- Запись аргумента
BEGIN
    -- Получаем операцию
    SELECT OPERATION INTO v_operation
    FROM FUNCT_R
    WHERE ID_FUNCT = p_id_funct AND TYPE_F = 1;
    
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Арифметическая функция с ID % не найдена', p_id_funct;
    END IF;
    
    -- Получаем ID вызова функции
    SELECT ID_FACT_FUN INTO v_id_fact_fun
    FROM FACT_FUN
    WHERE ID_FUNCT = p_id_funct AND ID_PR = p_id_pr AND NUM_CALL = p_num_call;
    
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Вызов функции % для объекта % не найден', p_id_funct, p_id_pr;
    END IF;
    
    -- Получаем и обрабатываем аргументы
    FOR rec_arg IN
        SELECT fp.VAL_NUM, af.NUM_ARG
        FROM FACT_PAR fp
        JOIN ARG_FUNCT af ON fp.ID_ARG = af.ID_ARG
        WHERE fp.ID_FACT_FUN = v_id_fact_fun
        ORDER BY af.NUM_ARG
    LOOP
        v_arg_num := rec_arg.VAL_NUM;
        
        IF v_first_arg THEN
            v_result := v_arg_num;
            v_first_arg := FALSE;
        ELSE
            -- Применяем операцию
            CASE v_operation
                WHEN '+' THEN v_result := v_result + v_arg_num;
                WHEN '-' THEN v_result := v_result - v_arg_num;
                WHEN '*' THEN v_result := v_result * v_arg_num;
                WHEN '/' THEN 
                    IF v_arg_num = 0 THEN
                        RAISE EXCEPTION 'Деление на ноль';
                    END IF;
                    v_result := v_result / v_arg_num;
                ELSE RAISE EXCEPTION 'Неизвестная арифметическая операция: %', v_operation;
            END CASE;
        END IF;
    END LOOP;
    
    RETURN v_result;
END;
$$;

COMMENT ON FUNCTION CALC_AR IS 'Вычисление арифметического выражения (+, -, *, /)';

-- ============================================================================
-- CALC_LOG - Вычисление логического выражения
-- ============================================================================

CREATE OR REPLACE FUNCTION CALC_LOG(
    p_id_funct INTEGER,      -- ID логической функции
    p_id_pr INTEGER,         -- ID объекта
    p_num_call INTEGER DEFAULT 1  -- Номер вызова
)
RETURNS BOOLEAN
LANGUAGE plpgsql
AS $$
DECLARE
    v_operation VARCHAR;     -- Логическая операция
    v_id_fact_fun INTEGER;   -- ID вызова функции
    v_result BOOLEAN;        -- Результат вычисления
    v_arg_bool BOOLEAN;      -- Значение аргумента
    v_first_arg BOOLEAN := TRUE;  -- Флаг первого аргумента
    rec_arg RECORD;          -- Запись аргумента
BEGIN
    -- Получаем операцию
    SELECT OPERATION INTO v_operation
    FROM FUNCT_R
    WHERE ID_FUNCT = p_id_funct AND TYPE_F = 2;
    
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Логическая функция с ID % не найдена', p_id_funct;
    END IF;
    
    -- Получаем ID вызова функции
    SELECT ID_FACT_FUN INTO v_id_fact_fun
    FROM FACT_FUN
    WHERE ID_FUNCT = p_id_funct AND ID_PR = p_id_pr AND NUM_CALL = p_num_call;
    
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Вызов функции % для объекта % не найден', p_id_funct, p_id_pr;
    END IF;
    
    -- Обработка операции NOT (унарная)
    IF v_operation = 'NOT' THEN
        -- Получаем единственный аргумент
        SELECT (fp.VAL_NUM::INTEGER = 1) INTO v_arg_bool
        FROM FACT_PAR fp
        JOIN ARG_FUNCT af ON fp.ID_ARG = af.ID_ARG
        WHERE fp.ID_FACT_FUN = v_id_fact_fun
        LIMIT 1;
        
        RETURN NOT v_arg_bool;
    END IF;
    
    -- Обработка операций AND, OR (бинарные и более)
    FOR rec_arg IN
        SELECT (fp.VAL_NUM::INTEGER = 1) as bool_val, af.NUM_ARG
        FROM FACT_PAR fp
        JOIN ARG_FUNCT af ON fp.ID_ARG = af.ID_ARG
        WHERE fp.ID_FACT_FUN = v_id_fact_fun
        ORDER BY af.NUM_ARG
    LOOP
        v_arg_bool := rec_arg.bool_val;
        
        IF v_first_arg THEN
            v_result := v_arg_bool;
            v_first_arg := FALSE;
        ELSE
            -- Применяем операцию
            CASE v_operation
                WHEN 'AND' THEN v_result := v_result AND v_arg_bool;
                WHEN 'OR' THEN v_result := v_result OR v_arg_bool;
                ELSE RAISE EXCEPTION 'Неизвестная логическая операция: %', v_operation;
            END CASE;
        END IF;
    END LOOP;
    
    RETURN v_result;
END;
$$;

COMMENT ON FUNCTION CALC_LOG IS 'Вычисление логического выражения (AND, OR, NOT)';

-- ============================================================================
-- CALC_VAL_F - Универсальное вычисление функции
-- ============================================================================

CREATE OR REPLACE FUNCTION CALC_VAL_F(
    p_id_funct INTEGER,      -- ID функции
    p_id_pr INTEGER,         -- ID объекта
    p_id_tariff INTEGER DEFAULT NULL  -- ID тарифа (для контекста)
)
RETURNS DOUBLE PRECISION
LANGUAGE plpgsql
AS $$
DECLARE
    v_type_f INTEGER;        -- Тип функции
    v_result DOUBLE PRECISION;  -- Результат вычисления
    v_bool_result BOOLEAN;   -- Булев результат
BEGIN
    -- Определяем тип функции
    SELECT TYPE_F INTO v_type_f
    FROM FUNCT_R
    WHERE ID_FUNCT = p_id_funct;
    
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Функция с ID % не найдена', p_id_funct;
    END IF;
    
    -- Вычисляем в зависимости от типа
    CASE v_type_f
        WHEN 0 THEN -- Предикат
            v_bool_result := CALC_PRED(p_id_funct, p_id_pr);
            v_result := CASE WHEN v_bool_result THEN 1.0 ELSE 0.0 END;
            
        WHEN 1 THEN -- Арифметическое выражение
            v_result := CALC_AR(p_id_funct, p_id_pr);
            
        WHEN 2 THEN -- Логическое выражение
            v_bool_result := CALC_LOG(p_id_funct, p_id_pr);
            v_result := CASE WHEN v_bool_result THEN 1.0 ELSE 0.0 END;
            
        WHEN 3 THEN -- Функция выбора (CASE)
            v_result := CASE_ARG(p_id_funct, p_id_pr);
            
        ELSE
            RAISE EXCEPTION 'Неизвестный тип функции: %', v_type_f;
    END CASE;
    
    -- Сохраняем результат в ROLE_VAL
    PERFORM UPDATE_VAL_ROLE(p_id_funct, p_id_pr, NULL, NULL, v_result);
    
    RETURN v_result;
END;
$$;

COMMENT ON FUNCTION CALC_VAL_F IS 'Универсальная функция вычисления выражений любого типа';

-- ============================================================================
-- CASE_ARG - Функция выбора (IF-THEN-ELSE)
-- ============================================================================

CREATE OR REPLACE FUNCTION CASE_ARG(
    p_id_funct INTEGER,      -- ID функции выбора
    p_id_pr INTEGER          -- ID объекта
)
RETURNS DOUBLE PRECISION
LANGUAGE plpgsql
AS $$
DECLARE
    v_result DOUBLE PRECISION := 0;  -- Результат
    rec_decision RECORD;     -- Запись решения
    v_condition_result BOOLEAN;      -- Результат условия
BEGIN
    -- Перебираем решения по приоритету
    FOR rec_decision IN
        SELECT dr.ID_FUNCT_DEC, dr.PRIORITET
        FROM DECISION_RULE dr
        WHERE dr.ID_FUNCT = p_id_funct AND dr.ID_PR = p_id_pr
        ORDER BY dr.PRIORITET
    LOOP
        -- Вычисляем условие (функцию-решение может быть предикатом)
        BEGIN
            v_result := CALC_VAL_F(rec_decision.ID_FUNCT_DEC, p_id_pr);
            
            -- Если это первое решение с приоритетом 0, проверяем условие
            -- Если условие истинно или это безусловное решение, возвращаем результат
            IF rec_decision.PRIORITET = 0 OR v_result <> 0 THEN
                RETURN v_result;
            END IF;
        EXCEPTION
            WHEN OTHERS THEN
                -- Пропускаем решение при ошибке
                CONTINUE;
        END;
    END LOOP;
    
    RETURN v_result;
END;
$$;

COMMENT ON FUNCTION CASE_ARG IS 'Функция выбора решения по условию (IF-THEN-ELSE)';

-- ============================================================================
-- Конец скрипта процедур калькулятора
-- ============================================================================
