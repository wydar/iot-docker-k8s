import paho.mqtt.client as mqtt


mqtt_username = "mqttusr"
mqtt_password = "mqttrpi"
mqtt_topic_sub = "led-micro"
mqtt_topic_pub = ["esp8266","led"]
mqtt_broker_ip = "192.168.1.55"


def on_connect(client, userdata, flag, rc):
    print "Connected!", str(rc)
    client.subscribe(mqtt_topic_sub)

def on_message(client, userdata, msg):
    m =  str(msg.payload)
    print "Topic: ", msg.topic + "\nMessage: " + x
    if x == '1' or x == '0':    #El mensaje es para la pagina web
        client.publish(mqtt_topic_pub[1], x)
    else:   #El mensaje es para la placa
        client.publish(mqtt_topic_pub[0], x)
    