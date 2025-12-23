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
            IF rec_decision.PRIORITЕТ = 0 OR v_result <> 0 THEN
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
-- CALC_ORDER_COST - Расчет стоимости заказа по тарифу
-- ============================================================================

CREATE OR REPLACE FUNCTION CALC_ORDER_COST(
    p_id_order INTEGER,      -- ID заказа
    p_id_tariff INTEGER DEFAULT NULL  -- ID тарифа (если NULL, используем тариф заказа)
)
RETURNS DOUBLE PRECISION
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_tariff INTEGER;
    v_id_service_type INTEGER;
    v_base_cost DOUBLE PRECISION := 0;
    v_total_cost DOUBLE PRECISION := 0;
    v_coeff DOUBLE PRECISION := 1.0;
    v_is_with_vat INTEGER;
    v_vat_rate DOUBLE PRECISION;
    rec_rate RECORD;
    rec_param RECORD;
    rec_item RECORD;
    rec_rule RECORD;
    rec_coeff RECORD;
BEGIN
    -- Получаем информацию о заказе
    SELECT ID_TARIFF, ID_SERVICE_TYPE INTO v_id_tariff, v_id_service_type
    FROM SERVICE_ORDER
    WHERE ID_ORDER = p_id_order;
    
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Заказ с ID % не найден', p_id_order;
    END IF;
    
    -- Используем переданный тариф или тариф заказа
    v_id_tariff := COALESCE(p_id_tariff, v_id_tariff);
    
    IF v_id_tariff IS NULL THEN
        RAISE EXCEPTION 'Для заказа % не указан тариф', p_id_order;
    END IF;
    
    -- Получаем информацию о тарифе
    SELECT IS_WITH_VAT, VAT_RATE INTO v_is_with_vat, v_vat_rate
    FROM TARIFF
    WHERE ID_TARIFF = v_id_tariff;
    
    -- Расчет по правилам тарифа
    FOR rec_rule IN
        SELECT tr.ID_FUNCT, tr.PRIORITY
        FROM TARIFF_RULE tr
        WHERE tr.ID_TARIFF = v_id_tariff AND tr.IS_ACTIVE = 1
        ORDER BY tr.PRIORITY
    LOOP
        BEGIN
            -- Здесь можно добавить логику вызова правил расчета
            -- v_base_cost := v_base_cost + CALC_VAL_F(rec_rule.ID_FUNCT, ...);
            NULL;
        EXCEPTION
            WHEN OTHERS THEN
                CONTINUE;
        END;
    END LOOP;
    
    -- Расчет по ставкам тарифа
    FOR rec_rate IN
        SELECT tr.RATE_VALUE, tr.COD_RATE
        FROM TARIFF_RATE tr
        WHERE tr.ID_TARIFF = v_id_tariff
    LOOP
        -- Простой расчет: суммируем ставки
        -- В реальности нужна более сложная логика сопоставления параметров
        v_base_cost := v_base_cost + rec_rate.RATE_VALUE;
    END LOOP;
    
    -- Применяем повышающие коэффициенты
    FOR rec_coeff IN
        SELECT tc.COEFF_VALUE
        FROM TARIFF_COEFFICIENT tc
        WHERE tc.ID_TARIFF = v_id_tariff
    LOOP
        v_coeff := v_coeff * rec_coeff.COEFF_VALUE;
    END LOOP;
    
    v_total_cost := v_base_cost * v_coeff;
    
    -- Учитываем НДС
    IF v_is_with_vat = 0 THEN
        -- Тариф без НДС, добавляем НДС
        v_total_cost := v_total_cost * (1 + v_vat_rate / 100);
    END IF;
    
    -- Обновляем стоимость заказа
    UPDATE SERVICE_ORDER
    SET TOTAL_COST = v_total_cost, ID_TARIFF = v_id_tariff
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
    v_quantity DOUBLE PRECISION;
    v_unit_cost DOUBLE PRECISION := 0;
    v_total_cost DOUBLE PRECISION;
    rec_param RECORD;
    rec_rate RECORD;
BEGIN
    -- Получаем количество
    SELECT QUANTITY INTO v_quantity
    FROM ORDER_ITEM
    WHERE ID_ORDER_ITEM = p_id_order_item;
    
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Позиция заказа с ID % не найдена', p_id_order_item;
    END IF;
    
    -- Находим подходящую ставку тарифа
    -- Упрощенная логика: берем первую ставку
    SELECT RATE_VALUE INTO v_unit_cost
    FROM TARIFF_RATE
    WHERE ID_TARIFF = p_id_tariff
    LIMIT 1;
    
    v_unit_cost := COALESCE(v_unit_cost, 0);
    v_total_cost := v_unit_cost * v_quantity;
    
    -- Обновляем позицию
    UPDATE ORDER_ITEM
    SET UNIT_COST = v_unit_cost, TOTAL_COST = v_total_cost
    WHERE ID_ORDER_ITEM = p_id_order_item;
    
    RETURN v_total_cost;
END;
$$;

COMMENT ON FUNCTION CALC_ORDER_ITEM_COST IS 'Расчет стоимости позиции заказа';

-- ============================================================================
-- FIND_OPTIMAL_EXECUTOR - Поиск оптимального исполнителя по стоимости
-- ============================================================================

CREATE OR REPLACE FUNCTION FIND_OPTIMAL_EXECUTOR(
    p_id_service_type INTEGER,
    p_target_date DATE DEFAULT CURRENT_DATE
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
BEGIN
    RETURN QUERY
    SELECT 
        e.ID_EXECUTOR,
        e.NAME_EXECUTOR,
        t.ID_TARIFF,
        t.NAME_TARIFF,
        (SELECT COALESCE(SUM(tr.RATE_VALUE), 0) FROM TARIFF_RATE tr WHERE tr.ID_TARIFF = t.ID_TARIFF) as estimated_cost
    FROM EXECUTOR e
    JOIN TARIFF t ON t.ID_EXECUTOR = e.ID_EXECUTOR
    WHERE e.IS_ACTIVE = 1
      AND t.IS_ACTIVE = 1
      AND t.ID_SERVICE_TYPE = p_id_service_type
      AND t.DATE_BEGIN <= p_target_date
      AND (t.DATE_END IS NULL OR t.DATE_END >= p_target_date)
    ORDER BY estimated_cost ASC;
END;
$$;

COMMENT ON FUNCTION FIND_OPTIMAL_EXECUTOR IS 'Поиск оптимального исполнителя по стоимости для типа услуги';

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
    v_id_service_type INTEGER;
    v_order_date DATE;
BEGIN
    -- Получаем тип услуги и дату заказа
    SELECT ID_SERVICE_TYPE, EXECUTION_DATE INTO v_id_service_type, v_order_date
    FROM SERVICE_ORDER
    WHERE ID_ORDER = p_id_order;
    
    IF NOT FOUND THEN
        RAISE EXCEPTION 'Заказ с ID % не найден', p_id_order;
    END IF;
    
    v_order_date := COALESCE(v_order_date, CURRENT_DATE);
    
    RETURN QUERY
    SELECT 
        t.ID_TARIFF,
        t.NAME_TARIFF,
        COALESCE(e.NAME_EXECUTOR, 'Без исполнителя'::VARCHAR) as executor_name,
        CALC_ORDER_COST(p_id_order, t.ID_TARIFF) as estimated_cost
    FROM TARIFF t
    LEFT JOIN EXECUTOR e ON t.ID_EXECUTOR = e.ID_EXECUTOR
    WHERE t.IS_ACTIVE = 1
      AND t.ID_SERVICE_TYPE = v_id_service_type
      AND t.DATE_BEGIN <= v_order_date
      AND (t.DATE_END IS NULL OR t.DATE_END >= v_order_date)
    ORDER BY estimated_cost ASC;
END;
$$;

COMMENT ON FUNCTION FIND_OPTIMAL_TARIFF IS 'Поиск оптимального тарифа для заказа';

-- ============================================================================
-- VALIDATE_ORDER - Валидация заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION VALIDATE_ORDER(
    p_id_order INTEGER
)
RETURNS TABLE (
    is_valid BOOLEAN,
    error_message TEXT
)
LANGUAGE plpgsql
AS $$
DECLARE
    v_id_service_type INTEGER;
    v_order_exists BOOLEAN;
    rec_param RECORD;
    v_errors TEXT := '';
    v_is_valid BOOLEAN := TRUE;
BEGIN
    -- Проверяем существование заказа
    SELECT EXISTS(SELECT 1 FROM SERVICE_ORDER WHERE ID_ORDER = p_id_order) INTO v_order_exists;
    
    IF NOT v_order_exists THEN
        RETURN QUERY SELECT FALSE, 'Заказ не найден'::TEXT;
        RETURN;
    END IF;
    
    -- Получаем тип услуги
    SELECT ID_SERVICE_TYPE INTO v_id_service_type
    FROM SERVICE_ORDER
    WHERE ID_ORDER = p_id_order;
    
    -- Проверяем наличие обязательных параметров
    FOR rec_param IN
        SELECT stp.ID_PAR, p.NAME_PAR, stp.MIN_VAL, stp.MAX_VAL
        FROM SERVICE_TYPE_PARAM stp
        JOIN PARAMETR1 p ON stp.ID_PAR = p.ID_PAR
        WHERE stp.ID_SERVICE_TYPE = v_id_service_type
          AND stp.IS_REQUIRED = 1
    LOOP
        -- Проверяем, есть ли значение параметра в заказе
        IF NOT EXISTS(
            SELECT 1 FROM ORDER_PARAM op
            WHERE op.ID_ORDER = p_id_order
              AND op.ID_PAR = rec_param.ID_PAR
              AND (op.VAL_NUM IS NOT NULL OR op.VAL_STR IS NOT NULL OR 
                   op.VAL_DATE IS NOT NULL OR op.ID_VAL_ENUM IS NOT NULL)
        ) THEN
            v_is_valid := FALSE;
            v_errors := v_errors || 'Не задан обязательный параметр: ' || rec_param.NAME_PAR || '; ';
        ELSE
            -- Проверяем ограничения min/max для числовых параметров
            IF rec_param.MIN_VAL IS NOT NULL OR rec_param.MAX_VAL IS NOT NULL THEN
                DECLARE
                    v_val DOUBLE PRECISION;
                BEGIN
                    SELECT VAL_NUM INTO v_val
                    FROM ORDER_PARAM
                    WHERE ID_ORDER = p_id_order AND ID_PAR = rec_param.ID_PAR;
                    
                    IF v_val IS NOT NULL THEN
                        IF rec_param.MIN_VAL IS NOT NULL AND v_val < rec_param.MIN_VAL THEN
                            v_is_valid := FALSE;
                            v_errors := v_errors || 'Параметр ' || rec_param.NAME_PAR || 
                                        ' меньше минимального значения (' || rec_param.MIN_VAL || '); ';
                        END IF;
                        
                        IF rec_param.MAX_VAL IS NOT NULL AND v_val > rec_param.MAX_VAL THEN
                            v_is_valid := FALSE;
                            v_errors := v_errors || 'Параметр ' || rec_param.NAME_PAR || 
                                        ' больше максимального значения (' || rec_param.MAX_VAL || '); ';
                        END IF;
                    END IF;
                END;
            END IF;
        END IF;
    END LOOP;
    
    IF v_errors = '' THEN
        v_errors := 'OK';
    END IF;
    
    RETURN QUERY SELECT v_is_valid, v_errors;
END;
$$;

COMMENT ON FUNCTION VALIDATE_ORDER IS 'Валидация заказа: проверка обязательных параметров и ограничений';

-- ============================================================================
-- GET_ALL_EI - Получение всех единиц измерения
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_EI()
RETURNS TABLE (
    id_ei INTEGER,
    cod_ei VARCHAR,
    name_ei VARCHAR,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY SELECT e.ID_EI, e.COD_EI, e.NAME_EI, e.NOTE FROM EI e ORDER BY e.ID_EI;
END;
$$;

-- ============================================================================
-- GET_ALL_ENUMS - Получение всех перечислений
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_ENUMS()
RETURNS TABLE (
    id_enum INTEGER,
    cod_enum VARCHAR,
    name_enum VARCHAR,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY SELECT e.ID_ENUM, e.COD_ENUM, e.NAME_ENUM, e.NOTE FROM ENUM_VAL_R e ORDER BY e.ID_ENUM;
END;
$$;

-- ============================================================================
-- GET_ENUM_VALUES - Получение значений перечисления
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ENUM_VALUES(p_id_enum INTEGER)
RETURNS TABLE (
    id_pos_enum INTEGER,
    cod_pos VARCHAR,
    name_pos VARCHAR,
    num_pos INTEGER,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY 
    SELECT pe.ID_POS_ENUM, pe.COD_POS, pe.NAME_POS, pe.NUM_POS, pe.NOTE 
    FROM POS_ENUM pe 
    WHERE pe.ID_ENUM = p_id_enum
    ORDER BY pe.NUM_POS;
END;
$$;

-- ============================================================================
-- GET_ALL_CLASSES - Получение всех классов
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_CLASSES()
RETURNS TABLE (
    id_chem INTEGER,
    cod_chem VARCHAR,
    name_chem VARCHAR,
    parent_id INTEGER,
    lev INTEGER,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY 
    SELECT c.ID_CHEM, c.COD_CHEM, c.NAME_CHEM, c.PARENT_ID, c.LEV, c.NOTE 
    FROM CHEM_CLASS c 
    ORDER BY c.LEV, c.ID_CHEM;
END;
$$;

-- ============================================================================
-- GET_ALL_SERVICE_TYPES - Получение всех типов услуг
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_SERVICE_TYPES()
RETURNS TABLE (
    id_service_type INTEGER,
    cod_service VARCHAR,
    name_service VARCHAR,
    id_class INTEGER,
    class_name VARCHAR,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY 
    SELECT st.ID_SERVICE_TYPE, st.COD_SERVICE, st.NAME_SERVICE, st.ID_CLASS, 
           c.NAME_CHEM as class_name, st.NOTE 
    FROM SERVICE_TYPE st
    LEFT JOIN CHEM_CLASS c ON st.ID_CLASS = c.ID_CHEM
    ORDER BY st.ID_SERVICE_TYPE;
END;
$$;

-- ============================================================================
-- GET_ALL_EXECUTORS - Получение всех исполнителей
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_EXECUTORS()
RETURNS TABLE (
    id_executor INTEGER,
    cod_executor VARCHAR,
    name_executor VARCHAR,
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
    SELECT e.ID_EXECUTOR, e.COD_EXECUTOR, e.NAME_EXECUTOR, e.ADDRESS, 
           e.PHONE, e.EMAIL, e.IS_ACTIVE, e.NOTE 
    FROM EXECUTOR e 
    ORDER BY e.ID_EXECUTOR;
END;
$$;

-- ============================================================================
-- GET_ALL_TARIFFS - Получение всех тарифов
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_TARIFFS()
RETURNS TABLE (
    id_tariff INTEGER,
    cod_tariff VARCHAR,
    name_tariff VARCHAR,
    id_service_type INTEGER,
    service_name VARCHAR,
    id_executor INTEGER,
    executor_name VARCHAR,
    date_begin DATE,
    date_end DATE,
    is_with_vat INTEGER,
    vat_rate DOUBLE PRECISION,
    is_active INTEGER,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY 
    SELECT t.ID_TARIFF, t.COD_TARIFF, t.NAME_TARIFF, t.ID_SERVICE_TYPE,
           st.NAME_SERVICE as service_name, t.ID_EXECUTOR,
           e.NAME_EXECUTOR as executor_name, t.DATE_BEGIN, t.DATE_END,
           t.IS_WITH_VAT, t.VAT_RATE, t.IS_ACTIVE, t.NOTE 
    FROM TARIFF t
    LEFT JOIN SERVICE_TYPE st ON t.ID_SERVICE_TYPE = st.ID_SERVICE_TYPE
    LEFT JOIN EXECUTOR e ON t.ID_EXECUTOR = e.ID_EXECUTOR
    ORDER BY t.ID_TARIFF;
END;
$$;

-- ============================================================================
-- GET_TARIFF_RATES - Получение ставок тарифа
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_TARIFF_RATES(p_id_tariff INTEGER)
RETURNS TABLE (
    id_tariff_rate INTEGER,
    cod_rate VARCHAR,
    name_rate VARCHAR,
    rate_value DOUBLE PRECISION,
    id_ei INTEGER,
    unit_name VARCHAR,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY 
    SELECT tr.ID_TARIFF_RATE, tr.COD_RATE, tr.NAME_RATE, tr.RATE_VALUE,
           tr.ID_EI, ei.NAME_EI as unit_name, tr.NOTE 
    FROM TARIFF_RATE tr
    LEFT JOIN EI ei ON tr.ID_EI = ei.ID_EI
    WHERE tr.ID_TARIFF = p_id_tariff
    ORDER BY tr.ID_TARIFF_RATE;
END;
$$;

-- ============================================================================
-- GET_ALL_ORDERS - Получение всех заказов
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_ORDERS()
RETURNS TABLE (
    id_order INTEGER,
    cod_order VARCHAR,
    id_service_type INTEGER,
    service_name VARCHAR,
    order_date DATE,
    execution_date DATE,
    status INTEGER,
    status_name VARCHAR,
    id_executor INTEGER,
    executor_name VARCHAR,
    id_tariff INTEGER,
    tariff_name VARCHAR,
    total_cost DOUBLE PRECISION,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY 
    SELECT o.ID_ORDER, o.COD_ORDER, o.ID_SERVICE_TYPE,
           st.NAME_SERVICE as service_name, o.ORDER_DATE, o.EXECUTION_DATE,
           o.STATUS,
           CASE o.STATUS 
               WHEN 0 THEN 'Новый'
               WHEN 1 THEN 'В работе'
               WHEN 2 THEN 'Выполнен'
               WHEN 3 THEN 'Отменен'
               ELSE 'Неизвестно'
           END::VARCHAR as status_name,
           o.ID_EXECUTOR, e.NAME_EXECUTOR as executor_name,
           o.ID_TARIFF, t.NAME_TARIFF as tariff_name,
           o.TOTAL_COST, o.NOTE 
    FROM SERVICE_ORDER o
    LEFT JOIN SERVICE_TYPE st ON o.ID_SERVICE_TYPE = st.ID_SERVICE_TYPE
    LEFT JOIN EXECUTOR e ON o.ID_EXECUTOR = e.ID_EXECUTOR
    LEFT JOIN TARIFF t ON o.ID_TARIFF = t.ID_TARIFF
    ORDER BY o.ID_ORDER DESC;
END;
$$;

-- ============================================================================
-- GET_ORDER_PARAMS - Получение параметров заказа
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ORDER_PARAMS(p_id_order INTEGER)
RETURNS TABLE (
    id_par INTEGER,
    cod_par VARCHAR,
    name_par VARCHAR,
    type_par INTEGER,
    val_num DOUBLE PRECISION,
    val_str TEXT,
    val_date DATE,
    id_val_enum INTEGER,
    enum_name VARCHAR,
    unit_name VARCHAR
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY 
    SELECT op.ID_PAR, p.COD_PAR, p.NAME_PAR, p.TYPE_PAR,
           op.VAL_NUM, op.VAL_STR, op.VAL_DATE, op.ID_VAL_ENUM,
           pe.NAME_POS as enum_name, ei.NAME_EI as unit_name
    FROM ORDER_PARAM op
    JOIN PARAMETR1 p ON op.ID_PAR = p.ID_PAR
    LEFT JOIN POS_ENUM pe ON op.ID_VAL_ENUM = pe.ID_POS_ENUM
    LEFT JOIN EI ei ON p.EI = ei.ID_EI
    WHERE op.ID_ORDER = p_id_order
    ORDER BY p.ID_PAR;
END;
$$;

-- ============================================================================
-- GET_ALL_PARAMETERS - Получение всех параметров
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_PARAMETERS()
RETURNS TABLE (
    id_par INTEGER,
    cod_par VARCHAR,
    name_par VARCHAR,
    class_par INTEGER,
    type_par INTEGER,
    type_name VARCHAR,
    id_ei INTEGER,
    unit_name VARCHAR,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY 
    SELECT p.ID_PAR, p.COD_PAR, p.NAME_PAR, p.CLASS_PAR, p.TYPE_PAR,
           CASE p.TYPE_PAR 
               WHEN 0 THEN 'Число'
               WHEN 1 THEN 'Строка'
               WHEN 2 THEN 'Дата'
               WHEN 3 THEN 'Перечисление'
               ELSE 'Неизвестно'
           END::VARCHAR as type_name,
           p.EI, ei.NAME_EI as unit_name, p.NOTE
    FROM PARAMETR1 p
    LEFT JOIN EI ei ON p.EI = ei.ID_EI
    ORDER BY p.ID_PAR;
END;
$$;

-- ============================================================================
-- GET_ALL_COEFFICIENTS - Получение всех коэффициентов
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_ALL_COEFFICIENTS()
RETURNS TABLE (
    id_coefficient INTEGER,
    cod_coeff VARCHAR,
    name_coeff VARCHAR,
    value_min DOUBLE PRECISION,
    value_max DOUBLE PRECISION,
    value_default DOUBLE PRECISION,
    note TEXT
)
LANGUAGE plpgsql
AS $$
BEGIN
    RETURN QUERY 
    SELECT c.ID_COEFFICIENT, c.COD_COEFF, c.NAME_COEFF,
           c.VALUE_MIN, c.VALUE_MAX, c.VALUE_DEFAULT, c.NOTE
    FROM COEFFICIENT c
    ORDER BY c.ID_COEFFICIENT;
END;
$$;

-- ============================================================================
-- GET_SERVICE_TYPE_PARAMS - Получение параметров типа услуги
-- ============================================================================

CREATE OR REPLACE FUNCTION GET_SERVICE_TYPE_PARAMS(p_id_service_type INTEGER)
RETURNS TABLE (
    id_par INTEGER,
    cod_par VARCHAR,
    name_par VARCHAR,
    type_par INTEGER,
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
    SELECT stp.ID_PAR, p.COD_PAR, p.NAME_PAR, p.TYPE_PAR,
           stp.IS_REQUIRED, stp.DEFAULT_VAL_NUM, stp.DEFAULT_VAL_STR,
           stp.MIN_VAL, stp.MAX_VAL, ei.NAME_EI as unit_name
    FROM SERVICE_TYPE_PARAM stp
    JOIN PARAMETR1 p ON stp.ID_PAR = p.ID_PAR
    LEFT JOIN EI ei ON p.EI = ei.ID_EI
    WHERE stp.ID_SERVICE_TYPE = p_id_service_type
    ORDER BY p.ID_PAR;
END;
$$;

-- ============================================================================
-- Конец скрипта процедур калькулятора
-- ============================================================================
