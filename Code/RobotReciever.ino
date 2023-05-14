#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//----------CLASS DEFINITION AND INSTANTIATION----------------//
struct bot {int spd; bool m11; bool m12; bool m21; bool m22; bool m31; bool m32; bool m41; bool m42;};
bot bot1;

// Radio Instantiation
RF24 radio(10,9); // CE, CSN
const byte address[6] = "00001";
//------------------------END SECTION-------------------------//

//-----------------Motor Directional Control----------------//
// forward default is mx1 is true, mx2 is false
// initial condition for motors is stopped (1,1) or (0,0)

// RIGHT SIDE MOTORS (front (1), back (2))
//int m1p1 = 8; int m1p2 = 7; // IN1, IN2
//int m2p1 = 6; int m2p2 = 5; // IN3, IN4

// LEFT SIDE MOTORS (front (3), back (4))
//int m3p1 = 2; int m3p2 = 4; // IN3, IN4
//int m4p1 = A0; int m4p2 = A1; // IN1, IN2

// REAR MOTORS (right (1), left (2))
int m1p1 = A0; int m1p2 = A1; // IN1, IN2
int m2p1 = 6; int m2p2 = 5; // IN3, IN4

// FRONT MOTORS (right (3), left (4))
int m3p1 = 8; int m3p2 = 7; // IN1, IN2
int m4p1 = 2; int m4p2 = 4; // IN3, IN4

// PWM Pins
int enab = 3;
//--------------------END SECTION--------------------------//

//---------------------SETUP-----------------------------//
void setup() {
  // Motor Directional Control
  pinMode(m1p1, OUTPUT); pinMode(m1p2, OUTPUT);
  pinMode(m2p1, OUTPUT); pinMode(m2p2, OUTPUT);
  pinMode(m3p1, OUTPUT); pinMode(m3p2, OUTPUT);
  pinMode(m4p1, OUTPUT); pinMode(m4p2, OUTPUT);

  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop() {
  if (radio.available()){
    radio.read(&bot1, sizeof(bot1));
    Serial.print("spd:");
    Serial.print(bot1.spd);
//    Serial.print("y signal:");
//    Serial.print(bot1.y);
//    Serial.print("t signal:");
//    Serial.println(bot1.t);
  }

  analogWrite(enab, bot1.spd);
  digitalWrite(m1p1, bot1.m11); digitalWrite(m1p2, bot1.m12);
  digitalWrite(m2p1, bot1.m21); digitalWrite(m2p2, bot1.m22);
  digitalWrite(m3p1, bot1.m31); digitalWrite(m3p2, bot1.m32);
  digitalWrite(m4p1, bot1.m41); digitalWrite(m4p2, bot1.m42);
}
