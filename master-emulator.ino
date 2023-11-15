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

#define rxPin 10
#define txPin 11
#define msgLength 10

byte blankMsg[] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0b00111111, 0x00, 0x00, 0x00 };
byte outMsg[msgLength];

SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);
unsigned long currentTime, prevTime;
unsigned long redTime, greenTime, lYellowTime, rYellowTime;
bool red, green, lYellow, rYellow, lWhite, rWhite;

void setup() {
  prevTime = currentTime = redTime = greenTime = lYellowTime = rYellowTime = millis();
  red = green = lYellow = rYellow = lWhite = rWhite = false;
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial.begin(9600);
  mySerial.begin(2400);  // DATA-LINE Serial type: 2400-N-8-1

  pinMode(A0, INPUT_PULLUP);  // A line left
  pinMode(A1, INPUT);         // B line left

  pinMode(A4, INPUT_PULLUP);  // A line right
  pinMode(A5, INPUT);         // B line right

  // C lines to arduino GND
}

void loop() {

  currentTime = millis();

  if ((red == true) && (currentTime > (redTime + 1200))) {
    red = false;
  }

  if ((lYellow == true) && (currentTime > (lYellowTime + 300))) {
    lYellow = false;
  }

  if ((green == true) && (currentTime > (greenTime + 1200))) {
    green = false;
  }

  if ((rYellow == true) && (currentTime > (rYellowTime + 300))) {
    rYellow = false;
  }

  int lxA = analogRead(A0);
  int lxB = analogRead(A1);

  int rxA = analogRead(A4);
  int rxB = analogRead(A5);

  if (lxB > 1000) {
    red = true;
    redTime = millis();
  }
  if (lxA + lxB < 30) {
    lYellow = true;
    lYellowTime = millis();
  }

  if (rxB > 1000) {
    green = true;
    greenTime = millis();
  }
  if (rxA + rxB < 30) {
    rYellow = true;
    rYellowTime = millis();
  }

  if (currentTime % 500 == 0) {
    Serial.print(lxA);
    Serial.print(", ");
    Serial.print(lxB);
    Serial.print(", ");
    Serial.print(rxA);
    Serial.print(", ");
    Serial.println(rxB);
  }

  /*

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
