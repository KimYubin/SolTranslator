CREATE TABLE IF NOT EXISTS history_data
(
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    engine      TEXT,
    source_lang TEXT,
    target_lang TEXT,
    source_text TEXT,
    target_text TEXT,
    text_style  TEXT,
    revision    INTEGER
);
