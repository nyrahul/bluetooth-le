#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include "isync.h"
#include "epoll_util.h"
#include "isync_transport.h"

static int module_id = ISYNC_TRANSPORT;

#if 0

typedef struct _sess_
{
    fdinfo_t fi;
} session_t;

session_t *ssn_alloc(int fd, int is_server);

void ssn_release(session_t *ssn)
{
    if (!ssn)
        return;
    INFO("closing session with fd:%d", ssn->fi.fd);
    CLOSE(ssn->fi.fd);
    FREE(ssn);
}

int accept_handler(fdinfo_t *fi)
{
    ERROR("GENERAL ACCEPT HANDLER NOT IMPLEMENTED\n");
    return FAILURE;
}

int l2cap_accept_handler(fdinfo_t *fi)
{
    int client     = -1;
    session_t *ssn = NULL;

    client = l2cap_accept(fi->fd);
    ret_chk(client < 0, "l2cap_accept failed");

    ssn = ssn_alloc(client, 0);
    ret_chk(!ssn, "ssn_alloc failed");

    client = -1;

    return SUCCESS;

ret_fail:
    ssn_release(ssn);
    CLOSE(client);
    return FAILURE;
}

int rfcomm_accept_handler(fdinfo_t *fi)
{
    int client     = -1;
    session_t *ssn = NULL;

    client = rfcomm_accept(fi->fd);
    ret_chk(client < 0, "rfcomm_accept failed");

    ssn = ssn_alloc(client, 0);
    ret_chk(!ssn, "ssn_alloc failed");

    client = -1;

    return SUCCESS;

ret_fail:
    ssn_release(ssn);
    CLOSE(client);
    return FAILURE;
}

int rd_handler(fdinfo_t *fi)
{
    char buf[1024] = {0};
    ssize_t n;

    n = read(fi->fd, buf, sizeof(buf) - 1);
    ret_chk(n <= 0, "read failed");

    buf[n] = 0;
    INFO("received n=%zu [%s]", n, buf);

    return SUCCESS;
ret_fail:
    CLOSE(fi->fd);
    return SUCCESS;
}

int error_handler(fdinfo_t *fi)
{
    session_t *ssn = fi->ptr;
    ssn_release(ssn);
    return SUCCESS;
}

#    define SSN_IS_SERVER (1 << 0)
#    define SSN_IS_L2CAP (1 << 1)
#    define SSN_IS_RFCOMM (1 << 2)

cb_fdinfo_t get_accept_handler(int flags)
{
    if (flags & SSN_IS_L2CAP)
    {
        return l2cap_accept_handler;
    }
    else if (flags & SSN_IS_RFCOMM)
    {
        return rfcomm_accept_handler;
    }
    return accept_handler;
}

session_t *ssn_alloc(int fd, int flags)
{
    session_t *ssn = NULL;
    int ret;

    ssn = calloc(sizeof(session_t), 1);
    ret_chk(!ssn, "calloc failed");

    ssn->fi.fd    = fd;
    ssn->fi.er_cb = error_handler;
    if (flags & SSN_IS_SERVER)
    {
        ssn->fi.rd_cb = get_accept_handler(flags);
    }
    else
    {
        ssn->fi.rd_cb = rd_handler;
        // ssn->fi.wr_cb = wr_handler;
    }
    ssn->fi.ptr = ssn;

    ret = epoll_add_fd(&ssn->fi);
    ret_chk(ret < 0, "epoll_add_fd failed");

    return ssn;
ret_fail:
    FREE(ssn);
    return NULL;
}

#    if USE_RFCOMM
#        define FLAGS (SSN_IS_SERVER | SSN_IS_RFCOMM)
#        define srv_port USE_RFCOMM
#        define srv_ble_xport rfcomm_start_server
#        define cli_ble_xport rfcomm_start_cli
#        define cli_close rfcomm_close
#    else // L2CAP
#        define FLAGS (SSN_IS_SERVER | SSN_IS_L2CAP)
#        define srv_port ISYNC_L2CAP_PSM
#        define srv_ble_xport l2cap_start_server
#        define cli_ble_xport l2cap_start_cli
#        define cli_close l2cap_close
#    endif

void *ble_transport_start_cli(const char *peeraddr)
{
    session_t *ssn = NULL;
    int sfd        = -1;

    sfd = cli_ble_xport(peeraddr);
    if (sfd < 0)
    {
        ERROR("start_cli failed\n");
        return NULL;
    }

    ssn = ssn_alloc(sfd, 0);
    if (!ssn)
    {
        ERROR("ssn_alloc failed");
        cli_close(sfd);
    }

    return ssn;
}

int ble_transport_start_ssn(void)
{
    session_t *ssn = NULL;
    int sfd        = -1;

    sfd = srv_ble_xport(srv_port);
    if (sfd < 0)
    {
        ERROR("start_server failed");
        return FAILURE;
    }

    ssn = ssn_alloc(sfd, FLAGS);
    if (!ssn)
    {
        ERROR("ssn_alloc failed\n");
        cli_close(sfd);
        return FAILURE;
    }

    return SUCCESS;
}
#endif

void ssn_releaseall(void)
{
    // TODO release all sessions
}

int isync_transport_init(void)
{
#if 0
    int ret = SUCCESS;
    ret = ble_transport_start_ssn();
    ret_chk(ret != SUCCESS, "epoll_init failed");
#endif

    INFO("isync transport inited");
    return SUCCESS;
}

void isync_transport_cleanup(void) { ssn_releaseall(); }
