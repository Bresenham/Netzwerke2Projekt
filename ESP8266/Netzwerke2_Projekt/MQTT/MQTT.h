/* 
 * Created @ 20.05.2019
 * 
*/

#ifndef MQTT_H
#define MQTT_H

#include "stdint.h"
#include "ets_sys.h"
#include "mqtt_private.h"


typedef struct MQTT {
    uint8_t fix_header[PUBLISH_FIXED_HEADER_SIZE];
    uint8_t var_header[PUBLISH_VAR_HEADER_SIZE];
    uint8_t payload[PUBLISH_PAYLOAD_SIZE];

    bool hasSentConnect;
    uint8_t connectPacketLength;
    bool hasNewTempData;

    uint8_t (*createPublishPacket)(struct MQTT*, int32_t);
    void (*fillPublishPacket)(struct MQTT*, uint8_t*);
    void (*fillConnectPacket)(struct MQTT*, uint8_t*);
} MQTT;

MQTT mqtt;
uint8_t mqttSize;
void ICACHE_FLASH_ATTR initMQTT(MQTT *s);

#endif /* MQTT_H */