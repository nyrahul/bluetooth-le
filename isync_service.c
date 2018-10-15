#include "isync.h"
#include "isync_appexp.h"
#include "isync_service.h"
#include "isync_db.h"

typedef struct _sevice_info_
{
    int id;
} service_info_t;

isync_servnotify_cb g_srvnotify_cb;
void *g_service_db;

#define MAX_SERVICES 5

int service_cmp(void *recptr, void *myptr)
{
    service_info_t *rec = recptr;
    service_info_t *my  = myptr;

    if (rec->id == my->id)
    {
        return 1;
    }
    return 0;
}

int isync_service_init(void)
{
    ret_chk(g_service_db, "service_init already done");

    g_service_db = db_init(MAX_SERVICES, sizeof(service_info_t), service_cmp);
    ret_chk(!g_service_db, "db_init service failed");

    return SUCCESS;
ret_fail:
    return FAILURE;
}

void isync_serviceall_cleanup(void)
{
    db_cleanup(g_service_db);
    g_service_db = NULL;
    return;
}

int isync_service_observer(isync_servnotify_cb cb)
{
    g_srvnotify_cb = cb;
    return SUCCESS;
}

service_info_t *get_service_from_id(int id)
{
    service_info_t service;
    memset(&service, 0, sizeof(service));
    service.id = id;
    return db_search(g_service_db, &service);
}

int isync_start_service(const int id)
{
    service_info_t *si = NULL;

    si = get_service_from_id(id);
    ret_chk(si, "service with id %d is already present", id);

    si = db_alloc(g_service_db);
    ret_chk(!si, "service could not be alloced");

    memset(si, 0, sizeof(service_info_t));
    si->id = id;

    INFO("Started service id=%d\n", id);
    return SUCCESS;
ret_fail:
    return FAILURE;
}

int isync_stop_service(const int id)
{
    service_info_t *si = NULL;

    si = get_service_from_id(id);
    ret_chk(!si, "service with id %d NOT present", id);

    db_free(g_service_db, si);

    return SUCCESS;
ret_fail:
    return FAILURE;
}

int isync_publish(const int id, const uint8_t *buf, const size_t len)
{
    /* TODO */
    return SUCCESS;
}

int isync_subscribe(const int id, device_t *dev)
{
    /* TODO */
    return SUCCESS;
}

int isync_send(const service_t *service, const uint8_t *buf, const size_t len)
{
    /* TODO */
    return SUCCESS;
}
