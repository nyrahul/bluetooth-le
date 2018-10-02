#include <signal.h>
#include "isync.h"
#include "isync_advertise.h"
#include "isync_scan.h"
#include "ble_transport.h"

void cleanup(int sig)
{
    ble_transport_cleanup();
    isync_adv_cleanup();
    isync_scan_cleanup();
    printf("bye.\n");
    exit(0);
}

int main(void)
{
    int ret;

    signal(SIGINT, cleanup);

    ret = ble_transport_init();
    ret_chk(ret != SUCCESS, "ble_transport_init failed");

    ret = isync_advertise();
    ret_chk(ret != SUCCESS, "isync_advertise failed");

    ret = isync_scan(); // this is a blocking call
    ret_chk(ret != SUCCESS, "isync_scan failed");

ret_fail:
    cleanup(0);
    return 1;
}
