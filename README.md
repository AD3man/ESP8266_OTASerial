# OTASerial library
  (for ESP8266)
  
  This library 'swaps' the original Serial object with one defined within the library,
  to allow printing of debug output to the Arduino IDE console over WiFi network.


  This library is to be used with a modified Arduino IDE, which receives serial output over WiFi,
  accesible here: https://github.com/AD3man/Arduino
          
   
  ## Usage:   
  ** See OTASerial_Examlpe sketch for an additional example of use.
  
    1. Download the library (both OTASerial.h and OTASerial.cpp) and put them inside your sketch folder.
    
    2. Include OTASerial.h: #include "OTASerial.h"
    
    3. Define WiFi access point's name (ESSID) and password with
          Serial.configWiFi(ESSID,password);
        both ESSID and password are of type char[] (string).
        
    4. Initialize the library with Serial.begin(<baud rate>);
        With this the board connects to WiFi access point,
        initializes ArduinoOTA and initializes itself (Text server etc).
    5. Now every Serial.print command will send output over WiFi to Arduino IDE's console.
        Opening Serial monitor should now open OTAMonitor which connects to server running on this board 
         and prints its 'serial' output.
        
        
  ## !!! What doesn't work: !!!
  For now any methods of object Serial that work with pins aren't supported by this library.
  For that use Serial.USBSerial pointer which points towards the original Serial object.


  ### Have fun programming Wirelessly! =)
