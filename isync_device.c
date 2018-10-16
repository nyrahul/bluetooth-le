#include "isync.h"
#include <pthread.h>
#include "isync_pal.h"
#include "isync_device.h"
#include "isync_db.h"
#include "timer_util.h"
#include "epoll_util.h"

static int module_id = ISYNC_DEVICE;

#define MAX_DEVICES 20

typedef struct _device_info_
{
    char addr[MAX_ADDRESS_LEN];
    char name[MAX_NAME_LEN];
    // int devtype;
    struct timeval lastseen;
} dev_info_t;

void *g_dev_db;
void *g_dev_timer;
pthread_mutex_t g_dev_lock = PTHREAD_MUTEX_INITIALIZER;

#define DEV_PERIODIC_TIMER 2000
#define DEV_GONE_TIME_MS 5000

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
    if (!dev)
        return;
    INFO("Removing device addr:[%s], name:[%s]", dev->addr, dev->name);
    db_free(g_dev_db, (void *)dev);
}

void dev_periodic_cb(void *usp)
{
    int idx = -1, ms;
    dev_info_t *dev;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    DBG("Device periodic cb");

    pthread_mutex_lock(&g_dev_lock);
    while ((dev = db_get_next(g_dev_db, &idx)))
    {
        DBG("GOT DEV name:%s addr:%s idx=%d", dev->name, dev->addr, idx);
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

void isync_device_cleanup(void)
{
    if (g_dev_db)
        db_cleanup(g_dev_db);
    if (g_dev_timer)
        isync_timer_delete(g_dev_timer);
    g_dev_db    = NULL;
    g_dev_timer = NULL;
    return;
}

dev_info_t *add_new_device(const scan_info_t *sin)
{
    dev_info_t *dev = NULL;

    pthread_mutex_lock(&g_dev_lock);
    dev = db_alloc(g_dev_db);
    ret_chk(!dev, "Could not alloc new dev");

    strncpy(dev->addr, sin->addr, sizeof(dev->addr));
    strncpy(dev->name, sin->name, sizeof(dev->name));

    INFO("Added New device name:[%s], addr:[%s]", dev->name, dev->addr);

ret_fail:
    pthread_mutex_unlock(&g_dev_lock);
    return dev;
}

int isync_device_handle_advertisement(const scan_info_t *sin)
{
    dev_info_t *dev = NULL;

    dev = get_devinfo_from_addr(sin->addr);
    if (!dev)
    {
        /* New Device */
        INFO("NEW device advertisement rcvd");
        dev = add_new_device(sin);
    }
    ret_chk(!dev, "could not get dev");

    gettimeofday(&dev->lastseen, NULL);

    return SUCCESS;
ret_fail:
    return FAILURE;
}

int isync_device_init(void)
{
    ret_chk(g_dev_db, "dev list db already inited");

    g_dev_db = db_init(MAX_DEVICES, sizeof(dev_info_t), device_cmp);
    ret_chk(!g_dev_db, "dev db init failed");

    g_dev_timer = isync_timer_start(
        DEV_PERIODIC_TIMER, TIMER_PERIODIC, dev_periodic_cb, NULL);
    ret_chk(!g_dev_timer, "dev timer start failed");

#if 0
    /* Add Dummy device - for testing */
    scan_info_t sin;
    strncpy(sin.addr, "aa:bb:cc:dd:ee:11", sizeof(sin.addr));
    strncpy(sin.name, "dummy", sizeof(sin.name));
    isync_device_handle_advertisement(&sin);
#endif

    return SUCCESS;
ret_fail:
    isync_device_cleanup();
    return FAILURE;
}
