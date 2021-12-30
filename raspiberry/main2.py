import mqttdemo
import time
import json
import datetime
import sounddemo
import paho.mqtt.publish as publish

######################全局变量###################
dictInfo = ''
startTime=time.time()+1000
startTimer=0
todayTime=0
timer = '0'
stop = 0
ring = False #是否响铃
send = False #是否已发送起床用时
player = sounddemo.Player()
alarm=0

# 服务器地址
strBroker = "test.ranye-iot.net"
# 通信端口
port = 1883

while(True):
    
    dictInfo = mqttdemo.start()
    print(dictInfo)
    print("+++++++++++++++++++++++++++++++")
    dayTime = int(time.mktime(datetime.date.today().timetuple())) #当天0点时间戳
    nextDayTime= dayTime + 3600 * 24 #明天0点时间戳
    currentTime = time.time()  # 当前的时间戳
    print("++++++++++++++++++++"+str(currentTime-startTime))
    if(todayTime == 0): #如果todayTime没进行初始化
        todayTime = dayTime #把今天0点的时间戳给todayTime
    elif(dayTime != todayTime): #如果到了明天
        todayTime = dayTime #明天0点的时间戳给todayTime
        startTime = todayTime + (timeInput) #更新响铃时间
        ring = False
        send = False
    if dictInfo!='': #接受到非空数据
        if 'timer' in dictInfo:
            timer = int(dictInfo['timer'])
        if 'stop' in dictInfo:
            stop = int(dictInfo['stop'])
        if 'alarm' in dictInfo:
            if dictInfo['alarm']==1:
                print("hasAlarm")
                player.play("3.mp3")
                dictInfo['alarm']=0
            
            
            
        # print("===================================")
        if startTimer != timer: #起床时间改变了,更新起床时间
            ring = False
            send = False
            startTimer = timer
            timeInput = timer #响铃的时间戳
            if todayTime+int(timeInput) <= currentTime:
                startTime= nextDayTime + int(timeInput)
            else:
                startTime= todayTime + int(timeInput)
        print("stop:"+str(stop))
        if stop == 0:
            print('==================================')
            if startTime <= currentTime and not ring:
                player.play("2.mp3")
                ring = True
        elif stop == 1: #stop为1，响铃停止
            player.stop()
            if not send:
                duration = {'duration':currentTime - startTime} #将发送的起床时间设置为字典格式
                sendMsg = json.dumps(duration) #字典格式转为json格式
                publish.single("LSD", sendMsg, hostname=strBroker) #在mqtt的test topic发布消息
                send = True
            stop = 0
        
    else: #接收到空数据
        if stop == 0:
            if startTime <= currentTime and not ring:
                player.play("2.mp3")
                ring = True
        elif stop == 1: #stop为1，响铃停止
            player.stop()
            if not send:
                tmp=int(currentTime - startTime)
                duration = {'duration':tmp} #将发送的起床时间设置为字典格式
                sendMsg = json.dumps(duration) #字典格式转为json格式
                publish.single("LSD", sendMsg, hostname=strBroker) #在mqtt的test topic发布消息
                send = True
            stop = 0
    dictInfo=""