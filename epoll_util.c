#include <pthread.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include "epoll_util.h"
#include "isync.h"

int g_epollfd = -1;

int epoll_add_fd(fdinfo_t *fi)
{
    struct epoll_event event;

    event.data.ptr = fi;
    event.events   = EPOLLIN;

    return epoll_ctl(g_epollfd, EPOLL_CTL_ADD, fi->fd, &event);
}

#define CALL_CB(FI, CB)                                                        \
    if ((FI) && (FI)->CB)                                                      \
        (FI)->CB(FI);                                                          \
    else                                                                       \
        ERROR("could not called %s", #CB);

#define ER_CB(FI) CALL_CB(FI, er_cb)
#define RD_CB(FI) CALL_CB(FI, rd_cb)
#define WR_CB(FI) CALL_CB(FI, wr_cb)

#define MAXEVENTS 64
void *epoll_thread(void *arg)
{
    struct epoll_event *events = NULL;
    int n, i, efd = (int)(uintptr_t)arg;
    fdinfo_t *fi;

    events = calloc(MAXEVENTS, sizeof(struct epoll_event));
    ret_chk(!events, "calloc events failed");

    while (1)
    {
        n = epoll_wait(efd, events, MAXEVENTS, -1);
        if (n <= 0)
        {
            ERROR("epoll_wait returned n=%d", n);
            break;
        }
        for (i = 0; i < n; i++)
        {
            fi = events[i].data.ptr;

            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN)))
            {
                ERROR("epoll error");
                ER_CB(fi);
                continue;
            }
            if (events[i].events & EPOLLIN)
            {
                RD_CB(fi);
            }
            if (events[i].events & EPOLLOUT)
            {
                WR_CB(fi);
            }
        }
    }

ret_fail:
    FREE(events);
    return NULL;
}

int epoll_init(void)
{
    pthread_t tid;
    int ret;

    if (g_epollfd >= 0)
    {
        ERROR("transport is already inited");
        return SUCCESS;
    }

    g_epollfd = epoll_create1(0);
    ret_chk(g_epollfd < 0, "epoll_create1 failed");

    ret =
        pthread_create(&tid, NULL, epoll_thread, (void *)(uintptr_t)g_epollfd);
    ret_chk(ret, "pthread_create failed");

    return SUCCESS;

ret_fail:
    CLOSE(g_epollfd);
    return FAILURE;
}

void epoll_deinit(void) { CLOSE(g_epollfd); }
