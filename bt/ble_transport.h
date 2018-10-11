#pragma once

#include <bluetooth/bluetooth.h>

void *ble_transport_start_cli(const char *addr);
int ble_transport_init(void);
void ble_transport_cleanup(void);

// L2cap helper APIs
int l2cap_start_cli(const char *peeraddr);
int l2cap_start_server(uint16_t psm);
void l2cap_close(int s);
int l2cap_accept(int sfd);

// RFCOMM helper APIs
int rfcomm_start_cli(const char *peeraddr);
int rfcomm_start_server(uint8_t chn);
void rfcomm_close(int s);
int rfcomm_accept(int sfd);

