#include "isync.h"
#include "isync_db.h"
#include "timer_util.h"
#include "epoll_util.h"
#include <sys/timerfd.h>

static int module_id = ISYNC_TIMER;

typedef struct _timer_info_
{
    fdinfo_t fi;
    timer_cb cb;
    void *usp;
} timer_info_t;

void *g_timer_db;

#define MAX_TIMERS 100
int isync_timer_init(void)
{
    if (g_timer_db)
        return SUCCESS;

    g_timer_db = db_init(MAX_TIMERS, sizeof(timer_info_t), NULL);
    ret_chk(!g_timer_db, "timer db init failed");

    return SUCCESS;
ret_fail:
    return FAILURE;
}

void isync_timer_cleanup(void)
{
    if (!g_timer_db)
        return;
    db_cleanup(g_timer_db);
    g_timer_db = NULL;
    INFO("timers cleaned up");
}

void isync_timer_delete(void *ptr)
{
    timer_info_t *ti = ptr;
    if (!ti)
        return;
    CLOSE(ti->fi.fd);
    db_free(g_timer_db, ti);
}

int timer_read_handler(fdinfo_t *fi)
{
    uint64_t exp;
    int ret;
    timer_info_t *ti = fi->ptr;

    ret = read(fi->fd, &exp, sizeof(exp));
    ret_chk(ret != sizeof(uint64_t), "read invalid exp");

    if (ti->cb)
    {
        ti->cb(ti->usp);
    }
    return SUCCESS;
ret_fail:
    return FAILURE;
}

int timer_error_handler(fdinfo_t *fi)
{
    timer_info_t *ti = fi->ptr;
    ERROR("recevied error handler for timer");
    isync_timer_delete(ti);
    return SUCCESS;
}

void *isync_timer_start(int ms, int flags, timer_cb cb, void *usp)
{
    timer_info_t *ti = NULL;
    fdinfo_t *fi     = NULL;
    struct itimerspec ts;
    struct timespec now;
    int ret;

    ti = db_alloc(g_timer_db);
    ret_chk(!ti, "timer db_alloc failed");

    fi = &ti->fi;
    memset(fi, 0, sizeof(fdinfo_t));
    fi->fd = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC);
    ret_chk(fi->fd < 0, "timer create failed");

    memset(&ts, 0, sizeof(ts));
    ret = clock_gettime(CLOCK_REALTIME, &now);
    ret_chk(ret, "clock_gettime failed");

    ts.it_value.tv_sec  = now.tv_sec + (ms / 1000);
    ts.it_value.tv_nsec = now.tv_nsec; // + ((ms % 1000) * 1000 * 1000);
    if (flags & TIMER_PERIODIC)
    {
        ts.it_interval.tv_sec  = (ms / 1000);
        ts.it_interval.tv_nsec = 0; // (ms % 1000) * 1000 * 1000;
    }

    ret = timerfd_settime(fi->fd, TFD_TIMER_ABSTIME, &ts, NULL);
    ret_chk(ret, "timerfd_settime failed");

    fi->er_cb = timer_error_handler;
    fi->ptr   = ti;
    fi->rd_cb = timer_read_handler;
    ret       = epoll_add_fd(fi);
    ret_chk(SUCCESS != ret, "epoll_add_fd timer failed");

    ti->usp = usp;
    ti->cb  = cb;
    INFO("timer started ms=%d", ms);

    return (void *)ti;
ret_fail:
    isync_timer_delete(ti);
    return NULL;
}
