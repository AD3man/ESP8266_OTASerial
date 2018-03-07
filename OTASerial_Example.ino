/*
 * This sketch is a simple example on how to use OTASerial library.
 *  
 */

#include "OTASerial.h"


char * ESSID =  "NameOFAccesPoint";
char * password = "APpassword";

uint32_t prevFreeHeap = 0;
uint32_t prevFreeSkSpace = 0;


void setup() {
  Serial.configWiFi(ESSID,password);
  Serial.begin(115200);
  Serial.print("This is Setup!");
}

void loop() {
  uint32_t freeHeap = ESP.getFreeHeap();
  uint32_t freeSkSpace = ESP.getFreeSketchSpace();

  Serial.println((char *)"Hello!");
  if(freeHeap != prevFreeHeap || freeSkSpace != prevFreeSkSpace){
    Serial.println((char *)"Memory changed");
    Serial.printf((char *)"Free heap size: %u ", freeHeap);
    Serial.printf((char *)"Free sketch space size: %u\n", freeSkSpace);
    prevFreeHeap = freeHeap;
    prevFreeSkSpace = freeSkSpace;
  }
  
  
  Serial.print("no newLine ");
  Serial.println("New line after this ");
  Serial.printf("Formatted %s%u\n\n","Text",2992);



  
  delay(1000);
}



 

 
 


