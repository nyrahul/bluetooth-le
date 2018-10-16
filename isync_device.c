#include "isync.h"
#include <pthread.h>
#include "isync_pal.h"
#include "isync_device.h"
#include "isync_db.h"
#include "epoll_util.h"

#define MAX_DEVICES 20

typedef struct _device_info_
{
    char addr[MAX_ADDRESS_LEN];
    char name[MAX_NAME_LEN];
    // int devtype;
    struct timeval lastseen;
} dev_info_t;

void *g_dev_db;
pthread_mutex_t g_dev_lock = PTHREAD_MUTEX_INITIALIZER;
#define DEV_GONE_TIME_MS 10000

dev_info_t *get_devinfo_from_addr(const char *addr)
{
    dev_info_t dev;
    memset(&dev, 0, sizeof(dev));
    strncpy(dev.addr, addr, sizeof(dev.addr));
    return db_search(g_dev_db, &dev);
}

int device_cmp(void *recptr, void *myptr)
{
    dev_info_t *rec = recptr;
    dev_info_t *my  = myptr;

    if (!strcmp(rec->addr, my->addr))
        return 1;
    return 0;
}

void device_remove(dev_info_t *dev)
{
    INFO("Removing device..");
    db_free(g_dev_db, (void *)dev);
}

void device_periodic_timer(void)
{
    int idx = -1, ms;
    dev_info_t *dev;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    pthread_mutex_lock(&g_dev_lock);
    while ((dev = db_get_next(g_dev_db, &idx)))
    {
        ms = get_timediff(&tv, &dev->lastseen);
        if (ms > DEV_GONE_TIME_MS)
        {
            INFO("DEVICE GONE OFFLINE name:[%s], addr:[%s]", dev->name,
                dev->addr);
            device_remove(dev);
        }
    }
    pthread_mutex_unlock(&g_dev_lock);
    return;
}

int isync_device_init(void)
{
    ret_chk(g_dev_db, "dev list db already inited");

    g_dev_db = db_init(MAX_DEVICES, sizeof(dev_info_t), device_cmp);
    ret_chk(!g_dev_db, "dev db init failed");

    // g_dev_periodic_timer = timer_period_set(device_periodic_timer, );

    return SUCCESS;
ret_fail:
    return FAILURE;
}

void isync_device_cleanup(void)
{
    db_cleanup(g_dev_db);
    g_dev_db = NULL;
    return;
}

dev_info_t *add_new_device(const scan_info_t *sin)
{
    dev_info_t *dev = NULL;

    dev = db_alloc(g_dev_db);
    ret_chk(!dev, "Could not alloc new dev");

    strncpy(dev->addr, sin->addr, sizeof(dev->addr));
    strncpy(dev->name, sin->name, sizeof(dev->name));

    return dev;
ret_fail:
    return NULL;
}

int isync_device_handle_advertisement(const scan_info_t *sin)
{
    dev_info_t *dev;

    dev = get_devinfo_from_addr(sin->addr);
    if (dev)
    {
        /* Existing device */
        INFO("existing device advertisement rcvd");
    }
    else
    {
        /* New Device */
        INFO("NEW device advertisement rcvd");
        add_new_device(sin);
    }
    ret_chk(!dev, "could not get dev");

    gettimeofday(&dev->lastseen, NULL);

    return SUCCESS;
ret_fail:
    return FAILURE;
}
