#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "isync.h"
#include "isync_pal.h"

int isync_set_service_info(uint8_t *arr, size_t len)
{
    /* TODO */
    return SUCCESS;
}
