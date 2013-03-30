/*

 MicroPD
 by Jeff Highsmith
 
 Two light-dependent resistors are spaced apart 19cm and glued to
 the underside of the track. The main loop monitors those
 sensors and calculates the elapsed time between passings of
 the car. This time is used to calculate the scale speed of the
 passing car. The speed is displayed on two seven-segment LEDs
 mounted on a tiny "Speed Limit/Your Speed" trailer.

 A single servo controls a Micro Chargers toy car launcher. 
 The servo is mounted such that turning it in one direction 
 pushes on the "charge" button and turning it in the other 
 direction pushes on the "release" button. If the speed limit
 is exceeded, the launcher is operated, dispatching a toy
 police car.

*/


#include <Servo.h> 

#define sensorDistance 190 // millimeters
#define toyWheelbase 16 // millimeters
#define realWheelbase 2692 // Toyota Prius wheelbase, in millimeters
#define speedLimit 25 // Miles per hour

#define AA 8
#define BB 9
#define CC 17
#define DD 18
#define EE 19
#define FF 7
#define GG 6
#define A 12
#define B 16
#define C 5
#define D 4
#define E 2
#define pF 11
#define G 10

#define SERVO_PIN 3 // Digital 3
#define LDR1_PIN 15 // Analog 0
#define LDR2_PIN 14 // Analog 1

#define CHARGECAR 30 // Servo position to press charge button
#define OPENGATE 100 // Servo position to press gate button
#define RESTPOSITION 65 // Halfway between

#define CHARGETIME 8000 // 8 seconds of charging
 
Servo launcherservo; // This servo is mounted over the launcher buttons

int ambientLight1;
int ambientLight2;
int currentLight1;
int currentLight2;
unsigned long firstTriggerTime;
unsigned long secondTriggerTime;
unsigned long lastLaunchTime;
unsigned long lastDisplayTime;
unsigned long elapsedTime;
float scaledSpeed = 0.0;
int scaleFactor = (realWheelbase/toyWheelbase);

void setup() 
{ 
  //Setup our LED pins
  pinMode(AA, OUTPUT);
  pinMode(BB, OUTPUT);
  pinMode(CC, OUTPUT);
  pinMode(DD, OUTPUT);
  pinMode(EE, OUTPUT);
  pinMode(FF, OUTPUT);
  pinMode(GG, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(pF, OUTPUT);
  pinMode(G, OUTPUT);
  clearDisplay(); // Set the pins HIGH to turn off the LEDs

  pinMode(SERVO_PIN,OUTPUT);
  launcherservo.attach(SERVO_PIN);
  launcherservo.write(RESTPOSITION);
  pinMode(LDR1_PIN,INPUT);
  pinMode(LDR2_PIN,INPUT);
  Serial.begin(9600);
  ambientLight1 = analogRead(LDR1_PIN); // Collect ambient light readings for
  ambientLight2 = analogRead(LDR2_PIN); // each sensor.
} 

void loop() 
{ 
  currentLight1 = analogRead(LDR1_PIN);
  if (currentLight1 < (ambientLight1 - 100)) {
    firstTriggerTime = millis();
    waitForSecondTrigger();
  }
  if ((lastDisplayTime + 20000) < millis()) {
    clearDisplay();
  }
}

void waitForSecondTrigger() {
  while (1) {
    currentLight2 = analogRead(LDR2_PIN);
    if (currentLight2 < (ambientLight2 - 100)) {
      secondTriggerTime = millis();
      elapsedTime = (secondTriggerTime - firstTriggerTime);
      scaledSpeed = ((sensorDistance*0.00000062)/(elapsedTime*0.00000028))/pow((1.0/scaleFactor),0.5);
      displayNumber((int)scaledSpeed);
      lastDisplayTime = millis();
      if ((int) scaledSpeed > speedLimit) {
        Serial.print(scaledSpeed);
        Serial.print("MPH. ");
        Serial.print("You're MINE! ");
        if ((lastLaunchTime + 10000) < millis())
        {
          launchCar();
        } else {
          Serial.println("Ten second delay between launches.");
        }
      } else {
        Serial.print(scaledSpeed);
        Serial.print("MPH. ");
        Serial.println("You're fine. ");
      }
     break; // Bust out of the while() loop after a second trigger
    }
  }
}

void launchCar() {
  launcherservo.write(CHARGECAR); // Hold the charge button...
  Serial.println("Charging");
  for (int i=0; i<30; i++) {
    delay(CHARGETIME/60);
    clearDisplay();
    delay(CHARGETIME/60);
    displayNumber(scaledSpeed);
  }
  launcherservo.write(OPENGATE); // Open the gate to release the car
  delay(500); // allow the car to get out
  launcherservo.write(RESTPOSITION); // Don't hold any buttons
  lastLaunchTime = millis();
  clearDisplay();
}

void cycleSegments()
{
  // Cycles through all LED segments for troubleshooting purposes.
  // Use this function to figure out if you plugged your segments
  // in right.
  clearDisplay();
  digitalWrite(A, LOW);
  delay(1000);
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  delay(1000);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  delay(1000);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  delay(1000);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  delay(1000);
  digitalWrite(E, HIGH);
  digitalWrite(pF, LOW);
  delay(1000);
  digitalWrite(pF, HIGH);
  digitalWrite(G, LOW);
  delay(1000);
  digitalWrite(G, HIGH);
  digitalWrite(AA, LOW);
  delay(1000);
  digitalWrite(AA, HIGH);
  digitalWrite(BB, LOW);
  delay(1000);
  digitalWrite(BB, HIGH);
  digitalWrite(CC, LOW);
  delay(1000);
  digitalWrite(CC, HIGH);
  digitalWrite(DD, LOW);
  delay(1000);
  digitalWrite(DD, HIGH);
  digitalWrite(EE, LOW);
  delay(1000);
  digitalWrite(EE, HIGH);
  digitalWrite(FF, LOW);
  delay(1000);
  digitalWrite(FF, HIGH);
  digitalWrite(GG, LOW);
  delay(1000);
  digitalWrite(GG, HIGH);
}

void countToNinetyNine()
{
  // For debugging purposes
  clearDisplay();
  for(int i=0; i<100; i++) {
    displayNumber(i);
    delay(1000);
  }
}

void clearDisplay()
{
  //Clears the LED
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(pF, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(AA, HIGH);
  digitalWrite(BB, HIGH);
  digitalWrite(CC, HIGH);
  digitalWrite(DD, HIGH);
  digitalWrite(EE, HIGH);
  digitalWrite(FF, HIGH);
  digitalWrite(GG, HIGH);

}

void displayNumber(int inputNum)
{
  displayTens(inputNum/10);
  displayOnes(inputNum%10);
}

void displayOnes(int inputDigit)
{
  switch (inputDigit) {
    case 0:
      zero();
      break;
    case 1:
      one();
      break;
    case 2:
      two();
      break;
    case 3:
      three();
      break;
    case 4:
      four();
      break;
    case 5:
      five();
      break;
    case 6:
      six();
      break;
    case 7:
      seven();
      break;
    case 8:
      eight();
      break;
    case 9:
      nine();
      break;
  }
}

void displayTens(int inputDigit)
{
  switch (inputDigit) {
    case 0:
      lzero();
      break;
    case 1:
      ten();
      break;
    case 2:
      twenty();
      break;
    case 3:
      thirty();
      break;
    case 4:
      forty();
      break;
    case 5:
      fifty();
      break;
    case 6:
      sixty();
      break;
    case 7:
      seventy();
      break;
    case 8:
      eighty();
      break;
    case 9:
      ninety();
      break;
  }
}

void one()
{
  //Displays 1
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(pF, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
}

void ten()
{
  //Displays 1x
  digitalWrite(DD, HIGH);
  digitalWrite(EE, HIGH);
  digitalWrite(FF, HIGH);
  digitalWrite(GG, HIGH);
  digitalWrite(AA, HIGH);
  digitalWrite(BB, LOW);
  digitalWrite(CC, LOW);
}

void two()
{
  //Displays 2
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(pF, HIGH);
  digitalWrite(G, LOW);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
}

void twenty()
{
  //Displays 2x
  digitalWrite(DD, LOW);
  digitalWrite(EE, LOW);
  digitalWrite(FF, HIGH);
  digitalWrite(GG, LOW);
  digitalWrite(AA, LOW);
  digitalWrite(BB, LOW);
  digitalWrite(CC, HIGH);
}

void three()
{
  //Displays 3
  digitalWrite(D, LOW);
  digitalWrite(E, HIGH);
  digitalWrite(pF, HIGH);
  digitalWrite(G, LOW);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
}

void thirty()
{
  //Displays 3x
  digitalWrite(DD, LOW);
  digitalWrite(EE, HIGH);
  digitalWrite(FF, HIGH);
  digitalWrite(GG, LOW);
  digitalWrite(AA, LOW);
  digitalWrite(BB, LOW);
  digitalWrite(CC, LOW);
}

void four()
{
  //Displays 4
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(pF, LOW);
  digitalWrite(G, LOW);
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
}

void forty()
{
  //Displays 4x
  digitalWrite(DD, HIGH);
  digitalWrite(EE, HIGH);
  digitalWrite(FF, LOW);
  digitalWrite(GG, LOW);
  digitalWrite(AA, HIGH);
  digitalWrite(BB, LOW);
  digitalWrite(CC, LOW);
}

void five()
{
  //Displays 5
  digitalWrite(D, LOW);
  digitalWrite(E, HIGH);
  digitalWrite(pF, LOW);
  digitalWrite(G, LOW);
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
}

void fifty()
{
  //Displays 5x
  digitalWrite(DD, LOW);
  digitalWrite(EE, HIGH);
  digitalWrite(FF, LOW);
  digitalWrite(GG, LOW);
  digitalWrite(AA, LOW);
  digitalWrite(BB, HIGH);
  digitalWrite(CC, LOW);
}

void six()
{
  //Displays 6
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(pF, LOW);
  digitalWrite(G, LOW);
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
}

void sixty()
{
  //Displays 6x
  digitalWrite(DD, LOW);
  digitalWrite(EE, LOW);
  digitalWrite(FF, LOW);
  digitalWrite(GG, LOW);
  digitalWrite(AA, LOW);
  digitalWrite(BB, HIGH);
  digitalWrite(CC, LOW);
}

void seven()
{
  //Displays 7
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(pF, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
}

void seventy()
{
  //Displays 7x
  digitalWrite(DD, HIGH);
  digitalWrite(EE, HIGH);
  digitalWrite(FF, HIGH);
  digitalWrite(GG, HIGH);
  digitalWrite(AA, LOW);
  digitalWrite(BB, LOW);
  digitalWrite(CC, LOW);
}

void eight()
{
  //Displays 8
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(pF, LOW);
  digitalWrite(G, LOW);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
}

void eighty()
{
  //Displays 8x
  digitalWrite(DD, LOW);
  digitalWrite(EE, LOW);
  digitalWrite(FF, LOW);
  digitalWrite(GG, LOW);
  digitalWrite(AA, LOW);
  digitalWrite(BB, LOW);
  digitalWrite(CC, LOW);
}

void nine()
{
  //Displays 9
  digitalWrite(D, LOW);
  digitalWrite(E, HIGH);
  digitalWrite(pF, LOW);
  digitalWrite(G, LOW);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
}

void ninety()
{
  //Displays 9x
  digitalWrite(DD, LOW);
  digitalWrite(EE, HIGH);
  digitalWrite(FF, LOW);
  digitalWrite(GG, LOW);
  digitalWrite(AA, LOW);
  digitalWrite(BB, LOW);
  digitalWrite(CC, LOW);
}

void zero()
{
  //Displays 0
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(pF, LOW);
  digitalWrite(G, HIGH);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
}

void lzero()
{
  //Displays a leading zero
  digitalWrite(DD, LOW);
  digitalWrite(EE, LOW);
  digitalWrite(FF, LOW);
  digitalWrite(GG, HIGH);
  digitalWrite(AA, LOW);
  digitalWrite(BB, LOW);
  digitalWrite(CC, LOW);
}
