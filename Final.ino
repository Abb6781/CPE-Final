//Abigail Barnes & Kevin Lopez De Dios
//December 14, 2024

//copied from arduino, working, but needs to be changed so that
// it doesnt use libraries

//press and hold button to rotate stepper motor to desired angle.

//includes for temp/humidity
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//includes for stepper motor
#include <Stepper.h>
#define STEPS 100
Stepper stepper(STEPS, 8, 9, 10, 11);

//includes for water sensor 
#define POWER_PIN  7
#define SIGNAL_PIN A5

int value = 0; // variable to store the sensor value

//includes for the clock
#include <RTClib.h>
RTC_DS3231 rtc;
volatile int count = 0;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool lastState = false;

//includes for interrupt
const byte ledPin = 3;
const byte interruptPin = 4;
volatile byte state = LOW;

//includes for LCD
#include <LiquidCrystal.h>
const int RS = 22, EN = 24, D4 = 26, D5 = 28, D6 = 30, D7 = 32;
LiquidCrystal lcd(RS,EN,D4,D5,D6,D7);
void setup() {
  Serial.begin(9600);
  //interrupt
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin),blink, CHANGE);

  // humidity/temp reader
  Serial.begin(9600);
  dht.begin();

  //stepper motor
  stepper.setSpeed(30);
  
  //fan motor, change before submitting
  pinMode(5, OUTPUT);

  //water sensor, change
  pinMode(POWER_PIN, OUTPUT);   // configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
  
  //Clock
  rtc.begin();
  customPutchar('\n');
  attachInterrupt(digitalPinToInterrupt(6), cStep, RISING);

  //leds 
  pinMode(12, OUTPUT); //green
  pinMode(13, OUTPUT); //red
  //blue connected to pin 5. (only on when fan is on)
  //yellow connected to 3 (see interrupt)
}

void loop() {
  //interrupt
  bool d = digitalRead(4);
  Serial.println(d);
  digitalWrite(ledPin, state);
  //water sensor
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF
  Serial.println(value);
  if (value > 220 ){
    digitalWrite(13, HIGH);
    digitalWrite(12, LOW);
    lcd.write("ERROR, LOW WATER");
  }else{
    digitalWrite(13, LOW);
    digitalWrite(12, HIGH);
  }
  // humidity/temp

  int h = dht.readHumidity();
  float t = dht.readTemperature();
  int f = dht.readTemperature(true);

  if(isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f,h);
  float hic = dht.computeHeatIndex(t,h, false);
  if(value <= 220){
    /*Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("% Temperature: "));
    Serial.print(t);
    Serial.print(F(" C "));
    Serial.print(f);
    Serial.print(F("F Heat index:"));
    Serial.print(hic);
    Serial.print(F("C "));
    Serial.print(hif);
    Serial.println(F("F"));
    */
    lcd.print("temp:");
    lcd.print(f);
    lcd.print(" humid:");
    lcd.print(h);
  }
  //turn on/off fan motor when temperature changes (change values)
  //change digital write before submitting
  if((f < 75 && f>50) && (value < 220)){
    digitalWrite(5, LOW);
    digitalWrite(12, HIGH);
  }else if (value <220){
    digitalWrite(5, HIGH);
    digitalWrite(12, LOW);
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
