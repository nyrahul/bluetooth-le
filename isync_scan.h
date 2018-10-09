#pragma once

typedef int (*scan_notify_cb)(const char *addrstr, size_t len);

int     isync_scan(scan_notify_cb);
void    isync_scan_cleanup(void);
