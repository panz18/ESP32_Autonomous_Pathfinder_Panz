# ESP32_Autonomous_Pathfinder_Panz 🤖🛤️

**DEC50242 Embedded Robotic - Mini Project**  
**Politeknik Sultan Salahuddin Abdul Aziz Shah**

## Project Overview
This repository contains the source code and flowcharts for an **Autonomous Line Following Robot with Obstacle Avoidance and Line Reacquisition**. All control decisions are processed entirely by an ESP32 microcontroller without the need for a Single Board Computer (SBC). 

## Key Features
- **3-Channel Line Tracking:** Uses left, center, and right IR sensors for smooth navigation.
- **Turn Memory Algorithm:** Remembers the last known position of the line to recover from sharp 90° turns.
- **Obstacle Avoidance:** Uses an HC-SR04 ultrasonic sensor to detect obstacles (≤ 4cm), stop, and execute a timed detour sequence.
- **Line Reacquisition:** Automatically searches for and realigns to the path after bypassing an obstacle.
- **Auto-Stop (Finish):** Safely halts the motors when the finish marker is detected.

## Hardware Components
- ESP32 Development Board
- L298N Dual H-Bridge Motor Driver
- 5-Channel IR Sensor Array (3 channels utilized: S1, S2, S3)
- HC-SR04 Ultrasonic Sensor
- 2WD Differential Drive Robot Chassis
- 2-Cell Li-ion Battery Pack (7.4V)

## Files in this Repository
- `MINI_PROJECT_EMB_ROBTIC_FULL.ino` - The complete and final source code including line following, obstacle detour, and reacquisition.
- `MINI_PROJECT_EMB_ROBTIC.ino` - The earlier, basic version of the line-following code.
- `FLOWCHART_EMB_ROBOTIC` - System flowcharts and block diagrams.

## Circuit Pinout (ESP32)
| Component | Pin | Component | Pin |
| :--- | :--- | :--- | :--- |
| **IR_LEFT (S1)** | GPIO 5 | **L298N ENA** | GPIO 32 |
| **IR_CENTER (S2)**| GPIO 15 | **L298N IN1** | GPIO 33 |
| **IR_RIGHT (S3)** | GPIO 34 | **L298N IN2** | GPIO 25 |
| **HC-SR04 TRIG** | GPIO 4 | **L298N IN3** | GPIO 26 |
| **HC-SR04 ECHO** | GPIO 2 | **L298N IN4** | GPIO 27 |
| | | **L298N ENB** | GPIO 14 |

---
**Developers:** 
* Irffan Irsyad bin Mohd Harudin (08DEU24F1170) 
* Azrul Amrie bin Jamhari (08DEU24F1140)
