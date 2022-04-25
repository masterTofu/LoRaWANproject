#include <SPI.h>
#include <RH_RF95.h>
RH_RF95 rf95;
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <stdio.h>  

#define DHTPIN 4
#define DHTTYPE    DHT11

static const PROGMEM uint32_t NWKSKEY[32] = { 0x832FBFBF4C3A6E1A042E1232E0F417FE};
static const PROGMEM uint32_t APPSKEY[32] = { 0x99,0xCA,0x35,0x26,0x44,0x89,0x34,0xB0,0x2C,0xB2,0xD9,0x26,0x28,0x34,0xCF,0x4E};
static const PROGMEM uint16_t JOINEUI[16] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
static const uint16_t DEVARR = {0x70B3D57ED004F29F};

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void setup() 
{
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("client init failed");
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("Version v1.3"));
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
}

void loop()
{
  float h,t;
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    t = event.temperature;
    Serial.print(t);
    Serial.println(F("°C"));
  }

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    h = event.relative_humidity;
    Serial.println(h);
    Serial.println(F("%"));
  }

  Serial.print("Sending to rf95_server: ");
  Serial.print(h);
  Serial.println(t);  

  //variables used in formatting:
  byte sendLen;
  char Hstr[8],Tstr[8];
  char buffer[50]; //final byte array that gets passed to radio.send
  dtostrf(h, 3,2, Hstr); // convert a double variable into a small string (byte array) (float variable to be read, length of string being created inc decimal point, number of digits after DP to print, array to store results)
  dtostrf(t, 3,2, Tstr); // convert a double variable into a small string (byte array) (float variable to be read, length of string being created inc decimal point, number of digits after DP to print, array to store results)
  sprintf(buffer, "The Humidity: %s and Temperature: %s", Hstr, Tstr);
  sendLen = strlen(buffer);  //get the length of buffer  
  rf95.send((uint8_t *) buffer, sendLen);
  
  rf95.waitPacketSent();
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000))
  { 
    if (rf95.recv(buf, &len))
   {
      Serial.print("got reply: ");
      Serial.println((char*)buf); 
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }
  delay(400);
}
