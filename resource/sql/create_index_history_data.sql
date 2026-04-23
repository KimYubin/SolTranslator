CREATE UNIQUE INDEX IF NOT EXISTS history_data_index
    ON history_data (
          source_text
        , engine
        , source_lang
        , target_lang
        , revision DESC
);
