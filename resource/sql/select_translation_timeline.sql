SELECT history_data.id
     , history_data.engine_type
     , history_data.source_lang
     , history_data.target_lang
     , history_data.source_text
     , history_data.target_text
     , history_timeline.id
     , history_timeline.accessed_time
     , history_data.text_style
FROM history_timeline
         JOIN history_data
              ON history_timeline.history_data_id = history_data.id

ORDER BY history_timeline.accessed_time DESC;