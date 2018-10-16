#include <sys/socket.h>
#include <errno.h>
#include "isync.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

static int module_id = ISYNC_BLE;

int l2cap_accept(int sfd)
{
    char buf[256];
    int client                  = -1;
    struct sockaddr_l2 rem_addr = {0};
    socklen_t opt               = sizeof(rem_addr);

    client = accept(sfd, (struct sockaddr *)&rem_addr, &opt);
    ret_chk(client < 0, "accept failed");

    ba2str(&rem_addr.l2_bdaddr, buf);
    INFO("accepted l2cap connection from %s\n", buf);

    return client;
ret_fail:
    return FAILURE;
}

int l2cap_start_cli(const char *peeraddr)
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
    str2ba(peeraddr, &addr.l2_bdaddr);
    // addr.l2_bdaddr = *bda;

    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    ret_chk(status, "connect failed %m");

    status = write(s, "hello!", 6);
    ret_chk(status <= 0, "write failed");

    ret = SUCCESS;
    return s;

ret_fail:
    CLOSE(s);
    return ret;
}

int l2cap_start_server(uint16_t psm)
{
    struct sockaddr_l2 loc_addr = {0};
    int s                       = -1, ret;

    s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    ret_chk(s < 0, "socket failed");

    loc_addr.l2_family = AF_BLUETOOTH;
    loc_addr.l2_bdaddr = *BDADDR_ANY;
    loc_addr.l2_psm    = htobs(psm);

    ret = bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
    ret_chk(ret < 0, "bind failed");

    ret = listen(s, 10);
    ret_chk(ret < 0, "listen failed");

    return s;

ret_fail:
    CLOSE(s);
    return FAILURE;
}

void l2cap_close(int s)
{
    INFO("closing l2cap fd:%d\n", s);
    CLOSE(s);
}
