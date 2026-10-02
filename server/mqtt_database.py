import json
import sqlite3
import time
from datetime import datetime

import paho.mqtt.client as mqtt


# ==========================
# MQTT配置
# ==========================

BROKER = "10.200.187.157"

PORT = 1883


# 多设备订阅
# device/1
# device/2
# device/3
TOPIC = "gateway/device/+/data"



# ==========================
# 数据库
# ==========================

DB_NAME = "sensor_data.db"



# ==========================
# 初始化数据库
# ==========================

def init_database():

    conn = sqlite3.connect(DB_NAME)

    cursor = conn.cursor()


    # 温湿度历史数据

    cursor.execute(
        """
        CREATE TABLE IF NOT EXISTS temperature_data
        (
            id INTEGER PRIMARY KEY AUTOINCREMENT,

            device INTEGER,

            temperature REAL,

            humidity REAL,

            time TEXT
        )
        """
    )



    # 设备状态

    cursor.execute(
        """
        CREATE TABLE IF NOT EXISTS device_status
        (
            device INTEGER PRIMARY KEY,

            status TEXT,

            last_time TEXT
        )
        """
    )


    conn.commit()

    conn.close()



# ==========================
# 保存采集数据
# ==========================

def save_data(data):


    conn = sqlite3.connect(DB_NAME)

    cursor = conn.cursor()



    cursor.execute(
        """
        INSERT INTO temperature_data
        (
            device,
            temperature,
            humidity,
            time
        )

        VALUES
        (?, ?, ?, ?)

        """,

        (
            data["device"],

            data["temperature"],

            data["humidity"],

            data["time"]

        )
    )


    conn.commit()

    conn.close()





# ==========================
# 更新设备状态
# ==========================

def update_device_status(data):


    conn = sqlite3.connect(DB_NAME)

    cursor = conn.cursor()



    cursor.execute(
        """
        INSERT INTO device_status
        (
            device,

            status,

            last_time
        )

        VALUES
        (
            ?,

            ?,

            ?

        )


        ON CONFLICT(device)

        DO UPDATE SET

        status=?,

        last_time=?

        """,

        (

            data["device"],

            "ONLINE",

            data["time"],


            "ONLINE",

            data["time"]

        )

    )



    conn.commit()

    conn.close()





# ==========================
# 检查设备离线
# ==========================

def check_offline():


    conn = sqlite3.connect(DB_NAME)

    cursor = conn.cursor()



    now = datetime.now()



    cursor.execute(
        """
        SELECT device,last_time

        FROM device_status

        """
    )



    devices = cursor.fetchall()



    for device,last_time in devices:



        try:


            last = datetime.strptime(

                last_time,

                "%Y-%m-%d %H:%M:%S"

            )



            diff = (

                now-last

            ).total_seconds()



            if diff > 10:



                cursor.execute(

                    """
                    UPDATE device_status

                    SET status=?

                    WHERE device=?

                    """,

                    (

                        "OFFLINE",

                        device

                    )

                )



        except Exception as e:


            print(
                "Time parse error:",
                e
            )



    conn.commit()

    conn.close()





# ==========================
# MQTT连接回调
# ==========================

def on_connect(client, userdata, flags, rc):


    if rc == 0:


        print("MQTT connected")


        client.subscribe(TOPIC)


        print(
            "Subscribe:",
            TOPIC
        )


    else:


        print(
            "MQTT connect failed:",
            rc
        )





# ==========================
# MQTT数据接收
# ==========================

def on_message(client, userdata, msg):


    try:


        payload = msg.payload.decode(
            "utf-8"
        )


        data = json.loads(payload)



        print(
            "\n========== DATA =========="
        )



        print(
            "Device:",
            data["device"]
        )


        print(
            "Temperature:",
            data["temperature"],
            "℃"
        )


        print(
            "Humidity:",
            data["humidity"],
            "%RH"
        )


        print(
            "Time:",
            data["time"]
        )



        # 保存历史数据

        save_data(data)



        # 更新在线状态

        update_device_status(data)



        print(
            "Database saved"
        )


        print(
            "=========================="
        )



    except Exception as e:


        print(
            "Error:",
            e
        )





# ==========================
# 主程序
# ==========================

if __name__ == "__main__":



    init_database()



    client = mqtt.Client()



    client.on_connect = on_connect


    client.on_message = on_message




    print(
        "Connecting MQTT Broker..."
    )



    client.connect(

        BROKER,

        PORT,

        60

    )



    # 开启MQTT后台线程

    client.loop_start()



    # 后台检测设备状态

    while True:


        check_offline()


        time.sleep(2)