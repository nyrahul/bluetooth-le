#define _ISYNC_COMMON_

#include "isync.h"
#include "isync_pal.h"
#include "isync_transport.h"
#include "isync_service.h"
#include "isync_device.h"
#include "isync_appexp.h"
#include "timer_util.h"
#include "epoll_util.h"

static int module_id = ISYNC_COMMON;

typedef struct _isync_config_
{
    device_t dev;
    isync_servnotify_cb serv_notify_cb;
} isync_config_t;

isync_config_t g_config;

#define DEV (&g_config.dev)
#define CFG (&g_config)

int isync_set_devid(const char *devid)
{
    strncpy(DEV->devid, devid, sizeof(DEV->devid) - 1);
    return SUCCESS;
}

int isync_get_devid(char *devid, const size_t len)
{
    return snprintf(devid, len, "%s", DEV->devid);
}

int isync_set_devact(const char *act)
{
    strncpy(DEV->hwact, act, sizeof(DEV->hwact) - 1);
    return SUCCESS;
}

int isync_get_devact(char *act, const size_t len)
{
    return snprintf(act, len, "%s", DEV->hwact);
}

int isync_set_devtype(const int type)
{
    DEV->devtype = type;
    return SUCCESS;
}

int isync_get_devtype(void) { return DEV->devtype; }

void isync_stop(void)
{
    epoll_deinit();
    isync_device_cleanup();
    isync_serviceall_cleanup();
    isync_transport_cleanup();
    isync_adv_cleanup();
    isync_scan_cleanup();
    isync_timer_cleanup();
}

int ble_scan_notify(const scan_info_t *sin)
{
    isync_device_handle_advertisement(sin);
    return SUCCESS;
}

int isync_init(void)
{
    int ret;

    ret = epoll_init();
    ret_chk(ret != SUCCESS, "epoll_init failed");

    ret = isync_timer_init();
    ret_chk(ret != SUCCESS, "timer_init failed");

    ret = isync_device_init();
    ret_chk(ret != SUCCESS, "device_init failed");

    ret = isync_service_init();
    ret_chk(ret != SUCCESS, "srvice_init failed");

    ret = isync_transport_init();
    ret_chk(ret != SUCCESS, "ble_transport_init failed");

    ret = isync_advertise(0xcafedead, 0xbabe);
    ret_chk(ret != SUCCESS, "isync_advertise failed");

    ret = isync_scan(ble_scan_notify);
    ret_chk(ret != SUCCESS, "isync_scan failed");

    return SUCCESS;

ret_fail:
    isync_stop();
    return ret;
}
