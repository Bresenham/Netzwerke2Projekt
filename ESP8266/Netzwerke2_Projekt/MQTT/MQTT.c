/* 
 * Created @ 20.05.2019
 * 
*/

#include "mem.h"
#include "osapi.h"
#include "mqtt.h"

/********************/
/* PRIVATE FUNCTIONS */
/********************/
uint8_t ICACHE_FLASH_ATTR mqttCreatePublishPacket(MQTT *self, int32_t tempData) {
    self->fix_header[0] = PACKET_TYPE_PUBLISH | PACKET_PUBLISH_NO_DUP | PACKET_PUBLISH_QOS_AT_MOST_ONCE | PACKET_PUBLISH_RETAIN;
    self->fix_header[1] = 0x09;

    self->var_header[0] = 0x00;
    self->var_header[1] = 0x03;
    self->var_header[2] = 'a';
    self->var_header[3] = '/';
    self->var_header[4] = 'b';

    self->payload[0] = (tempData >> 24) & 0xFF;
    self->payload[1] = (tempData >> 16) & 0xFF;
    self->payload[2] = (tempData >> 8) & 0xFF;
    self->payload[3] = (tempData & 0xFF);

    return 11;
}

void ICACHE_FLASH_ATTR mqttFillConnectPacket(MQTT *self, uint8_t *packet) {
    packet[0] = PACKET_TYPE_CONNECT;
    packet[1] = 25;

    /* PROTOCOL NAME LENGTH */
    packet[2] = 0x00;
    packet[3] = 0x04;

    /* PROTOCOL NAME */
    packet[4] = 'M';
    packet[5] = 'Q';
    packet[6] = 'T';
    packet[7] = 'T';

    /* PROTOCOL VERSION */
    packet[8] = PROTOCOL_TYPE_3_1_1;

    /* CONNECT FLAGS */
    packet[9] = CONNECT_CLEAN_SESSION_FLAG;

    /* KEEP ALIVE DURATION IN SEC */
    packet[10] = 0x00;
    packet[11] = 0xF0;

    /* CLIENT ID LENGTH */
    packet[12] = 0x00;
    packet[13] = 0x0D;

    /* CLIENT ID */
    packet[14] = 'E';
    packet[15] = 'S';
    packet[16] = 'P';
    packet[17] = '8';
    packet[18] = '2';
    packet[19] = '6';
    packet[20] = '6';
    packet[21] = 'N';
    packet[22] = 'E';
    packet[23] = 'T';
    packet[24] = 'Z';
    packet[25] = 'E';
    packet[26] = '2';
}

void ICACHE_FLASH_ATTR mqttFillPacket(MQTT *self, uint8_t *packet) {
    // Fill Fixed Header
    packet[0] = self->fix_header[0];
    packet[1] = self->fix_header[1];

    // Fill Variable Header
    packet[2] = self->var_header[0];
    packet[3] = self->var_header[1];
    packet[4] = self->var_header[2];
    packet[5] = self->var_header[3];
    packet[6] = self->var_header[4];
    
    // Fill payload
    for(uint8_t i = 0; i < PUBLISH_PAYLOAD_SIZE; i++) {
        packet[7 + i] = self->payload[i];
    }
}


/********************/
/* PUBLIC FUNCTIONS */
/********************/
void ICACHE_FLASH_ATTR initMQTT(MQTT *self) {
    self->createPublishPacket = &mqttCreatePublishPacket;
    self->fillConnectPacket = &mqttFillConnectPacket;
    self->fillPublishPacket = &mqttFillPacket;
    self->connectPacketLength = CONNECT_FIXED_HEADER_SIZE + CONNECT_VAR_HEADER_SIZE + CONNECT_PAYLOAD_SIZE;
    self->hasNewTempData = false;
}