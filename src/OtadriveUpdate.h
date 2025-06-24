#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#define MakeFirmwareInfo(k, v) "&_FirmwareInfo&k=" k "&v=" v "&FirmwareInfo_&"

int OtadriveUpdate();
int OtadriveUpdateFlash();
String getChipId();

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

int OtadriveUpdate()
{
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);

  String url = "http://otadrive.com/DeviceApi/update?";
  url += "&s=" + getChipId();
  url += MakeFirmwareInfo(ProductKey, Version);

  Serial.println(url);
  WiFiClient client; 
  t_httpUpdate_return ret = ESPhttpUpdate.update(client, url, Version);;

  switch (ret)
  {
    case HTTP_UPDATE_FAILED:
      Serial.println("Update failed!");
      Serial.println(ESPhttpUpdate.getLastErrorString());
      return -1;
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No new update available");
      return 2;
      break;
    case HTTP_UPDATE_OK:
      Serial.println("Update OK");
      return 1;
      break;
    default:
      return -2;
      break;
  }
}

int OtadriveUpdateFlash()
{
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);

  String url = "http://otadrive.com/DeviceApi/update?";
  url += "&s=" + getChipId()+"_D";
  url += MakeFirmwareInfo(ProductKey, Version);

  Serial.println(url);
  WiFiClient client; 
  t_httpUpdate_return ret = ESPhttpUpdate.updateFS(client, url, Version);

  switch (ret)
  {
    case HTTP_UPDATE_FAILED:
      Serial.println("Update failed!");
      Serial.println(ESPhttpUpdate.getLastErrorString());
      return -1;
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No new update available");
      return 2;
      break;
    case HTTP_UPDATE_OK:
      Serial.println("Update OK");
      return 1;
      break;
    default:
      return -2;
      break;
  }
}

String getChipId()
{
  uint64_t chipid = ESP.getChipId();
  String ChipIdHex = String((uint32_t)(chipid>>32), HEX);
  ChipIdHex += String((uint32_t)chipid, HEX);
  return ChipIdHex;
}
