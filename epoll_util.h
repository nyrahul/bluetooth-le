#pragma once

struct _fdinfo_;
typedef int (*cb_fdinfo_t)(struct _fdinfo_ *);
typedef struct _fdinfo_
{
    int fd;
    cb_fdinfo_t rd_cb;
    cb_fdinfo_t wr_cb;
    cb_fdinfo_t er_cb;
    void *ptr;
} fdinfo_t;

int epoll_add_fd(fdinfo_t *fi);
void *epoll_thread(void *);
void epoll_deinit(void);
int epoll_init(void);

static inline int get_timediff(struct timeval *etv, struct timeval *stv)
{
    int ms;
    ms = (etv->tv_sec - stv->tv_sec) * 1000;
    ms += (etv->tv_usec - stv->tv_usec) / 1000;
    return ms;
}
