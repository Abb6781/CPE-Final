//Abigail Barnes & Kevin Lopez De Dios
//December 14, 2024

//copied from arduino, working, but needs to be changed so that
//Displays to LCD instead of serial monitor
#include "DHT.h"
#define DHTPIN 2

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
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
}
