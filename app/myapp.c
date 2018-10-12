#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <isync_appexp.h>

void cleanup(int sig)
{
    isync_stop();
    printf("bye.\n");
    exit(0);
}

int service_notification(const int id, device_t *dev,
                const uint8_t *buf, const size_t len)
{
    printf("GOT SERVICE NOTIFICATION:\n");
    return 0;
}

int main(void)
{
    int ret;

    signal(SIGINT, cleanup);

    isync_set_devid("myapp-dev");
    isync_set_devact("myuser@huawei.com");
    isync_set_devtype(0x1234);

    ret = isync_init();
    if(ret)
    {
        printf("isync_init failed ret:%d\n", ret);
        return 1;
    }
    printf("isync init success\n");

    isync_service_observer(service_notification);

    ret = isync_start_service(ID_CLIPBOARD);
    if(ret)
    {
        printf("start_service failed\n");
        return 1;
    }
#if 0
    ret = isync_start_service(10);
    ret = isync_start_service(11);
    ret = isync_start_service(12);
    ret = isync_start_service(13);
    ret = isync_start_service(14);

    {
        int i;
        for(i=10; i< 15; i++)
        {
            ret = isync_stop_service(i);
        }
    }
#endif

    pause();
    return 1;
}

