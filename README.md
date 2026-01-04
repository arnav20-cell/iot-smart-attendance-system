# iot-smart-attendance-system
ESP32-based IoT attendance system with LCD, keypad, persistent memory, and local web dashboard
# IoT-Enabled Smart Attendance System (ESP32)

This project is a mini IoT-based attendance system built using the ESP32.

## Features
- Keypad-based attendance input
- 16x2 I2C LCD for real-time feedback
- Persistent data storage using ESP32 flash memory
- Local Wi-Fi web dashboard (no cloud required)
- Admin-controlled data reset

## Components Used
- ESP32 DevKit V1
- 16x2 I2C LCD
- 4x4 Matrix Keypad
- Buzzer
- Breadboard & jumper wires

## How It Works
1. User enters an ID via keypad
2. ESP32 validates and stores attendance
3. Status shown on LCD
4. Attendance data displayed on a local web dashboard
5. Data retained even after restart

## Demo
Access the dashboard after connecting to ESP32 Wi-Fi:
