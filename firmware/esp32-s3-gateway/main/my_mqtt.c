#include <stdio.h>
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "my_mqtt.h"


#define TAG "MQTT"


/*
 * Mosquitto地址
 *
 * 这里填你的电脑IP
 *
 * 例如:
 * mqtt://10.200.187.157:1883
 *
 */
#define MQTT_BROKER_URI "mqtt://10.200.187.157:1883"



static esp_mqtt_client_handle_t client = NULL;



static void mqtt_event_handler(
        void *handler_args,
        esp_event_base_t base,
        int32_t event_id,
        void *event_data)
{

    esp_mqtt_event_handle_t event =
        (esp_mqtt_event_handle_t)event_data;


    switch(event_id)
    {

        case MQTT_EVENT_CONNECTED:

            ESP_LOGI(TAG,
                    "MQTT connected");

            break;



        case MQTT_EVENT_DISCONNECTED:

            ESP_LOGW(TAG,
                    "MQTT disconnected");

            break;



        default:
            break;
    }

}



void mqtt_app_start(void)
{

    esp_mqtt_client_config_t mqtt_cfg =
    {
        .broker.address.uri =
            MQTT_BROKER_URI,
    };


    client =
        esp_mqtt_client_init(&mqtt_cfg);


    esp_mqtt_client_register_event(
        client,
        ESP_EVENT_ANY_ID,
        mqtt_event_handler,
        NULL);



    esp_mqtt_client_start(client);


}



void mqtt_publish_sensor(
        int device_id,
        float temperature,
        float humidity)
{

    if(client == NULL)
    {
        return;
    }


    char payload[256];
    char topic[64];


    char time_str[32];

    time_t now;

    struct tm timeinfo;


    time(&now);

    localtime_r(
        &now,
        &timeinfo
    );


    strftime(
        time_str,
        sizeof(time_str),
        "%Y-%m-%d %H:%M:%S",
        &timeinfo
    );



    snprintf(
        payload,
        sizeof(payload),
        "{"
        "\"device\":%d,"
        "\"temperature\":%.2f,"
        "\"humidity\":%.2f,"
        "\"time\":\"%s\""
        "}",
        device_id,
        temperature,
        humidity,
        time_str
    );

    snprintf(topic,
        sizeof(topic),
        "gateway/device/%d/data",
        device_id);



    esp_mqtt_client_publish(

            client,

            topic,

            payload,

            0,

            1,

            0);



    ESP_LOGI(TAG,
            "Topic:%s",
            topic);

    ESP_LOGI(TAG,
            "Publish:%s",
            payload);


}