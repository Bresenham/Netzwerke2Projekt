/* 
 * Created @ 20.05.2019
 * 
*/

#include "MQTT.h"
#include "mem.h"

/********************/
/* PRIVATE FUNCTIONS */
/********************/
uint8_t mqttCreatePacket(MQTT *self, int32_t tempData) {
    self->fix_header[0] = PACKET_TYPE_PUBLISH | PACKET_PUBLISH_NO_DUP | PACKET_PUBLISH_QOS_AT_MOST_ONCE | PACKET_PUBLISH_RETAIN;

    const char topic[] = "measure/temp";
    const uint8_t topicStrLen = sizeof(topic) / sizeof(topic[0]);
    self->var_header_size = 1 + 1 + topicStrLen;

    os_free(self->var_header);
    self->var_header = os_zalloc(sizeof(uint8_t) * self->var_header_size);

    self->var_header[0] = 0;
    self->var_header[1] = topicStrLen;
    for(uint8_t i = 0; i < topicStrLen; i++) {
        self->var_header[2 + i] = topic[i];
    }

    self->payload[0] = (tempData >> 24) & 0xFF;
    self->payload[1] = (tempData >> 16) & 0xFF;
    self->payload[2] = (tempData >> 8) & 0xFF;
    self->payload[3] = (tempData & 0xFF);

    uint8_t completeLen = (1 + 1) + topicStrLen + PAYLOAD_SIZE;

    do {
        uint8_t encodedByte = completeLen % 128;
        completeLen /= 128;
        if(completeLen > 0)
            encodedByte = encodedByte | 128;
        else {
            self->fix_header[1] = encodedByte;
            break;
        }
    }
    while(completeLen > 0);

    // Return complete length of packet including fixed header sizes
    return  (1 + 1) + completeLen;
}

void mqttFillPacket(MQTT *self, uint8_t *packet) {
    // Fill Fixed Header
    packet[0] = self->fix_header[0];
    packet[1] = self->fix_header[1];

    // Fill Variable Header
    packet[2] = self->fix_header[1];
    packet[3] = self->fix_header[2];

    // Fill variable TopicBytes
    for(uint8_t i = 0; i < self->var_header_size - 2; i++) {
        packet[4 + i] = self->var_header[i + 2];
    }
    
    // Fill payload
    for(uint8_t i = 0; i < PAYLOAD_SIZE; i++) {
        packet[4 + self->var_header_size] = self->payload[i];
    }
}


/********************/
/* PUBLIC FUNCTIONS */
/********************/
void initMQTT(MQTT *self) {
    self->createPacket = &mqttCreatePacket;
    self->fillPacket = &mqttFillPacket;
}