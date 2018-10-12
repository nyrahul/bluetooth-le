#include "isync.h"
#include "isync_appexp.h"
#include "isync_service.h"

isync_servnotify_cb g_srvnotify_cb;

int isync_service_init(void)
{
    /* TODO */
    return SUCCESS;
}

void isync_serviceall_cleanup(void)
{
    /* TODO */
    return;
}

int isync_service_observer(isync_servnotify_cb cb)
{
    g_srvnotify_cb = cb;
    return SUCCESS;
}

int isync_start_service(const int id)
{
    /* TODO */
    return SUCCESS;
}

int isync_stop_service(const int id)
{
    /* TODO */
    return SUCCESS;
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

int isync_send(const device_t *dev, const uint8_t *buf, const size_t len)
{
    /* TODO */
    return SUCCESS;
}
