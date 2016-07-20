#include <ArduinoJson.h>

char MQTT_HEALTH_CHECK_PATH[] = "/iot/devices";
int const health_intervall = 30000;


String DisplayAddress(IPAddress address)
{
  return String(address[0]) + "." +
         String(address[1]) + "." +
         String(address[2]) + "." +
         String(address[3]);
}

void doHealthCheckMessage(String deviceId,IPAddress ip,PubSubClient mqttClient) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["id"] = deviceId;
  root["ip"] = DisplayAddress(ip);
  String jsonString = "";
  root.printTo(jsonString);
  Serial.println(jsonString);
  int commandLenJson = jsonString.length()  + 1;
  char message_buffCommandJson[commandLenJson];
  jsonString.toCharArray(message_buffCommandJson, commandLenJson);
  boolean response = mqttClient.publish(MQTT_HEALTH_CHECK_PATH, message_buffCommandJson);
}

