# Wireless-Timing-Gates

This is repository contains the code to run the gate-side and car-side modules for wireless timing during testing. These gates are easy to deploy and have functionality for lap timing, sector timing, and instantaneous speed detection.

## Functionality Overview

### Wireless Communication

These gates use the ESPNOW protocol to wirelessly transmit data from a triggered gate on the ground to the recording module on the car. There is a time clock running on the car module which records the time when it receives a trigger from the gate module. Once the triggered pulse is sent, the gate module also times between gate 1 and gate 2 connected to the module. This time difference can be used to solve for max speed, which is then sent as a value to the car module to be recorded. 

### Functional Purpose of Gate Number

Gate | Purpose
--- | ---
0 | Resets the lap time to 0 seconds
1-8 | Sends impuse to car module to mark sector time
9 | Ends and records lap time counter*

*If 9 is not pressed, 0 will record and restart the count 

All gates are capable of running with or without a speed detector

## Speed Detection Setup

If running gates without speed detection, only connect a gate to the Gate 1 connector. If speed detection is desired, simply add a second gate to the Gate 2 connector and place the leading edge of each gate 12 inches apart. For accurate results the gates should be hit dead on, not during a turn. 
