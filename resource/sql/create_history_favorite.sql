CREATE TABLE IF NOT EXISTS history_favorite
(
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    added_time      INTEGER,
    history_data_id INTEGER NOT NULL,
    group_id        INTEGER NOT NULL,
    UNIQUE (history_data_id, group_id),

    FOREIGN KEY (history_data_id)
        REFERENCES history_data (id)
        ON UPDATE CASCADE
        ON DELETE CASCADE,

    FOREIGN KEY (group_id)
        REFERENCES history_favorite_group (id)
        ON UPDATE CASCADE
        ON DELETE CASCADE
);
