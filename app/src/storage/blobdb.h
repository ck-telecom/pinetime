#ifndef _BLOBDB_H
#define _BLOBDB_H

#include <zephyr.h>

#include <fs/fs.h>

enum {
    Blob_Insert = 0x01,
    Blob_Delete = 0x04,
    Blob_Clear  = 0x05,
};

typedef enum rdb_operator {
    RDB_OP_NONE = 0,
    RDB_OP_GREATER,
    RDB_OP_LESS,
    RDB_OP_EQ,
    RDB_OP_NEQ,
    RDB_OP_RESULT = 0x80,
    RDB_OP_RESULT_FULLY_LOAD
} rdb_operator_t;

enum {
    Blob_Success = 0x01,
    Blob_GeneralFailure = 0x02,
    Blob_InvalidOperation = 0x03,
    Blob_InvalidDatabaseID = 0x04,
    Blob_InvalidData = 0x05,
    Blob_KeyDoesNotExist = 0x06,
    Blob_DatabaseFull = 0x07,
    Blob_DataStale = 0x08,
    Blob_NotSupported = 0x09,
    Blob_Locked = 0xA,
    Blob_TryLater = 0xB,
};

enum {
    RDB_ID_TEST = 0,
    RDB_ID_PIN = 1,
    RDB_ID_APP = 2,
    RDB_ID_REMINDER = 3,
    RDB_ID_NOTIFICATION = 4,
    RDB_ID_APP_GLANCE = 11,
    RDB_ID_APP_PERSIST = 16,
    RDB_ID_BLUETOOTH = 128,
    RDB_ID_HEALTH = 129,
};

struct rdb_database;

struct rdb_iter {
    struct fs_file_t fd;
    uint8_t key_len;
    uint16_t data_len;
};

#define RDB_SELECTOR_OFFSET_KEY 0xFFFF

struct rdb_selector {
    uint16_t offsetof;
    uint16_t size;
    rdb_operator_t operator;
    void *val;
};

struct rdb_select_result {
    sys_dnode_t node;
    struct rdb_iter it; /* pointer to data */
    void *key;
    int nres;
    void *result[];
};

typedef sys_dlist_t rdb_select_result_list;

struct rdb_database *rdb_open(uint16_t database_id);
void rdb_close(struct rdb_database *db);
int rdb_insert(const struct rdb_database *db, const uint8_t *key, uint16_t key_size, const uint8_t *data, uint16_t data_size);
int rdb_delete(struct rdb_iter *it);
int rdb_update(const struct rdb_database *db, const uint8_t *key, const uint16_t key_size, const uint8_t *data, const size_t data_size);
int rdb_create(const struct rdb_database *db);

/* Each returns true if the iterator points to a valid header. */
int rdb_iter_start(const struct rdb_database *db, struct rdb_iter *it);
int rdb_iter_next(struct rdb_iter *it);

int rdb_iter_read_key(struct rdb_iter *it, void *key);
int rdb_iter_read_data(struct rdb_iter *it, int ofs, void *data, int len);

int rdb_select(struct rdb_iter *it, rdb_select_result_list *head, struct rdb_selector *selectors);
void rdb_select_free_result(struct rdb_select_result *res);
void rdb_select_free_all(rdb_select_result_list *head);

#define rdb_select_result_foreach(res, lh) SYS_DLIST_FOR_EACH_CONTAINER(lh, res, node)
#define rdb_select_result_head(lh) SYS_DLIST_CONTAINER(sys_dlist_peek_head(lh), struct rdb_select_result, node)
#define rdb_select_result_next(res, lh) SYS_DLIST_CONTAINER(sys_dlist_peek_next(lh, &(res)->node), res, node)
#define rdb_select_result_prev(res, lh) SYS_DLIST_CONTAINER(sys_dlist_peek_prev(lh, &(res)->node), res, node)

#endif /* _BLOBDB_H */