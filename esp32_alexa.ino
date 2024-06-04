/*
   An example that demonstrates most capabilities of Espalexa v2.4.0
*/
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
//#define ESPALEXA_ASYNC            //uncomment for async operation (can fix empty body issue)
//#define ESPALEXA_NO_SUBPAGE       //disable /espalexa status page
//#define ESPALEXA_DEBUG            //activate debug serial logging
//#define ESPALEXA_MAXDEVICES 15    //set maximum devices add-able to Espalexa
#include <Espalexa.h>
#include <FastLED.h>

// Change this!!
const char* ssid = "Zaffari 2.4GHz";
const char* password = "zaffari87";

#define NUM_LEDS 36
#define DATA_PIN 15
#define COLOR_ORDER BRG

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

// prototypes
bool connectWifi();

//callback functions
void deskLedStrip(EspalexaDevice* dev);

//create devices yourself
EspalexaDevice* epsilon;

bool wifiConnected = false;

Espalexa espalexa;

void setup()
{
  Serial.begin(115200);
  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (!wifiConnected) {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }

  // Define your devices here.
  espalexa.addDevice("Led mesa", deskLedStrip, EspalexaDeviceType::color); //color device


  EspalexaDevice* d = espalexa.getDevice(0); //this will get "Led mesa", the index is zero-based
  d->setPercent(50); //set value "brightness" in percent

  FastLED.addLeds<WS2811, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();

  espalexa.begin();
}

void loop()
{
  espalexa.loop();
  delay(1);
}


void deskLedStrip(EspalexaDevice* d)
{
  if (d == nullptr) return;

  Serial.print("D changed to ");
  Serial.print(d->getValue());
  Serial.print(", color R");
  Serial.print(d->getR());
  Serial.print(", G");
  Serial.print(d->getG());
  Serial.print(", B");
  Serial.println(d->getB());


  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(d->getR(), d->getG(), d->getB());
  }
  FastLED.setBrightness(d->getValue());
  FastLED.show();


  /*//alternative
    uint32_t rgb = d->getRGB();
    Serial.print(", R "); Serial.print((rgb >> 16) & 0xFF);
    Serial.print(", G "); Serial.print((rgb >>  8) & 0xFF);
    Serial.print(", B "); Serial.println(rgb & 0xFF); */
}

// connect to wifi – returns true if successful or false if not
bool connectWifi() {
  bool state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}
