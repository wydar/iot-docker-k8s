import paho.mqtt.client as mqtt
from bbddhandler import Data_Handler as bd
import datetime

mqtt_username = "mqttusr"
mqtt_password = "mqttrpi"
mqtt_topic_sub = ["temperature-micro","humidity-micro","voltage-micro"]
mqtt_broker_ip = "192.168.1.55"

stack_values = [0,0,0]
stack_number = [0,0,0]
hours = [0,0,0]


def on_connect(client, userdata, flag, rc):
    global hours
    print "Connected!", str(rc)
    hour = datetime.datetime.today().hour
    i=0
    for x in mqtt_topic_sub:
        client.subscribe(x)
        hours[i] = hour
        i+=1
    

def on_message(client, userdata, msg): 
    print "Topic: ", msg.topic + "\nMessage: " + str(msg.payload)
    i = 0
    match = False
    while match == False:
        if msg.topic == mqtt_topic_sub[i]:
            match = True
        else:
            i += 1
    save_data(float(msg.payload),i)
    

def save_data(data,pos):
    global stack_values
    global stack_number
    global hours
    if data > -999.0:
        print stack_values
        stack_values[pos] = stack_values[pos] + data
        stack_number[pos] += 1
      #  store_in_bbdd(pos)  #THIS LINE IS ONLY FOR TESTING
    if hours[pos]<datetime.datetime.today().hour:
        store_in_bbdd(pos)
    

def store_in_bbdd(pos):
    global stack_values
    global stack_number
    global hours

    now = datetime.datetime.today()
    date = str(now.year) + "/" + str(now.month) + "/" + str(now.day)         
    bd(mqtt_topic_sub[pos].replace('-micro',''),float(stack_values[pos]/stack_number[pos]),date,str(now.hour))
    
    stack_values[pos] = 0
    stack_number[pos] = 0
    hours[pos] = now.hour

client = mqtt.Client()
client.username_pw_set(mqtt_username, mqtt_password)

client.on_connect = on_connect
client.on_message = on_message

client.connect(mqtt_broker_ip, 1883)

client.loop_forever()
client.disconnect()
