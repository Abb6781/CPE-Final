//Abigail Barnes & Kevin Lopez De Dios
//December 14, 2024

//copied from arduino, working, but needs to be changed so that
//Displays to LCD instead of serial monitor

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

void setup() {
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
}

void loop() {
  // humidity/temp
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if(isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f,h);
  float hic = dht.computeHeatIndex(t,h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% Termperature: "));
  Serial.print(t);
  Serial.print(F(" C "));
  Serial.print(f);
  Serial.print(F("F Heat index:"));
  Serial.print(hic);
  Serial.print(F("C "));
  Serial.print(hif);
  Serial.println(F("F"));
  //turn on/off fan motor when temperature changes (change values)
  //change digital write before submitting
  if(t < 70 && t>60){
    digitalWrite(5, HIGH);
  }else{
    digitalWrite(5, LOW);
  }
  //stepper motor
  stepper.step(150);

  //water sensor
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF
  if(value < 100){
    //change to error state
  }

}
