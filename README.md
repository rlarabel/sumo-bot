# Mini Sumo Bot
## Description
This Mini-class sumo robot measures 10"x10" and weighs 500 g. Designed for competitive sumo matches, it can detect enemy robots, navigate the arena, and identify the arena's edge to avoid falling out. Its lightweight frame and responsive sensors enable agile movement and strategic positioning during battles.

## Specs for SumoBot 
- Class: Mini
- Size: 10”x10”
- Weight: 500 g
- Last 3 min >=80% battery power
- Detect objects within 77 cm 
- Be able to move forward, reverse, rotate, turn, and stop
- Have variable speed: 100%, 75%, 50%, 25% 
- Detect the edge of the arena within 100 ms

## Input/output
### Input
- Start/Stop Signal
- IR Sensors
- Range Detectors
### Output
- PWM Signal
- Terminal Output

## Required Specs for Microcontroller
### Peripherals 
- 8 GPIO (figure out after SCHEMATIC)
- 2x PWM 
- 1x Timer 
- 1x UART
- 4x ADC channels
- 3x I2C slaves
### Size 
- 24 Kb
### Speed
- N/A
### Power Consumption
- N/A
### Price
- N/A

## Tools
### Soldering Tools
- Solder gun
- Solder wire
- Stand
- Sponge
- Tweezer
- Flux
- Small paint brush for flux
### Cleaning
- Iso Alcohol 
- Soft brush
### Software tools
- MSP430-GCC 
- Git
- Make
- elfsize
- readelf
- checkcpp
- Docker
- Clang
- GitHub Actions
- KiCad

(Yet to Add)  
Table 1. Development tools

## Schematic
Figure 1. Schematic (See Docs)

(Yet to Add)  
Table 2. Part List for SumoBot Electronics

## Software Architecture Structure
Statement -> Function -> Modules -> Architecture  

Separate hardware dependent code (device drivers) from hardware independent code (application)

Figure 2. Software architecture for the SumoBot (See Docs)

## Git Work Flow (Agile)
Branch-> Code Changes -> Format -> Build -> cppcheck -> Test -> Commit -> Push-> Pull -> Check -> Merge
