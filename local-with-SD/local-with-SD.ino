//#include "WiFiEsp.h"
#include "UDHttp.h"
#include "SD.h"
// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

char ssid[] = "DR";            // your network SSID (name)
char pass[] = "12345678";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

File root;
//these callbacks will be invoked to read and write data to sdcard
//and process response
//and showing progress 
int responsef(uint8_t *buffer, int len){
//  Serial.println(buffer);
  return 0;
}
//read data callback
int rdataf(uint8_t *buffer, int len){
  //read file to upload
  if (root.available()) {
    return root.read(buffer, len);
  }
  return 0;
}
//write data callback
int wdataf(uint8_t *buffer, int len){
  //write downloaded data to file
  return root.write(buffer, len);
}

void progressf(int percent){
  Serial.println(percent);
}

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial1);


  Serial.print("Initializing SD card...");
  if(!SD.begin(4)) Serial.println("failed");
  else Serial.println("ok");
  Serial.println("initialization done.");

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  
}

void loop()
{
  //put your main code here, to run repeatedly:
  UDHttp udh;
    //open file on sdcard to read
    root = SD.open("1.wav");
    if (!root) {
       Serial.println("can not open file!");
       return;
    }
    //upload downloaded file to local server
    udh.upload("http://127.0.0.1:5000/upload", "File", root.size(), rdataf, progressf, responsef);
    root.close();
    Serial.println("done uploading\n");
    delay(10000);
}
