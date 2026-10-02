import sqlite3


conn = sqlite3.connect(
    "sensor_data.db"
)


cursor = conn.cursor()


cursor.execute(
    """
    SELECT *
    FROM device_status
    """
)


for row in cursor.fetchall():

    print(row)


conn.close()