// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 60

byte led_mode = 18;
byte rgb_color[1][3] = { {0xff, 0x00, 0x00} };
uint8_t skip_font = 0;
std::string matrix_text = "    Hello World!";

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

#include "effects.h"
#include "font.h"
#include "text2matrix.h"
/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
        {
          Serial.print(rxValue[i]);
        }
        Serial.println();
        Serial.println("*********");
        int pos_mode  = rxValue.find("mode");
        if (pos_mode != -1) {
          led_mode = std::stoi(rxValue.substr(pos_mode + 4, pos_mode + 7));
          Serial.print("mode: ");
          Serial.println(led_mode);
        }

        int pos_col  = rxValue.find("color");
        if (pos_col != -1) {
          byte color = std::stoi(rxValue.substr(pos_col + 5, pos_col + 6));
          Serial.print("color: ");
          Serial.println(color);

          if (color == 1) {
            rgb_color[0][0] = 0xff;
            rgb_color[0][1] = 0x00;
            rgb_color[0][2] = 0x00;
          }
          else if (color == 2) {
            rgb_color[0][0] = 0x00;
            rgb_color[0][1] = 0xff;
            rgb_color[0][2] = 0x00;
          }
          else if (color == 3) {
            rgb_color[0][0] = 0x00;
            rgb_color[0][1] = 0x00;
            rgb_color[0][2] = 0xff;
          }
          else if (color == 4) {
            rgb_color[0][0] = 0xff;
            rgb_color[0][1] = 0xff;
            rgb_color[0][2] = 0xff;
          }
          else if (color == 5) {
            rgb_color[0][0] = 0xff;
            rgb_color[0][1] = 0xff;
            rgb_color[0][2] = 0x00;
          }
          else if (color == 6) {
            rgb_color[0][0] = 0xff;
            rgb_color[0][1] = 0x00;
            rgb_color[0][2] = 0xff;
          }
          else if (color == 7) {
            rgb_color[0][0] = 0x00;
            rgb_color[0][1] = 0xff;
            rgb_color[0][2] = 0xff;
          }
          else {
            rgb_color[0][0] = 0x00;
            rgb_color[0][1] = 0x00;
            rgb_color[0][2] = 0x00;
          }

        }

        if(pos_mode == -1 && pos_col == -1)
        {
          matrix_text = rxValue;
        }

      }
    }
};


void setup() {

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("UART Service");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE
                                          );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {

  if (deviceConnected) {
    // pTxCharacteristic->setValue(&txValue, 1);
    // pTxCharacteristic->notify();
    // txValue++;

    // Fill along the length of the strip in various colors...
    switch (led_mode) {

      case 0  : {
          // RGBLoop - no parameters
          RGBLoop();
          break;
        }
      case 1  : {
          // FadeInOut - Color (red, green. blue)
          FadeInOut(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2]);
          break;
        }
      case 2  : {
          // Strobe - Color (red, green, blue), number of flashes, flash speed, end pause
          Strobe(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2], 10, 50, 1000); // red
          break;
        }
      case 3  : {
          // HalloweenEyes - Color (red, green, blue), Size of eye, space between eyes, fade (true/false), steps, fade delay, end pause
          HalloweenEyes(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2],
                        1, 4,
                        true, random(5, 50), random(50, 150),
                        random(1000, 10000));
          HalloweenEyes(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2],
                        1, 4,
                        true, random(5, 50), random(50, 150),
                        random(1000, 10000));
          break;
        }
      case 4  : {
          // CylonBounce - Color (red, green, blue), eye size, speed delay, end pause
          CylonBounce(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2], 4, 10, 50);

          break;
        }
      case 5  : {
          // NewKITT - Color (red, green, blue), eye size, speed delay, end pause
          NewKITT(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2], 8, 10, 50);
          break;
        }
      case 6  : {
          // Twinkle - Color (red, green, blue), count, speed delay, only one twinkle (true/false)
          Twinkle(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2], 10, 100, false);
          break;
        }
      case 7  : {
          // TwinkleRandom - twinkle count, speed delay, only one (true/false)
          TwinkleRandom(20, 100, false);
          break;
        }
      case 8  : {
          // Sparkle - Color (red, green, blue), speed delay
          Sparkle(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2], 0);
          break;
        }
      case 9  : {
          // SnowSparkle - Color (red, green, blue), sparkle delay, speed delay
          SnowSparkle(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2], 20, random(100, 1000));
          break;
        }
      case 10 : {
          // Running Lights - Color (red, green, blue), wave dealy
          RunningLights(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2], 50);
          break;
        }
      case 11 : {
          // colorWipe - Color (red, green, blue), speed delay
          colorWipe(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2], 50);
          colorWipe(0x00, 0x00, 0x00, 50);
          break;
        }
      case 12 : {
          // rainbowCycle - speed delay
          rainbowCycle(20);
          break;
        }
      case 13 : {
          // theatherChase - Color (red, green, blue), speed delay
          theaterChase(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2], 50); // red
          break;
        }
      case 14 : {
          // theaterChaseRainbow - Speed delay
          theaterChaseRainbow(50);
          break;
        }
      case 15 : {
          // Fire - Cooling rate, Sparking rate, speed delay
          Fire(55, 120, 15);
          break;
        }

      // simple bouncingBalls not included, since BouncingColoredBalls can perform this as well as shown below
      // BouncingColoredBalls - Number of balls, color (red, green, blue) array, continuous
      // CAUTION: If set to continuous then this effect will never stop!!!
      case 16 : {
          // mimic BouncingBalls
          BouncingColoredBalls(1, rgb_color, false);
          break;
        }
      case 17 : {
          // multiple colored balls
          byte colors[3][3] = { {0xff, 0x00, 0x00},
            {0xff, 0xff, 0xff},
            {0x00, 0x00, 0xff}
          };
          BouncingColoredBalls(3, colors, false);
          break;
        }
      case 18 : {
          // meteorRain - Color (red, green, blue), meteor size, trail decay, random trail decay (true/false), speed delay
          meteorRain(rgb_color[0][0], rgb_color[0][1], rgb_color[0][2], 10, 64, true, 30);
          break;
        }
       case 99 : {
          
          text2pixel(matrix_text, skip_font);

          break;
        }
    }
    // bluetooth stack will go into congestion, if too many packets are sent
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}
