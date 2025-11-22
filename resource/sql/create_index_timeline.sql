CREATE UNIQUE INDEX IF NOT EXISTS timeline_index
    ON history_timeline (accessed_time DESC);
