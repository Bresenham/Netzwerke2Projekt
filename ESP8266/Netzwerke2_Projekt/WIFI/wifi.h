/* 
 * Created @ 11.05.2019
 * 
*/

#ifndef WIFI_H
#define WIFI_H

#include "../MQTT/MQTT.h"
#include "ip_addr.h"
#include "os_type.h"
#include "ets_sys.h"
#include "osapi.h"
#include "espconn.h"
#include "eagle_soc.h"
#include "user_interface.h"
#include "gpio.h"
#include "mem.h"
#include "espconn.h"

typedef struct Wifi {
    void (*publishData)(struct Wifi*);
} Wifi;

extern void ICACHE_FLASH_ATTR initWifi(Wifi *self);

#endif /* WIFI_H */