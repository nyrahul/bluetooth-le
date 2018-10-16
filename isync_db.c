#include "isync.h"
#include "isync_db.h"

typedef struct _db_info_
{
    uint8_t *blk;
    uint32_t *usedmap;
    int mapsz;
    int usecnt;
    int maxcnt;
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
    ret_chk(!db->blk, "malloc %d %zu failed", recnum, recsz);

    db->maxcnt = recnum;
    db->usecnt = 0;
    db->recsz  = recsz;
    db->cb     = cb;

    return db;
ret_fail:
    db_cleanup(db);
    return NULL;
}

static inline int get_rec_idx(db_info_t *db, void *rec)
{
    return ((uint8_t *)rec - db->blk) / sizeof(db->recsz);
}

static inline int is_rec_inuse(db_info_t *db, void *rec, int *iptr, int *offptr)
{
    int recnum, i, off;

    recnum = get_rec_idx(db, rec);
    ret_chk(recnum < 0, "invalid rec ptr");

    i   = recnum / sizeof(uint32_t);
    off = recnum % sizeof(uint32_t);
    if (iptr)
        *iptr = i;
    if (offptr)
        *offptr = i;
    if (db->usedmap[i] & (1 << off))
    {
        return 1;
    }
    return 0;
ret_fail:
    return FAILURE;
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

    if (db->usecnt >= db->maxcnt)
    {
        ERROR("DB max limit exceeded maxcnt:%d", db->maxcnt);
        return NULL;
    }

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
                db->usecnt++;
                return rec;
            }
        }
    }
    return NULL;
}

void *db_get_next(void *dbptr, int *state)
{
    db_info_t *db = dbptr;
    void *rec;
    int idx = 0, ret;

    if (*state >= 0)
    {
        (*state)++;
    }
    else
    {
        *state = 0;
    }

    for (idx = *state; idx < db->maxcnt; idx++)
    {
        rec = get_rec(db, idx);
        ret = is_rec_inuse(db, rec, NULL, NULL);
        if (ret < 0)
        {
            ERROR("problem with getting rec");
            return NULL;
        }
        if (ret)
        {
            *state = idx;
            return rec;
        }
    }
    return NULL;
}

void db_free(void *dbptr, void *ptr)
{
    db_info_t *db = dbptr;
    int i, off, ret;

    ret_chk(!dbptr || !ptr, "Sanity chk failed");

    ret = is_rec_inuse(db, ptr, &i, &off);
    ret_chk(ret <= 0, "STRANGE THE REC SHOWS UNUSED")

        db->usedmap[i] |= ~(1 << off);
    db->usecnt--;
    if (db->usecnt < 0)
    {
        ERROR("SOMETHING IS TERRIBLY WRONG");
        db->usecnt = 0;
    }
ret_fail:
    return;
}
