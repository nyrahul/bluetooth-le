#pragma once

/*
    Init/Configuration Interfaces
    -----------------------------
*/

int isync_init(void);
void isync_stop(void);

/*
    Device Discovery Interfaces
    ---------------------------
*/
#define ISYNC_MAX_DEVID_LEN 32
#define ISYNC_HWACT_LEN 128
typedef struct _device_
{
    char devid[ISYNC_MAX_DEVID_LEN];
    char hwact[ISYNC_HWACT_LEN];
    int devtype;
} device_t;

int isync_set_devid(const char *devid);
int isync_get_devid(char *devid, const size_t len);

int isync_set_devact(const char *act);
int isync_get_devact(char *act, const size_t len);

int isync_set_devtype(const int type);
int isync_get_devtype(void);

#define ID_SYNC 0
#define ID_CLIPBOARD 1

typedef struct _service_
{
    int id;
    device_t dev;
} service_t;

typedef int (*isync_servnotify_cb)(
    const int id, device_t *dev, const uint8_t *buf, const size_t len);

int isync_service_observer(isync_servnotify_cb cb);

int isync_start_service(const int id);
int isync_stop_service(const int id);

int isync_publish(const int id, const uint8_t *buf, const size_t len);
int isync_subscribe(const int id, device_t *dev);

int isync_send(const device_t *dev, const uint8_t *buf, const size_t len);
