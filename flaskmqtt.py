import eventlet
import json
from flask import Flask, render_template
from flask_mqtt import Mqtt
from flask_socketio import SocketIO
from bbddhandler import selectAll

eventlet.monkey_patch()

app = Flask(__name__)
app.config['SECRET'] = 'my secret key'
app.config['TEMPLATES_AUTO_RELOAD'] = True
app.config['MQTT_BROKER_URL'] = '192.168.1.55'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = 'mqttusr'
app.config['MQTT_PASSWORD'] = 'mqttrpi'
app.config['MQTT_KEEPALIVE'] = 5
app.config['MQTT_TLS_ENABLED'] = False


mqtt = Mqtt(app)
socketio = SocketIO(app)
#bootstrap = Bootstrap(app)


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/records')
def records():
    select = selectAll('SELECT * FROM Data')   
    return render_template('records.html',values=select)



@socketio.on('publish')
def handle_publish(json_str):
    data = json.loads(json_str)
    mqtt.publish(data['topic'], data['message'])


@socketio.on('subscribe')
def handle_subscribe(json_str):
    data = json.loads(json_str)
    mqtt.subscribe(data['topic'])

@socketio.on('changeLed')
def handle_changeLed(data):
    print data
    mqtt.publish("esp8266", data)


@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    data = dict(
        topic=message.topic,
        payload=message.payload.decode()
    )
    print("datos recibidos: ", data)
    print("topic: ",message.topic)
    print("mensaje: ",message.payload)
    socketio.emit('mqtt_message', data=data)
    socketio.send(message.payload,broadcast = True)


@mqtt.on_log()
def handle_logging(client, userdata, level, buf):
    print(level, buf)


if __name__ == '__main__':
    mqtt.subscribe('voltage')
    mqtt.subscribe('temperature')
    mqtt.subscribe('humidity')
    mqtt.subscribe('led')
    socketio.run(app, host='0.0.0.0', port=5000, use_reloader=True, debug=True)