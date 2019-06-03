/* 
 * Created @ 20.05.2019
 * 
*/

#ifndef MQTT_H
#define MQTT_H

#include "MQTT_private.h"
#include "stdint.h"


typedef struct MQTT {
    uint8_t fix_header[FIXED_HEADER_SIZE];
    uint8_t *var_header;
    uint8_t payload[PAYLOAD_SIZE];

    uint8_t var_header_size;

    uint8_t (*createPacket)(struct MQTT*, int32_t);
    void (*fillPacket)(struct MQTT*, uint8_t*);
} MQTT;

extern void initMQTT(MQTT *self);

#endif /* MQTT_H */