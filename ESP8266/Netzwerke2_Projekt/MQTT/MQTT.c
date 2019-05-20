/* 
 * Created @ 20.05.2019
 * 
*/

#include "MQTT.h"

/********************/
/* PRIVATE FUNCTIONS */
/********************/
void mqttSetHeader(MQTT *self) {
    self->fixed_header = PACKET_TYPE_PUBLISH | PACKET_PUBLISH_NO_DUP | PACKET_PUBLISH_QOS_AT_MOST_ONCE | PACKET_PUBLISH_RETAIN;
}


/********************/
/* PUBLIC FUNCTIONS */
/********************/
void initMQTT(MQTT *self) {
    self->setHeader = &mqttSetHeader;
}