
# Design and Optimization of Energy-Efficient Static Wireless Charging System for Electric Vehicles

## Project Overview

This project presents the design and development of a static wireless charging system for Electric Vehicles (EVs). It uses transmitter and receiver coil PCBs to transfer electrical energy wirelessly, eliminating the need for a direct charging cable connection.

The system integrates ESP32 microcontrollers, sensors, an OLED display, and a Wi-Fi-based monitoring interface to monitor battery charging parameters.

## Objectives

1. Develop a transmitter–receiver coil system for wireless EV charging.
2. Monitor battery voltage, charging current, state of charge (SOC), and charging status.
3. Display charging information locally and through a website/mobile interface.
4. Incorporate relay-based charging control.

## System Architecture

The system consists of two main sections:

### 1. Transmitter – ESP32 #1
- 12 V, 5 A SMPS power supply.
- LM2596 DC-DC buck converter.
- Transmitter coil PCB.
- IRF540N MOSFET.
- Relay module.
- ESP32 #1 for transmitter-side control.

### 2. Receiver – ESP32 #2
- Receiver coil PCB.
- ACS712 current sensor.
- Voltage sensor.
- TP4056 charging module.
- 3.7 V lithium-ion battery.
- ESP32 #2 for monitoring and data processing.
- 1.3-inch SH1106 OLED display.

## Software and Tools

- MATLAB Simulink and Simscape Electrical
- Arduino IDE
- ESP32
- Visual Studio Code
- Wi-Fi and IoT communication
- Blynk IoT / Web-based monitoring interface

## Simulation Parameters

| Parameter | Value |
|---|---:|
| Input DC voltage | 48 V |
| Operating frequency | 85 kHz |
| Primary inductance | 200 µH |
| Secondary inductance | 200 µH |
| Compensation capacitance | 17.5 nF |
| Coupling coefficient | 0.85 |

## Prototype Monitoring Results

The prototype monitoring interface displayed the following readings during testing:

| Parameter | Observed Value |
|---|---:|
| Charging Status | CHARGING |
| Battery SOC | 74% |
| Battery Voltage | 3.94 V |
| Charging Current | 0.10 A |
| Charging Power | 0.40 W |

Charging power is calculated using:

P = V × I

## Repository Structure

```text
wireless-ev-charging-system/
│
├── ESP32_Code/
│   ├── ESP32_1_Transmitter.ino
│   ├── ESP32_2_Receiver.ino
│   └── README.md
│
└── README.md
```

## Applications

- Wireless charging for electric vehicles.
- Residential EV charging systems.
- Smart parking infrastructure.
- Electric scooters and two-wheelers.
- IoT-based battery monitoring.

## Future Scope

- Improve wireless power transfer efficiency.
- Optimize transmitter and receiver coil alignment.
- Enhance battery monitoring accuracy.
- Improve IoT dashboard functionality.
- Validate and improve automatic charging control.
- Extend the system for higher-power EV applications.

## Project Domain

Electrical and Electronics Engineering | Electric Vehicles | Wireless Power Transfer | Embedded Systems | Internet of Things

## Institution

Department of Electronics and Communication Engineering  
Global Academy of Technology, Bengaluru, Karnataka, India.
