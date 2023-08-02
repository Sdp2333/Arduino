/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo myservo1;  // create servo object to control a servo
Servo myservo2;
// twelve servo objects can be created on most boards
int sensor1 = 45;
int sensor2=36;
int beep = 46;
int pos = 0;    // variable to store the servo position
long frequency = 3000; //频率, 单位Hz
int melody[267]={0, 523, 466, 0, 440, 349, 392, 0, 392, 0, 587, 0, 523, 0, 466, 0, 440, 0, 440, 0, 523, 0, 466, 0, 440, 392, 0, 392, 0, 932, 880, 932, 880, 932, 392, 0, 392, 932, 880, 932, 880, 932, 392, 0, 392, 587, 523, 0, 466, 0, 440, 0, 440, 440, 0, 523, 0, 466, 0, 440, 0, 392, 0, 392, 0, 932, 880, 932, 0, 880, 932, 392, 0, 392, 932, 0, 880, 932, 0, 880, 932, 392, 0, 392, 587, 0, 523, 0, 466, 0, 440, 0, 440, 0, 523, 0, 466, 0, 440, 0, 392, 0, 392, 0, 932, 880, 0, 932, 880, 0, 932, 392, 0, 392, 0, 932, 880, 0, 932, 880, 932, 0, 466, 466, 466, 466, 0, 587, 587, 587, 587, 523, 523, 523, 523, 698, 698, 698, 698, 0, 784, 784, 784, 784, 784, 784, 784, 784, 784, 784, 784, 784, 0, 523, 466, 440, 349, 392, 0, 392, 0, 587, 0, 523, 0, 466, 0, 440, 0, 440, 0, 440, 523, 0, 466, 0, 440, 0, 392, 0, 392, 0, 932, 880, 932, 880, 0, 932, 392, 0, 392, 932, 880, 932, 880, 932, 392, 0, 392, 587, 523, 0, 466, 0, 440, 0, 440, 0, 523, 0, 466, 0, 440, 0, 392, 0, 392, 932, 0, 880, 932, 0, 880, 0, 932, 392, 0, 392, 0, 932, 0, 880, 932, 0, 880, 932, 466, 466, 466, 466, 587, 587, 587, 587, 523, 523, 523, 523, 0, 698, 698, 698, 698, 0, 784, 784, 784, 784, 784, 784, 784, 784, 784, 784, 784, 784};
int noteDurations[267]={9000, 375, 281, 94, 281, 469, 375, 188, 281, 94, 281, 188, 375, 375, 281, 469, 375, 281, 375, 375, 375, 281, 281, 94, 375, 469, 94, 375, 94, 375, 375, 281, 375, 375, 375, 281, 375, 375, 375, 375, 281, 469, 375, 281, 375, 281, 562, 188, 375, 375, 469, 188, 375, 375, 94, 375, 281, 281, 94, 281, 94, 375, 281, 281, 94, 281, 375, 281, 94, 375, 469, 375, 188, 375, 281, 94, 375, 281, 94, 281, 469, 469, 188, 281, 375, 94, 469, 188, 375, 375, 375, 281, 469, 188, 469, 375, 375, 94, 281, 94, 375, 94, 375, 94, 281, 375, 94, 281, 281, 94, 562, 375, 94, 375, 94, 281, 281, 94, 281, 375, 562, 94, 375, 281, 375, 281, 94, 281, 375, 281, 281, 375, 375, 375, 375, 281, 281, 375, 281, 94, 375, 375, 375, 375, 281, 281, 281, 375, 375, 375, 375, 375, 94, 375, 375, 375, 281, 375, 375, 281, 94, 281, 94, 375, 281, 375, 375, 469, 281, 281, 94, 281, 375, 375, 281, 94, 281, 94, 375, 281, 281, 94, 281, 375, 375, 281, 94, 375, 375, 281, 375, 375, 281, 375, 375, 469, 375, 281, 281, 375, 469, 281, 375, 281, 469, 281, 469, 281, 375, 281, 375, 94, 281, 94, 375, 188, 375, 375, 94, 281, 281, 94, 281, 94, 469, 375, 94, 281, 188, 281, 94, 281, 375, 94, 281, 375, 375, 375, 375, 375, 281, 375, 281, 375, 375, 375, 281, 375, 94, 375, 375, 281, 375, 94, 375, 281, 375, 281, 375, 375, 281, 281, 281, 375, 281};

void setup() {
  pinMode(sensor1, INPUT);
  pinMode(46, OUTPUT);
  pinMode(36, INPUT);
  tone(46, frequency );
  delay(100);
  noTone(46);
  myservo1.attach(7);
  myservo2.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo1.write(90);
  myservo2.write(42);
}

void loop() {
  if(!digitalRead(45))
  {
   
  for (pos = 85; pos <= 93; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(85);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 39; pos <= 43; pos += 1) { // goes from 180 degrees to 0 degrees
    myservo2.write(pos);              // tell servo to go to position in variable 'pos'
    delay(85);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 93; pos >= 85; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(85);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 43; pos >= 39; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo2.write(pos);              // tell servo to go to position in variable 'pos'
    delay(85);                       // waits 15 ms for the servo to reach the position
  }



  }

if(!digitalRead(36))
  {
    myservo2.write(40);
  for (pos = 86; pos <= 92; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(85);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 41; pos <= 44; pos += 1) { // goes from 180 degrees to 0 degrees
    myservo2.write(pos);              // tell servo to go to position in variable 'pos'
    delay(85);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 92; pos >= 86; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(85);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 44; pos >= 41; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo2.write(pos);              // tell servo to go to position in variable 'pos'
    delay(85);                       // waits 15 ms for the servo to reach the position
  }
  }

}
