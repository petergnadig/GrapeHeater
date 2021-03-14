#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "LittleFS.h"
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define UpdateMinutes 120
#define ProductKey "328d8051-5a20-4fb2-8eb2-c976d425b98e"
#define Version "21.03.14.01"
#define FlashVersion "21.03.14.51"
#include "OtadriveUpdate.h"

#define RELAY_PIN 5
#define ONE_WIRE_BUS 4 // Gyulai kütyün ez 4!!!!

#define CHARTDATANO 15

int update_ret;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer[2];
int deviceCount = 0;

AsyncWebServer server(80);
IPAddress myIP;
char ssidCl[33] = "12otb24f";
char passwordCl[65] = "Sukoro70";
const char *ssidAp = "SzolloMelegito";
const char *passwordAp = "Gyula5700";

String inputMessage;
char newnetwork[33];
char newpassword[65];
bool eempromchange = false;
bool wifichange = false;

float SetTemperature = 10;
float T[3];

struct { 
    float setTemp = 0.0;
    char wifinetwork[33] = "";
    char wifipassword[65] = "";
    char waswritten[10] ="";
  } eepromdata;

typedef struct Chartstruct{
  unsigned long time = 0;
  float settemp = 0;
  float temp1 = 0;
  float temp2 = 0;
  bool futes = false;
};
Chartstruct chartdata[1024];
int cdatacounter=0;
unsigned long time_last_update=millis();

bool Heating = false;

void(* resetFunc) (void) = 0;//declare reset function at address 0

void writeep() {
  EEPROM.put(0,eepromdata);
  EEPROM.commit(); 
}

void readep() {
  EEPROM.get(0,eepromdata);
}

void wifisetup() {
  WiFi.hostname("SZOLLOMELEGITO");
  WiFi.begin(ssidCl, passwordCl);
  Serial.print("Connecting");
  int Try=30;
  while (WiFi.status() != WL_CONNECTED && Try>=0 )
  {
    delay(500);
    Serial.print(".");
    Serial.print(Try);
    Serial.print(".");
    Try-- ;
  }
  Serial.println();
  if (Try>0)
  { 
    Serial.print("Connected, IP address: ");
    myIP = WiFi.localIP();
    Serial.println(myIP);
    WiFi.printDiag(Serial);
  }
  else
  {  
    Serial.print("Configuring access point…");
    WiFi.mode(WIFI_AP);
    WiFi.softAPdisconnect(true);
    Serial.println(WiFi.softAP(ssidAp,passwordAp,6,false,2) ? "Ready" : "Failed!");
    myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());
  }  
}

float getTempFloat(int DeviceNumber) {
  float temperature = sensors.getTempC(Thermometer[DeviceNumber]);
  return temperature;
}

String getTempString(int DeviceNumber) {
  float temperature = sensors.getTempC(Thermometer[DeviceNumber]);
  return String(temperature);
}

String getSetTemperature() {
  float temperature = eepromdata.setTemp;
  return String(temperature);
}

String getWifiNetwork() {
  String WifiNetwork= eepromdata.wifinetwork;
  return WifiNetwork;
}

String getWifiPassword() {
  String WifiPassword= eepromdata.wifipassword;
  return WifiPassword;
}

String prephtml(const String& var){
  Serial.println("PROC!!!");
  Serial.println(var);
  if (var == "TEMPERATURE1"){
    return getTempString(1);
  }
  else if (var == "TEMPERATURE2"){
    return getTempString(2);
  } 
  else if (var == "TEMPSET") {
    return getSetTemperature();
  }
  else if (var == "WIFINETWORK") {
    return getWifiNetwork();
  }
  else if (var == "WIFIPASSWORD") {
    return getWifiPassword();
  }
  else if (var == "HEATING") {
    if (Heating) 
      {return "ON";}
    else
      {return "OFF";}
  return "Nope!!!";
  }
  return "";
}

void printAddress(DeviceAddress deviceAddress)
{ 
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}

void prepJsonResponseFile() {
  if (LittleFS.exists("data.json")) {
    LittleFS.remove("data.json");
  }
  File file=LittleFS.open("data.json", "w");
  //Serial.println("---------JOSON FILE CREATE ----------");
  int jsoncounter = cdatacounter++;
  jsoncounter = jsoncounter & CHARTDATANO;
  int count =0;
  file.println("{");
  while (count<= CHARTDATANO) {
    file.print("{ time:");
      file.print(chartdata[jsoncounter].time);
      file.print(" , ");
    file.print(" ST:");
      file.print(chartdata[jsoncounter].settemp);
      file.print(" , ");
    file.print(" T1:");
      file.print(chartdata[jsoncounter].temp1);
      file.print(" , ");
    file.print(" T2:");
      file.print(chartdata[jsoncounter].temp2);
      file.print(" , ");
    file.print(" HE:");
      file.print(chartdata[jsoncounter].futes);
      file.print(" }");
    file.println();
    count++;
    jsoncounter++;
    jsoncounter = jsoncounter & CHARTDATANO;
  }
  file.println("}");
  file.close();
  //Serial.println("---------JOSON FILE CREATE ----------");

  file=LittleFS.open("data.json", "r");    
  //while(file.available()){
  //  Serial.write(file.read());
  //}
  file.close();
}


void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  wifisetup();
  
  Serial.print("----Flash Update try----");
  update_ret= OtadriveUpdateFlash();
  Serial.print("----Flash Update result----");
  Serial.println(update_ret);


  Serial.print("----Program Update try----");
  update_ret= OtadriveUpdate();
  Serial.print("----Program Update result----");
  Serial.println(update_ret);
  
  pinMode(RELAY_PIN,OUTPUT);
  digitalWrite(RELAY_PIN,LOW);
  Heating = false;

  delay(1000);
  
  EEPROM.begin(512);
  readep();
  Serial.println("EepromData:");
  Serial.println(eepromdata.setTemp);
  Serial.println(eepromdata.wifinetwork);
  Serial.println(eepromdata.wifipassword);
  if (strcmp(eepromdata.waswritten,"123456789")!=0) {
    strcpy(eepromdata.waswritten,"123456789");
    eepromdata.setTemp=0;
    strcpy(eepromdata.wifinetwork,ssidCl);
    strcpy(eepromdata.wifipassword,passwordCl);
    Serial.println("EepromDatainit");
    Serial.println(eepromdata.setTemp);
    Serial.println(eepromdata.wifinetwork);
    Serial.println(eepromdata.wifipassword);
    writeep();
  }
  SetTemperature = eepromdata.setTemp;
  strcpy(ssidCl,eepromdata.wifinetwork);
  strcpy(passwordCl,eepromdata.wifipassword);
  
  Serial.println(LittleFS.begin() ? "File system Ready" : "File System Failed!");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Index!!!");
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/index.html", String(), false, prephtml);
    //response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Style!!!");
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/style.css", "text/css"); 
    //response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/submit", HTTP_GET, [](AsyncWebServerRequest *request) {
    
    Serial.println("Submit!!!");
    eempromchange = false;
    wifichange = false;
    if (request->hasParam("newtemp")) {
      inputMessage = request->getParam("newtemp")->value();
      eepromdata.setTemp=inputMessage.toFloat();
      Serial.print("New Temperature: ");
      Serial.println(eepromdata.setTemp);
      eempromchange=true;
    }
    if (request->hasParam("wifinetwork")) {
      inputMessage = request->getParam("wifinetwork")->value();
      inputMessage.toCharArray(newnetwork,33);
      if (strcmp(eepromdata.wifinetwork,newnetwork)!=0) {
        strcpy(eepromdata.wifinetwork,newnetwork);
        Serial.print("New wifi network: ");
        Serial.println(eepromdata.wifinetwork);
        eempromchange = true;
        wifichange = true;
      }
    } 
    if (request->hasParam("wifipassword")) {
      inputMessage = request->getParam("wifipassword")->value();
      inputMessage.toCharArray(newpassword,65);
      if (strcmp(eepromdata.wifipassword,newpassword)!=0) {
        strcpy(eepromdata.wifipassword,newpassword);
        Serial.print("New wifi password: ");
        Serial.println(eepromdata.wifipassword);
        eempromchange = true;
        wifichange = true;
      }
    } 
    if (eempromchange) {writeep();}
    if (wifichange) {resetFunc();}
    
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/index.html", String(), false, prephtml);
    //response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/temperature1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTempString(1).c_str());
  });

  server.on("/temperature2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTempString(2).c_str());
  });

  server.on("/heating", HTTP_GET, [](AsyncWebServerRequest *request){
    String HS = (Heating) ? "ON" : "OFF";
    request->send_P(200, "text/plain", HS.c_str());
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    //Serial.println("DATA");
    prepJsonResponseFile();
    //Serial.println("DATA ready");
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, "data.json", "text/plain"); 
    //response->addHeader("Content-Encoding", "gzip");
    //Serial.println("response ready");
    request->send(response);
    //Serial.println("response sent");
  });

  server.begin();
  Serial.println("HTTP server started");

  sensors.begin();
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  Serial.println("");
  if (!sensors.getAddress(Thermometer[1], 0)) Serial.println("Unable to find address for Device 0"); 
  if (!sensors.getAddress(Thermometer[2], 1)) Serial.println("Unable to find address for Device 1"); 
  printAddress(Thermometer[1]);
  printAddress(Thermometer[2]);
}

void loop() {
  sensors.requestTemperatures();
  T[1]=getTempFloat(1);
  T[2]=getTempFloat(2);
  T[0]=(T[1]+T[2])/2;

  if (T[0]<(eepromdata.setTemp-0.25)) {
    digitalWrite(RELAY_PIN,HIGH);
    Heating = true;
  }
  if (T[0]>(eepromdata.setTemp+0.25)) {
    digitalWrite(RELAY_PIN,LOW);
    Heating = false;
  }

  chartdata[cdatacounter].time= millis();
  chartdata[cdatacounter].settemp = eepromdata.setTemp;
  chartdata[cdatacounter].temp1 = T[1];
  chartdata[cdatacounter].temp2 = T[2];
  chartdata[cdatacounter].futes = Heating;

  Serial.print("DC: ");
  Serial.print(cdatacounter);
  Serial.print(" Time: ");
  Serial.print(chartdata[cdatacounter].time);
  Serial.print(" SetTemp: ");
  Serial.print(chartdata[cdatacounter].settemp);
  Serial.print(" T1: ");
  Serial.print(chartdata[cdatacounter].temp1);
  Serial.print(" T2: ");
  Serial.print(chartdata[cdatacounter].temp2);
  Serial.print(" Heating: ");
  Serial.print(chartdata[cdatacounter].futes);
  Serial.println();
  cdatacounter++;
  cdatacounter=cdatacounter & CHARTDATANO;
}