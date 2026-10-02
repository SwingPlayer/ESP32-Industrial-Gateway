from flask import Flask, render_template, jsonify
import sqlite3


app = Flask(__name__)


DATABASE = "sensor_data.db"



def get_latest_data():

    conn = sqlite3.connect(DATABASE)

    cursor = conn.cursor()


    cursor.execute(
        """
        SELECT *
        FROM temperature_data
        ORDER BY id DESC
        LIMIT 1
        """
    )


    data = cursor.fetchone()


    conn.close()

    return data



def get_history():

    conn = sqlite3.connect(DATABASE)

    cursor = conn.cursor()


    cursor.execute(
        """
        SELECT *
        FROM temperature_data
        ORDER BY id DESC
        LIMIT 10
        """
    )


    data = cursor.fetchall()


    conn.close()

    return data



# ==========================
# 网页主页
# ==========================

@app.route("/")
def index():

    latest = get_latest_data()

    history = get_history()


    return render_template(
        "index.html",
        latest=latest,
        history=history
    )



# ==========================
# 实时数据接口
# ==========================

@app.route("/api/latest")
def api_latest():

    data = get_latest_data()


    return jsonify(
        {
            "device": data[1],
            "temperature": data[2],
            "humidity": data[3],
            "time": data[4]
        }
    )


@app.route("/api/history")
def api_history():

    data = get_history()

    result = []

    for row in data:
        result.append(
            {
                "id": row[0],
                "device": row[1],
                "temperature": row[2],
                "humidity": row[3],
                "time": row[4]
            }
        )


    return jsonify(result)

@app.route("/api/status")
def api_status():

    conn = sqlite3.connect("sensor_data.db")

    cursor = conn.cursor()

    cursor.execute(
        """
        SELECT 
            device,
            status,
            last_time
        FROM device_status
        ORDER BY device
        """
    )


    rows = cursor.fetchall()

    conn.close()


    data = []

    for row in rows:

        data.append(
            {
                "device": row[0],
                "status": row[1],
                "time": row[2]
            }
        )


    return jsonify(data)


@app.route("/api/device/<int:device_id>")
def api_device(device_id):

    conn = sqlite3.connect(DATABASE)

    cursor = conn.cursor()


    cursor.execute(
        """
        SELECT *
        FROM temperature_data

        WHERE device=?

        ORDER BY id DESC

        LIMIT 1
        """,
        (device_id,)
    )


    row = cursor.fetchone()


    conn.close()



    if row is None:

        return jsonify(
            {
                "error":"device not found"
            }
        )


    return jsonify(
        {
            "device":row[1],
            "temperature":row[2],
            "humidity":row[3],
            "time":row[4]
        }
    )


@app.route("/api/devices")
def api_devices():

    conn = sqlite3.connect(DATABASE)

    cursor = conn.cursor()


    cursor.execute(
        """
        SELECT
            d.device,
            d.status,
            d.last_time,

            t.temperature,
            t.humidity

        FROM device_status d

        LEFT JOIN temperature_data t

        ON d.device=t.device

        AND t.id =
        (
            SELECT id
            FROM temperature_data
            WHERE device=d.device
            ORDER BY id DESC
            LIMIT 1
        )

        ORDER BY d.device

        """
    )


    rows = cursor.fetchall()


    conn.close()


    result=[]


    for row in rows:

        result.append(
            {
                "device":row[0],
                "status":row[1],
                "time":row[2],
                "temperature":row[3],
                "humidity":row[4]
            }
        )


    return jsonify(result)


if __name__ == "__main__":


    app.run(
        host="0.0.0.0",
        port=5000,
        debug=True
    )