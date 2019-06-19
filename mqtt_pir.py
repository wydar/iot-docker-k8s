import paho.mqtt.client as mqtt


mqtt_username = "mqttusr"
mqtt_password = "mqttrpi"
mqtt_topic_sub = "pir-micro"
mqtt_topic_pub = ["pir","esp8266"]
mqtt_broker_ip = "192.168.1.55"


def on_connect(client, userdata, flag, rc):
    print "Connected!", str(rc)
    client.subscribe(mqtt_topic_sub)


def on_message(client, userdata, msg): 
    print "Topic: ", msg.topic + "\nMessage: " + str(msg.payload)
  #  if str(msg.payload) == "2":
  #     client.publish("esp8266", str(msg.payload))
    client.publish(mqtt_topic_pub[0], str(msg.payload))
    client.publish(mqtt_topic_pub[1], str(msg.payload))

client = mqtt.Client()
client.username_pw_set(mqtt_username, mqtt_password)

client.on_connect = on_connect
client.on_message = on_message

client.connect(mqtt_broker_ip, 1883)

client.loop_forever()
client.disconnect()