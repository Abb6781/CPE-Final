//Abigail Barnes & Kevin Lopez De Dios
//December 15, 2024

//copied from arduino, working

//press and hold button to rotate stepper motor to desired angle.

//includes for temp/humidity. libraries included are allowed
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMillis = 0;
const long interval = 60000;

//includes for stepper motor. libraries are allowed
#include <Stepper.h>
#define STEPS 100
Stepper stepper(STEPS, 8, 9, 10, 11);

//includes for water sensor 
#define POWER_PIN  7
#define SIGNAL_PIN A5

int value = 0; // variable to store the sensor value

//includes for the clock. libraries allowed
#include <RTClib.h>
RTC_DS3231 rtc;
volatile int count = 0;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool lastState = false;

//includes for interrupt
const byte ledPin = 3;
const byte interruptPin = 4;
volatile byte state = LOW;

//includes for LCD. libraries used in prior lab
#include <LiquidCrystal.h>
const int RS = 22, EN = 24, D4 = 26, D5 = 28, D6 = 30, D7 = 32;
LiquidCrystal lcd(RS,EN,D4,D5,D6,D7);

volatile unsigned char* port_b = (unsigned char*) 0x25;
volatile unsigned char* ddr_b  = (unsigned char*) 0x24;
volatile unsigned char* port_d = (unsigned char*) 0x2B;
volatile unsigned char* ddr_d  = (unsigned char*) 0x2A;
volatile unsigned char* port_c = (unsigned char*) 0x28;
volatile unsigned char* ddr_c  = (unsigned char*) 0x27;

// Pin Definitions

// Port D (Pins 2-7)
volatile unsigned char* pin_2  = (unsigned char*) 0x2A;
volatile unsigned char* pin_3  = (unsigned char*) 0x2B;
volatile unsigned char* pin_4  = (unsigned char*) 0x2C;
volatile unsigned char* pin_5  = (unsigned char*) 0x2D;
volatile unsigned char* pin_6  = (unsigned char*) 0x2E;
volatile unsigned char* pin_7  = (unsigned char*) 0x2F;

// Port B (Pins 8-13)
volatile unsigned char* pin_8  = (unsigned char*) 0x23;
volatile unsigned char* pin_9  = (unsigned char*) 0x24;
volatile unsigned char* pin_10 = (unsigned char*) 0x25;
volatile unsigned char* pin_11 = (unsigned char*) 0x26;
volatile unsigned char* pin_12 = (unsigned char*) 0x27;
volatile unsigned char* pin_13 = (unsigned char*) 0x28;

// Port C (Analog Pins 22-32 / PC0 - PC5)
volatile unsigned char* pin_22 = (unsigned char*) 0x26;
volatile unsigned char* pin_24 = (unsigned char*) 0x27;
volatile unsigned char* pin_26 = (unsigned char*) 0x28;
volatile unsigned char* pin_28 = (unsigned char*) 0x29;
volatile unsigned char* pin_30 = (unsigned char*) 0x2A; 
volatile unsigned char* pin_32 = (unsigned char*) 0x2B;
volatile unsigned char* pin_34 = (unsigned char*) 0x2C;

void setup() {
  //interrupt
  *ledPin |= (1 << 3);
  *interruptPin |= (1 << 4);
  attachInterrupt(digitalPinToInterrupt(interruptPin),blink, CHANGE);

  // humidity/temp reader
  dht.begin();

  //stepper motor
  stepper.setSpeed(30);
  
  //fan motor, change before submitting
  *pin_5 |= (1 << 5);

  //water sensor, change
  *POWER_PIN |= (1 << 7);   // configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
  *pin_34 (1 << 6);
  
  //Clock
  rtc.begin();
  customPutchar('\n');
  attachInterrupt(digitalPinToInterrupt(6), cStep, RISING);

  //leds 
  *pin_12 |= (1 << 4); //green
  *pin_13 |= (1 << 5); //red
  //blue connected to pin 5. (only on when fan is on)
  //yellow connected to 3 (see interrupt)
}

void loop() {
  //interrupt
  bool d = *pin_4 & (1 << 4);
  digitalWrite(ledPin, state);
  //water sensor
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON 
  if(*pin_34 & (1 << 6)){
    value = analogRead(SIGNAL_PIN);
  } // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF
  if (value > 220 ){
    digitalWrite(13, HIGH);
    digitalWrite(12, LOW);
    lcd.write("ERROR, LOW WATER");
  }else{
    digitalWrite(13, LOW);
    digitalWrite(12, HIGH);
  }
  // humidity/temp
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int h = dht.readHumidity();
    int f = dht.readTemperature(true);
    if(value <= 220){
      lcd.print("temp:");
      lcd.print(f);
      lcd.print(" humid:");
      lcd.print(h);
    }
  
    //turn on/off fan motor when temperature changes (change values)
    if((f < 75 && f>50) && (value < 220)){
      analogWrite(5, 0);
      analogWrite(12, 255);
    }else if (value <220){
      analogWrite(5, 255);
      analogWrite(12, 0);
    }
  }
  //stepper motor
  if(value < 220){
    stepper.step(150);
  }
  //Clock
  DateTime now = rtc.now(); // Get current time

  bool currentState = ((count / 2) % 2 == 0); // Determine the current state (ON or OFF)
  if (currentState != lastState) {  // If the state has changed
    if (currentState) {
      printString("MOTOR OFF @");
    } else {
      printString("MOTOR ON @");
    }
    customPutchar('\n');
    printNumber(now.year());
    customPutchar('/');
    printNumber(now.month());
    customPutchar('/');
    printNumber(now.day());
    customPutchar(' ');
    customPutchar('(');
    printString(daysOfTheWeek[now.dayOfTheWeek()]);
    customPutchar(')');
    customPutchar(' ');
    printNumber(now.hour());
    customPutchar(':');
    printNumber(now.minute());
    customPutchar(':');
    printNumber(now.second());
    customPutchar('\n');
  
    lastState = currentState;    // Update the last state to the current state
  }
  

}
void blink(){
  state = !state;
}
void cStep() { // counts every time pin 2 is RISING
  count++;
}

// Custom function to replace putchar() with Serial.write()
void customPutchar(char c) {
  Serial.write(c);  // Send character to Serial Monitor
}

// Print number as a string of digits
void printNumber(int num) {
  if (num < 10) {
    customPutchar('0'); 
  } 
  String numStr = String(num);// Convert the number to a string and print each character
  for (int i = 0; i < numStr.length(); i++) {
    customPutchar(numStr[i]);
  }
}

void printString(const char* str) {
  while (*str) {
    customPutchar(*str++);
  }
}
