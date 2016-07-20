/**
   Receiving IR Commands and send the Command to a MQTT Topic
*/
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "MqttDeviceHealthCheck.h"
#include <IRremote.h>
#include <ArduinoJson.h>

/**
   Need the IR Sender LED on Pin 3
*/
IRsend irsend;

long MOPIDY_NEXT_TRACK = 0xa35; //NextTrack

// MAC Adresse des Ethernet Shields
byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xFF };

IPAddress ip(192, 168, 1, 75);

// IP des MQTT Servers
char server[]            = "maxchen";

char DEVICE_ID[] = "ir-sender-livingroom";

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient apiClient;
PubSubClient mqttClient(server, 1883, callback, apiClient);

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

char MQTT_IR_COMMANDS_OUT_PATH[] = "/iot/ircommands/out";
char message_buff[100];
String subscribeTarget;
void setup() {

  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(2500);
  if (mqttClient.connect(DEVICE_ID))
  {
    Serial.println("Connected");
    subscribeTarget = String(MQTT_IR_COMMANDS_OUT_PATH) + "/" + DEVICE_ID;
    String publishTargetFull = String(subscribeTarget);
    int singleTargetLen = publishTargetFull.length() + 1;
    char singleTarget_buffer[singleTargetLen];
    publishTargetFull.toCharArray(singleTarget_buffer, singleTargetLen);
    mqttClient.subscribe(singleTarget_buffer);
  } else {
    Serial.println("Not Connected");
  }
  doHealthCheckMessage(String(DEVICE_ID),ip, mqttClient);


  //register topic MQTT_IR_COMMANDS_OUT_PATH/DEVICE_ID

  Serial.println("Start MQTT IR Sender Hub");

}

void loop() {

  // put your main code here, to run repeatedly:
  // health_timer->Update();
  mqttClient.loop();

}

void callback(char* topic, byte* payload, unsigned int length) {
  if (String(topic).equals(String(subscribeTarget)))
  {
    int i = 0;
    // create character buffer with ending null terminator (string)
    for (i = 0; i < length; i++) {
      message_buff[i] = payload[i];
    }
    message_buff[i] = '\0';
    String msgString = String(message_buff);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(msgString);
    const char* command = root["value"];
    int decode_type = root["decode_type"];
    executeIrCommand(command, decode_type);

    Serial.println("Payload: " + msgString);
  }
}

void executeIrCommand(const char* command, int decode_type)
{
  long commandIr = 0;  
  switch (decode_type) {
    default:
    case UNKNOWN:      Serial.println("UNKNOWN");       break ;
    case NEC:          Serial.println("NEC");           break ;
    case SONY:
      //this one are using for controll the Homeassistant and Mopidy Part
      Serial.println("SONY");
      commandIr = convertIncommingCommandToSony(command);
      for (int i = 0; i < 3; i++) {
        irsend.sendSony(commandIr, 12);
        delay(40);
      }
      break ;
    case RC5:          Serial.println("RC5");           break ;
    case RC6:          Serial.println("RC6");           break ;
    case DISH:         Serial.println("DISH");          break ;
    case SHARP:        Serial.println("SHARP");         break ;
    case JVC:          Serial.println("JVC");           break ;
    case SANYO:        Serial.println("SANYO");         break ;
    case MITSUBISHI:   Serial.println("MITSUBISHI");    break ;
    case SAMSUNG:      Serial.println("SAMSUNG");       break ;
    case LG:           Serial.println("LG");            break ;
    case WHYNTER:      Serial.println("WHYNTER");       break ;
    case AIWA_RC_T501: Serial.println("AIWA_RC_T501");  break ;
    case PANASONIC:    Serial.println("PANASONIC");     break ;
    case DENON:        Serial.println("Denon");         break ;
  }

}
long convertIncommingCommandToSony(const char* command){
  char prefix[] = "0x";
  int commandLength = sizeof(command) + sizeof(prefix);
  char fullcommand[commandLength];
  sprintf(fullcommand, "%s%s", prefix, command);
  long li = strtol(fullcommand, NULL, 0);
  return li;
}

