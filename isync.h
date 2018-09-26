#pragma once

#define FAILURE -1
#define SUCCESS 0

#define COMPANY_ID_HUAWEI   0x027D  // As defined in bluetooth SIG

#define TYPE_ISYNC      0x88    // First byte of manufacture specific data after company id

#define ISYNC_ADV_TYPE_DEV_AUTH  0x01

#define ISYNC_VERSION   0x01

typedef struct _isync_devau_
{
    uint32_t rid;   //Random id in network byte order
}__attribute__((packed)) hdr_dev_auth_t;

typedef struct _isync_dev_
{
    uint8_t type;
    uint8_t version;        //Version
    uint8_t isync_type;     //ISYNC_ADV_TYPE_*
} __attribute__ ((packed)) hdr_dev_t;

#define GETB(DST, SRC, NUM, OFFSET) \
    memcpy(DST, SRC, NUM);\
    (OFFSET) += (NUM);

#define ERROR(...)                                                             \
    printf(__VA_ARGS__);                                                       \
    fflush(stdout);
#define INFO(...)                                                              \
    printf(__VA_ARGS__);                                                       \
    fflush(stdout);

#define TRUE 1
#define FALSE 0

