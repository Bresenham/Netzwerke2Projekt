import paho.mqtt.publish as publish
 
MQTT_SERVER = "10.149.13.154"
MQTT_PATH = "a/b"
 
publish.single(MQTT_PATH, "Hello World!", hostname=MQTT_SERVER)