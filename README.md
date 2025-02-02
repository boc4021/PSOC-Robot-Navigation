# Line Following Robot with IR Sensors FPGA Project

## Project Overview
This project implements a line following robot using a PSOC (Programmable System-on-Chip) architecture with FPGA hardware and software control. The system uses both line detection sensors and IR sensors for navigation and obstacle detection.

## Hardware Architecture
The hardware architecture of the system is implemented on a Zynq-based FPGA, as shown in the block diagram. The main components include:
![diag](https://github.com/user-attachments/assets/d4de9e29-0fea-43dd-be81-f16ae77b805f)

- **Processing System (ps7.0_axi_periph)**: Central Zynq processing system
- **Motor Controllers (this is an IP that was developped in VHDL )**:
  - mycontroller2111_1: Right motor control (Mot_Dir_D, Mot_PWM_D outputs)
  - mycontroller2111_0: Left motor control (Mot_Dir_G, Mot_PWM_G outputs)
- **Sensor Interface (Recup_donnee_0)**:
  - Digital inputs for line sensors (Ligne_D, Ligne_M,Ligne_G)
- **XADC Interface**: For analog IR sensor readings
- **Clock Management**: Utility Vector Logic for clock distribution
- **System Reset Module (rst_ps7_0_50M)**: For system initialization and reset control

The architecture features AXI interconnect for communication between the processing system and the custom IP cores, ensuring efficient data transfer and control.

## Hardware Components
- FPGA board with Xilinx ZynqcZ720 architecture
- Line detection sensors (digital input)
- IR sensors (analog input via XADC)
- Dual motor control system
- Interrupt-capable GPIO pins for emergency stops

## Software Architecture

### Key Features
- Dual sensor system implementation:
  - Digital line detection for path following
  - Analog IR sensors for proximity detection
- Real-time motor control with variable speed
- Interrupt-driven collision avoidance
- XADC configuration for analog sensor reading

### Main Control Components
1. **Motor Control**
   - Independent left and right motor control
   - Variable speed control (0-200 speed units)
   - Functions for movement:
     - Forward motion
     - Turning (left/right)
     - Emergency stop
     - Reverse motion

2. **Sensor Systems**
   - Line Detection:
     - 3-bit digital input (0x1 through 0x6)
     - Path deviation detection
   - IR Sensors:
     - Dual XADC channels (AUX07 and AUX14)
     - Proximity threshold detection
     - Differential readings for direction determination

3. **Interrupt System**
   - Dual interrupt channels (IRQ_F2P[0:0] and IRQ_F2P[1:1])
   - Priority-based interrupt handling
   - Emergency stop and reverse functionality

## Implementation Steps

### 1. FPGA Hardware Setup
1. Design the hardware system in Vivado as shown in the architecture diagram
2. Configure the following IP cores:
   - XADC for analog sensor input
   - AXI interfaces for motor control
   - Interrupt controller
   - GPIO for sensor inputs
3. Export the hardware as XSA file

### 2. Vitis Software Implementation
1. Import the XSA file into Vitis
2. Configure the software project with necessary libraries:
   - xsysmon.h for XADC
   - xscugic.h for interrupt controller
   - platform-specific headers

### 3. System Configuration
```c
// Base Configuration
- XADC channels: AUX07 (right sensor), AUX14 (left sensor)
- Interrupt Priorities: 
  - IRQ_F2P[0:0]: 0xA0
  - IRQ_F2P[1:1]: 0xA8
```

## Control Algorithm

### Main Control Loop
1. Check line sensor status
2. If line detected:
   - Follow line using proportional control
3. If line lost:
   - Use IR sensors for navigation
   - Compare left/right IR readings
   - Adjust direction based on stronger signal

### Navigation Logic
- Forward motion on clear path (0x5)
- Right turn on right deviation (0x3 or 0x1)
- Left turn on left deviation (0x6 or 0x4)
- IR-based navigation when no line detected
- Emergency stop and reverse on collision detection

## Setup Instructions

1. **Hardware Setup**
   - Load the FPGA bitstream
   - Connect sensors to appropriate pins
   - Verify power connections

2. **Software Setup**
   - Import project into Vitis
   - Configure build settings
   - Compile and load the program

3. **System Initialization**
   - Initialize XADC
   - Configure interrupt system
   - Verify sensor readings

## Troubleshooting

### Common Issues
1. Sensor Reading Errors
   - Check XADC configuration
   - Verify sensor connections
   - Calibrate threshold values

2. Motor Control Issues
   - Verify AXI interface connections
   - Check motor driver connections
   - Test individual motor functions

3. Interrupt Problems
   - Verify interrupt controller initialization
   - Check priority settings
   - Test interrupt triggers

## Performance Optimization
- Adjust speed values for smooth operation
- Fine-tune IR threshold values
- Calibrate turning parameters
- Modify interrupt priorities if needed

## Safety Features
- Collision detection and avoidance
- Prioritized interrupt handling

## Future Improvements
- Add PI control for smoother line following
- Implement speed optimization
- Enhance obstacle avoidance algorithms

