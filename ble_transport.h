#pragma once

#include <bluetooth/bluetooth.h>

int ble_transport_start_ssn(bdaddr_t *bda);
int ble_transport_start_cli(bdaddr_t *bda);
int ble_transport_init(void);
void ble_transport_cleanup(void);
