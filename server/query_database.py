import sqlite3


conn = sqlite3.connect(
    "sensor_data.db"
)


cursor = conn.cursor()


cursor.execute(
    """
    SELECT *
    FROM temperature_data
    ORDER BY id DESC
    LIMIT 10
    """
)


rows = cursor.fetchall()


print("====================")

for row in rows:
    print(row)


print("====================")


conn.close()