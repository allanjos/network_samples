import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected to broker. result=" + str(rc))

    print('Publishing to topic...')

    client.publish("xyz", "54321")

    print('Subscribing to topic...')

    client.subscribe("xxx", 0)

def on_message(client, userdata, msg):
    print('[message] topic=' + msg.topic + " " + str(msg.payload))

    print('Publishing to topic...')

    client.publish("xyz", "54321")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("localhost", 1883, 60)

client.loop_forever()