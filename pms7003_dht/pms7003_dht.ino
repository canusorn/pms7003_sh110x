/*
   -PMS7003-
   5V - VCC
   GND - GND
   D4  - TX
   D3  - RX(not use in this code)

   D1 -> SCL
   D2 -> SDA
   5V -> VCC
   GND > GND
*/

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <DHT.h>
#include <PMS.h>
#include <SoftwareSerial.h>

#define DHTPIN D6
// Uncomment whatever type you're using!
#define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
//  #define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define OLED_RESET -1     // can set an oled reset pin if desired
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 100000);

SoftwareSerial pmsSerial(D4, D3); // RX,TX
PMS pms(pmsSerial);
PMS::DATA data;

unsigned long previousMillis = 0;

uint8_t t_connecting;
uint8_t numVariables;
uint8_t sampleUpdate, updateValue = 5;
uint8_t sensorNotDetect = updateValue;
float humid, temp;

void setup()
{
  Serial.begin(115200);
  pmsSerial.begin(9600);
  Wire.begin();
  dht.begin();

  // for dht11
  digitalWrite(D7, HIGH);
  digitalWrite(D8, LOW);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);

  display.begin(0x3D, true); // Address 0x3D default
  //display.setContrast (0); // dim display

  display.display();
  // Clear the buffer.
  display.clearDisplay();
}

void loop()
{

  //------get data from PMS7003------
  if (pms.read(data))
  {
    sensorNotDetect = 0;

    display_update(); // update OLED
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000)
  { // run every 1 second
    previousMillis = currentMillis;
    if (sensorNotDetect < updateValue)
      sensorNotDetect++;
    else
    {
      display_update(); // update OLED
    }

    humid = dht.readHumidity();
    temp = dht.readTemperature();
    Serial.println("Humidity: " + String(humid) + "%  Temperature: " + String(temp) + "°C ");

    sampleUpdate++;
    if (sampleUpdate >= updateValue && sensorNotDetect < updateValue)
    {
      sampleUpdate = 0;
    }
  }
}

void display_update()
{
  //------Update OLED------
  if (sensorNotDetect < updateValue)
  {
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SH110X_WHITE);        // Draw white text
    display.setCursor(0, 0);            // Start at top-left corner
    display.print("1.0: ");
    display.println(data.PM_AE_UG_1_0);
    display.print("2.5: ");
    display.println(data.PM_AE_UG_2_5);
    display.print("10: ");
    display.println(data.PM_AE_UG_10_0);
    display.print("Temp : ");
    display.print(temp, 1);
    display.println(" C");
    display.print("Humid : ");
    display.print(humid, 0);
    display.print("  %");
    display.display();
  }
  // if no data from sensor
  else
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.println("no sensor detect!");
  }

  //------print on serial moniter------
  if (sensorNotDetect <= 5)
  {
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_1_0);
    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);
    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);
  }
  else
  {
    Serial.println("no sensor detect");
  }
}
