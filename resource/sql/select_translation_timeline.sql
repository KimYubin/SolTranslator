SELECT history_data.id
     , history_data.target_text
     , history_timeline.accessed_time
FROM history_timeline
         JOIN history_data
              ON history_timeline.history_data_id = history_data.id

ORDER BY history_timeline.accessed_time DESC;