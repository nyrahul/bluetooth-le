#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Advertisement APIs */
int isync_advertise(const uint32_t rid, const uint16_t devtype);
void isync_adv_cleanup(void);

/* Scan APIs */
#define MAX_ADDRESS_LEN 64
#define MAX_NAME_LEN 64
typedef struct _scan_info_
{
    char addr[MAX_ADDRESS_LEN];
    char name[MAX_NAME_LEN];
} scan_info_t;

typedef int (*scan_notify_cb)(const scan_info_t *);
int isync_scan(scan_notify_cb);
void isync_scan_cleanup(void);

/* Service Info APIs */
int isync_set_service_info(uint8_t *arr, size_t len);

/* Transport */

#ifdef __cplusplus
}
#endif

