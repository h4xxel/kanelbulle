#!/usr/bin/env python
import json
import mosquitto

# The callback for when the client receives a CONNACK response from the server.
def on_connect(obj, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("Testsites/MunktellSiencePark/meterevent")

# The callback for when a PUBLISH message is received from the server.
def on_message(obj, msg):
    x = json.loads(str(msg.payload))
    print(x['power'])
    a = float(x['power'])
    print a/10.0

client = mosquitto.Mosquitto("kanelbulle")
client.on_connect = on_connect
client.on_message = on_message

if client.connect("op-en.se", 1883, 60)>0:
	print "kanelbulle"

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
while 1:
	client.loop()
