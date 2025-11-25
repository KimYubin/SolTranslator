SELECT history_data.id, history_data.target_text
FROM history_data
WHERE history_data.engine_type = :engine_type
  AND history_data.source_lang = :source_lang
  AND history_data.target_lang = :target_lang
  AND history_data.source_text = :source_text;
