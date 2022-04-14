#include <stdbool.h>

#include "health_storage_persist.h"

#include "blobdb.h"

status_t health_persist_write_step(const uint32_t key, HealthValue val)
{
	HealthValue data = val;
	return health_persist_write(key, &data, sizeof(HealthValue));
}

status_t health_persist_read_step(const uint32_t key, HealthValue *val)
{
	HealthValue data;

	status_t r = health_persist_read(key, &data, sizeof(HealthValue));
	if (r != sizeof(HealthValue)) {
		return E_INVALID_ARGUMENT;
	}
	*val = data;
	return 0;
}

status_t health_persist_write(const uint32_t key, const void *data, size_t size)
{
    rdb_select_result_list head;
    sys_dlist_init(&head);
    const uint32_t c_key = key;

    if (size > PERSIST_DATA_MAX_LENGTH)
        return E_INVALID_ARGUMENT;

    bool exists = health_persist_exists(key);

    struct rdb_database *db = rdb_open(RDB_ID_HEALTH);
    assert(db);

    if (!exists) {
        if (rdb_insert(db, (uint8_t *)&c_key, sizeof(c_key), data, size) != Blob_Success) {
            rdb_close(db);
            return E_ERROR;
        }
    } else {
        if (rdb_update(db, &c_key, sizeof(c_key), data, size) != Blob_Success) {
            rdb_close(db);
            return E_ERROR;
        }
    }

    rdb_close(db);
    return size;
}

status_t health_persist_read(const uint32_t key, const void *buffer, const size_t size)
{
    struct rdb_iter it;
    rdb_select_result_list head;
    sys_dlist_init(&head);
    const uint32_t c_key = key;

    struct rdb_database *db = rdb_open(RDB_ID_HEALTH);
    assert(db);

    int rv = rdb_iter_start(db, &it);
    if (!rv) {
        rdb_close(db);
        return E_ERROR;
    }

    struct rdb_selector selectors[] = {
        { RDB_SELECTOR_OFFSET_KEY, sizeof(c_key) , RDB_OP_EQ, &c_key },
        { 0, 0, RDB_OP_RESULT_FULLY_LOAD },
        { }
    };
    int n = rdb_select(&it, &head, selectors);

    if (!n) {
        rv =  E_DOES_NOT_EXIST;
        goto done;
    }

    struct rdb_select_result *res = rdb_select_result_head(&head);
    size_t sz = size <= res->it.data_len ? size : res->it.data_len;
    memcpy(buffer, res->result[0], sz);
    rv = sz;

done:
    rdb_close(db);
    rdb_select_free_all(&head);
    return rv;
}

bool health_persist_exists(const uint32_t key)
{
    char buffer[1];
    return health_persist_read(key, buffer, 1) == 1;
}
