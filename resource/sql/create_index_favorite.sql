CREATE INDEX IF NOT EXISTS history_favorite_index
    ON history_favorite (group_id, added_time DESC);
