# WOEMpet - Automated Pet Feeder System

An automated pet feeder with programmable feeding times using the LPC2378 microcontroller. This project combines real-time scheduling, pet detection via ultrasonic sensing, and automated food dispensing with servo motor control.

## Overview

WOEMpet is an embedded systems solution that monitors pet presence, validates feeding times, and automatically dispenses food at programmed intervals using PWM-controlled servo actuation. The system integrates multiple hardware interfaces including sensors, actuators, and communication protocols to create an intelligent feeding system.

## What It Does

- **Programmable Feeding Schedule**: Set specific feeding times at which food is to be dispensed
- **Pet Detection**: Ultrasonic HC-SR04 sensor monitors for pet presence within configurable buffer intervals
- **Automatic Dispensing**: PWM-controlled MG995 servo motor actuates the feeder mechanism
- **Real-Time Display**: 4-bit LCD displays current time, distance, and system status
- **RFID Identification**: PN532 RFID reader supports optional pet card identification
- **Timing Validation**: Ensures pets are present before dispensing food

## Hardware

The following major components are used:

- **NXP LPC2378** - ARM Cortex-M3 32-bit microcontroller
  - 12 MHz XTAL with PLL → 48 MHz processor clock
  - 12 MHz peripheral clock
  - Flash-based program storage
  
- **Tower Pro MG995 Servo Motor** - PWM-controlled food dispenser
  - 50 Hz frequency (20ms period)
  - Duty cycle control: 5% (closed) to 10% (fully open)
  
- **HC-SR04 Ultrasonic Sensor** - Pet presence detection
  - GPIO-based trigger and echo measurement
  - Timer1 microsecond-precision measurements
  - Configurable detection range (1-30 cm)
  
- **PN532 RFID Module** - Optional pet identification (SPI interface)
  - ISO14443A card polling
  - UID extraction and validation

- **4-bit LCD Display** - System feedback
  - Shows current time (HH:MM:SS)
  - Displays distance measurements
  - Status indicators

## Software Architecture

The program is organized into modular components with self-explanatory names:

### Core Modules

**main.c** - Main feeding control logic
- Implements the core feeding algorithm
- Coordinates sensor inputs and actuator outputs
- Manages feeding schedule and pet presence validation

**RTC.c** - Real-Time Clock (Timer0-based)
- 1ms interrupt tick generation
- Maintains hour, minute, second counters
- Displays formatted time on LCD

```c
// Timer0: 1ms interrupts at 12 MHz peripheral clock
T0MR0 = 11999;  // 1msec interval
T0MCR = 3;      // Interrupt and Reset on Match
```

**Servo_MG995.c** - PWM Servo Control
- 50 Hz PWM frequency (240kHz base timer)
- Duty cycle mapping:
  - 5% = 0° (closed/dispenser off)
  - 7.5% = 90° (opening position)
  - 10% = 180° (fully open)

```c
PWM1MR0 = 240000;                    // 20ms period (50 Hz)
PWM1MR1 = (dutyCycle * PWM1MR0) / 100; // Set pulse width
```

**Ultrasonic_HC_SR04.c** - Distance Sensor Driver
- Timer1-based microsecond precision timing
- 10µs trigger pulse on P0.2
- Echo measurement on P0.3
- Distance formula: `distance = (0.0343 * echoTime) / 2`

```c
sendPulse(n);                 // Generate trigger pulse
distance = calDistance(m);    // Measure echo time and calculate
```

**RFID_PN532.c** - RFID Card Detection (SPI)
- SPI0 communication at 7.5 MHz
- InListPassiveTarget command for card polling
- UID extraction and checksum validation
- (Code present but optional integration)

**LCD_4bit.c** - 4-bit LCD Display Driver
- GPIO-based parallel interface
- Character display and cursor control
- Formatting utilities for time/distance output

**IRQ.c** - Interrupt Handler
- Timer0 interrupt service routine
- Manages 1ms clock tick generation

**LPC2300.s** - ARM Cortex-M3 Startup Code
- Vector table initialization
- Hardware setup sequence
- Entry point to main()

## Operating Parameters

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Feeding Start Time** | Configurable (minutes) | Default begins at 00:00 |
| **Schedule Window** | 30 seconds | Buffer interval for pet presence detection |
| **Detection Range** | 1 - 30 cm | Valid distance for pet recognition |
| **Servo Active Duration** | ~2 seconds | Time dispenser remains open |
| **Feed Release Interval** | 1 minute | Minimum time between successive feedings |
| **LCD Update Rate** | 1 second | Display refresh interval |

## How It Works

The main feeding algorithm operates in a continuous loop with 1-second intervals:

```
Loop every second:
  1. Update RTC counter (increment seconds)
  2. Send ultrasonic trigger pulse to HC-SR04
  3. Calculate distance to pet (echo time measurement)
  
  4. Check if current time matches feeding schedule
  
  If feeding_time AND pet_present (distance < 30cm):
    - Capture current second as dispense timestamp
    - Set servo to 7.5% duty cycle (open position)
    - Mark food as dispensed
  
  If 2 seconds have elapsed since dispense:
    - Return servo to 5% duty cycle (closed)
    - Reset dispense flag
  
  Update LCD Display:
    - Current distance and release time counter
    - Current time in HH:MM:SS format
    - Clear and refresh for next cycle
```

## Building & Deployment

### Prerequisites
- Keil MDK (µVision 4+) or equivalent ARM compiler
- LPC2378 JTAG/Flash programmer
- Target board: MCB2300 Evaluation Board (or compatible)

### Compilation
The project uses Keil µVision project files:
- `ADC.Uv2` - Project workspace
- `ADC.Opt` - Project options and compiler settings
- `LPC2300.s` - Startup assembly code with interrupt vectors

### Program Directory

```
WOEMpet/
├── main.c                    # Main feeding control logic
├── RTC.c                     # Real-time clock and timer interrupt
├── Ultrasonic_HC_SR04.c     # Distance sensor driver
├── Servo_MG995.c            # PWM servo control
├── RFID_PN532.c             # RFID SPI communication
├── LCD_4bit.c               # 4-bit LCD display driver
├── LCD.h                    # LCD interface header
├── IRQ.c                    # Timer interrupt handler
├── Serial.c                 # UART debug interface
├── LPC2300.s                # ARM Cortex-M3 startup code
└── Build files              # .hex, .axf, .map, etc.
```

### Flashing
```bash
# Build project in Keil MDK
Build → Rebuild all

# Program MCU
Project → Download → Program current target device
```

## Key Technical Features

✅ **Interrupt-Driven RTC** - 1ms timer ticks for precise second-level scheduling  
✅ **Pet Presence Validation** - Ultrasonic distance measurement before food release  
✅ **PWM Servo Control** - Smooth, precise actuation with configurable duty cycles  
✅ **Real-Time Display** - LCD shows distance, time, and feeding status  
✅ **Modular Drivers** - Independent hardware interface modules  
✅ **Microsecond Precision** - Timer1-based ultrasonic echo measurements  
✅ **SPI Communication** - Optional RFID integration for pet identification  

## Technologies & Topics

`embedded-systems` • `automated-feeding` • `lpc2378` • `servo-motor` • `hc-sr04` • `pet-feeder` • `pwm` • `rtos` • `arm-cortex-m3`

## License

Unspecified

## Author

InvincibleJuggernaut

---

*This project demonstrates integration of multiple embedded systems concepts including interrupt handling, PWM control, sensor interfacing, and real-time scheduling on an ARM microcontroller.*
