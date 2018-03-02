/* 
  OTASerial.h - Library to allow serial output to Arduino IDE console over WiFi network.
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


