#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define MOISTURE_PIN  0
#define SOLENOID_PIN 3

// DHT configs
#define DHT_PIN 2
#define DHTTYPE DHT11

// Device signature
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x97
#define SIGNATURE_2 0x03


DHT_Unified dht(DHT_PIN, DHTTYPE);


const String  CONTROLLER_ID = String(SIGNATURE_0) + String(SIGNATURE_1) + String(SIGNATURE_2);

const int START_WATER = 700;        // Which reading to start the water supply
const int RUN_WATER_SECS = 2;       // How long to run the water
const long WATER_INTERVAL = 30;     // How long to wait in between allowing water to turn on again
const int OFF_THRESHOLD = 1024;     // Saftey to keep an unplugged sensor from starting the water.
                                    // Note that setting this >= 1023 will disable and allow the solenoid to always fire.
                                    
const int DELAY_TIME_SECS = 2;      // Overall loop sleep

long water_time = DELAY_TIME_SECS;  // Ensure we have valid readings before we try to run

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(Controller_id);
  delay(5000);
  pinMode(SOLENOID_PIN, OUTPUT);

  dht.begin();

}

void loop() {
  digitalWrite(SOLENOID_PIN, LOW);  // Ensure our water is off

  // Get moisture reading
  int moisture = analogRead(MOISTURE_PIN);

  sensors_event_t event;
  // Get Temperature
  dht.temperature().getEvent(&event);
  Serial.print ("Temp = ");
  Serial.print(event.temperature * 1.8 + 32);  // Convert ºC to ºF because I'm American and we suck.

  // Get Humidity, Note that this is done after temperature.  The class will overwrite itself if you do them both, I don't know why but should investigate...
  dht.humidity().getEvent(&event);

  Serial.print("ºF \tHumidity = ");
  Serial.print(event.relative_humidity);
  Serial.print("%\tMoisture: ");
  Serial.println(moisture);

  // If our soil moisture is to low (high value) and we're not greater than our fail-safe threshold AND we are beyond our water interval, let's run some water.
  if (moisture >= START_WATER && moisture < OFF_THRESHOLD && water_time == 0) {
    runWater();
  }
   
  // This is how we configure our interval.  Note that we never want it to go negative, hence setting to 0.
  if (water_time >= DELAY_TIME_SECS) {
    water_time -= DELAY_TIME_SECS;
  } else {
    water_time = 0;
  }
  
  delay(DELAY_TIME_SECS * 1000);
  
}

/**
 * runWater turns the solenoid on for RUN_WATER_SECS seconds.
 */
void runWater() {
  Serial.print("Starting water for ");
  Serial.print(RUN_WATER_SECS);
  Serial.println(" seconds");
  digitalWrite(SOLENOID_PIN, HIGH);
  delay(RUN_WATER_SECS * 1000);
  digitalWrite(SOLENOID_PIN, LOW);
  water_time = WATER_INTERVAL;
}
