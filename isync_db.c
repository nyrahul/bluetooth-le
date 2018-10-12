#include "isync.h"
#include "isync_db.h"

typedef struct _db_info_
{
    uint8_t *blk;
    uint32_t *usedmap;
    int mapsz;
    size_t recsz;
    rec_cmp_cb cb;
} db_info_t;

void db_cleanup(void *ptr)
{
    db_info_t *db = ptr;
    if (!db)
        return;
    FREE(db->blk);
    FREE(db->usedmap);
    FREE(db);
}

void *db_init(int recnum, size_t recsz, rec_cmp_cb cb)
{
    db_info_t *db = NULL;

    db = (db_info_t *)calloc(1, sizeof(db_info_t));
    ret_chk(!db, "calloc dbinfo failed");

    db->mapsz   = recnum / sizeof(uint32_t) + 1;
    db->usedmap = calloc(db->mapsz, sizeof(uint32_t));
    ret_chk(!db->usedmap, "calloc usedmap failed");

    db->blk = (uint8_t *)malloc(recnum * recsz);
    ret_chk(!db->blk, "malloc %d %zu failed", recnum, recsz)

        db->recsz = recsz;

    return db;
ret_fail:
    db_cleanup(db);
    return NULL;
}

static inline void *get_rec(db_info_t *db, int idx)
{
    return db->blk + (idx * db->recsz);
}

void *db_search(void *dbptr, void *ptr)
{
    db_info_t *db = dbptr;
    void *rec;
    int i, j;

    for (i = 0; i < db->mapsz; i++)
    {
        if (!db->usedmap[i])
            continue;
        for (j = 0; j < sizeof(uint32_t); j++)
        {
            if (db->usedmap[i] & (1 << j))
            {
                rec = get_rec(db, i * sizeof(uint32_t) + j);
                if (db->cb(rec, ptr))
                {
                    return rec;
                }
            }
        }
    }
    return NULL;
}

void *db_alloc(void *dbptr)
{
    db_info_t *db = dbptr;
    void *rec;
    int i, j;

    for (i = 0; i < db->mapsz; i++)
    {
        if (db->usedmap[i] == (~(uint32_t)0))
            continue;
        for (j = 0; j < sizeof(uint32_t); j++)
        {
            if (!(db->usedmap[i] & (1 << j)))
            {
                rec = get_rec(db, i * sizeof(uint32_t) + j);
                db->usedmap[i] |= (1 << j);
                return rec;
            }
        }
    }
    return NULL;
}

void db_free(void *dbptr, void *ptr)
{
    db_info_t *db = dbptr;
    int recnum, i, off;

    recnum = ((uint8_t *)ptr) - db->blk;
    i      = recnum / sizeof(uint32_t);
    off    = recnum % sizeof(uint32_t);
    if (!(db->usedmap[i] & (1 << off)))
    {
        ERROR("STRANGE THE RECORD SHOWS UNUSED");
        return;
    }
    db->usedmap[i] |= ~(1 << off);
}
