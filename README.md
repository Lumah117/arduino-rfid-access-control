# Arduino RFID Access Control System

An Arduino-based electronic access-control system developed during my university studies.

The project combines RFID identification with keypad PIN authentication to create a two-stage access-control process. A servo motor operates the physical locking mechanism, while an I²C LCD and status LEDs provide feedback to the user.

## Project Overview

The system was designed to require two authentication stages before granting access:

```text
             User
               |
               v
          Scan RFID Tag
               |
        +------+------+
        |             |
    Authorised     Unknown
        |             |
        v             v
    Request PIN   ACCESS DENIED
        |
        v
     Enter PIN
        |
        v
   Validate PIN
        |
   +----+----+
   |         |
Correct   Incorrect
   |         |
   v         v
 Unlock     Locked
```

An authorised RFID tag alone is therefore not intended to provide access. The user must also provide the correct PIN through the keypad.

## Technologies

- Arduino
- Embedded C/C++
- RFID
- SPI communication
- I²C communication
- Matrix keypad input
- Servo control
- LCD user interface
- LED status indication

## Repository Structure

```text
arduino-rfid-access-control/
│
├── README.md
├── LICENSE
│
└── src/
    └── rfid_access_control.ino
```

## Hardware

The project integrates:

- Arduino-compatible microcontroller
- RFID reader
- RFID cards/tags
- 4×4 matrix keypad
- Servo motor
- 20×4 I²C LCD
- Red status LED
- Green status LED
- Associated wiring and power connections

## Software Libraries

The implementation uses several Arduino libraries:

```cpp
#include <SPI.h>
#include <RFID.h>
#include <Servo.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
```

This allowed multiple peripherals using different communication and control mechanisms to operate within the same embedded application.

## RFID Authentication

The RFID reader communicates with the Arduino using SPI.

When a card or tag is detected, the controller reads its identifier and compares it against the configured list of authorised tags.

Conceptually:

```text
RFID Tag
   |
   v
RFID Reader
   |
   | SPI
   v
Arduino
   |
   v
Read Tag Identifier
   |
   v
Compare Against
Authorised Tags
   |
+--+--+
|     |
v     v
Match No Match
|       |
v       v
PIN   Denied
```

The public portfolio version uses placeholder RFID identifiers rather than the original prototype credentials.

## Two-Stage Authentication

A recognised RFID tag initiates the second authentication stage.

The LCD displays a request for the user to enter a PIN using the 4×4 keypad.

Only after successful authentication is the locking actuator commanded to the unlocked position.

This provides a simple example of multi-stage authentication:

```text
Something the user has
       RFID Tag
           +
Something the user knows
          PIN
           |
           v
      Access Granted
```

## Keypad

The project uses a 4×4 matrix keypad:

```text
1   2   3   A
4   5   6   B
7   8   9   C
*   0   #   D
```

The keypad is connected using four row and four column lines and configured through the Arduino `Keypad` library.

The original prototype used a three-digit PIN.

The credential shown in the public source is an example value and should be changed before use.

## Servo Lock

A servo motor represents the physical locking mechanism.

The software defines separate positions corresponding to:

```text
LOCKED
   |
   v
Servo Position
   |
   | Successful authentication
   v
UNLOCKED
```

The lock state is also tracked in software so that the controller can determine whether the mechanism is currently locked or unlocked.

## User Feedback

The system provides feedback through several interfaces.

### LCD

The 20×4 I²C LCD displays messages including:

```text
PLEASE SCAN YOUR TAG

RFID CARD ACCEPTED

ACCESS DENIED

PLEASE ENTER PIN
```

### Status LEDs

Red and green LEDs indicate lock and authentication status.

Conceptually:

```text
RED LED    -> Locked / denied
GREEN LED  -> Unlocked / accepted
```

LED sequences are also used to provide visual feedback following RFID authentication attempts.

### Serial Monitor

The serial interface is used during development for:

- RFID tag identification
- Authentication feedback
- Troubleshooting

The serial connection operates at 9600 baud.

## Communication Interfaces

One useful aspect of this project was integrating several different hardware interfaces within the same application.

```text
                    Arduino
                       |
       +---------------+---------------+
       |               |               |
       v               v               v
      SPI             I²C            GPIO
       |               |               |
       v               v          +----+----+
 RFID Reader          LCD         |         |
                              Keypad      LEDs
                                   \
                                    \
                                    Servo
```

This required coordinating sensing, user input, display output and physical actuation within a single embedded program.

## Control Sequence

The intended operating sequence is:

```text
START
  |
  v
Initialise Hardware
  |
  v
Lock Door
  |
  v
Display RFID Prompt
  |
  v
Wait for RFID Tag
  |
  v
Read Tag Identifier
  |
  +-------- Invalid --------> ACCESS DENIED
  |
 Valid
  |
  v
Display PIN Prompt
  |
  v
Read Keypad Input
  |
  v
Validate PIN
  |
  +-------- Incorrect ------> Remain Locked
  |
 Correct
  |
  v
Unlock Servo
  |
  v
Access Granted
```

## Concepts Demonstrated

This project provided practical experience with:

- Embedded-system integration
- RFID identification
- Multi-stage authentication
- SPI communication
- I²C communication
- Matrix keypad scanning
- Servo actuation
- LCD interfacing
- Digital output
- State tracking
- User-interface feedback
- Conditional control logic
- Multiple Arduino libraries
- Hardware/software integration

## Original Implementation

The implementation in this repository is based on the original university project.

The authentication credentials have been replaced with example/placeholder values before publication.

Other than credential sanitisation, the source is retained as an example of my embedded-programming ability at the time rather than rewritten to reflect my current software-development practices.

## Retrospective

This project represented a substantial increase in embedded-system integration compared with my earlier Arduino work.

Rather than controlling a single peripheral, the application coordinates multiple input, output and communication devices to implement an overall system behaviour.

Reviewing the implementation with my current embedded and robotics experience highlights several areas I would approach differently today.

### Authentication State Machine

The original implementation distributes authentication behaviour across the main loop and several functions.

A modern implementation would use an explicit finite-state machine:

```text
WAITING_FOR_TAG
       |
       v
TAG_AUTHENTICATED
       |
       v
WAITING_FOR_PIN
       |
       v
ACCESS_GRANTED
       |
       v
UNLOCKED
       |
       v
LOCKED
```

This would make transitions and failure behaviour much clearer.

### PIN Input

The original `askForPin()` implementation reads a single keypad event during each call rather than maintaining a dedicated blocking or asynchronous PIN-entry sequence.

A more robust implementation would collect the complete PIN, support deletion/reset, enforce a maximum number of attempts and explicitly handle incorrect PINs.

### Credentials

Credentials are stored directly in the source code.

A production access-control system should not store authentication secrets as plain-text source constants.

Depending on the platform, credentials could instead be stored using protected non-volatile storage and compared using an appropriate secure representation.

### Servo Configuration

The source defines configured lock and unlock positions, although some later calls use literal servo angles.

A cleaner implementation would consistently use:

```cpp
lockPos
unlockPos
```

throughout the application.

### Non-Blocking Operation

Several visual-feedback sequences use `delay()`.

A more capable embedded controller would use non-blocking timing or a task/state-based architecture so that RFID detection, keypad input and other functionality can remain responsive.

### LCD Management

The LCD is repeatedly initialised during authentication events.

Initialisation should occur once during `setup()`, with later operations limited to clearing/updating the required display content.

### Security

This project was an educational prototype rather than a security-certified access-control product.

A real deployment would require additional consideration of:

- Credential storage
- RFID cloning/replay resistance
- Brute-force PIN protection
- Authentication attempt logging
- Lockout policies
- Tamper detection
- Fail-safe/fail-secure behaviour
- Power-loss behaviour
- Physical security of the actuator and controller

## Portfolio Context

This project demonstrates a clear progression in my embedded-systems work.

```text
Arduino Traffic Lights
        |
        | Digital I/O
        v
Ultrasonic Servo Controller
        |
        | Sensors + Actuation
        v
RFID Access Control
        |
        | Multiple peripherals
        | SPI + I²C
        | User interaction
        | Authentication logic
        | Physical actuation
        v
More Advanced Embedded /
Robotics Systems
```

It is particularly useful within my portfolio because it demonstrates the integration of several hardware and software components into a single system with a defined real-world purpose.
