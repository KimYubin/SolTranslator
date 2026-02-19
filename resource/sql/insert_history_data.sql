INSERT INTO history_data( engine_type
                        , source_lang
                        , target_lang
                        , source_text
                        , target_text
                        , text_style
                        , revision)
SELECT :engine_type
     , :source_lang
     , :target_lang
     , :source_text
     , :target_text
     , :text_style
     , COALESCE(MAX(history_data.revision), 0) + 1
FROM history_data
WHERE history_data.source_text = :source_text
  AND history_data.engine_type = :engine_type
  AND history_data.source_lang = :source_lang
  AND history_data.target_lang = :target_lang

