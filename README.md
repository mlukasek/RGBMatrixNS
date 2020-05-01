# RGBMatrixNS
Nightscout monitor for RGB Matrix panel

Very preliminary version, not suitable for end users. More proof of concept than usable product.

All credentials (WiFi and Nightscout) are in arduino_secrets.h, this file has to be edited and updated to your info.

Used components:  
Adafruit Feather M0 WiFi with ATWINC1500 (PRODUCT ID: 3010)  
Adafruit RGB Matrix Featherwing Kit - For M0 and M4 Feathers (PRODUCT ID: 3036)  
Adafruit 32x32 RGB LED Matrix Panel - 5mm Pitch (PRODUCT ID: 2026)  

Required Arduino IDE boards:  
https://adafruit.github.io/arduino-board-index/package_adafruit_index.json  
Adafruit AVR Boards  
Adafruit SAMD Boards  

Required Arduino IDE libraries:  
WiFi101 by Arduino  
Adafruit GFX Library (will install other Adafruit libraries too)  
ArduinoJson by Benoit Blanchon  
RGB matrix Panel by Adafruit  
