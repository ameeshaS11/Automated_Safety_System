#include <SoftwareSerial.h>
#define RX 5
#define TX 6
SoftwareSerial esp8266(RX,TX);

#include "UDHttp.h"
#include "SD.h"
 
 
String AP = "WIFI_NAME";       // CHANGE ME
String PASS = "WIFI_PASSWORD"; // CHANGE ME

int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
 
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
  // put your setup code here, to run once:
  //Serial.begin(115200);
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  
  //WiFi.begin(ssid, password);
 
 // while (WiFi.status() != WL_CONNECTED) {
  //    delay(500);
  //   Serial.print(".");
 // }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
    
  Serial.print("Initializing SD card...");
  if(!SD.begin(4)) Serial.println("failed");
  else Serial.println("ok");
  Serial.println("initialization done.");
}
 
void loop() {
  // put your main code here, to run repeatedly:

  sendCommand("AT+CIPMUX=1",5,"OK");
  //sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  
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

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
