#include "isync.h"
#include "isync_pal.h"
#include "isync_device.h"

typedef struct _device_info_
{
    char addr[MAX_ADDRESS_LEN];
    char name[MAX_NAME_LEN];
    int devtype;
} peerdev_info_t;

int isync_device_handle_advertisement(const scan_info_t *sin)
{
    return SUCCESS;
}
