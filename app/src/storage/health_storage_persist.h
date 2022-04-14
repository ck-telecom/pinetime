#ifndef _HEALTH_STORAGE_PERSIST_H
#define _HEALTH_STORAGE_PERSIST_H

status_t health_persist_write_step(const uint32_t key, HealthValue val);
status_t health_persist_read_step(const uint32_t key, HealthValue *val)
status_t health_persist_write(const uint32_t key, const void *data, size_t size);
status_t health_persist_read(const uint32_t key, const void *data, const size_t size);
bool health_persist_exists(const uint32_t key);

#endif /* _HEALTH_STORAGE_PERSIST_H */