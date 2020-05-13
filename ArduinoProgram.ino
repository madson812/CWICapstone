/***************************************************************************************************************************************************

                                                                  Grant Madson 2020
                                                               CWI Programming Capstone
                                                                 Project Wakey-Wakey

                                     Uses the Stepper, Servo, and LiquidCrystal libraries from the Arduino library


***************************************************************************************************************************************************/

#include <Stepper.h>
#include <Servo.h>
#include <LiquidCrystal.h>

#define cinchMotorLeftEnable 2
#define cinchMotorRightEnable 7
#define leftFlap 3
#define rightFlap 4
#define outsideFlap 5
#define insideFlap 6
#define armBaseOne 8
#define armBaseTwo 9
#define armBaseThree 10
#define armBaseFour 11
#define armHandRotateOne 12
#define armHandRotateTwo 13
#define armHandRotateThree 22
#define armHandRotateFour 23
#define armBaseServo 24
#define armMidServo 25
#define screwRightEnable 26
#define screwLeftEnable 27
#define blenderPower 33
#define blenderPulse 34
#define enable 28
#define menu 29
#define up 30
#define down 31
#define select 32

bool enabled = false;
const int stepsPerRevolution = 200;

Stepper armBase = Stepper(stepsPerRevolution, armBaseOne, armBaseTwo, armBaseThree, armBaseFour);
Stepper armHand = Stepper(stepsPerRevolution, armHandRotateOne, armHandRotateTwo, armHandRotateThree, armHandRotateFour);

Servo servoArmBase;
Servo armMid;
Servo leftFlapServo;
Servo rightFlapServo;
Servo outsideFlapServo;
Servo insideFlapServo;

LiquidCrystal lcd = LiquidCrystal(33, 34, 35, 36, 37, 38);

bool mainscreen = true; 
bool mainscreenOpTwo = false;
bool setdelay = false;
int offset = 500;

void setup() {
  pinMode(cinchMotorLeftEnable, OUTPUT);
  pinMode(cinchMotorRightEnable, OUTPUT);
  pinMode(leftFlap, OUTPUT);
  pinMode(rightFlap, OUTPUT);
  pinMode(outsideFlap, OUTPUT);
  pinMode(insideFlap, OUTPUT);
  pinMode(armBaseOne, OUTPUT);
  pinMode(armBaseTwo, OUTPUT);
  pinMode(armBaseThree, OUTPUT);
  pinMode(armBaseFour, OUTPUT);
  pinMode(armHandRotateOne, OUTPUT);
  pinMode(armHandRotateTwo, OUTPUT);
  pinMode(armHandRotateThree, OUTPUT);
  pinMode(armHandRotateFour, OUTPUT);
  pinMode(armBaseServo, OUTPUT);
  pinMode(armMidServo, OUTPUT);
  pinMode(screwRightEnable, OUTPUT);
  pinMode(screwLeftEnable, OUTPUT);
  pinMode(blenderPower, OUTPUT);
  pinMode(blenderPulse, OUTPUT);
  
  pinMode(enable, INPUT);
  pinMode(menu, INPUT);
  pinMode(up, INPUT);
  pinMode(down, INPUT);
  pinMode(select, INPUT);

  armBase.setSpeed(100);
  armHand.setSpeed(100);

  servoArmBase.attach(armBaseServo);
  armMid.attach(armMidServo);
  leftFlapServo.attach(leftFlap);
  rightFlapServo.attach(rightFlap);
  outsideFlapServo.attach(outsideFlap);
  insideFlapServo.attach(insideFlap);

  lcd.begin(16, 2);
}

void loop() {
  // put your main code here, to run repeatedly:
  if( digitalRead(enable) == HIGH){
    enabled = true;
  }else{
    enabled = false;
  }

  if(mainscreen){
    mainScreen();
    if(digitalRead(up) == HIGH){
      mainscreen = false;
      mainscreenOpTwo = true;
    }
    if(digitalRead(down) == HIGH){
      mainscreen = false;
      mainscreenOpTwo = true;
    }
    if(digitalRead(select) == HIGH){
      mainscreen = false;
      mainscreenOpTwo = false;
      setdelay = true;
    }
  }else if(mainscreenOpTwo){
    mainScreenOpTwo();
    if(digitalRead(up) == HIGH){
      mainscreen = true;
      mainscreenOpTwo = false;
    }
    if(digitalRead(down) == HIGH){
      mainscreen = true;
      mainscreenOpTwo = false;
    }
    if(digitalRead(select) == HIGH){
      wakeywakey();
    }
  }else if(setdelay){
    setDelay(offset);
    if(digitalRead(up) == HIGH){
      offset++;
      delay(10);
    }else if(digitalRead(down) == HIGH){
      offset--;
      delay(10);
    }else if(digitalRead(menu) == HIGH){
      setdelay = false;
      mainscreen = true;
      mainscreenOpTwo = false;
    }else if(digitalRead(select) == HIGH){
      lcd.setCursor(2, 1);
      lcd.print("                ");
      lcd.setCursor(2,0);
      lcd.print("Delay set!");
      safeWait(offset*1000*60);
      wakeywakey();
    }
  }
  
}

void wakeywakey(){
  cookTheEggs();

  //remove blender from blender base
   rotateBlender(3000, false);
   safeWait(2000);
   moveArmVertically(45);

   //put blender into cap remover
   //guarenteed to fail right now, would need testing with an actual prototype to get working correctly.

   servoArmBase.write(0);
   armBase.step(50);
   armHand.step(100);
   safeWait(2000);
   moveArmVertically(90);
   servoArmBase.write(45);
   safeWait(1000);
   moveArmVertically(75);

   removeBlenderCap();

   //After removing the cap, dump contents onto tortilla

   armBase.step(50);
   moveArmVertically(0);
   servoArmBase.write(45);
   safeWait(1000);
   armHand.step(100);
   for(int i = 0 ; i < 3 ; i++){
     moveArmVertically(15);
     safeWait(50);
     moveArmVertically(0);
     safeWait(50);
   }
   moveArmVertically(0);
   servoArmBase.write(0);
   safeWait(1000);

   //fold the burrito

   foldBurrito();

   lcd.setCursor(2,1);
   lcd.print("                ");
   lcd.setCursor(2,0);
   lcd.print("Enjoy!");
   
  
  
  //run last
  motorStop(cinchMotorLeftEnable, cinchMotorRightEnable);
}

void mainScreen(){
  lcd.setCursor(2, 0);
  lcd.print("Wakey-Wakey");
  lcd.setCursor(2, 1);
  lcd.print(">Set delay");
}

void mainScreenOpTwo(){
  lcd.setCursor(2,0);
  lcd.print("Set delay");
  lcd.setCursor(2,1);
  lcd.print(">Start now");
}

void setDelay(int offset){
  lcd.setCursor(2,0);
  lcd.print("Delay in minutes");
  lcd.setCursor(2,1);
  lcd.print(offset);
}

void moveArmVertically(int degrees){
  if(degrees > 180){
    degrees = 180;
  }else if(degrees < 0){
    degrees = 0;
  }
  armMid.write(degrees);
  delay(100);
}

void moveArmHorizontally(int steps){
   armBase.step(steps);
   delay(2000);
}

void cookTheEggs(){
  /*
   * to calculate the approximate seconds, I take the specific heat formula, Q = SMdT
   * Assuming 3.18 kJ/(kg*C) as given by engineering toolbox.com
   * Assuming mass to be approximately 63.8 grams per egg, and we will cook 6. This is 0.3828 kg.
   * Assuming dT to be (158 - 69) = 89 and then giving a slight safety factor by saying it is 95. This is 35 C.
   * We find that Q = 42.60564 kJ.
   * The blender that is being used is a 1100 Watt blender.
   * Watts are equal 1 Joule/ second
   * It would take this blender 38.7324 seconds to cook these eggs
   * 
   * This assumes eggs are pre-loaded
   */
   digitalWrite(blenderPower, HIGH);
   safeWait(5000);
   digitalWrite(blenderPulse, HIGH);
   safeWait(38732);
   digitalWrite(blenderPulse, LOW);
   digitalWrite(blenderPower, LOW);
}

void removeBlenderCap(){
  /*
   * The method that is going to be used to remove the blender cap is bad for the motor, but the motor is cheap to replace.
   * Planned obsolescence will bring this company great profits. The cap remover must be replaced every seconds the motor dies.
   */
   motorClockwise(cinchMotorLeftEnable, cinchMotorRightEnable);
   safeWait(1000);
   rotateBlender(3000, true);
   moveArmVertically(90);
   for(int i = 90; i < 135; i++){
    moveArmVertically(i);
    servoArmBase.write((180-i));
   }
}

void foldBurrito(){
  leftFlapServo.write(180);
  rightFlapServo.write(180);
  safeWait(1000);
  leftFlapServo.write(0);
  rightFlapServo.write(0);
  safeWait(1000);
  outsideFlapServo.write(180);
  safeWait(1000);
  outsideFlapServo.write(0);
  safeWait(1000);
  insideFlapServo.write(180);
  safeWait(1000);
  insideFlapServo.write(0);
  safeWait(1000);
}

void rotateBlender(int seconds, bool whichWay){
  if(whichWay){
    motorCounterClockwise(screwLeftEnable, screwRightEnable);
  }else{
    motorClockwise(screwLeftEnable, screwRightEnable);
  }
  safeWait(seconds);
  motorStop(screwLeftEnable, screwRightEnable);
}

void safeWait(long seconds){
  for (long i = 0; i < seconds; i++){
    if(enabled == false){
      return;
    }
    delay(1);
  }
}

void motorStop(int left, int right){
  digitalWrite(left, LOW);
  digitalWrite(right, LOW);
}

void motorClockwise(int left, int right){
  digitalWrite(left, HIGH);
  digitalWrite(right, LOW);
}

void motorCounterClockwise(int left, int right){
  digitalWrite(left, LOW);
  digitalWrite(right, HIGH);
}
