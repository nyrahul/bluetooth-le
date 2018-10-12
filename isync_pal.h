#pragma once

/* Advertisement APIs */
int     isync_advertise(const uint32_t rid, const uint16_t devtype);
void    isync_adv_cleanup(void);

/* Scan APIs */
typedef int (*scan_notify_cb)(const char *addrstr, size_t len);
int     isync_scan(scan_notify_cb);
void    isync_scan_cleanup(void);

/* Transport */
