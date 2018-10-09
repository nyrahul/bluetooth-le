#include "isync.h"
#include "isync_advertise.h"
#include "isync_scan.h"
#include "ble_transport.h"
#include "isync_appexp.h"

hisync_notify_cb g_notifycb;

void hisync_stop(void)
{
    ble_transport_cleanup();
    isync_adv_cleanup();
    isync_scan_cleanup();
}

int ble_scan_notify(const char *addrstr)
{
    void *ssn;
    ssn = ble_transport_start_cli(addrstr);
    INFO("ble_transport_start_cli ssn=%p addr=%s", ssn, addrstr);

    return SUCCESS;
}

int hisync_init(const node_cfg_t *cfg, hisync_notify_cb notifycb)
{
    int ret;

    g_notifycb = notifycb;

    ret = ble_transport_init();
    ret_chk(ret != SUCCESS, "ble_transport_init failed");

    ret = isync_advertise(cfg->rid, cfg->devtype);
    ret_chk(ret != SUCCESS, "isync_advertise failed");

    ret = isync_scan(ble_scan_notify); // this is a blocking call
    ret_chk(ret != SUCCESS, "isync_scan failed");

    return SUCCESS;

ret_fail:
    hisync_stop();
    return ret;
}
