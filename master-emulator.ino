/*

The Full-Arm-01, Full-Arm-05 and FA-07 (masters) use an optoisolated port, with a "20mA current loop" interface for the connection to a Repeater.

The master sends out every 42ms a string of 10 bytes.

 Bytes:

  - 1°: 0xFF = Beginning of the string

  - 2°: 0x06 = The right score is 6

  - 3°: 0x12 = The left score is 12

  - 4°: 0x56 = The seconds are 56

  - 5°: 0x02 = The minutes are 2

  - 6°: 0x00 = 0b 0   0   0   0   0   0   0   0   State of the lamps: red, green, whites (w) and yellows (y)
                  |   |   |   |   |   |   |   |         In this example all the lamps are off
                always 0  | rx y  |  red  | lx w              Remember 0=OFF, 1=ON
                        lx y    green   rx w

  - 7°: 0x00 = 0b00000000 = Number of matches and priorite signals. b0+b1 = number of matches (00 = 0 .. 11 = 3). b2 = right priorite. b3 = left priorite

  - 8°: 0x00 = Always different from 0xFF

  - 9°: 0x00 = 0b00000000 = Red and yellow penalty cards. b0 = right red penalty card, b1 = left red penalty card, b2 = right yellow penalty card, b3 = left yellow penalty card, b4 and b5 can be 0 or 1 (useless), b6 and b7 are always 0

  - 10°: Checksum, it is the module of the sum of the 9 previous bytes


Thanks for some of these precious information to https://www.reddit.com/r/Fencing/comments/cufcku/do_anyone_know_where_to_find_those_lightings_and/

*/


#include <SoftwareSerial.h>  //https://docs.arduino.cc/learn/built-in-libraries/software-serial

#define msHitTime 5
#define msDoubleTime 40
#define msHitLampDelay 1200
#define msGNDLampDelay 0

#define supHit 600
#define extGND 300
#define aGND 400
#define v5Drop 1000
#define realGND 100

#define rxPin 10
#define txPin 11
#define msgLength 10

byte blankMsg[] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0b00111111, 0x00, 0x00, 0x00 };
byte outMsg[msgLength];

SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);
unsigned long currentTime, prevTime;
unsigned long lxATime, rxATime, lxGNDTime, rxGNDTime;
unsigned long redTime, greenTime, lYellowTime, rYellowTime;
bool red, green, lYellow, rYellow, lWhite, rWhite;
int lxAt, lxGNDt, rxAt, rxGNDt;

void setup() {
  lxATime = rxATime = lxGNDTime = rxGNDTime = 0;
  prevTime = currentTime = redTime = greenTime = lYellowTime = rYellowTime = millis();
  red = green = lYellow = rYellow = lWhite = rWhite = false;
  lxAt = lxGNDt = rxAt = rxGNDt = 0;

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  pinMode(A0, INPUT);  // A line lame left               rosso a line
  pinMode(A1, INPUT);  // B line left                    nero  b line
  pinMode(A2, INPUT);  //                                verde gnd

  pinMode(A3, INPUT);  // A line right
  pinMode(A4, INPUT);  // B line right
  pinMode(A5, INPUT);  //gnd right

  Serial.begin(9600);
  mySerial.begin(2400);  // DATA-LINE Serial type: 2400-N-8-1
}

void loop() {

  currentTime = millis();

  if ((currentTime > (lxATime + msHitTime)) && lxATime != 0) {
    if (lxAt >= msHitTime) {
      lxAt = 0;
      if (lYellow == false) {  // DA CONTROLLARE
        if ((green == true) && (millis() <= greenTime + msDoubleTime)) {
          red = true;
          redTime = millis();
        } else {
          if (green == false) {
            red = true;
            redTime = millis();
          }
        }
      }
    }
  }

  if ((currentTime > (rxATime + msHitTime)) && rxATime != 0) {
    if (rxAt >= msHitTime) {
      rxAt = 0;
      if (rYellow == false) {  // DA CONTROLLARE
        if ((red == true) && (millis() <= redTime + msDoubleTime)) {
          green = true;
          greenTime = millis();
        } else {
          if (red == false) {
            green = true;
            greenTime = millis();
          }
        }
      }
    }
  }

  if ((red == true) && (currentTime > (redTime + msHitLampDelay))) {
    red = false;
  }

  if ((lYellow == true) && (currentTime > (lYellowTime + msGNDLampDelay))) {
    lYellow = false;
  }

  if ((green == true) && (currentTime > (greenTime + msHitLampDelay))) {
    green = false;
  }

  if ((rYellow == true) && (currentTime > (rYellowTime + msGNDLampDelay))) {
    rYellow = false;
  }

  int lxA = analogRead(A0);
  int lxB = analogRead(A1);
  int lxGND = analogRead(A2);

  int rxA = analogRead(A3);
  int rxB = analogRead(A4);
  int rxGND = analogRead(A5);

  // GNDs
  if ((lxA < aGND && (lxGND > extGND || rxGND > extGND)) || (lxB < v5Drop && (lxGND > extGND || rxGND > extGND)) || lxB < realGND || lxA < realGND) {
    lYellow = true;
    lYellowTime = millis();
  }
  if ((rxA < aGND && (rxGND > extGND || lxGND > extGND)) || (rxB < v5Drop && (rxGND > extGND || lxGND > extGND)) || rxB < realGND || rxA < realGND) {
    rYellow = true;
    rYellowTime = millis();
  }

  // HITs
  if (rxA > supHit) {
    if (rxAt == 0) rxATime = millis();
    rxAt++;
  }
  if (lxA > supHit) {
    if (lxAt == 0) lxATime = millis();
    lxAt++;
  }
  

  if (currentTime % 100 == 0) {
    Serial.print(lxA);
    Serial.print(", ");
    Serial.print(lxB);
    Serial.print(", ");
    Serial.print(lxGND);
    Serial.print(", ");
    Serial.print(rxA);
    Serial.print(", ");
    Serial.print(rxB);
    Serial.print(", ");
    Serial.println(rxGND);
  }

  /*

  DEBUG ZONE

  if (Serial.available() > 0) {
    String in = Serial.readString();
    in.trim();

    if (in == "red") red ? red = false : red = true;
    if (in == "green") green ? green = false : green = true;
    if (in == "lYellow") lYellow ? lYellow = false : lYellow = true;
    if (in == "rYellow") rYellow ? rYellow = false : rYellow = true;
    if (in == "lWhite") lWhite ? lWhite = false : lWhite = true;
    if (in == "rWhite") rWhite ? rWhite = false : rWhite = true;
  }

  red = random(0, 2);
  green = random(0, 2);
  lYellow = random(0, 2);
  rYellow = random(0, 2);
  lWhite = random(0, 2);
  rWhite = random(0, 2);
  */

  if (currentTime >= (prevTime + 42)) {  //The master sends out every 42ms a string of 10 bytes.

    byte checksum = 0;
    for (int i = 0; i < msgLength; i++) {
      if (i == 9) {
        checksum = checksum % 256;
        outMsg[i] = checksum;
      } else {
        if (i == 5) {

          byte lamps = 0;

          if (lWhite) lamps += 0b00000001;
          if (rWhite) lamps += 0b00000010;
          if (red) lamps += 0b00000100;
          if (green) lamps += 0b00001000;
          if (rYellow) lamps += 0b00010000;
          if (lYellow) lamps += 0b00100000;

          outMsg[i] = lamps;
          checksum += lamps;
        } else {
          outMsg[i] = blankMsg[i];
          checksum += blankMsg[i];
        }
      }
    }

    mySerial.write(outMsg, sizeof(outMsg));

    prevTime = currentTime;
  }
}
