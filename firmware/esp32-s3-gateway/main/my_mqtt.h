#ifndef MY_MQTT_H
#define MY_MQTT_H


void mqtt_app_start(void);


void mqtt_publish_sensor(
        int device_id,
        float temperature,
        float humidity
);


#endif