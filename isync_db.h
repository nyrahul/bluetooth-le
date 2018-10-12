#pragma once

typedef int (*rec_cmp_cb)(void *, void *);

void db_cleanup(void *ptr);
void *db_init(int recnum, size_t recsz, rec_cmp_cb cb);
void *db_search(void *dbptr, void *ptr);
void *db_alloc(void *dbptr);
void db_free(void *dbptr, void *ptr);

