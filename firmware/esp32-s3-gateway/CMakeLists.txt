idf_component_register(
    SRCS
        "main.c"
        "modbus_rtu.c"
        "temp_humi_sensor.c"
        "wifi_sta.c"
        "my_mqtt.c"
        "time_sync.c"

    INCLUDE_DIRS
        "."

    REQUIRES
        esp_driver_uart
        esp_timer
        freertos
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        mqtt
)