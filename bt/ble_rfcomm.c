#include <sys/socket.h>
#include <errno.h>
#include "isync.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int rfcomm_accept(int sfd)
{
    char buf[256];
    int client                  = -1;
    struct sockaddr_rc rem_addr = {0};
    socklen_t opt               = sizeof(rem_addr);

    client = accept(sfd, (struct sockaddr *)&rem_addr, &opt);
    ret_chk(client < 0, "accept failed");

    ba2str(&rem_addr.rc_bdaddr, buf);
    INFO("accepted rfcomm connection from %s\n", buf);

    return client;
ret_fail:
    return FAILURE;
}

int rfcomm_start_cli(const char *peeraddr)
{
    struct sockaddr_rc addr = {0};
    int s                   = -1, status;
    int ret                 = FAILURE;

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    ret_chk(s < 0, "socket failed");

    // set the connection parameters (who to connect to)
    addr.rc_family  = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t)USE_RFCOMM;
    str2ba(peeraddr, &addr.rc_bdaddr);
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

int rfcomm_start_server(uint8_t chn)
{
    struct sockaddr_rc loc_addr = {0};
    int s                       = -1, ret;
    socklen_t sl                = 0;

    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    ret_chk(s < 0, "socket failed");

    loc_addr.rc_family  = AF_BLUETOOTH;
    loc_addr.rc_bdaddr  = *BDADDR_ANY;
    loc_addr.rc_channel = chn;

    ret = bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
    ret_chk(ret < 0, "bind failed");

    ret = getsockname(s, (struct sockaddr *)&loc_addr, &sl);
    ret_chk(ret < 0, "getsockname failed");
    INFO("RFCOMM CHANNEL used:%d sl=%d\n", loc_addr.rc_channel, sl);

    ret = listen(s, 10);
    ret_chk(ret < 0, "listen failed");

    return s;

ret_fail:
    CLOSE(s);
    return FAILURE;
}

void rfcomm_close(int s)
{
    INFO("closing rfcomm fd:%d\n", s);
    CLOSE(s);
}
