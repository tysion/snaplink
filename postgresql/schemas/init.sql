-- Создаем схему snaplink
CREATE SCHEMA IF NOT EXISTS snaplink;

-- Создаем тип snaplink.url
CREATE DOMAIN snaplink.url AS TEXT
    CHECK (VALUE ~ '^https?://');

-- Создаем таблицу snaplink.urls
CREATE TABLE IF NOT EXISTS snaplink.urls (
    id TEXT PRIMARY KEY CHECK (LENGTH(id) <= 32), -- Ограничиваем длину id
    link snaplink.url NOT NULL
);

-- Создаем хеш-индекс для быстрого поиска по id
CREATE INDEX IF NOT EXISTS urls_id_hash_idx ON snaplink.urls USING HASH (id);

-- Функция добавления новой ссылки
CREATE OR REPLACE FUNCTION snaplink.add_new_url(p_id TEXT, p_link snaplink.url)
RETURNS VOID AS $$
BEGIN
    INSERT INTO snaplink.urls (id, link) VALUES (p_id, p_link)
    ON CONFLICT (id) DO NOTHING;
END;
$$ LANGUAGE plpgsql;

-- Функция получения ссылки по id
CREATE OR REPLACE FUNCTION snaplink.get_url_by_id(p_id TEXT)
RETURNS snaplink.url AS $$
DECLARE
    result snaplink.url;
BEGIN
    SELECT link INTO result FROM snaplink.urls WHERE id = p_id;
    RETURN result;
END;
$$ LANGUAGE plpgsql;