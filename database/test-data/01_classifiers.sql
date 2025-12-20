-- ============================================================================
-- Тестовые данные: Классификаторы
-- СУБД: PostgreSQL 12+
-- Описание: Заполнение базовых справочников системы
-- ============================================================================

-- ============================================================================
-- 1. Единицы измерения
-- ============================================================================

INSERT INTO EI (COD_EI, NAME_EI, NOTE) VALUES
('т', 'тонна', 'Единица массы'),
('м3', 'кубический метр', 'Единица объема'),
('шт', 'штука', 'Единица количества'),
('ч', 'час', 'Единица времени'),
('км', 'километр', 'Единица расстояния'),
('руб', 'рубль', 'Денежная единица'),
('день', 'день', 'Единица времени'),
('мес', 'месяц', 'Единица времени');

-- ============================================================================
-- 2. Перечисления
-- ============================================================================

-- Класс автомобиля
INSERT INTO ENUM_VAL_R (COD_ENUM, NAME_ENUM, NOTE) VALUES
('VEHICLE_CLASS', 'Класс автомобиля', 'Типы автомобилей для грузоперевозок');

INSERT INTO POS_ENUM (ID_ENUM, COD_POS, NAME_POS, NUM_POS) VALUES
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'VEHICLE_CLASS'), 'CLOSED', 'Закрытый', 1),
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'VEHICLE_CLASS'), 'OPEN', 'Открытый', 2),
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'VEHICLE_CLASS'), 'REFRIGERATOR', 'Рефрижератор', 3);

-- Временной интервал
INSERT INTO ENUM_VAL_R (COD_ENUM, NAME_ENUM, NOTE) VALUES
('TIME_INTERVAL', 'Временной интервал', 'Интервалы времени для аренды автомобиля');

INSERT INTO POS_ENUM (ID_ENUM, COD_POS, NAME_POS, NUM_POS) VALUES
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'TIME_INTERVAL'), 'HOURS_4', '4 часа', 1),
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'TIME_INTERVAL'), 'HOURS_8', '8 часов', 2),
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'TIME_INTERVAL'), 'HOURS_4_PLUS_1', '4+1 час', 3),
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'TIME_INTERVAL'), 'HOURS_5_PLUS_1', '5+1 час', 4),
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'TIME_INTERVAL'), 'HOURS_6_PLUS_1', '6+1 час', 5);

-- Тип маршрута
INSERT INTO ENUM_VAL_R (COD_ENUM, NAME_ENUM, NOTE) VALUES
('ROUTE_TYPE', 'Тип маршрута', 'Виды маршрутов перевозки');

INSERT INTO POS_ENUM (ID_ENUM, COD_POS, NAME_POS, NUM_POS) VALUES
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'ROUTE_TYPE'), 'CITY', 'По городу', 1),
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'ROUTE_TYPE'), 'REGION', 'По области', 2),
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'ROUTE_TYPE'), 'INTERCITY', 'Межгород', 3);

-- Тип операции
INSERT INTO ENUM_VAL_R (COD_ENUM, NAME_ENUM, NOTE) VALUES
('OPERATION_TYPE', 'Тип операции', 'Типы операций приемки/отгрузки');

INSERT INTO POS_ENUM (ID_ENUM, COD_POS, NAME_POS, NUM_POS) VALUES
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'OPERATION_TYPE'), 'MECHANIZED', 'Механизированная', 1),
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'OPERATION_TYPE'), 'MANUAL', 'Ручная', 2),
((SELECT ID_ENUM FROM ENUM_VAL_R WHERE COD_ENUM = 'OPERATION_TYPE'), 'PIECE_GOODS', 'Штучный товар', 3);

-- ============================================================================
-- 3. Классификатор компонентов системы
-- ============================================================================

-- Корневые классы
SELECT INS_CLASS('ROOT', 'Корень', NULL, 'Корневой класс системы');
SELECT INS_CLASS('SERVICE', 'Услуги', 
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'ROOT'), 
    'Классификатор услуг');
SELECT INS_CLASS('TARIFF', 'Тарифы', 
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'ROOT'), 
    'Классификатор тарифов');
SELECT INS_CLASS('ORDER', 'Заказы', 
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'ROOT'), 
    'Классификатор заказов');

-- Классы услуг
SELECT INS_CLASS('CARGO_SERVICE', 'Грузоперевозки', 
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'SERVICE'), 
    'Услуги грузоперевозок');
SELECT INS_CLASS('STORAGE_SERVICE', 'Ответственное хранение', 
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'SERVICE'), 
    'Услуги складского хранения');

-- ============================================================================
-- 4. Константы
-- ============================================================================

-- Константы для грузоперевозок
SELECT INS_CONST('HOUR_RATE', 'Стоимость часа', 800.0, NULL, NULL, 
    (SELECT ID_EI FROM EI WHERE COD_EI = 'руб'), 
    'Базовая стоимость часа работы автомобиля');

SELECT INS_CONST('KM_RATE', 'Стоимость километра', 50.0, NULL, NULL,
    (SELECT ID_EI FROM EI WHERE COD_EI = 'руб'),
    'Базовая стоимость километра перепробега');

SELECT INS_CONST('BASE_HOURS', 'Базовое количество часов', 4.0, NULL, NULL,
    (SELECT ID_EI FROM EI WHERE COD_EI = 'ч'),
    'Базовое количество часов в тарифе');

SELECT INS_CONST('BASE_KM', 'Базовый километраж', 50.0, NULL, NULL,
    (SELECT ID_EI FROM EI WHERE COD_EI = 'км'),
    'Базовый километраж, включенный в тариф');

-- Константы для хранения
SELECT INS_CONST('PALLET_DAY_RATE', 'Стоимость паллето-места в день', 15.0, NULL, NULL,
    (SELECT ID_EI FROM EI WHERE COD_EI = 'руб'),
    'Базовая стоимость хранения одного паллето-места в день');

SELECT INS_CONST('TURNOVER_FACTOR', 'Коэффициент оборачиваемости', 0.1, NULL, NULL,
    NULL,
    'Коэффициент влияния оборачиваемости на стоимость');

-- ============================================================================
-- 5. Параметры
-- ============================================================================

-- Параметры для грузоперевозок
INSERT INTO PARAMETR1 (COD_PAR, NAME_PAR, CLASS_PAR, TYPE_PAR, EI, NOTE) VALUES
('CARGO_WEIGHT', 'Вес груза', 
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'CARGO_SERVICE'),
    0, (SELECT ID_EI FROM EI WHERE COD_EI = 'т'), 
    'Вес перевозимого груза в тоннах'),
('CARGO_VOLUME', 'Объем груза',
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'CARGO_SERVICE'),
    0, (SELECT ID_EI FROM EI WHERE COD_EI = 'м3'),
    'Объем груза в кубических метрах'),
('PALLET_COUNT', 'Количество паллет',
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'CARGO_SERVICE'),
    0, (SELECT ID_EI FROM EI WHERE COD_EI = 'шт'),
    'Количество паллет'),
('PLANNING_TIME', 'Планируемое время',
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'CARGO_SERVICE'),
    0, (SELECT ID_EI FROM EI WHERE COD_EI = 'ч'),
    'Планируемое время работы в часах'),
('DISTANCE_CITY', 'Расстояние по городу',
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'CARGO_SERVICE'),
    0, (SELECT ID_EI FROM EI WHERE COD_EI = 'км'),
    'Расстояние перевозки по городу'),
('DISTANCE_REGION', 'Расстояние по области',
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'CARGO_SERVICE'),
    0, (SELECT ID_EI FROM EI WHERE COD_EI = 'км'),
    'Расстояние перевозки по области');

-- Параметры для хранения
INSERT INTO PARAMETR1 (COD_PAR, NAME_PAR, CLASS_PAR, TYPE_PAR, EI, NOTE) VALUES
('AVG_PALLET_PLACES', 'Средний объем паллето-мест',
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'STORAGE_SERVICE'),
    0, (SELECT ID_EI FROM EI WHERE COD_EI = 'шт'),
    'Средний объем паллето-мест в день'),
('PALLET_MODEL', 'Модель паллеты',
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'STORAGE_SERVICE'),
    1, NULL,
    'Модель паллеты (размеры)'),
('TURNOVER', 'Оборачиваемость',
    (SELECT ID_CHEM FROM CHEM_CLASS WHERE COD_CHEM = 'STORAGE_SERVICE'),
    0, NULL,
    'Коэффициент оборачиваемости');

-- ============================================================================
-- Связь параметров с классами
-- ============================================================================

INSERT INTO PAR_CLASS1 (ID_PAR, ID_CHEM)
SELECT p.ID_PAR, c.ID_CHEM
FROM PARAMETR1 p
CROSS JOIN CHEM_CLASS c
WHERE p.CLASS_PAR = c.ID_CHEM;

-- ============================================================================
-- Конец скрипта тестовых данных (классификаторы)
-- ============================================================================
