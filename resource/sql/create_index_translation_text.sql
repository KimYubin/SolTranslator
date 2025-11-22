CREATE UNIQUE INDEX IF NOT EXISTS translation_text_index
    ON history_data (source_text, engine_type, source_lang, target_lang);
