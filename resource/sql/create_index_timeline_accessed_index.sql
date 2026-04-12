CREATE INDEX IF NOT EXISTS timeline_accessed_index
    ON history_timeline (accessed_time DESC);
