CREATE UNIQUE INDEX IF NOT EXISTS favorite_group_index
    ON history_favorite (group_id, added_time DESC);
