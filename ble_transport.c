#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include "isync.h"
#include "epoll_util.h"
#include "ble_transport.h"
#include <bluetooth/l2cap.h>

typedef struct _sess_
{
    fdinfo_t fi;
} session_t;

session_t *ssn_alloc(int fd, int is_server);

void ssn_release(session_t *ssn)
{
    if (!ssn)
        return;
    CLOSE(ssn->fi.fd);
    FREE(ssn);
}

int accept_handler(fdinfo_t *fi)
{
    char buf[256];
    int client                  = -1;
    session_t *ssn              = NULL;
    struct sockaddr_l2 rem_addr = {0};
    socklen_t opt               = sizeof(rem_addr);

    client = accept(fi->fd, (struct sockaddr *)&rem_addr, &opt);
    ret_chk(client < 0, "accept failed");

    ba2str(&rem_addr.l2_bdaddr, buf);
    INFO("accepted connection from %s\n", buf);

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
    ERROR("Error handler called");
    ssn_release(ssn);
    return SUCCESS;
}

session_t *ssn_alloc(int fd, int is_server)
{
    session_t *ssn = NULL;

    ssn = calloc(sizeof(session_t), 1);
    ret_chk(!ssn, "calloc failed");

    ssn->fi.fd    = fd;
    ssn->fi.er_cb = error_handler;
    if (is_server)
    {
        ssn->fi.rd_cb = accept_handler;
    }
    else
    {
        ssn->fi.rd_cb = rd_handler;
        // ssn->fi.wr_cb = wr_handler;
    }
    ssn->fi.ptr = ssn;

    return ssn;
ret_fail:
    FREE(ssn);
    return NULL;
}

int ble_transport_start_cli(bdaddr_t *bda)
{
    struct sockaddr_l2 addr = {0};
    int s                   = -1, status;
    int ret                 = FAILURE;

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    ret_chk(s < 0, "socket failed");

    // set the connection parameters (who to connect to)
    addr.l2_family = AF_BLUETOOTH;
    addr.l2_psm    = htobs(ISYNC_L2CAP_PSM);
    addr.l2_bdaddr = *bda;

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    ret_chk(status, "connect failed %m");

    status = write(s, "hello!", 6);
    ret_chk(status <= 0, "write failed");

    ret = SUCCESS;

ret_fail:
    CLOSE(s);
    return ret;
}

int l2cap_start_server(uint16_t psm)
{
    struct sockaddr_l2 loc_addr = {0};
    int s                       = -1, ret;
    session_t *ssn              = NULL;

    s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    ret_chk(s < 0, "socket failed");

    loc_addr.l2_family = AF_BLUETOOTH;
    loc_addr.l2_bdaddr = *BDADDR_ANY;
    loc_addr.l2_psm    = htobs(psm);

    ret = bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
    ret_chk(ret < 0, "bind failed");

    ret = listen(s, 10);
    ret_chk(ret < 0, "listen failed");

    ssn = ssn_alloc(s, 1);
    ret_chk(!ssn, "ssn_alloc failed");

    ret = epoll_add_fd(&ssn->fi);
    ret_chk(ret < 0, "epoll_add_fd failed");

    return s;

ret_fail:
    CLOSE(s);
    FREE(ssn);
    return FAILURE;
}

void ssn_releaseall(void)
{
    // TODO release all sessions
}

int ble_transport_start_ssn(bdaddr_t *bda)
{
    int sfd = -1;

    if (!bda)
    {
        sfd = l2cap_start_server(ISYNC_L2CAP_PSM);
        ret_chk(sfd < 0, "l2cap_start_server failed");
    }

    return SUCCESS;
ret_fail:
    CLOSE(sfd);
    return FAILURE;
}

int ble_transport_init(void)
{
    int ret;

    ret = epoll_init();
    ret_chk(ret != SUCCESS, "epoll_init failed");

    ret = ble_transport_start_ssn(NULL);
    ret_chk(ret != SUCCESS, "epoll_init failed");

    return SUCCESS;
ret_fail:
    epoll_deinit();
    return FAILURE;
}

void ble_transport_cleanup(void)
{
    epoll_deinit();
    ssn_releaseall();
}
