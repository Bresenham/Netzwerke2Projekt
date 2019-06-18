import paho.mqtt.client as mqtt
 
MQTT_SERVER = "192.168.0.100"
MQTT_PATH = "a/b"
 
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
 
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(MQTT_PATH)
 
# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	temp = float((msg.payload[0] << 24 | msg.payload[1] << 16 | msg.payload[2] << 8 | msg.payload[3]) / 100.0)
	print(msg.topic,"Temperatur:",str(temp))
    # more callbacks, etc
 
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
 
client.connect(MQTT_SERVER, 1883, 60)
 
# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()

while(True):
	print(a)