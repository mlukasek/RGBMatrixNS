/*
This example creates a client object that connects and transfers
data using always SSL.

It is compatible with the methods normally related to plain
connections, like client.connect(host, port).

Written by Arturo Guadalupi
last revision November 2015

*/

#include <SPI.h>
#include <WiFi101.h>
#include <RGBmatrixPanel.h>
#include "ArduinoJson.h"

// FeatherWing pinouts for M0 and M4
#define CLK  13
#define OE   1  // TX
#define LAT  0  // RX
#define A   A5
#define B   A4
#define C   A3
#define D   A2
// the RGB data pins on featherwing, must be on same PORT as CLK
uint8_t rgbpins[] = { 6,5,9,11,10,12 };
 
// Create a 32-pixel tall matrix with the defined pins
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 32, rgbpins);

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = NIGHTSCOUT_URL;    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiSSLClient client;

unsigned long msCount = 0;


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup() {
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
    
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  } */

  matrix.begin();
  // draw a pixel in solid white
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(7, 7, 7));
  delay(500);

  // fix the screen with green
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    // delay(5000);
  }

  while (status != WL_CONNECTED) {
    delay(500);
  }
  
  Serial.println("Connected to wifi");
  printWiFiStatus();

}

void update_glycemia() {
  
  String json;
  int datago=0;
  
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 443)) {
    Serial.println("connected to server");
    Serial.println(client.available());
    // Make a HTTP request:
    client.println("GET /api/v2/properties/bgnow,delta HTTP/1.1");
    client.println("Host: adadia.herokuapp.com");
    client.println("Connection: close");
    client.println();
  }

  delay(1000);
  Serial.println("Client read");

  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
    if(c=='{')
      datago=1;
    if(datago)
      json+=c;
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // Serial.print("json = "); Serial.println(json);
  
    const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(9) + 330;
    DynamicJsonDocument doc(capacity);
    
    // const char* json = "{\"bgnow\":{\"mean\":142,\"last\":142,\"mills\":1560841668336,\"sgvs\":[{\"mgdl\":142,\"mills\":1560841668336,\"device\":\"BLU01211\",\"direction\":\"Flat\",\"filtered\":171882,\"unfiltered\":171882,\"noise\":1,\"rssi\":100,\"scaled\":\"7.9\"}]},\"delta\":{\"absolute\":-5,\"elapsedMins\":5.017766666666667,\"interpolated\":false,\"mean5MinsAgo\":147,\"mgdl\":-5,\"scaled\":-0.3,\"display\":\"-0.3\",\"previous\":{\"mean\":147,\"last\":147,\"mills\":1560841367270,\"sgvs\":[{\"mgdl\":147,\"mills\":1560841367270,\"device\":\"BLU01211\",\"direction\":\"Flat\",\"filtered\":176471,\"unfiltered\":176471,\"noise\":1,\"rssi\":100}]}}}";
    
    deserializeJson(doc, json);
    
    JsonObject bgnow = doc["bgnow"];
    int bgnow_mean = bgnow["mean"]; // 142
    int bgnow_last = bgnow["last"]; // 142
    long bgnow_mills = bgnow["mills"]; // 1560841668336
    
    JsonObject bgnow_sgvs_0 = bgnow["sgvs"][0];
    int bgnow_sgvs_0_mgdl = bgnow_sgvs_0["mgdl"]; // 142
    long bgnow_sgvs_0_mills = bgnow_sgvs_0["mills"]; // 1560841668336
    const char* bgnow_sgvs_0_device = bgnow_sgvs_0["device"]; // "BLU01211"
    const char* bgnow_sgvs_0_direction = bgnow_sgvs_0["direction"]; // "Flat"
    long bgnow_sgvs_0_filtered = bgnow_sgvs_0["filtered"]; // 171882
    long bgnow_sgvs_0_unfiltered = bgnow_sgvs_0["unfiltered"]; // 171882
    int bgnow_sgvs_0_noise = bgnow_sgvs_0["noise"]; // 1
    int bgnow_sgvs_0_rssi = bgnow_sgvs_0["rssi"]; // 100
    const char* bgnow_sgvs_0_scaled = bgnow_sgvs_0["scaled"]; // "7.9"
    
    JsonObject delta = doc["delta"];
    int delta_absolute = delta["absolute"]; // -5
    float delta_elapsedMins = delta["elapsedMins"]; // 5.017766666666667
    bool delta_interpolated = delta["interpolated"]; // false
    int delta_mean5MinsAgo = delta["mean5MinsAgo"]; // 147
    int delta_mgdl = delta["mgdl"]; // -5
    float delta_scaled = delta["scaled"]; // -0.3
    const char* delta_display = delta["display"]; // "-0.3"
    
    JsonObject delta_previous = delta["previous"];
    int delta_previous_mean = delta_previous["mean"]; // 147
    int delta_previous_last = delta_previous["last"]; // 147
    long delta_previous_mills = delta_previous["mills"]; // 1560841367270
    
    JsonObject delta_previous_sgvs_0 = delta_previous["sgvs"][0];
    int delta_previous_sgvs_0_mgdl = delta_previous_sgvs_0["mgdl"]; // 147
    long delta_previous_sgvs_0_mills = delta_previous_sgvs_0["mills"]; // 1560841367270
    const char* delta_previous_sgvs_0_device = delta_previous_sgvs_0["device"]; // "BLU01211"
    const char* delta_previous_sgvs_0_direction = delta_previous_sgvs_0["direction"]; // "Flat"
    long delta_previous_sgvs_0_filtered = delta_previous_sgvs_0["filtered"]; // 176471
    long delta_previous_sgvs_0_unfiltered = delta_previous_sgvs_0["unfiltered"]; // 176471
    int delta_previous_sgvs_0_noise = delta_previous_sgvs_0["noise"]; // 1
    int delta_previous_sgvs_0_rssi = delta_previous_sgvs_0["rssi"]; // 100
  
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
    
    matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves
  
    matrix.setCursor(1, 0);    // start at top left, with one pixel of spacing
    matrix.setTextSize(1);     // size 1 == 8 pixels high
    matrix.setTextColor(matrix.Color333(0,0,7));
    matrix.println(bgnow_sgvs_0_direction);
  
    matrix.setCursor(1, 8);
    matrix.setTextSize(1);
    matrix.setTextColor(matrix.Color333(7,7,7));
    matrix.println(delta_display);
    
    float sgv = bgnow_sgvs_0_mgdl;
    sgv/=18.0;
    if(sgv<3.6 || sgv>11)
      matrix.setTextColor(matrix.Color333(7,0,0));
    else
      if(sgv<4.4 || sgv>9)
        matrix.setTextColor(matrix.Color333(7,7,0));
      else
        matrix.setTextColor(matrix.Color333(0,7,0));
    matrix.setTextSize(2);
    matrix.setCursor(1, 17);
    matrix.println(bgnow_sgvs_0_scaled[0]);
    matrix.setCursor(10, 17);
    matrix.println(bgnow_sgvs_0_scaled[1]);
    matrix.setCursor(20, 17);
    matrix.println(bgnow_sgvs_0_scaled[2]);
  }
}

void loop() {
  // update glycemia every 15s
  if(millis()-msCount>15000) {
    update_glycemia();
    msCount = millis();  
  } else {
    /*
      // update current time on display
      M5.Lcd.setFreeFont(FSSB12);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      if(!getLocalTime(&localTimeInfo)) {
        // unknown time
        strcpy(localTimeStr,"??:??");
        lastMin = 61;
      } else {
        if(getLocalTime(&localTimeInfo)) {
          sprintf(localTimeStr, "%02d:%02d  %d.%d.  ", localTimeInfo.tm_hour, localTimeInfo.tm_min, localTimeInfo.tm_mday, localTimeInfo.tm_mon+1);  
        } else {
          strcpy(localTimeStr, "??:??");
          lastMin = 61;
        }
      }
      if(lastMin!=localTimeInfo.tm_min) {
        lastSec=localTimeInfo.tm_sec;
        lastMin=localTimeInfo.tm_min;
        M5.Lcd.drawString(localTimeStr, 0, 0, GFXFF);
      }
    */  
  }

}
