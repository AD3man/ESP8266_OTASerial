/* 
  OTASerial.h - Library to allow serial output to Arduino IDE console over WiFi network.

  This library 'swaps' the original Serial object with one defined within the library,
  to allow printing of debug output to the Arduino IDE console over WiFi network.

  GitHub page: https://github.com/AD3man/ESP8266_OTASerial

  This library is to be used with a modified Arduino IDE, which receives serial output over WiFi,
  accesible here: https://github.com/AD3man/Arduino
          
   
  Usage:   ** See OTASerial_Examlpe sketch for an additional example of use.
  
    1. Download the library (both this file and OTASerial.cpp) and put them inside your sketch folder.
    
    2. Include this file: #include "OTASerial.h"
    
    3. Define WiFi access point's name (ESSID) and password with
          Serial.configWiFi(ESSID,password);
        both ESSID and password are of type char[] (string).
        
    4. Initialize the library with Serial.begin(<baud rate>);
        With this the board connects to WiFi access point,
        initializes ArduinoOTA and initializes itself (Text server etc).

    5. Now every Serial.print command will send output over WiFi to Arduino IDE's console.
        Opening Serial monitor should now open OTAMonitor which connects to server running on this board 
         and prints its 'serial' output.

        

    !!! What doesn't work: !!!
    For now any methods of object Serial that work with pins aren't supported by this library.
    For that use Serial.USBSerial pointer which points towards the original Serial object.
    
  Have fun programming Wirelessly! =)

*/
#ifndef OTASerial_h
#define OTASerial_h

#include <Stream.h>
#include <HardwareSerial.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>

typedef unsigned long uLong;

class OTASerial : public Stream {
  
  private:
    WiFiServer *debugServer = nullptr;  
    WiFiClient client;
    
    unsigned long lastClientCheck = 0;

    bool _connected = false;
    bool _initialised = false;
 
    char * ESSID =  nullptr;
    char * password = nullptr;
    
    void setupOTA();                                            
    size_t readClient(WiFiClient &cl, char *&buffer);
    bool checkForClient();
    
  public:
    HardwareSerial *USBSerial = nullptr;

    bool OTADefined = false;
    bool WiFiDefined = false;
    bool internalOTAHandle = true;
   unsigned long clientCheckInterval = 1000;
    
    unsigned int OTAPort = 8266;
    unsigned int debugPort = 23;

  
    OTASerial();    //konstruktor
    OTASerial(HardwareSerial *ser);
    ~OTASerial();

    void configWiFi(const char * essid , const char * pass);
       
    virtual void begin(uLong baud){
       begin(baud, SERIAL_8N1, SERIAL_FULL, 1);
    }
    
    virtual void begin(uLong baud, SerialConfig config) {
      begin(baud, config, SERIAL_FULL, 1);
    }
    
    virtual void begin(uLong baud, SerialConfig config, SerialMode mode) {
      begin(baud, config, mode, 1);
    }

    virtual void begin(uLong baud, SerialConfig config, SerialMode mode, uint8_t tx_pin);
    
    virtual void end();
     
    int available() override;
    int read() override;
    int peek() override;
    void flush() override;

    size_t write(uint8_t c) override{
      write((uint8_t *)c,1);
    }
    size_t write(const uint8_t *buffer, size_t size) override;
    
};


/*
  OTASerialObject will be initialised in OTASerial.cpp.
  Define macro is used to override mentions of the 'Serial' object in the code.
    
  Example: Serial.begin(1123) --->> OTASerialObject.begin(1123)
*/
extern OTASerial OTASerialObject;
#define Serial OTASerialObject

#endif


