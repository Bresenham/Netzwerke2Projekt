/* 
 * Created @ 11.05.2019
 * 
*/

#ifndef WIFI_PRIVATE_H
#define WIFI_PRIVATE_H

#include "Wifi.h"

extern void ICACHE_FLASH_ATTR wifiConnect(Wifi *self);

extern void ICACHE_FLASH_ATTR wifiSetDataToPublish(Wifi *self, const uint8_t *data, const uint8_t length);

#endif /* WIFI_PRIVATE_H */