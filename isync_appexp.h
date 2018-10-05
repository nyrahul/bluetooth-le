#pragma once

/*
    Init/Configuration Interfaces
    -----------------------------
*/

typedef enum {
    DEV_DISC = 1,
    SRV_DISC,
    CHN_DISC,
    DATA_TRANSFER,
    FILE_TRANSFER,
}type_e;

typedef enum {
    OP_SUCCESS,
    OP_FAILURE,
    ADD,
    REM,
}subtype_e;

typedef int (*hisync_notify_cb)(type_e type, subtype_e sub_type, void *buf, size_t buflen);

int hisync_init(uint32_t rid, int devtype, hisync_notify_cb notifycb);


/*
    Device Discovery Interfaces
    ---------------------------
*/
typedef struct _dev_info_
{
    int devtype; // type of device ... phone/laptop/...
    int devid;   // device id ... single user might have multiple devices of same type
}dev_info_t;

// Add Device: notifycb(DEV_DISC, ADD, dev_info_t *, sizeof(dev_info_t))
// Rem Device: notifycb(DEV_DISC, REM, dev_info_t *, sizeof(dev_info_t))


/*
    Service Discovery Interfaces
    ----------------------------
*/
typedef struct _service_info_
{
    int service_type;
    int broadcast_service; // Is the service data available to all devs of the user?
    //specify channel characteristics here
    //does chn need reliability?
    //is it high/low thruput, hi/lo latency?
}service_info_t;

int hisync_service_add(const service_info_t *service);
// Return service_handle

int hisync_service_update(const int service_handle, const service_info_t *service);

/*
    Channel Interfaces and Data transfer
    ------------------------------------
*/
int hisync_channel_start(int service_handle);
//Return channel_handle

int hisync_channel_send(int channel_handle, uint8_t *buf, size_t buflen, int flags);
// Data is always received as full packets on receiver

int hisync_channel_sendfile(int channel_handle, const char *filepath, int flags);
// Does async send
// On timeout/send-success/failure .. reports event asynchronously

int hisync_channel_close(int handle);

