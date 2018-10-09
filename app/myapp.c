#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <isync_appexp.h>

void cleanup(int sig)
{
    hisync_stop();
    printf("bye.\n");
    exit(0);
}

int hidev_notification(const notify_type_e type, const subtype_e sub_type, const void *buf, const size_t buflen)
{
    printf("GOT NOTIFICATION:\n");
    return 0;
}

int main(void)
{
    int ret;

    signal(SIGINT, cleanup);

    ret = hisync_init(0xcafebabe, 0xface, hidev_notification);
    printf("hisync_init ret:%d\n", ret);

    pause();
    return 1;
}
