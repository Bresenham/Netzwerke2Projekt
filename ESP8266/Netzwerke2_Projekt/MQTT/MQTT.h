/* 
 * Created @ 20.05.2019
 * 
*/

#ifndef MQTT_H
#define MQTT_H

#include "MQTT_private.h"
#include "stdint.h"

typedef struct MQTT {
    uint8_t fixed_header;

    void (*setHeader)(struct MQTT*);
} MQTT;

extern void initMQTT(MQTT *self);

#endif /* MQTT_H */