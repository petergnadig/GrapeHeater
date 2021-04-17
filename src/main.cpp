#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "LittleFS.h"
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define UpdateMinutes 120
#define ProductKey "328d8051-5a20-4fb2-8eb2-c976d425b98e"
#define Version "21.03.14.02"
#define FlashVersion "21.03.14.52"
#include "OtadriveUpdate.h"

#define RELAY_PIN 5
#define ONE_WIRE_BUS 4 // Gyulai kütyün ez 4!!!!

#define CHARTDATANO 1024        // 5 percenként kb két hét adatát tudja tárolni
#define CHARTUPDATEMILLIS 30000 // 300.000 millisec kb 5 perc

//#include "MQTTClient.h"
//#include "EspMQTTClient.h" // https://github.com/plapointe6/EspMQTTClient
//------------------------------------------------------------------
#include <PubSubClient.h>

const char *brokerClientId = "Peti";
const char *brokerUser = "Peti";
const char *brokerPass = "Peti";
const char *broker = "broker.emqx.io";
const char *outTopic = "HelloGP12345";

WiFiClient espClient;
PubSubClient client(espClient);
int count = 0;
char messages[100];
float LastSentTemp1 = 0;
float LastSentTemp2 = 0;
long currentTime, lastTime;
//--------------------------------------------------------------------

int update_ret;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer[2];
int deviceCount = 0;

AsyncWebServer server(80);
IPAddress myIP;
//char ssidCl[33] = "DIGI_77e598";
//char passwordCl[65] = "aa21881a";
char ssidCl[33] = "iPGX";
char passwordCl[65] = "1234567890";
//char ssidCl[33] = "12otb24e";
//char passwordCl[65] = "Sukoro70";
const char *ssidAp = "SzolloMelegito";
const char *passwordAp = "Gyula5700";
const char ssid[23] = "szollo";

String inputMessage;
char newnetwork[33];
char newpassword[65];
bool eempromchange = false;
bool wifichange = false;

float SetTemperature = 10;
float T[3];

struct
{
  float setTemp = 0.0;
  char wifinetwork[33] = "";
  char wifipassword[65] = "";
  char waswritten[10] = "";
} eepromdata;

typedef struct Chartstruct
{
  unsigned long time = 0;
  float settemp = 0;
  float temp1 = 0;
  float temp2 = 0;
  bool futes = false;
};
Chartstruct chartdata[CHARTDATANO];
int cdatacounter = 0;
unsigned long time_last_update = millis();

unsigned long wifi_last_check = millis();

bool Heating = false;

void (*resetFunc)(void) = 0; //declare reset function at address 0

void writeep()
{
  EEPROM.put(0, eepromdata);
  EEPROM.commit();
}

void readep()
{
  EEPROM.get(0, eepromdata);
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("\nConnected to");
    Serial.println(broker);
    if (client.connect(brokerClientId, brokerUser, brokerPass))
    {
      Serial.println("Connected");
      Serial.println(broker);
    }
    else
    {
      Serial.println("trying againg");
      delay(5000);
    }
  }
}

void wifisetup()
{
  WiFi.setAutoConnect(false);
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname(ssid);
  Serial.print("WifI SSID:");
  Serial.println(ssidCl);
  Serial.print("WifI pass:");
  Serial.println(passwordCl);
  WiFi.begin(ssidCl, passwordCl);
  MDNS.begin(ssid);
  Serial.print("Connecting");
  int Try = 30;
  while (WiFi.status() != WL_CONNECTED && Try >= 0)
  {
    delay(500);
    Serial.print(".");
    Serial.print(Try);
    Serial.print(".");
    Try--;
  }
  Serial.println();
  if (Try > 0)
  {
    Serial.print("Connected, IP address: ");
    myIP = WiFi.localIP();
    Serial.println(myIP);
  }
  else
  {
    Serial.println("Network connection failed !!");
  }
  Serial.print("Configuring access point…");
  WiFi.softAPdisconnect(true);
  Serial.println(WiFi.softAP(ssidAp, passwordAp, 6, false, 2) ? "  AP is Ready" : "  AP Start Failed!");
  myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());

  Serial.printf("New hostname: %s\n", WiFi.hostname().c_str());
}

float getTempFloat(int DeviceNumber)
{
  float temperature = sensors.getTempC(Thermometer[DeviceNumber]);
  return temperature;
}

String getTempString(int DeviceNumber)
{
  float temperature = sensors.getTempC(Thermometer[DeviceNumber]);
  return String(temperature);
}

String getSetTempString()
{
  float settemp = eepromdata.setTemp;
  return String(settemp);
}

String getWifiNetwork()
{
  String WifiNetwork = eepromdata.wifinetwork;
  return WifiNetwork;
}

String getWifiPassword()
{
  String WifiPassword = eepromdata.wifipassword;
  return WifiPassword;
}

String prephtml(const String &var)
{
  Serial.println("PROC!!!");
  Serial.println(var);
  if (var == "TEMPERATURE1")
  {
    return getTempString(1);
  }
  else if (var == "TEMPERATURE2")
  {
    return getTempString(2);
  }
  else if (var == "TEMPSET")
  {
    return getSetTempString();
  }
  else if (var == "WIFINETWORK")
  {
    return getWifiNetwork();
  }
  else if (var == "WIFIPASSWORD")
  {
    return getWifiPassword();
  }
  else if (var == "SETTEMP")
  {
    return getSetTempString();
  }
  else if (var == "HEATING")
  {
    if (Heating)
    {
      return "ON";
    }
    else
    {
      return "OFF";
    }
    return "Nope!!!";
  }
  return "";
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7)
      Serial.print(", ");
  }
  Serial.println("");
}

void prepJsonResponseFile()
{
  Serial.println("---Prep Json---");
  if (LittleFS.exists("data.json"))
  {
    LittleFS.remove("data.json");
  }
  File file = LittleFS.open("data.json", "w");
  int jsoncounter = cdatacounter;
  jsoncounter = jsoncounter & (CHARTDATANO - 1);
  int count = 0;
  file.println("[");
  while (count < CHARTDATANO)
  {
    if (count > 0)
    {
      file.println(",");
    }
    file.print("{\"time\":");
    file.print(chartdata[jsoncounter].time);
    file.print(",");
    file.print("\"ST\":");
    file.print(chartdata[jsoncounter].settemp);
    file.print(",");
    file.print("\"T1\":");
    file.print(chartdata[jsoncounter].temp1);
    file.print(",");
    file.print("\"T2\":");
    file.print(chartdata[jsoncounter].temp2);
    file.print(",");
    file.print("\"HE\":");
    file.print(chartdata[jsoncounter].futes);
    file.print("}");
    count++;
    jsoncounter++;
    jsoncounter = jsoncounter & (CHARTDATANO - 1);
  }
  file.println("]");
  file.close();
  file = LittleFS.open("data.json", "r");
  file.close();
}



void setup()
{
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  EEPROM.begin(512);
  readep();
  Serial.println("EepromData:");
  Serial.println(eepromdata.setTemp);
  Serial.println(eepromdata.wifinetwork);
  Serial.println(eepromdata.wifipassword);
  if (strcmp(eepromdata.waswritten, "123456789") != 0)
  {
    strcpy(eepromdata.waswritten, "123456789");
    eepromdata.setTemp = 0;
    strcpy(eepromdata.wifinetwork, ssidCl);
    strcpy(eepromdata.wifipassword, passwordCl);
    Serial.println("EepromDatainit");
    Serial.println(eepromdata.setTemp);
    Serial.println(eepromdata.wifinetwork);
    Serial.println(eepromdata.wifipassword);
    writeep();
  }
  SetTemperature = eepromdata.setTemp;
  strcpy(ssidCl, eepromdata.wifinetwork);
  strcpy(passwordCl, eepromdata.wifipassword);

  wifisetup();

  //--------------------------------------------------
  client.setServer(broker, 1883);

  Serial.print("----Flash Update try----");
  update_ret = OtadriveUpdateFlash();
  Serial.print("----Flash Update result----");
  Serial.println(update_ret);

  Serial.print("----Program Update try----");
  update_ret = OtadriveUpdate();
  Serial.print("----Program Update result----");
  Serial.println(update_ret);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  Heating = false;

  Serial.println(LittleFS.begin() ? "File system Ready" : "File System Failed!");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html", String(), false, prephtml);
    request->send(response);
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html", String(), false, prephtml);
    request->send(response);
  });

  server.on("/indexScripts.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/indexScripts.js", "text");
    request->send(response);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Style!!!");
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/style.css", "text/css");
    request->send(response);
  });

  server.on("/submit", HTTP_GET, [](AsyncWebServerRequest *request) {
    eempromchange = false;
    wifichange = false;
    if (request->hasParam("newtemp"))
    {
      inputMessage = request->getParam("newtemp")->value();
      eepromdata.setTemp = inputMessage.toFloat();
      eempromchange = true;
    }
    if (request->hasParam("wifinetwork"))
    {
      inputMessage = request->getParam("wifinetwork")->value();
      inputMessage.toCharArray(newnetwork, 33);
      if (strcmp(eepromdata.wifinetwork, newnetwork) != 0)
      {
        strcpy(eepromdata.wifinetwork, newnetwork);
        eempromchange = true;
        wifichange = true;
      }
    }
    if (request->hasParam("wifipassword"))
    {
      inputMessage = request->getParam("wifipassword")->value();
      inputMessage.toCharArray(newpassword, 65);
      if (strcmp(eepromdata.wifipassword, newpassword) != 0)
      {
        strcpy(eepromdata.wifipassword, newpassword);
        eempromchange = true;
        wifichange = true;
      }
    }
    if (eempromchange)
    {
      writeep();
    }
    if (wifichange)
    {
      wifisetup();
    }
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html", String(), false, prephtml);
    request->send(response);
  });

  server.on("/temperature1", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", getTempString(1).c_str());
  });

  server.on("/temperature2", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", getTempString(2).c_str());
  });

  server.on("/settemp", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", getSetTempString().c_str());
  });

  server.on("/heating", HTTP_GET, [](AsyncWebServerRequest *request) {
    String HS = (Heating) ? "ON" : "OFF";
    request->send_P(200, "text/plain", HS.c_str());
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    prepJsonResponseFile();
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "data.json", "text/plain");
    request->send(response);
  });

  server.on("/myChart.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/myChart.html", "text/html");
    request->send(response);
  });

  server.on("/myChartScripts.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/myChartScripts.js", "text");
    request->send(response);
  });

  server.on("/myChartStyle.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/myChartStyle.css", "text/css");
    request->send(response);
  });

  server.on("/chart/Chart.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/chart/Chart.css", "text/css");
    request->send(response);
  });

  server.on("/chart/Chart.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/chart/Chart.js", "text");
    request->send(response);
  });

  server.on("/chart/moment.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/chart/moment.js", "text");
    request->send(response);
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/favicon.ico", "image/png");
    request->send(response);
  });
  //loading gif
  server.on("/loading.gif", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/loading.gif", "img");
    request->send(response);
  });

  server.on("/index2.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index2.html", String(), false, prephtml);
    request->send(response);
  });

  server.on("/index2newStyle.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index2newStyle.css", "text/css");
    request->send(response);
  });

  server.on("/index2Scripts.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index2Scripts.js", "text");
    request->send(response);
  });

  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/settings.html", String(), false, prephtml);
    request->send(response);
  });

  server.on("/settingsStyle.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/settingsStyle.css", "text/css");
    request->send(response);
  });

  server.begin();
  Serial.println("HTTP server started");

  sensors.begin();
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  Serial.println("");
  if (!sensors.getAddress(Thermometer[1], 0))
    Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(Thermometer[2], 1))
    Serial.println("Unable to find address for Device 1");
  printAddress(Thermometer[1]);
  printAddress(Thermometer[2]);
}

void loop()
{
  sensors.requestTemperatures();
  T[1] = getTempFloat(1);
  T[2] = getTempFloat(2);
  T[0] = (T[1] + T[2]) / 2;

  if (T[0] < (eepromdata.setTemp - 0.25))
  {
    digitalWrite(RELAY_PIN, HIGH);
    Heating = true;
  }
  if (T[0] > (eepromdata.setTemp + 0.25))
  {
    digitalWrite(RELAY_PIN, LOW);
    Heating = false;
  }

  chartdata[cdatacounter].time = millis();
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
  cdatacounter = cdatacounter & (CHARTDATANO - 1);

  Serial.print(" WiFi mode: ");
  Serial.print(WiFi.getMode());
  Serial.print(" WiFi status: ");
  Serial.print(WiFi.status());
  Serial.println();

  if (WiFi.status() != WL_CONNECTED && (millis() - wifi_last_check) >= 1000000)
  {
    wifi_last_check = millis();
    Serial.println("Try to reconnect to the normal AP");
    wifisetup();
  }

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  float tempKulombseg1 = chartdata[cdatacounter - 1].temp1 - LastSentTemp1;
  float tempKulombseg2 = chartdata[cdatacounter - 1].temp2 - LastSentTemp2;
  Serial.print("tempKulombseg1: ");
  Serial.println(tempKulombseg1);
  Serial.print("tempKulombseg2: ");
  Serial.println(tempKulombseg2);
  currentTime=millis();
  if (currentTime-lastTime > 1800000
  || tempKulombseg1 > 0.2 
  || tempKulombseg1 < -0.2 
  || tempKulombseg2 > 0.2 
  || tempKulombseg2 < -0.2 
  || chartdata[cdatacounter - 1].settemp != chartdata[cdatacounter - 2].settemp 
  || chartdata[cdatacounter - 1].futes != chartdata[cdatacounter - 2].futes)
  {
    LastSentTemp1 = chartdata[cdatacounter - 1].temp1;
    LastSentTemp2 = chartdata[cdatacounter - 1].temp2;
    snprintf(messages, 100, "DC: %d, Time: %d, SetTemp: %.2f, temp1: %.2f, temp2: %.2f, Heating: %d",
             cdatacounter - 1, chartdata[cdatacounter - 1].time, chartdata[cdatacounter - 1].settemp, chartdata[cdatacounter - 1].temp1, chartdata[cdatacounter - 1].temp2, chartdata[cdatacounter - 1].futes);
    Serial.print("----Sending messages: ");
    Serial.println(messages);
    client.publish(outTopic, messages);

    lastTime = millis();
  }

} //end loop