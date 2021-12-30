import json
import time
import datetime

########################全局变量########################
dictInfo = ''

# ======================================================
try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("MQTT client not find. Please install as follow:")
    print("git clone http://git.eclipse.org/gitroot/paho/org.eclipse.paho.mqtt.python.git")
    print("cd org.eclipse.paho.mqtt.python")
    print("sudo python setup.py install")

# 服务器地址
strBroker = "test.ranye-iot.net"
# 通信端口
port = 1883
# 订阅主题名
topic = 'LSD'


# ======================================================
def on_connect(mqttc, obj, rc, a):
    # print("OnConnetc, rc: " + str(rc))
    pass

def on_publish(mqttc, obj, mid):
    # print("OnPublish, mid: " + str(mid))
    pass

def on_subscribe(mqttc, obj, mid, granted_qos):
    # print("Subscribed: " + str(mid) + " " + str(granted_qos))
    pass

def on_log(mqttc, obj, level, string):
    # print("Log:" + string)
    pass

def on_message(mqttc, obj, msg):
    curtime = datetime.datetime.now()
    strcurtime = curtime.strftime("%Y-%m-%d %H:%M:%S")
    print(strcurtime + ": " + msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    on_exec(msg.payload)

def on_exec(strcmd):
    global dictInfo,timer,stop
    print("Exec:", strcmd) #strcmd是从mqtt接收到的Json信息
    if strcmd!=b'' and strcmd != b'0': #如果接收到mqtt上的非空数据
        dictInfo = json.loads(strcmd) #dictInfo是Json解码成的python字典格式
    else:
        dictInfo = ''

# =====================================================
def start():
    global dictInfo
    mqttc = mqtt.Client("mqttx_d1bd1a78")
    mqttc.on_message = on_message
    mqttc.on_connect = on_connect
    mqttc.on_publish = on_publish
    mqttc.on_subscribe = on_subscribe
    mqttc.on_log = on_log
    mqttc.connect(strBroker, port, 60)
    mqttc.subscribe(topic, 0)
    mqttc.loop_start()
    time.sleep(3)
    mqttc.loop_stop()
    return dictInfo