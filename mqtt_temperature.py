import paho.mqtt.client as mqtt
from bbddhandler import Data_Handler as bd
import datetime

mqtt_username = "mqttusr"
mqtt_password = "mqttrpi"
mqtt_topic_sub = "temperature-micro"
mqtt_topic_pub = "temperature"
mqtt_broker_ip = "192.168.1.50"

stack = []
total = 0
hour = 0


def on_connect(client, userdata, flag, rc):
    print "Connected!", str(rc)
    client.subscribe(mqtt_topic_sub)
    global hour
    hour = datetime.datetime.today().hour


def on_message(client, userdata, msg): 
    print "Topic: ", msg.topic + "\nMessage: " + str(msg.payload)
    client.publish(mqtt_topic_pub, str(msg.payload))
    stack.append(float(msg.payload))
    global total
    total+=1
    global hour
    #store_in_bbdd("Temperature")
    if hour<datetime.datetime.today().hour:
        store_in_bbdd("Temperature")


def store_in_bbdd(topic):
    now = datetime.datetime.today()
    date = str(now.year) + "/" + str(now.month) + "/" + str(now.day)
    val = 0
    global total
    global hour
    for x in stack:
        val += x    
    bd(topic,val/total,date,str(now.hour))
    total = 0
    hour = now.hour
    

client = mqtt.Client()
client.username_pw_set(mqtt_username, mqtt_password)

client.on_connect = on_connect
client.on_message = on_message

client.connect(mqtt_broker_ip, 1883)

client.loop_forever()
client.disconnect()