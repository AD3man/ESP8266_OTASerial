#include "OTASerial.h"


/*
  We undefine Serial macro so we can use the original Serial object
*/
#undef Serial

OTASerial::OTASerial(HardwareSerial *ser ) : USBSerial(ser)  {
  //Nothing is initialised here
}

OTASerial::~OTASerial() {
 //Nothing is destroyed here
}

/*
  This method initialises ArduinoOTA.
  Hostname and OTA port are set, the rest is the same as in "BasicOTA" sketch.
*/
void OTASerial::setupOTA() {
 
  ArduinoOTA.setHostname("esp8266-OTASerial");
  ArduinoOTA.setPort(OTAPort);
  
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    
    Serial.println("\nEnd");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

}

/*
  This method initialises USBSerial (the original 'Serial' object),
  WiFi, ArduinoOTA, and sets up the server and adds data to the MDNS response.
*/
void OTASerial::begin(uLong baud, SerialConfig config, SerialMode mode, uint8_t tx_pin) {


  if(_initialised) return;
  USBSerial->begin( baud,  config,  mode,  tx_pin);                           //1

  if (!WiFiDefined) {
    //WiFi is not defined
    WiFi.mode(WIFI_STA);
  
    if (ESSID != nullptr && password != nullptr) {
      WiFi.begin(ESSID, password);
    } else {
      USBSerial->println((char *)"No WiFi SSID and password was provided");
      String tmpEssid = "";
      String tmpPass = "";
      USBSerial->setTimeout(1000);
      const char *encryption[] = {"NONE", "WPA2", "AUTO", "WEP", "WPA"};
      while (tmpEssid.equals("") || tmpPass.equals("")) {
        int netN = WiFi.scanNetworks();
        
        USBSerial->println("List of detected networks:");
        for(int i = 0; i < netN; i++){
          uint8_t * MAC = WiFi.BSSID(i);
          uint8_t SSIDLen = strlen(WiFi.SSID(i).c_str());
           
          USBSerial->printf("%s, %ld dBm, %u-%u-%u-%u-%u-%u, %s\n",
                               WiFi.SSID(i).c_str(), WiFi.RSSI(i),
                                                           MAC[5], 
                                                           MAC[4], 
                                                           MAC[3], 
                                                           MAC[2],
                                                           MAC[1],
                                                           MAC[0],
                                                           encryption [(WiFi.encryptionType(i)*2) % 7]);

        }
        
        USBSerial->println("\nEnter SSID followed by newline:");
        while (USBSerial->available() >= 0) {
          delay(1000);
          tmpEssid = USBSerial->readStringUntil('\n');
          if (!tmpEssid.equals("")) break;
        }
      
        USBSerial->println("Enter password followed by newline:");
        while (USBSerial->available() >= 0) {
          delay(1000);
          tmpPass = USBSerial->readStringUntil('\n');
          if (!tmpPass.equals("")) break;
        }
        USBSerial->println("Trying to connect ...");
        WiFi.begin(tmpEssid.c_str(), tmpPass.c_str());

        if (WiFi.waitForConnectResult()  != WL_CONNECTED) {

          USBSerial->println("Connection Failed! Re-enter credentials!");
          delay(10);
          tmpEssid = "";
          tmpPass = "";

        } else {
          break;
        }
      }
       
    }

    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      USBSerial->println("Connection Failed! Rebooting...");
      delay(1000);
      ESP.restart();
    }
  }
  if (!OTADefined) {
    //OTA not defined
    setupOTA();
    ArduinoOTA.begin();

  }
  debugServer = new WiFiServer(debugPort);
  debugServer->begin();
  MDNS.addServiceTxt("arduino" , "tcp", "OTA_Serial_port", String(debugPort));
  MDNS.addServiceTxt("arduino" , "tcp", "OTA_Serial", "yes");

 
  USBSerial->println();
  USBSerial->print("WiFi connected to:");
  USBSerial->print(WiFi.SSID());
  USBSerial->print(" at IP: ");
  USBSerial->println(WiFi.localIP());
  USBSerial->print("OTA listening at port ");
  USBSerial->println(OTAPort);
  USBSerial->print("Server listening at port ");
  USBSerial->println(debugPort);
  _initialised = true;
}

void OTASerial::end(){
  if(!_initialised) return;
  if(_connected)client.stop();
  _connected = false;
  debugServer->close(); 
  delete debugServer;
  delete [] ESSID;
  delete [] password;
  ESSID = nullptr;
  password = nullptr;
  debugServer = nullptr;
  _initialised = false;
}

void OTASerial::configWiFi(const char * essid , const char * pass){
  if(ESSID != nullptr) delete [] ESSID;
  if(password != nullptr) delete [] password;
  ESSID = new char[strlen(essid)];
  strcpy(ESSID, essid);
  password = new char[strlen(pass)];
  strcpy(password, pass);
}


/*
  Private method for use inside the library.
  Reads data from 'client' including the newline and then terminates with 0.
*/
size_t OTASerial::readClient(WiFiClient &cl, char *&buffer) {
  size_t len = cl.available();
  long started = millis();
  while ((millis() - started) < 1000 && (len = cl.available()) == 0 );
  if (len == 0) return len;
  buffer = new char[len+1];
  len = cl.readBytes(buffer, len);
  buffer[len]=0;
  return len;
}


/*
  Private method for use inside the library.
  Check for new connecting clients and terminates connection if a client is already connected
    
  Returns true if a new client successfuly connected,
   false otherwise.
*/
bool OTASerial::checkForClient() {
  if (lastClientCheck > 0 && (millis() - lastClientCheck) < clientCheckInterval) return false;
  lastClientCheck = millis();
  if (debugServer->hasClient()) {
    WiFiClient newComer = debugServer->available();
    if(_connected){
      newComer.print("FULL:");
      newComer.print(newComer.remoteIP());
      newComer.println();
      newComer.stop();
      return false;
    }
    newComer.println("HELLO:OTASERIAL");
    char *response = nullptr;
    size_t len = readClient(newComer,response);
    if (strcmp(response, "HELLO:ARDUINO:OTAMONITOR\n") != 0) {
      delete [] response;
      newComer.stop();
      return false;
    }
    delete [] response;
    client = newComer;
    client.println("RESPONSE:OK");
    _connected = true;
    USBSerial->print("New client with IP: ");
    USBSerial->print(client.remoteIP());
    USBSerial->print(" has been accepted\n");
    return true;
  }
  return false;
}


/*
  This method receives data (buffer) from print() methods
   and sends that data to the client.
  
  If the client is disconnected,
   his status is updated.
*/
size_t OTASerial::write(const uint8_t *buffer, size_t size) {
  if(!_initialised) return 0;
  if(!OTADefined && internalOTAHandle) ArduinoOTA.handle();
  checkForClient();
  if (!_connected) return 0;

  
  if(client.status() == 0){
    client.stop();
    _connected = false;
    return 0;
  }
  return client.write( buffer, size);
}

int OTASerial::available() {
  if(!_initialised || !(_connected || checkForClient())) return -1;
  return client.available();
}

int OTASerial::read() {
  if(!_initialised || !(_connected || checkForClient()))  return -1;
  return client.read();
};
int OTASerial::peek() {
  if(!_initialised || !(_connected || checkForClient()))  return -1;
  return client.peek();
};
void OTASerial::flush() {
  if(!_initialised || !(_connected || checkForClient())) return;
  client.flush();
};

/*
  Definition of OTASerialObject.
*/
OTASerial OTASerialObject =  OTASerial(&Serial);



