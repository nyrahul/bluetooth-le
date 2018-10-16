#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>

#include "log.h"

#define FAILURE -1
#define SUCCESS 0

#define COMPANY_ID_HUAWEI 0xF27D // As defined in bluetooth SIG

// First byte of manufacture specific data after company id
#define TYPE_ISYNC 0x88

#define ISYNC_ADV_TYPE_DEV_AUTH 0x01

#define ISYNC_VERSION 0x01

typedef struct _isync_devau_
{
    uint32_t rid; // Random id in network byte order
} __attribute__((packed)) hdr_dev_auth_t;

typedef struct _isync_dev_
{
    uint8_t type;
    uint8_t version;    // Version
    uint8_t isync_type; // ISYNC_ADV_TYPE_*
} __attribute__((packed)) hdr_dev_t;

#define GETB(DST, SRC, NUM, OFFSET)                                            \
    memcpy(DST, SRC, NUM);                                                     \
    (OFFSET) += (NUM);

#define TRUE 1
#define FALSE 0

#define CLOSE(FD)                                                              \
    if (FD >= 0)                                                               \
    {                                                                          \
        close(FD);                                                             \
        (FD) = -1;                                                             \
    }

#define FREE(PTR)                                                              \
    if (PTR)                                                                   \
    {                                                                          \
        free(PTR);                                                             \
        (PTR) = 0;                                                             \
    }

#define ret_chk(COND, ...)                                                     \
    if (COND)                                                                  \
    {                                                                          \
        ERROR(__VA_ARGS__);                                                    \
        goto ret_fail;                                                         \
    }

#define ISYNC_L2CAP_PSM 0x1025
#define ISYNC_RFCOMM_CHN 0
