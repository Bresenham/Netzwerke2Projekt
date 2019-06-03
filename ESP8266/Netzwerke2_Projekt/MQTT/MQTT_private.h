#ifndef MQTT_PRIVATE_H
#define MQTT_PRIVATE_H

#define PACKET_TYPE_PUBLISH             0x30

#define PACKET_PUBLISH_NO_DUP           0x00

#define PACKET_PUBLISH_QOS_AT_MOST_ONCE 0x00

#define PACKET_PUBLISH_RETAIN           0x01

#define VARIABLE_HEADER_TOPIC_START     2

#define FIXED_HEADER_SIZE               2

#define PAYLOAD_SIZE                    4

#endif /* MQTT_PRIVATE_H */