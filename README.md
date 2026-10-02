# ESP32 Industrial Gateway

---

# 中文版

## 项目简介

ESP32 Industrial Gateway 是一个基于 ESP32-S3 的工业物联网数据采集网关项目。

本项目模拟真实工业现场的数据采集与传输流程，实现从工业传感器数据采集、通信协议转换、MQTT 数据上传、服务器数据存储，到 Web 可视化监控的完整物联网系统。

系统通过 RS485 Modbus RTU 协议连接工业传感器，ESP32-S3 作为边缘网关完成现场数据采集与处理，并通过 MQTT 协议将数据发送至服务器。

服务器端基于 Python 开发，实现 MQTT 数据接收、JSON 数据解析、SQLite 数据库存储以及设备在线状态检测。

同时搭建 Flask Web Dashboard，实现温湿度实时监测、历史数据查询以及多设备状态管理。

该项目完整模拟了工业 IoT 网关的典型架构。


---

# 系统架构

工业传感器
    |
    |
RS485 Modbus RTU
    |
    |
ESP32-S3 工业网关
    |
    |
MQTT 通信协议
    |
    |
Python 数据服务器
    |
    |
SQLite 数据库
    |
    |
Flask Web监控平台

![System Architecture](docs/images/architecture.png)

---

# 项目功能


## ESP32-S3 固件

- 基于 ESP32-S3 开发
- 使用 ESP-IDF 5.5.5 开发框架
- FreeRTOS 多任务架构
- RS485 Modbus RTU 数据采集
- 工业传感器通信
- MQTT 客户端通信
- 多设备数据支持


---

## 服务器端

- MQTT Broker 数据接收
- JSON 数据解析
- SQLite 数据存储
- 设备在线/离线检测
- 多设备状态管理
- 历史数据查询


---

## Web监控平台

- 实时温度显示
- 实时湿度显示
- 历史数据表格
- 实时数据曲线
- 自动刷新
- 设备在线状态显示


---

# 硬件平台


## 主控

- ESP32-S3-N16R8


## 通信模块

- RS485 转换模块
- USB-RS485 调试模块


## 传感器

- Modbus RTU 温湿度传感器


---

# 软件环境


## 嵌入式端

- ESP-IDF v5.5.5
- FreeRTOS
- C语言


## 服务器端

- Python
- MQTT
- Flask
- SQLite


## 前端

- HTML
- JavaScript
- Chart.js


---

# 项目目录

ESP32-Industrial-Gateway
├── firmware
│   └── ESP32-S3 firmware
│
├── server
│   ├── MQTT receiver
│   ├── Database management
│   └── Flask server
│
├── web
│   └── Web dashboard
│
├── docs
│   └── Documentation
│
├── requirements.txt
│
└── README.md

---

# 项目演示


目前系统已经实现：

- RS485 Modbus RTU 通信
- 温湿度数据采集
- MQTT 数据上传
- 数据库存储
- Web实时监控
- 多设备在线状态检测


---

# 后续计划


- 云端 MQTT Broker 部署
- 设备远程管理
- OTA 固件升级
- 更多工业协议支持
- 数据可视化优化



---


# English Version


## Project Introduction


ESP32 Industrial Gateway is an industrial IoT gateway project based on ESP32-S3.

This project simulates a real industrial data acquisition system, including sensor communication, protocol conversion, MQTT transmission, database storage, and web-based monitoring.


The gateway collects industrial sensor data through RS485 Modbus RTU protocol.

ESP32-S3 works as an edge gateway to process field data and transmit measurements through MQTT communication.

The server side is developed with Python, providing MQTT data receiving, JSON parsing, SQLite database storage, and device online/offline monitoring.

A Flask-based Web Dashboard is implemented for real-time monitoring, historical data visualization, and multi-device management.


This project demonstrates a complete architecture of an industrial IoT gateway.


---

# System Architecture

Industrial Sensor
        |
        |
RS485 Modbus RTU
        |
        |
ESP32-S3 Industrial Gateway
        |
        |
MQTT Communication
        |
        |
Python Server
        |
        |
SQLite Database
        |
        |
Flask Web Dashboard


---

# Features


## ESP32-S3 Firmware

- ESP32-S3 based embedded gateway
- ESP-IDF v5.5.5 development framework
- FreeRTOS multi-task architecture
- RS485 Modbus RTU communication
- Industrial sensor data acquisition
- MQTT client communication
- Multi-device support


---

## Server

- MQTT message receiving
- JSON data processing
- SQLite database storage
- Device online/offline detection
- Multi-device management
- Historical data query


---

## Web Dashboard

- Real-time temperature monitoring
- Real-time humidity monitoring
- Historical data display
- Data visualization
- Automatic refresh
- Device status monitoring


---

# Hardware


## MCU

- ESP32-S3-N16R8


## Communication

- RS485 interface module
- USB-RS485 converter


## Sensor

- Modbus RTU temperature and humidity sensor


---

# Software Stack


## Embedded System

- ESP-IDF v5.5.5
- FreeRTOS
- C language


## Backend

- Python
- MQTT
- Flask
- SQLite


## Frontend

- HTML
- JavaScript
- Chart.js


---

# Project Structure

ESP32-Industrial-Gateway
├── firmware
│   └── ESP32-S3 firmware
│
├── server
│   ├── MQTT receiver
│   ├── Database management
│   └── Flask server
│
├── web
│   └── Web dashboard
│
├── docs
│   └── Documentation
│
├── requirements.txt
│
└── README.md


---

# Demo


The system currently supports:

- RS485 Modbus RTU communication
- Sensor data acquisition
- MQTT data transmission
- Database storage
- Web-based monitoring
- Multi-device online status detection


---

# Future Improvements


- Cloud MQTT deployment
- Remote device management
- OTA firmware update
- More industrial communication protocols
- Advanced data visualization

