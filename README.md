# Wireless-Timing-Gates

This is repository contains the code to run the timing gate and timing gate receiver modules for wireless timing during testing. These gates are easy to deploy and have functionality for lap timing (circuit and point to point), sector timing, and instantaneous speed detection.

## Basic Setup

A conventional setup with these gates would start and finish lines, sectors if desired, and a timing gate receiver. 

### Start and Finish Lines
Start lines begin timing a lap, and finish lines end timing a lap. Every timing gate can be configured to be a Start gate, a Finish gate, or a Start/Finish gate. Two separate gates would be used in an acceleration test run, where the start and finish are in two different locations. A single Start/Finish gate would be used for an endurance test where a circuit is being completed each lap.

### Sector Gates
Sector gates grant the ability to measure durations of specific regions of a lap or instantaneous speed at certain parts of the lap. Sector numbers should count up from 1. For example, if the lap was to be segmented into 3 sectors, 2 gates would be needed to mark the end of sector 1 and the end of sector 2. Using sector 3 and 6 wouldn't work.

### Timing Gate Receiver
A Timing Gate Receiver is receives the signals from each timing gate. For best range performance and to display the number to the driver, the Timing Gate Receiver is best placed on the car itself.


## Functionality Overview

### Wireless Communication

These gates use the ESPNOW protocol to wirelessly transmit data from a triggered gate on the ground to the recording module on the car. There is a time clock running on the car module which records the time when it receives a trigger from the gate module. Once the triggered pulse is sent, the gate module also times between gate 1 and gate 2 connected to the module. This time difference can be used to solve for max speed, which is then sent as a value to the car module to be recorded. 

### Functional Purpose of Gate Number

Gate | Purpose
--- | ---
0 | Resets the lap time to 0 seconds
1-7 | Sends impuse to car module to mark sector time
8 | Ends and records lap time counter*
9 | Ends and records lap time counter and resets lap time to 0 seconds
 

All gates also detect speed.

## Speed Detection Setup

When each gate is hit, it measures the time between the front tire and rear tire going over the gate. This time difference is sent to the timing gate receiver, which scales it based on that car's wheelbase and a conversion factor. For more accurate results the gates should be hit dead on, not during a turn. 
