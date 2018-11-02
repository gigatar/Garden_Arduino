#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define MOISTURE_PIN  0
#define SOLENOID_PIN 3

// DHT configs
#define DHT_PIN 2
#define DHTTYPE DHT11


DHT_Unified dht(DHT_PIN, DHTTYPE);

const int START_WATER = 700;    // Which reading to start the water supply
const int RUN_WATER_SECS = 2;   // How long to run the water
const long WATER_INTERVAL = 30;   // How long to wait in between allowing water to turn on again
const int OFF_THRESHOLD = 1024; // Saftey to keep an unplugged sensor from starting the water.

const int DELAY_TIME_SECS = 2;  // Overall loop sleep

long water_time = DELAY_TIME_SECS;  // Ensure we have valid readings before we try to run

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SOLENOID_PIN, OUTPUT);
 
  dht.begin();

}

void loop() {
  digitalWrite(SOLENOID_PIN, LOW);  // Ensure our water is off
  
  int moisture = analogRead(MOISTURE_PIN);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  Serial.print ("Temp = ");
  Serial.print(event.temperature * 1.8 + 32);

  dht.humidity().getEvent(&event);

  Serial.print("ºF \tHumidity = ");
  Serial.print(event.relative_humidity);
  Serial.print("%\tMoisture: ");
  Serial.println(moisture);

  if (moisture >= START_WATER && moisture < OFF_THRESHOLD && water_time == 0) {
    runWater();
  }
  delay(DELAY_TIME_SECS * 1000);
  if (water_time >= DELAY_TIME_SECS) {
    water_time -= DELAY_TIME_SECS;
  } else {
    water_time = 0;
  }
}

void runWater() {
  Serial.print("Starting water for ");
  Serial.print(RUN_WATER_SECS);
  Serial.println(" seconds");
  digitalWrite(SOLENOID_PIN, HIGH);
  delay(RUN_WATER_SECS * 1000);
  digitalWrite(SOLENOID_PIN, LOW);
  water_time = WATER_INTERVAL;
}



