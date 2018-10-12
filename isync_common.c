#include "isync.h"
#include "isync_pal.h"
#include "isync_transport.h"
#include "isync_service.h"
#include "isync_appexp.h"

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
    isync_serviceall_cleanup();
    ble_transport_cleanup();
    isync_adv_cleanup();
    isync_scan_cleanup();
}

int ble_scan_notify(const char *addrstr, const size_t len)
{
    void *ssn;
    ssn = ble_transport_start_cli(addrstr);
    INFO("ble_transport_start_cli ssn=%p addr=%s", ssn, addrstr);
    return SUCCESS;
}

int isync_init(void)
{
    int ret;

    ret = isync_service_init();
    ret_chk(ret != SUCCESS, "srvice_init failed");

    ret = ble_transport_init();
    ret_chk(ret != SUCCESS, "ble_transport_init failed");

    ret = isync_advertise(0xcafe, 0xbabe);
    ret_chk(ret != SUCCESS, "isync_advertise failed");

    ret = isync_scan(ble_scan_notify); // this is a blocking call
    ret_chk(ret != SUCCESS, "isync_scan failed");

    return SUCCESS;

ret_fail:
    isync_stop();
    return ret;
}
