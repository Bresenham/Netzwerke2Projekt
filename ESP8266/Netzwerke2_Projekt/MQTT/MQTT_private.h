/* 
 * Created @ 20.05.2019
 * 
*/
#ifndef MQTT_PRIVATE_H
#define MQTT_PRIVATE_H

#define PACKET_TYPE_PUBLISH             0x30

#define PACKET_TYPE_CONNECT             0x10

#define PROTOCOL_TYPE_3_1_1             0x04

#define CONNECT_CLEAN_SESSION_FLAG      0x02

#define PACKET_PUBLISH_NO_DUP           0x00

#define PACKET_PUBLISH_QOS_AT_MOST_ONCE 0x00

#define PACKET_PUBLISH_RETAIN           0x00

#define VARIABLE_HEADER_TOPIC_START     2

#define PUBLISH_FIXED_HEADER_SIZE       2
#define PUBLISH_VAR_HEADER_SIZE         5
#define PUBLISH_PAYLOAD_SIZE            4

#define CONNECT_FIXED_HEADER_SIZE       2
#define CONNECT_VAR_HEADER_SIZE         10
#define CONNECT_PAYLOAD_SIZE            15

#endif /* MQTT_PRIVATE_H */