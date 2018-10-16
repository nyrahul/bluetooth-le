#include <errno.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "isync.h"
#include "isync_pal.h"

#define EIR_FLAGS 0X01
#define EIR_NAME_SHORT 0x08
#define EIR_NAME_COMPLETE 0x09
#define EIR_MANUFACTURE_SPECIFIC 0xFF

static int module_id = ISYNC_BLE;

int device_handle;

unsigned int *uuid_str_to_data(char *uuid)
{
    char conv[]        = "0123456789ABCDEF";
    int len            = strlen(uuid);
    unsigned int *data = (unsigned int *)malloc(sizeof(unsigned int) * len);
    unsigned int *dp   = data;
    char *cu           = uuid;

    for (; cu < uuid + len; dp++, cu += 2)
    {
        *dp = ((strchr(conv, toupper(*cu)) - conv) * 16) +
              (strchr(conv, toupper(*(cu + 1))) - conv);
    }

    return data;
}

unsigned int twoc(int in, int t)
{
    return (in < 0) ? (in + (2 << (t - 1))) : in;
}

int advertise_enable(int enable)
{
    struct hci_request rq;
    uint8_t status;
    le_set_advertise_enable_cp advertise_cp;

    memset(&advertise_cp, 0, sizeof(advertise_cp));
    advertise_cp.enable = enable;

    memset(&rq, 0, sizeof(rq));
    rq.ogf    = OGF_LE_CTL;
    rq.ocf    = OCF_LE_SET_ADVERTISE_ENABLE;
    rq.cparam = &advertise_cp;
    rq.clen   = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
    rq.rparam = &status;
    rq.rlen   = 1;

    return hci_send_req(device_handle, &rq, 1000);
}

int advertise_params_cp(int interval)
{
    struct hci_request rq;
    uint8_t status;
    le_set_advertising_parameters_cp adv_params_cp;

    memset(&adv_params_cp, 0, sizeof(adv_params_cp));
    adv_params_cp.min_interval = htobs(interval);
    adv_params_cp.max_interval = htobs(interval);
    // if (opt)
    //  adv_params_cp.advtype = atoi(opt);
    adv_params_cp.chan_map = 7; // RJ: Advertise on all 3 channels, 37,38,39

    memset(&rq, 0, sizeof(rq));
    rq.ogf    = OGF_LE_CTL;
    rq.ocf    = OCF_LE_SET_ADVERTISING_PARAMETERS;
    rq.cparam = &adv_params_cp;
    rq.clen   = LE_SET_ADVERTISING_PARAMETERS_CP_SIZE;
    rq.rparam = &status;
    rq.rlen   = 1;

    return hci_send_req(device_handle, &rq, 1000);
}

int get_device_handle(void)
{
    int device_id = hci_get_route(NULL);

    if ((device_handle = hci_open_dev(device_id)) < 0)
    {
        perror("Could not open device");
        exit(1);
    }
    return device_handle;
}

void isync_adv_cleanup(void)
{
    advertise_enable(0);
    hci_close_dev(device_handle);
}

#define UUID_STR "EAC5BDB5DFCB48D2BE60D0A5A71G96E0"
int advertise_data(void)
{
    struct hci_request rq;
    uint8_t status;
    uint8_t off = 1;
    le_set_advertising_data_cp dcp;

    memset(&dcp, 0, sizeof(dcp));

    dcp.data[dcp.length + off] = htobs(EIR_FLAGS);
    off++;
    dcp.data[dcp.length + off] = htobs(0x1A);
    off++;
    dcp.data[dcp.length] = htobs(off - 1);

    dcp.length += off;

    off = 1;
    // SET EIR_MANUFACTURE_SPECIFIC
    dcp.data[dcp.length + off] = htobs(EIR_MANUFACTURE_SPECIFIC);
    off++;
    // SET Manufacturer - Huawei=0x027D, Apple=0x004C
    dcp.data[dcp.length + off] = htobs(COMPANY_ID_HUAWEI & 0x00FF);
    off++;
    dcp.data[dcp.length + off] = htobs(COMPANY_ID_HUAWEI >> 8);
    off++;

    // Set Type 02=Beacon
    dcp.data[dcp.length + off] = htobs(0x02);
    off++;

    // Set Length 0x15=21 (16[UUID] + 2[MAJOR] + 2[MINOR] + 1[RSSI])
    dcp.data[dcp.length + off] = htobs(0x15);
    off++;

    // Set 16B UUID
    unsigned int *uuid = uuid_str_to_data(UUID_STR);
    int i;
    for (i = 0; i < strlen(UUID_STR) / 2; i++)
    {
        dcp.data[dcp.length + off] = htobs(uuid[i]);
        off++;
    }

    // Set 2B Major number
    int major_number           = atoi("5");
    dcp.data[dcp.length + off] = htobs(major_number >> 8 & 0x00FF);
    off++;
    dcp.data[dcp.length + off] = htobs(major_number & 0x00FF);
    off++;

    // Set 2B Minor number
    int minor_number           = atoi("1");
    dcp.data[dcp.length + off] = htobs(minor_number >> 8 & 0x00FF);
    off++;
    dcp.data[dcp.length + off] = htobs(minor_number & 0x00FF);
    off++;

    // Set 1B RSSI calibration
    dcp.data[dcp.length + off] = htobs(twoc(atoi("-24"), 8));
    off++;

    dcp.data[dcp.length] = htobs(off - 1);

    dcp.length += off;

    memset(&rq, 0, sizeof(rq));
    rq.ogf    = OGF_LE_CTL;
    rq.ocf    = OCF_LE_SET_ADVERTISING_DATA;
    rq.cparam = &dcp;
    rq.clen   = LE_SET_ADVERTISING_DATA_CP_SIZE;
    rq.rparam = &status;
    rq.rlen   = 1;

    return hci_send_req(device_handle, &rq, 1000);
}

int isync_set_dev(uint8_t *data, size_t data_len)
{
    hdr_dev_t *dev = (hdr_dev_t *)data;
    if (sizeof(hdr_dev_t) > data_len)
    {
        ERROR("Not enough space in data (%zu)\n", data_len);
        return FAILURE;
    }
    memset(dev, 0, sizeof(hdr_dev_t));
    dev->type       = htobs(TYPE_ISYNC);
    dev->version    = htobs(ISYNC_VERSION);
    dev->isync_type = htobs(ISYNC_ADV_TYPE_DEV_AUTH);
    return sizeof(hdr_dev_t);
}

int isync_set_devauth(const uint32_t rid, uint8_t *data, size_t data_len)
{
    hdr_dev_auth_t *devauth = (hdr_dev_auth_t *)data;
    if (sizeof(hdr_dev_auth_t) > data_len)
    {
        ERROR("Not enough space in data (%zu)\n", data_len);
        return FAILURE;
    }
    memset(devauth, 0, sizeof(hdr_dev_auth_t));
    devauth->rid = htobs(0xCAFEBABE);
    return sizeof(hdr_dev_auth_t);
}

#define DOFF (dcp.length + off)
int advertise_isync_data(const uint32_t rid)
{
    int ret;
    uint8_t status;
    struct hci_request rq;
    uint8_t off = 1;
    le_set_advertising_data_cp dcp;

    memset(&dcp, 0, sizeof(dcp));

    dcp.data[DOFF] = htobs(EIR_FLAGS);
    off++;
    dcp.data[DOFF] = htobs(0x1A);
    off++;
    dcp.data[dcp.length] = htobs(off - 1);

    dcp.length += off;

    off = 1;
    // SET EIR_MANUFACTURE_SPECIFIC
    dcp.data[DOFF] = htobs(EIR_MANUFACTURE_SPECIFIC);
    off++;
    // SET Manufacturer - Huawei=0x027D, Apple=0x004C
    dcp.data[DOFF] = htobs(COMPANY_ID_HUAWEI & 0x00ff);
    off++;
    dcp.data[DOFF] = htobs(COMPANY_ID_HUAWEI >> 8);
    off++;

    // Set Type 02=Beacon
    ret = isync_set_dev(&dcp.data[DOFF], sizeof(dcp.data) - DOFF);
    if (ret <= 0)
    {
        ERROR("isync set dev failed\n");
        return ret;
    }
    off += ret;

    ret = isync_set_devauth(rid, &dcp.data[DOFF], sizeof(dcp.data) - DOFF);
    if (ret <= 0)
    {
        ERROR("isync set dev failed\n");
        return ret;
    }
    off += ret;

    dcp.data[dcp.length] = htobs(off - 1);

    dcp.length += off;

    memset(&rq, 0, sizeof(rq));
    rq.ogf    = OGF_LE_CTL;
    rq.ocf    = OCF_LE_SET_ADVERTISING_DATA;
    rq.cparam = &dcp;
    rq.clen   = LE_SET_ADVERTISING_DATA_CP_SIZE;
    rq.rparam = &status;
    rq.rlen   = 1;

    return hci_send_req(device_handle, &rq, 1000);
}

int isync_advertise(const uint32_t rid, const uint16_t devtype)
{
    int ret;

    get_device_handle();

    ret = advertise_params_cp(atoi("200"));
    ret_chk(ret < 0, "advertise_params_cp failed");

    ret = advertise_isync_data(rid);
    ret_chk(ret < 0, "advertise_isync_data failed");

    ret = advertise_enable(1);
    ret_chk(ret < 0, "advertise_enable failed");

    return SUCCESS;
ret_fail:
    ERROR("Can't set advertise mode on hci: %s (%d)\n", strerror(errno), errno);
    isync_adv_cleanup();
    return FAILURE;
}
