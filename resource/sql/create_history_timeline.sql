CREATE TABLE IF NOT EXISTS history_timeline
(
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    accessed_time   INTEGER,
    history_data_id INTEGER NOT NULL,

    FOREIGN KEY (history_data_id)
        REFERENCES history_data (id)
        ON UPDATE CASCADE
        ON DELETE CASCADE
);
