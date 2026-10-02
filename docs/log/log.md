I (24) boot: ESP-IDF v5.5.5 2nd stage bootloader
I (25) boot: compile time Sep 29 2026 17:26:10
I (25) boot: Multicore bootloader
I (25) boot: chip revision: v0.2
I (28) boot: efuse block revision: v1.4
I (31) boot.esp32s3: Boot SPI Speed : 80MHz
I (35) boot.esp32s3: SPI Mode       : DIO
I (39) boot.esp32s3: SPI Flash Size : 16MB
I (43) boot: Enabling RNG early entropy source...
I (47) boot: Partition Table:
I (50) boot: ## Label            Usage          Type ST Offset   Length
I (56) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (63) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (69) boot:  2 factory          factory app      00 00 00010000 00100000
I (76) boot: End of partition table
I (79) esp_image: segment 0: paddr=00010020 vaddr=3c0b0020 size=20b9ch (134044) map
I (111) esp_image: segment 1: paddr=00030bc4 vaddr=3fc9b700 size=04e70h ( 20080) load
I (115) esp_image: segment 2: paddr=00035a3c vaddr=40374000 size=0a5dch ( 42460) load
I (125) esp_image: segment 3: paddr=00040020 vaddr=42000020 size=a72e4h (684772) map
I (251) esp_image: segment 4: paddr=000e730c vaddr=4037e5dc size=0d074h ( 53364) load
I (263) esp_image: segment 5: paddr=000f4388 vaddr=50000000 size=00020h (    32) load
I (273) boot: Loaded app from partition at offset 0x10000
I (273) boot: Disabling RNG early entropy source...
I (283) cpu_start: Multicore app
I (291) cpu_start: GPIO 44 and 43 are used as console UART I/O pins
I (292) cpu_start: Pro cpu start user code
I (292) cpu_start: cpu freq: 160000000 Hz
I (294) app_init: Application information:
I (297) app_init: Project name:     uart_echo_rs485
I (302) app_init: App version:      1
I (305) app_init: Compile time:     Sep 29 2026 17:38:20
I (310) app_init: ELF file SHA256:  44b6d4a39...
I (315) app_init: ESP-IDF:          v5.5.5
I (319) efuse_init: Min chip rev:     v0.0
I (322) efuse_init: Max chip rev:     v0.99 
I (326) efuse_init: Chip rev:         v0.2
I (330) heap_init: Initializing. RAM available for dynamic allocation:
I (337) heap_init: At 3FCA48F8 len 00044E18 (275 KiB): RAM
I (342) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (347) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (352) heap_init: At 600FE000 len 00001FE8 (7 KiB): RTCRAM
I (358) spi_flash: detected chip: generic
I (361) spi_flash: flash io: dio
I (364) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (370) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (377) main_task: Started on CPU0
I (407) main_task: Calling app_main()
I (437) pp: pp rom version: e7ae62f
I (437) net80211: net80211 rom version: e7ae62f
I (447) wifi:wifi driver task: 3fced510, prio:23, stack:6656, core=0
I (457) wifi:wifi firmware version: b9f67df
I (457) wifi:wifi certification version: v7.0
I (457) wifi:config NVS flash: enabled
I (457) wifi:config nano formatting: disabled
I (457) wifi:Init data frame dynamic rx buffer num: 32
I (467) wifi:Init static rx mgmt buffer num: 5
I (467) wifi:Init management short buffer num: 32
I (477) wifi:Init dynamic tx buffer num: 32
I (477) wifi:Init static tx FG buffer num: 2
I (487) wifi:Init static rx buffer size: 1600
I (487) wifi:Init static rx buffer num: 10
I (487) wifi:Init dynamic rx buffer num: 32
I (497) wifi_init: rx ba win: 6
I (497) wifi_init: accept mbox: 6
I (497) wifi_init: tcpip mbox: 32
I (507) wifi_init: udp mbox: 6
I (507) wifi_init: tcp mbox: 6
I (507) wifi_init: tcp tx win: 5760
I (517) wifi_init: tcp rx win: 5760
I (517) wifi_init: tcp mss: 1440
I (517) wifi_init: WiFi IRAM OP enabled
I (517) wifi_init: WiFi RX IRAM OP enabled
W (527) wifi:Password length matches WPA2 standards, authmode threshold changes from OPEN to WPA2
I (537) phy_init: phy_version 712,87e8c20e,Apr 13 2026,18:51:10
I (577) wifi:mode : sta (14:c1:9f:ce:c8:e8)
I (577) wifi:enable tsf
I (577) WIFI: WiFi STA initialized
I (577) WIFI: WiFi started, connecting...
I (587) wifi:new:<6,0>, old:<1,0>, ap:<255,255>, sta:<6,0>, prof:1, snd_ch_cfg:0x0
I (587) wifi:state: init -> auth (0xb0)
I (1357) wifi:state: auth -> assoc (0x0)
I (2357) wifi:state: assoc -> init (0x400)
W (2367) WIFI: WiFi disconnected, reconnecting...
I (4657) wifi:state: init -> auth (0xb0)
I (4687) wifi:state: auth -> assoc (0x0)
I (4737) wifi:state: assoc -> run (0x10)
I (4767) wifi:connected with Wangluolaji, aid = 9, channel 6, BW20, bssid = ba:2c:6e:3f:50:a9
I (4767) wifi:security: WPA3-SAE H2E, phy: bgn, rssi: -44, cipher(pairwise:0x3, group:0x3), pmf:1
I (4777) wifi:pm start, type: 1

I (4777) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (4787) wifi:set rx beacon pti, rx_bcn_pti: 0, bcn_timeout: 25000, mt_pti: 0, mt_time: 10000
I (4807) wifi:<ba-add>idx:0 (ifx:0, ba:2c:6e:3f:50:a9), tid:0, ssn:1, winSize:64
I (4867) wifi:dp: 2, bi: 102400, li: 4, scale listen interval from 307200 us to 409600 us
I (4867) wifi:AP's beacon interval = 102400 us, DTIM period = 2
I (5817) esp_netif_handlers: sta ip: 10.200.187.242, mask: 255.255.255.0, gw: 10.200.187.133
I (5817) WIFI: WiFi connected
I (5817) WIFI: IP address: 10.200.187.242
I (5817) MAIN: Starting MQTT...
I (5817) TIME_SYNC: Initializing SNTP
I (5827) TIME_SYNC: Waiting for system time...
I (7827) TIME_SYNC: Waiting for system time...
I (9827) TIME_SYNC: Time synchronized
I (9827) MODBUS: UART initialized

========== MODBUS READ ==========
TX: 01 04 00 02 00 04 50 09 
I (9827) main_task: Returned from app_main()
I (9877) MQTT: MQTT connected
RX LEN = 13
RX: 01 04 08 41 D1 C0 F4 42 47 D2 8E F8 80 
Device 1 | 26.22 C | 49.96 %RH | 9450 ms
I (9887) MQTT: Topic:gateway/device/1/data
I (9887) MQTT: Publish:{"device":1,"temperature":26.22,"humidity":49.96,"time":"2026-10-02 15:19:37"}

========== MODBUS READ ==========
TX: 02 04 00 02 00 04 50 3A 
E (10507) MODBUS: Modbus timeout: no valid response
E (10507) TEMP_HUMI: Sensor read failed

========== MODBUS READ ==========
TX: 01 04 00 02 00 04 50 09 
RX LEN = 13
RX: 01 04 08 41 D1 E1 C4 42 47 9E FE 8B 11 
I (12657) MAIN: Device 1 initial state: ONLINE
Device 1 | 26.24 C | 49.91 %RH | 12230 ms
I (12657) MQTT: Topic:gateway/device/1/data
I (12657) MQTT: Publish:{"device":1,"temperature":26.24,"humidity":49.91,"time":"2026-10-02 15:19:40"}

========== MODBUS READ ==========
TX: 02 04 00 02 00 04 50 3A 
E (13277) MODBUS: Modbus timeout: no valid response
E (13277) TEMP_HUMI: Sensor read failed

========== MODBUS READ ==========
TX: 01 04 00 02 00 04 50 09 
RX LEN = 13
RX: 01 04 08 41 D1 69 74 42 47 62 0E 95 07 
Device 1 | 26.18 C | 49.85 %RH | 15000 ms
I (15427) MQTT: Topic:gateway/device/1/data
I (15427) MQTT: Publish:{"device":1,"temperature":26.18,"humidity":49.85,"time":"2026-10-02 15:19:43"}

========== MODBUS READ ==========
TX: 02 04 00 02 00 04 50 3A 
E (16047) MODBUS: Modbus timeout: no valid response
E (16047) TEMP_HUMI: Sensor read failed
W (16047) MAIN: Device 2 initial state: OFFLINE

========== MODBUS READ ==========
TX: 01 04 00 02 00 04 50 09 
RX LEN = 13
RX: 01 04 08 41 D1 84 CC 42 47 3C 8E 1B A1 
Device 1 | 26.19 C | 49.81 %RH | 17770 ms
I (18197) MQTT: Topic:gateway/device/1/data
I (18197) MQTT: Publish:{"device":1,"temperature":26.19,"humidity":49.81,"time":"2026-10-02 15:19:46"}

========== MODBUS READ ==========
TX: 01 04 00 02 00 04 50 09 
RX LEN = 13
RX: 01 04 08 41 D1 A0 24 42 47 4A 9E 5A FE 
Device 1 | 26.20 C | 49.82 %RH | 19920 ms
I (20347) MQTT: Topic:gateway/device/1/data
I (20347) MQTT: Publish:{"device":1,"temperature":26.20,"humidity":49.82,"time":"2026-10-02 15:19:48"}

========== MODBUS READ ==========
TX: 01 04 00 02 00 04 50 09 
RX LEN = 13
RX: 01 04 08 41 D1 C0 F4 42 47 58 AE 9E 38 
Device 1 | 26.22 C | 49.84 %RH | 22070 ms
I (22497) MQTT: Topic:gateway/device/1/data
I (22497) MQTT: Publish:{"device":1,"temperature":26.22,"humidity":49.84,"time":"2026-10-02 15:19:50"}

========== MODBUS READ ==========
TX: 01 04 00 02 00 04 50 09 
RX LEN = 13
RX: 01 04 08 41 D1 84 CC 42 47 8C 3E 6F D5 
Device 1 | 26.19 C | 49.89 %RH | 24220 ms
I (24647) MQTT: Topic:gateway/device/1/data
I (24647) MQTT: Publish:{"device":1,"temperature":26.19,"humidity":49.89,"time":"2026-10-02 15:19:52"}

========== MODBUS READ ==========
TX: 01 04 00 02 00 04 50 09 
RX LEN = 13
RX: 01 04 08 41 D1 A0 24 42 47 81 4E 0C 52 
Device 1 | 26.20 C | 49.88 %RH | 26370 ms
I (26797) MQTT: Topic:gateway/device/1/data
I (26797) MQTT: Publish:{"device":1,"temperature":26.20,"humidity":49.88,"time":"2026-10-02 15:19:54"}