//---------------------LIBRARIES--------------------//
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//----------CLASS DEFINITION AND INSTANTIATION----------------//
RF24 radio(10,9); // CE, CSN
const byte address[6] = "00001"; //pipe to communicate to reciever

struct bot {int spd; bool m11; bool m12; bool m21; bool m22; bool m31; bool m32; bool m41; bool m42;};
bot bot1;

//------------------------END SECTION-------------------------//

//-----------------Motor Directional Control----------------//
// forward default is mx1 is true, mx2 is false
// initial condition for motors is stopped (1,1) or (0,0)

// (old) RIGHT SIDE MOTORS (front (1), back (2))
// (old) LEFT SIDE MOTORS (front (3), back (4))

// RIGHT SIDE MOTORS (front (3), back (1))
// LEFT SIDE MOTORS (front (4), back (2))
//--------------------END SECTION--------------------------//

// Joystick Switches
const int sw1 = 8; const int sw2 = 7;

// Misc. Vairable Definition
int j1_dx = 529; int j1_dy = 517; int j2_dy = 486; //default values for each joystick
int tol = 50; //defines buffer region around joystick datum/default value
int from_low; int from_high; // the low and high end of the "from" mapping range, for speed control

//---------------------SETUP-----------------------------//
void setup() {
  pinMode(sw1, INPUT_PULLUP); pinMode(sw2, INPUT_PULLUP); //JOYSTICK SWITCHES (must change to analog)
  
  pinMode(A2, INPUT); pinMode(A3, INPUT); pinMode(A4, INPUT); // Motor Speed Control: (j1x, j1y (lateral), j2y)

  // Radio initialization
  radio.begin();
  radio.openWritingPipe(address);
  //radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX); //Power Amplifier level - as you get furhter away, increase it
  radio.stopListening(); //sets as transmitter

  Serial.begin(9600);
}
//---------------------END SETUP------------------------//

void loop() {
  int x = analogRead(A2);
  int y = analogRead(A3);
  int t = analogRead(A4);

  if (x >= j1_dx+tol && y < j1_dy+tol && y > j1_dy-tol){ //go forward
    drive("FORWARD", "FORWARD", "FORWARD", "FORWARD");
    bot1.spd = x;
    from_low = j1_dx; from_high = 1023;
    Serial.print("fwd");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" ");
  }

  else if (x <= j1_dx-tol && y < j1_dy+tol && y > j1_dy-tol){ //go backwards
    drive("REVERSE", "REVERSE", "REVERSE", "REVERSE");
    bot1.spd = j1_dx-x; 
    from_low = 0; from_high = j1_dx;
    //OR
    //bot1.spd = x; // with the next line
    //from_low = j1_dx; from_high = 0: //DOES THIS MAPPING WORK?
    Serial.print("rev");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" "); 
  }

  else if (y >= j1_dy+tol && x < j1_dx+tol && x > j1_dx-tol){ // go right
    drive("FORWARD", "REVERSE", "REVERSE", "FORWARD");
    bot1.spd = y;
    from_low = j1_dy; from_high = 1023;
    Serial.print("right");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" ");
  }

  else if (y <= j1_dy-tol && x < j1_dx+tol && x > j1_dx-tol){ // go left
    drive("REVERSE", "FORWARD", "FORWARD", "REVERSE");
    bot1.spd = j1_dy-y;
    from_low = 0; from_high = j1_dy;
    Serial.print("left");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" ");
  }

  else if (x >= j1_dx+tol && y >= j1_dy+tol){ //quadrant 1 tramming
    drive("FORWARD", "STOP", "STOP", "FORWARD");
    bot1.spd = min(pow(pow(x,2) + pow(y,2),0.5), 1023); //1023 because havent mapped yet to (0,255)
    from_low = pow(pow(j1_dx+tol,2) + pow(j1_dy+tol,2), 0.5); from_high = 1023;
    Serial.print("quad 1 tram");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" ");
  }

  else if (x >= j1_dx+tol && y <= j1_dy-tol){ //quadrant 2 tramming
    drive("STOP", "FORWARD", "FORWARD", "STOP");
    bot1.spd = min(pow(pow(x,2) + pow(1023-y,2),0.5), 1023); //1023 because havent mapped yet to (0,255)
    from_low = pow(pow(j1_dx+tol,2) + pow(j1_dy+tol,2), 0.5); from_high = 1023;
    Serial.print("quad 2 tram");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" ");
  }

  else if (x <= j1_dx-tol && y <= j1_dy-tol){ //quadrant 3 tramming
    drive("REVERSE", "STOP", "STOP", "REVERSE");
    bot1.spd = min(pow(pow(1023-x,2) + pow(1023-y,2),0.5), 1023); //1023 because havent mapped yet to (0,255)
    from_low = pow(pow(j1_dx+tol,2) + pow(j1_dy+tol,2), 0.5); from_high = 1023;
    Serial.print("quad 3 tram");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" ");
  }

  else if (x <= j1_dx-tol && y >= j1_dy+tol){ //quadrant 4 tramming
    drive("STOP", "REVERSE", "REVERSE", "STOP");
    bot1.spd = min(pow(pow(1023-x,2) + pow(y,2),0.5), 1023); //1023 because havent mapped yet to (0,255)
    from_low = pow(pow(j1_dx+tol,2) + pow(j1_dy+tol,2), 0.5); from_high = 1023;
    Serial.print("quad 4 tram");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" ");
  }

  else if (t >= j2_dy+tol && x < j1_dx+tol && x > j1_dx-tol && y < j1_dy+tol && y > j1_dy-tol){ //twist clockwise w 2nd joystick
    drive("REVERSE", "FORWARD", "REVERSE", "FORWARD");
    bot1.spd = t;
    from_low = j2_dy; from_high = 1023;
    Serial.print("pivoting clockwise");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" ");
  }

  else if (t <= j2_dy-tol && x < j1_dx+tol && x > j1_dx-tol && y < j1_dy+tol && y > j1_dy-tol){ //twist counter-clockwise w 2nd joystick
    drive("FORWARD", "REVERSE", "FORWARD", "REVERSE");
    bot1.spd = j2_dy-t;
    from_low = 0; from_high = j2_dy;
    Serial.print("pivoting counter-clockwise");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" ");
  }

  else{ //nothing is happening
    drive("STOP", "STOP", "STOP", "STOP");
    from_low = 0; from_high = 1023;
    bot1.spd = 0;
    Serial.print("STOPPED");
    //Serial.print(bot1.spd);
    Serial.print(bot1.m11);
    Serial.print(bot1.m21);
    Serial.print(bot1.m31);
    Serial.print(bot1.m41);
    Serial.print(" ");
  }
  
  //Convert Speed to PWM
  bot1.spd = map(bot1.spd, from_low, from_high, 0, 255);
  Serial.println(bot1.spd);
  
  // Sending Data
  radio.write(&bot1, sizeof(bot1));
  //delay(1000);

}

// since bot1 is defined globally, does it need to be included as a function argument?
void drive(char wheel1[], char wheel2[], char wheel3[], char wheel4[]){
  if (strcmp(wheel1,"FORWARD")==0) {bot1.m11 = HIGH; bot1.m12 = LOW;}
  else if (strcmp(wheel1,"REVERSE")==0) {bot1.m11 = LOW; bot1.m12 = HIGH;}
  else {bot1.m11 = LOW; bot1.m12 = LOW;}

  if (strcmp(wheel2,"FORWARD")==0) {bot1.m21 = HIGH; bot1.m22 = LOW;}
  else if (strcmp(wheel2,"REVERSE")==0) {bot1.m21 = LOW; bot1.m22 = HIGH;}
  else {bot1.m21 = LOW; bot1.m22 = LOW;}

  if (strcmp(wheel3,"FORWARD")==0) {bot1.m31 = HIGH; bot1.m32 = LOW;}
  else if (strcmp(wheel3,"REVERSE")==0) {bot1.m31 = LOW; bot1.m32 = HIGH;}
  else {bot1.m31 = LOW; bot1.m32 = LOW;}

  if (strcmp(wheel4,"FORWARD")==0) {bot1.m41 = HIGH; bot1.m42 = LOW;}
  else if (strcmp(wheel4,"REVERSE")==0) {bot1.m41 = LOW; bot1.m42 = HIGH;}
  else {bot1.m41 = LOW; bot1.m42 = LOW;}
}
