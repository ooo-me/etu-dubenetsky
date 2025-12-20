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
