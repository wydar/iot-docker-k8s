import paho.mqtt.client as mqtt


mqtt_username = "mqttusr"
mqtt_password = "mqttrpi"
mqtt_topic_sub = "temperature-micro"
mqtt_topic_pub = "temperature"
mqtt_broker_ip = "192.168.1.50"


def on_connect(client, userdata, flag, rc):
    print "Connected!", str(rc)
    client.subscribe(mqtt_topic_sub)


def on_message(client, userdata, msg): 
    print "Topic: ", msg.topic + "\nMessage: " + str(msg.payload)
    client.publish(mqtt_topic_pub, str(msg.payload))
    

client = mqtt.Client()
client.username_pw_set(mqtt_username, mqtt_password)

client.on_connect = on_connect
client.on_message = on_message

client.connect(mqtt_broker_ip, 1883)

client.loop_forever()
client.disconnect()