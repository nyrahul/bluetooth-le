#pragma once

enum
{
    ISYNC_DB,
    ISYNC_DEVICE,
    ISYNC_SERVICE,
    ISYNC_TIMER,
    ISYNC_COMMON,
    ISYNC_TRANSPORT,
    ISYNC_BLE,
    MAX_MODULES
};

#define LVL_ERR 0
#define LVL_INFO 1
#define LVL_DBG 2

#define FLG_ALL (FLG_DBG | FLG_INFO | FLG_ERR)

#ifdef _ISYNC_COMMON_
int gLog[MAX_MODULES] = {
    [ISYNC_SERVICE] = LVL_DBG, [ISYNC_DEVICE] = LVL_INFO, [ISYNC_DB] = LVL_ERR};
#else
extern int gLog[MAX_MODULES];
#endif

#define PRN(STR, ...)                                                          \
    {                                                                          \
        struct timeval tv;                                                     \
        gettimeofday(&tv, NULL);                                               \
        printf("%-5s %5ld:%-4ld", STR, tv.tv_sec % 100000, tv.tv_usec / 1000); \
        printf(__VA_ARGS__);                                                   \
        printf("\n");                                                          \
        fflush(stdout);                                                        \
    }

#define ERROR(...)                                                             \
    if (gLog[module_id] >= LVL_ERR)                                            \
    {                                                                          \
        PRN("ERROR", __VA_ARGS__)                                              \
    }

#define INFO(...)                                                              \
    if (gLog[module_id] >= LVL_INFO)                                           \
    {                                                                          \
        PRN("INFO", __VA_ARGS__)                                               \
    }

#define DBG(...)                                                               \
    if (gLog[module_id] >= LVL_DBG)                                            \
    {                                                                          \
        PRN("DBG", __VA_ARGS__)                                                \
    }
