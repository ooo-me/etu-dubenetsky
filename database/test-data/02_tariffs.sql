-- ============================================================================
-- Тестовые данные: Тарифы и правила
-- СУБД: PostgreSQL 12+
-- Описание: Примеры тарифов на грузоперевозки и хранение
-- ============================================================================

-- ============================================================================
-- 1. Тариф на грузоперевозки "Стандарт"
-- ============================================================================

-- Создание тарифа
SELECT INS_OB(
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'TARIFF'),
    'TARIFF_CARGO_STD',
    'Тариф "Стандарт" на грузоперевозки',
    NULL,
    'Базовый тариф для грузоперевозок по городу и области'
) as tariff_id;
-- Предположим, что вернулся ID = 100

-- Создание функции расчета стоимости часа
SELECT INS_FUNCT(
    'CALC_HOUR_COST',
    'Расчет стоимости по часам',
    1,  -- Арифметическое выражение
    '*',
    'Умножение времени на стоимость часа'
);
-- Предположим, ID = 10

-- Добавление аргументов
SELECT INS_ARG_FUN(
    (SELECT ID_FUNCT FROM FUNCT_R WHERE COD_FUNCT = 'CALC_HOUR_COST'),
    1,
    NULL,
    'Планируемое время'
);

SELECT INS_ARG_FUN(
    (SELECT ID_FUNCT FROM FUNCT_R WHERE COD_FUNCT = 'CALC_HOUR_COST'),
    2,
    NULL,
    'Стоимость часа'
);

-- Создание вызова функции для тарифа
SELECT INS_FACT_FUN(
    (SELECT ID_FUNCT FROM FUNCT_R WHERE COD_FUNCT = 'CALC_HOUR_COST'),
    100,  -- ID тарифа
    1,
    'Расчет стоимости по времени'
);
-- Предположим, ID_FACT_FUN = 1

-- Запись параметров
CALL WRITE_FACT_PAR(
    1,  -- ID вызова функции
    (SELECT ID_ARG FROM ARG_FUNCT WHERE ID_FUNCT = (SELECT ID_FUNCT FROM FUNCT_R WHERE COD_FUNCT = 'CALC_HOUR_COST') AND NUM_ARG = 1),
    NULL,  -- ID константы
    NULL,  -- ID функции
    NULL,  -- ID вызова функции
    NULL,  -- VAL_NUM (будет из параметра заказа)
    NULL,
    NULL,
    NULL
);

CALL WRITE_FACT_PAR(
    1,
    (SELECT ID_ARG FROM ARG_FUNCT WHERE ID_FUNCT = (SELECT ID_FUNCT FROM FUNCT_R WHERE COD_FUNCT = 'CALC_HOUR_COST') AND NUM_ARG = 2),
    (SELECT ID_CONST FROM CONST WHERE COD_CONST = 'HOUR_RATE'),  -- Константа стоимости часа
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
);

-- Создание функции расчета перепробега по городу
SELECT INS_FUNCT(
    'CALC_CITY_DISTANCE_COST',
    'Расчет стоимости километража по городу',
    1,  -- Арифметическое
    '*',
    'Расчет перепробега по городу'
);
-- Предположим, ID = 11

-- Создание функции расчета перепробега по области
SELECT INS_FUNCT(
    'CALC_REGION_DISTANCE_COST',
    'Расчет стоимости километража по области',
    1,  -- Арифметическое
    '*',
    'Расчет перепробега по области с повышающим коэффициентом'
);
-- Предположим, ID = 12

-- Создание итоговой функции суммирования
SELECT INS_FUNCT(
    'CALC_TOTAL_CARGO_COST',
    'Итоговая стоимость грузоперевозки',
    1,  -- Арифметическое
    '+',
    'Сумма всех компонентов стоимости'
);
-- Предположим, ID = 13

-- ============================================================================
-- 2. Тариф на хранение "Эконом"
-- ============================================================================

SELECT INS_OB(
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'TARIFF'),
    'TARIFF_STORAGE_ECO',
    'Тариф "Эконом" на хранение',
    NULL,
    'Экономичный тариф для длительного хранения'
) as storage_tariff_id;
-- Предположим, ID = 101

-- Создание функции расчета базовой стоимости хранения
SELECT INS_FUNCT(
    'CALC_STORAGE_BASE',
    'Базовая стоимость хранения',
    1,  -- Арифметическое
    '*',
    'Количество паллет × стоимость × дни'
);
-- Предположим, ID = 20

-- Создание функции учета оборачиваемости
SELECT INS_FUNCT(
    'CALC_TURNOVER_DISCOUNT',
    'Скидка за оборачиваемость',
    1,  -- Арифметическое
    '*',
    'Расчет скидки на основе оборачиваемости'
);
-- Предположим, ID = 21

-- Создание функции итоговой стоимости хранения
SELECT INS_FUNCT(
    'CALC_STORAGE_TOTAL',
    'Итоговая стоимость хранения',
    1,  -- Арифметическое
    '-',
    'Базовая стоимость минус скидка'
);
-- Предположим, ID = 22

-- ============================================================================
-- 3. Примеры правил с условиями
-- ============================================================================

-- Правило: Если вес > 1т, то применить повышающий коэффициент
SELECT INS_FUNCT(
    'RULE_HEAVY_CARGO',
    'Правило для тяжелого груза',
    0,  -- Предикат
    '>',
    'Проверка веса груза'
);
-- Предположим, ID = 30

-- Добавление аргументов к правилу
SELECT INS_ARG_FUN(
    (SELECT ID_FUNCT FROM FUNCT_R WHERE COD_FUNCT = 'RULE_HEAVY_CARGO'),
    1, NULL, 'Вес груза'
);

SELECT INS_ARG_FUN(
    (SELECT ID_FUNCT FROM FUNCT_R WHERE COD_FUNCT = 'RULE_HEAVY_CARGO'),
    2, NULL, 'Пороговое значение'
);

-- Создание функции действия для тяжелого груза
SELECT INS_FUNCT(
    'ACTION_HEAVY_CARGO',
    'Увеличение стоимости для тяжелого груза',
    1,  -- Арифметическое
    '*',
    'Умножение на коэффициент 1.2'
);
-- Предположим, ID = 31

-- Привязка решения к правилу для тарифа
CALL INS_DEC_F(
    (SELECT ID_FUNCT FROM FUNCT_R WHERE COD_FUNCT = 'RULE_HEAVY_CARGO'),
    100,  -- ID тарифа
    1,    -- Номер вызова
    (SELECT ID_FUNCT FROM FUNCT_R WHERE COD_FUNCT = 'ACTION_HEAVY_CARGO'),
    1,    -- Приоритет
    'Применить повышающий коэффициент для тяжелого груза'
);

-- ============================================================================
-- 4. Пример заказа на грузоперевозку
-- ============================================================================

-- Создание заказа
SELECT INS_OB(
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'ORDER'),
    'ORDER_001',
    'Заказ на перевозку №001',
    NULL,
    'Тестовый заказ для проверки расчета'
) as order_id;
-- Предположим, ID = 200

-- Установка параметров заказа через PAR_PROD2
INSERT INTO PAR_PROD2 (ID_PAR, ID_PR, VAL_NUM, IS_REQUIRE)
VALUES
    ((SELECT ID_PAR FROM PARAMETR1 WHERE COD_PAR = 'CARGO_WEIGHT'), 200, 0.5, 1),
    ((SELECT ID_PAR FROM PARAMETR1 WHERE COD_PAR = 'CARGO_VOLUME'), 200, 1.0, 1),
    ((SELECT ID_PAR FROM PARAMETR1 WHERE COD_PAR = 'PLANNING_TIME'), 200, 6.0, 1),
    ((SELECT ID_PAR FROM PARAMETR1 WHERE COD_PAR = 'DISTANCE_CITY'), 200, 105.0, 1),
    ((SELECT ID_PAR FROM PARAMETR1 WHERE COD_PAR = 'DISTANCE_REGION'), 200, 200.0, 0);

-- ============================================================================
-- 5. Тестовые расчеты
-- ============================================================================

-- Расчет стоимости заказа по тарифу "Стандарт"
SELECT CALC_VAL_F(
    (SELECT ID_FUNCT FROM FUNCT_R WHERE COD_FUNCT = 'CALC_TOTAL_CARGO_COST'),
    200,  -- ID заказа
    100   -- ID тарифа
) as calculated_cost;

-- Проверка промежуточных результатов
SELECT 
    f.COD_FUNCT,
    f.NAME_FUNCT,
    rv.VAL_NUM as calculated_value
FROM ROLE_VAL rv
JOIN FUNCT_R f ON rv.ID_FUNCT = f.ID_FUNCT
WHERE rv.ID_PR = 200
ORDER BY f.ID_FUNCT;

-- ============================================================================
-- 6. Дополнительные тарифы для сравнения
-- ============================================================================

-- Тариф "Премиум"
SELECT INS_OB(
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'TARIFF'),
    'TARIFF_CARGO_PREMIUM',
    'Тариф "Премиум" на грузоперевозки',
    NULL,
    'Расширенный тариф с повышенным уровнем сервиса'
);

-- Тариф "Бюджет"
SELECT INS_OB(
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'TARIFF'),
    'TARIFF_CARGO_BUDGET',
    'Тариф "Бюджет" на грузоперевозки',
    NULL,
    'Экономичный тариф для стандартных перевозок'
);

-- ============================================================================
-- Примеры запросов для анализа
-- ============================================================================

-- Вывод всех тарифов
SELECT 
    p.ID_PR,
    p.COD_PR,
    p.NAME_PR,
    c.NAME_CHEM as class_name
FROM PROD p
JOIN CHEM_CLASS c ON p.CLASS_PR = c.ID_CHEM
WHERE c.COD_CHEM = 'TARIFF'
ORDER BY p.COD_PR;

-- Вывод параметров заказа
SELECT 
    p.COD_PAR,
    p.NAME_PAR,
    pp.VAL_NUM,
    pp.VAL_STR,
    e.NAME_EI as unit
FROM PAR_PROD2 pp
JOIN PARAMETR1 p ON pp.ID_PAR = p.ID_PAR
LEFT JOIN EI e ON p.EI = e.ID_EI
WHERE pp.ID_PR = 200
ORDER BY p.ID_PAR;

-- Вывод функций и их типов
SELECT 
    f.ID_FUNCT,
    f.COD_FUNCT,
    f.NAME_FUNCT,
    CASE f.TYPE_F
        WHEN 0 THEN 'Предикат'
        WHEN 1 THEN 'Арифметика'
        WHEN 2 THEN 'Логика'
        WHEN 3 THEN 'Выбор'
    END as function_type,
    f.OPERATION
FROM FUNCT_R f
ORDER BY f.TYPE_F, f.ID_FUNCT;

-- ============================================================================
-- Конец скрипта тестовых данных (тарифы)
-- ============================================================================
