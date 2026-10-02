#include <stdio.h>
#include <time.h>

#include "esp_log.h"
#include "esp_sntp.h"

#include "time_sync.h"


#define TAG "TIME_SYNC"



static void obtain_time(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");


    // 使用网络时间服务器
    esp_sntp_setoperatingmode(
        ESP_SNTP_OPMODE_POLL
    );


    esp_sntp_setservername(
        0,
        "pool.ntp.org"
    );


    esp_sntp_init();


    time_t now = 0;
    struct tm timeinfo = {0};


    int retry = 0;

    const int retry_count = 10;


    while(timeinfo.tm_year < (2020 - 1900)
          && ++retry < retry_count)
    {

        ESP_LOGI(TAG,
                 "Waiting for system time...");

        vTaskDelay(
            pdMS_TO_TICKS(2000)
        );


        time(&now);

        localtime_r(
            &now,
            &timeinfo
        );
    }


    if(timeinfo.tm_year >= (2020 - 1900))
    {
        ESP_LOGI(TAG,
                 "Time synchronized");
    }
    else
    {
        ESP_LOGE(TAG,
                 "Time sync failed");
    }
}



void time_sync_init(void)
{
    // 设置中国时区
    setenv(
        "TZ",
        "CST-8",
        1
    );

    tzset();


    obtain_time();
}