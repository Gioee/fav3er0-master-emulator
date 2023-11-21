# fav3er0-master-emulator
FULL-ARM-05/01 serial port emulator made with Arduino Uno

### Hardware
- Arduino UNO
- 12v power supply
- Colored wires
- 8x 1kohm resistor
- Serial type: 2400-N-8-1
- The master sends out every 42ms a string of 10 bytes.

### Instructions
- Connected the Repeater GND in common with the Arduino 12v in -
- Connected the pins 2 and 5 of the serial data-line cable in common to the Arduino 12v in +, they are normally used to power up the optoisolator from the master
- Connected the central pins, 3 and 4, to the Arduino pin 11 for the Tx with the SoftwareSerial library
- Soldered to A2 and A5 pull-down resistors to gnd
- Soldered to A1 and A4 pull-up resistors to +5v
- Soldered to A0 and A3 resistors to both +5v and gnd for a voltage drop to 2.5v

### Communication interface
| Byte | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|-|-|-|-|-|-|-|-|-|-|-|
|Example| 0xFF | 0x06 | 0x12 | 0x56 | 0x02 | 0x14 | 0x0A | 0x00 | 0x38 | 0xC5|
| Explanation| This Byte identifies the beginning of the string, it has to be always 0xFF | Right score, in this case 6 | Left score, in this case 12 | Seconds, in this case 56 | Minutes, in this case 2 | State of the lamps, case 0b00000000 no lamp is activated, case 0b00111111 every lamp is activated | Number of matches and priorite signals | Always 0x00 | Red and yellow penalty cards | Checksum = Sum of previous Bytes % 256 |


### Communication[^1] and serial data-line cable schemas + final result

<p float="left">
  
  <img src="https://github.com/Gioee/fav3er0-master-emulator/assets/48024736/e8c2e6cc-51c7-4423-824a-573d5c564e6f" width="28%" />
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  <img src="https://github.com/Gioee/fav3er0-master-emulator/assets/48024736/29c53337-1eb4-4f9b-9ebf-d71dfc16e4ef" width="32%" />
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  <img src="https://github.com/Gioee/fav3er0-master-emulator/assets/48024736/3bd9c282-c28a-4a0a-826d-aea281c185a4" width="26%" />
</p>


</br></br></br></br>

[^1]: Image of the optoisolator scheme taken from: https://www.reddit.com/r/Fencing/comments/cufcku/do_anyone_know_where_to_find_those_lightings_and/, thanks to [Dalboz989](https://www.reddit.com/user/Dalboz989/), all rights reserved.
