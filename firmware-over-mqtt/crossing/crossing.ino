#include <FastLED.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>

#define POCET_LEDEK 20
#define LEDKOVY_PIN_0 13
#define LEDKOVY_PIN_1 12
#define LEDKOVY_PIN_2 11
#define LEDKOVY_PIN_3 8
#define LEDKOVY_PIN_4 7
#define LEDKOVY_PIN_5 6
#define LEDKOVY_PIN_6 3
#define LEDKOVY_PIN_7 2

#define TLACITKOVY_PIN_0 A0
#define TLACITKOVY_PIN_1 A1
#define TLACITKOVY_PIN_2 A2
#define TLACITKOVY_PIN_3 A3
#define TLACITKOVY_PIN_4 A4
#define TLACITKOVY_PIN_5 A5

CRGB ledky_0[POCET_LEDEK];
CRGB ledky_1[POCET_LEDEK];
CRGB ledky_2[POCET_LEDEK];
CRGB ledky_3[POCET_LEDEK];
CRGB ledky_4[POCET_LEDEK];
CRGB ledky_5[POCET_LEDEK];
CRGB ledky_6[POCET_LEDEK];
CRGB ledky_7[POCET_LEDEK];

char wifiSSID[] = "Karak";        // your network SSID (name)
char wifiPassword[] = "123qweASD";    // your network password (use for WPA, or use as key for WEP)

WiFiSSLClient wifiClient;
MqttClient mqttClient(wifiClient);

const char mqttBroker[]         = "d57a0d1c39d54550b147b58411d86743.s2.eu.hivemq.cloud";
const int  mqttPort             = 8883;
const char mqttBrokerUsername[] = "robot";
const char mqttBrokerPassword[] = "P@ssW0rd!";

#define TOPIC_LED_0 "/crossing/led/13"
#define TOPIC_LED_1 "/crossing/led/12"
#define TOPIC_LED_2 "/crossing/led/11"
#define TOPIC_LED_3 "/crossing/led/8"
#define TOPIC_LED_4 "/crossing/led/7"
#define TOPIC_LED_5 "/crossing/led/6"
#define TOPIC_LED_6 "/crossing/led/3"
#define TOPIC_LED_7 "/crossing/led/2"
#define TOPIC_SEMAPHORE_0 "/crossing/semaphore/13"
#define TOPIC_SEMAPHORE_1 "/crossing/semaphore/12"
#define TOPIC_SEMAPHORE_2 "/crossing/semaphore/11"
#define TOPIC_SEMAPHORE_3 "/crossing/semaphore/8"
#define TOPIC_SEMAPHORE_4 "/crossing/semaphore/7"
#define TOPIC_SEMAPHORE_5 "/crossing/semaphore/6"
#define TOPIC_SEMAPHORE_6 "/crossing/semaphore/3"
#define TOPIC_SEMAPHORE_7 "/crossing/semaphore/2"
#define TOPIC_BUTTON_0 "/crossing/button/A0"
#define TOPIC_BUTTON_1 "/crossing/button/A1"
#define TOPIC_BUTTON_2 "/crossing/button/A2"
#define TOPIC_BUTTON_3 "/crossing/button/A3"
#define TOPIC_BUTTON_4 "/crossing/button/A4"
#define TOPIC_BUTTON_5 "/crossing/button/A5"
//#define TOPIC_BUTTON_ENABLE_0 "/crossing/button-enable/A0"
//#define TOPIC_BUTTON_ENABLE_1 "/crossing/button-enable/A1"
//#define TOPIC_BUTTON_ENABLE_2 "/crossing/button-enable/A2"
//#define TOPIC_BUTTON_ENABLE_3 "/crossing/button-enable/A3"
//#define TOPIC_BUTTON_ENABLE_4 "/crossing/button-enable/A4"
//#define TOPIC_BUTTON_ENABLE_5 "/crossing/button-enable/A5"

//boolean buttonsEnabled[6];

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }
  
  FastLED.addLeds<WS2812B, LEDKOVY_PIN_0, GRB>(ledky_0, POCET_LEDEK);
  FastLED.addLeds<WS2812B, LEDKOVY_PIN_1, GRB>(ledky_1, POCET_LEDEK);
  FastLED.addLeds<WS2812B, LEDKOVY_PIN_2, GRB>(ledky_2, POCET_LEDEK);
  FastLED.addLeds<WS2812B, LEDKOVY_PIN_3, GRB>(ledky_3, POCET_LEDEK);
  FastLED.addLeds<WS2812B, LEDKOVY_PIN_4, GRB>(ledky_4, POCET_LEDEK);
  FastLED.addLeds<WS2812B, LEDKOVY_PIN_5, GRB>(ledky_5, POCET_LEDEK);
  FastLED.addLeds<WS2812B, LEDKOVY_PIN_6, GRB>(ledky_6, POCET_LEDEK);
  FastLED.addLeds<WS2812B, LEDKOVY_PIN_7, GRB>(ledky_7, POCET_LEDEK);
  FastLED.setBrightness(32);
  FastLED.show();

  pinMode(TLACITKOVY_PIN_0, INPUT_PULLUP);
  pinMode(TLACITKOVY_PIN_1, INPUT_PULLUP);
  pinMode(TLACITKOVY_PIN_2, INPUT_PULLUP);
  pinMode(TLACITKOVY_PIN_3, INPUT_PULLUP);
  pinMode(TLACITKOVY_PIN_4, INPUT_PULLUP);
  pinMode(TLACITKOVY_PIN_5, INPUT_PULLUP);

  while (WiFi.begin(wifiSSID, wifiPassword) != WL_CONNECTED) {
    // failed, retry
    Serial.println("Connecting to WIFI");
    delay(5000);
  }
  Serial.println("Connected to WIFI");

  Serial.print("Connecting to the MQTT broker: ");
  Serial.print(mqttBroker);
  Serial.print(":");
  Serial.println(mqttPort);

  mqttClient.setUsernamePassword(mqttBrokerUsername, mqttBrokerPassword);

  if (!mqttClient.connect(mqttBroker, mqttPort)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  mqttClient.onMessage(onMessage);
  mqttClient.subscribe(TOPIC_LED_0);
  mqttClient.subscribe(TOPIC_LED_1);
  mqttClient.subscribe(TOPIC_LED_2);
  mqttClient.subscribe(TOPIC_LED_3);
  mqttClient.subscribe(TOPIC_LED_4);
  mqttClient.subscribe(TOPIC_LED_5);
  mqttClient.subscribe(TOPIC_LED_6);
  mqttClient.subscribe(TOPIC_LED_7);
  mqttClient.subscribe(TOPIC_SEMAPHORE_0);
  mqttClient.subscribe(TOPIC_SEMAPHORE_1);
  mqttClient.subscribe(TOPIC_SEMAPHORE_2);
  mqttClient.subscribe(TOPIC_SEMAPHORE_3);
  mqttClient.subscribe(TOPIC_SEMAPHORE_4);
  mqttClient.subscribe(TOPIC_SEMAPHORE_5);
  mqttClient.subscribe(TOPIC_SEMAPHORE_6);
  mqttClient.subscribe(TOPIC_SEMAPHORE_7);
//  mqttClient.subscribe(TOPIC_BUTTON_ENABLE_0);
//  mqttClient.subscribe(TOPIC_BUTTON_ENABLE_1);
//  mqttClient.subscribe(TOPIC_BUTTON_ENABLE_2);
//  mqttClient.subscribe(TOPIC_BUTTON_ENABLE_3);
//  mqttClient.subscribe(TOPIC_BUTTON_ENABLE_4);
//  mqttClient.subscribe(TOPIC_BUTTON_ENABLE_5);
  Serial.println("Connected to the MQTT broker"); 
}

void monitorButton(int buttonPosition, int tlacitkovyPin, String topic) {
//  if (buttonsEnabled[buttonPosition] == false) {
//    return;
//  }
  if (digitalRead(tlacitkovyPin) == LOW) {
    mqttClient.beginMessage(topic);
    mqttClient.print("pressed");
    mqttClient.endMessage();
    delay(200);
    while (digitalRead(tlacitkovyPin) == LOW) {
      // wait
    }
  }
}

void loop() {
  mqttClient.poll();

  monitorButton(0, TLACITKOVY_PIN_0, TOPIC_BUTTON_0);
  monitorButton(1, TLACITKOVY_PIN_1, TOPIC_BUTTON_1);
  monitorButton(2, TLACITKOVY_PIN_2, TOPIC_BUTTON_2);
  monitorButton(3, TLACITKOVY_PIN_3, TOPIC_BUTTON_3);
  monitorButton(4, TLACITKOVY_PIN_4, TOPIC_BUTTON_4);
  monitorButton(5, TLACITKOVY_PIN_5, TOPIC_BUTTON_5);
}

uint8_t hexChar(int8_t singleChar) {
  if (singleChar >= '0' && singleChar <= '9') {
    return singleChar - '0';
  } else if (singleChar >= 'A' && singleChar <= 'F') {
    return singleChar - 'A' + 10;
  } else if (singleChar >= 'a' && singleChar <= 'f') {
    return singleChar - 'a' + 10;
  }
  return 0;
}

void processLedMessage(int messageSize, CRGB *currentLedArray) {
  if (messageSize % 7 != 6) {
    Serial.println("Invalid message");
    return;
  }

  int ledPosition = 0;
  while (messageSize > 0) {
    char colorCode[6]; 
    for (int i=0; i<6; i++) {
      int8_t mqttByte = (int8_t)mqttClient.read();
      colorCode[i] = mqttByte;
    }
    uint8_t red = hexChar(colorCode[0]) * 16 + hexChar(colorCode[1]);
    uint8_t green = hexChar(colorCode[2]) * 16 + hexChar(colorCode[3]);
    uint8_t blue = hexChar(colorCode[4]) * 16 + hexChar(colorCode[5]);
    Serial.print(red);
    Serial.print(",");
    Serial.print(green);
    Serial.print(",");
    Serial.println(blue);
    CRGB color = CRGB(red, green, blue);
    currentLedArray[ledPosition] = color;
  
    messageSize = messageSize - 6;
  
    if (messageSize > 0) {
      messageSize--;
      int8_t mqttByteComma = (int8_t)mqttClient.read();
    }
    ledPosition++;
  }

  FastLED.show();
}

void processSemaphoreMessage(int messageSize, CRGB *currentLedArray) {
  if (messageSize != 3) {
    Serial.println("Invalid message");
    return;
  }

  int8_t semaphoreCommand = (int8_t)mqttClient.read();
  int8_t semaphorePosition0 = (int8_t)mqttClient.read() - '0';
  int8_t semaphorePosition1 = (int8_t)mqttClient.read() - '0';
  int ledPosition = semaphorePosition0 * 10 + semaphorePosition1;

  if (semaphoreCommand == 'R' || semaphoreCommand == 'r') {
    currentLedArray[ledPosition++] = CRGB(0, 0, 0);
    currentLedArray[ledPosition++] = CRGB(0, 0, 0);
    currentLedArray[ledPosition++] = CRGB(255, 0, 0);
  } else if (semaphoreCommand == 'P' || semaphoreCommand == 'p') {
    currentLedArray[ledPosition++] = CRGB(0, 0, 0);
    currentLedArray[ledPosition++] = CRGB(255, 96, 0);
    currentLedArray[ledPosition++] = CRGB(255, 0, 0);
  } else if (semaphoreCommand == 'G' || semaphoreCommand == 'g') {
    currentLedArray[ledPosition++] = CRGB(0, 255, 0);
    currentLedArray[ledPosition++] = CRGB(0, 0, 0);
    currentLedArray[ledPosition++] = CRGB(0, 0, 0);
  } else if (semaphoreCommand == 'O' || semaphoreCommand == 'o') {
    currentLedArray[ledPosition++] = CRGB(0, 0, 0);
    currentLedArray[ledPosition++] = CRGB(255, 96, 0);
    currentLedArray[ledPosition++] = CRGB(0, 0, 0);
  } else if (semaphoreCommand == 'D' || semaphoreCommand == 'd') {
    currentLedArray[ledPosition++] = CRGB(0, 0, 0);
    currentLedArray[ledPosition++] = CRGB(255, 0, 0);
  } else if (semaphoreCommand == 'W' || semaphoreCommand == 'w') {
    currentLedArray[ledPosition++] = CRGB(0, 255, 0);
    currentLedArray[ledPosition++] = CRGB(0, 0, 0);
  }

  FastLED.show();
}

//void processButtonEnableMessage(int messageSize, int buttonPosition) {
//  if (messageSize != 2 && messageSize != 3) {
//    Serial.print("Invalid message of size ");
//    Serial.println(messageSize);
//    return ;
//  }
//
//  int8_t enableCommand0 = (int8_t)mqttClient.read();
//  int8_t enableCommand1 = (int8_t)mqttClient.read();
//  if (messageSize == 2 && enableCommand0 == 'o' && enableCommand1 == 'n') {
//    buttonsEnabled[buttonPosition] = true;
//    return;
//  } else if ((messageSize == 3) && (enableCommand0 == 'o') && (enableCommand1 == 'f') && (((int8_t)mqttClient.read()) == 'f')) {
//    buttonsEnabled[buttonPosition] = false;
//    return;
//  } else {
//    Serial.println("Invalid message");
//  }
//}

void onMessage(int messageSize) {
  if (mqttClient.messageTopic() == TOPIC_LED_0) {
    processLedMessage(messageSize, ledky_0);
  } else if (mqttClient.messageTopic() == TOPIC_LED_1) {
    processLedMessage(messageSize, ledky_1);
  } else if (mqttClient.messageTopic() == TOPIC_LED_2) {
    processLedMessage(messageSize, ledky_2);
  } else if (mqttClient.messageTopic() == TOPIC_LED_3) {
    processLedMessage(messageSize, ledky_3);
  } else if (mqttClient.messageTopic() == TOPIC_LED_4) {
    processLedMessage(messageSize, ledky_4);
  } else if (mqttClient.messageTopic() == TOPIC_LED_5) {
    processLedMessage(messageSize, ledky_5);
  } else if (mqttClient.messageTopic() == TOPIC_LED_6) {
    processLedMessage(messageSize, ledky_6);
  } else if (mqttClient.messageTopic() == TOPIC_LED_7) {
    processLedMessage(messageSize, ledky_7);
  }

  if (mqttClient.messageTopic() == TOPIC_SEMAPHORE_0) {
    processSemaphoreMessage(messageSize, ledky_0);
  } else if (mqttClient.messageTopic() == TOPIC_SEMAPHORE_1) {
    processSemaphoreMessage(messageSize, ledky_1);
  } else if (mqttClient.messageTopic() == TOPIC_SEMAPHORE_2) {
    processSemaphoreMessage(messageSize, ledky_2);
  } else if (mqttClient.messageTopic() == TOPIC_SEMAPHORE_3) {
    processSemaphoreMessage(messageSize, ledky_3);
  } else if (mqttClient.messageTopic() == TOPIC_SEMAPHORE_4) {
    processSemaphoreMessage(messageSize, ledky_4);
  } else if (mqttClient.messageTopic() == TOPIC_SEMAPHORE_5) {
    processSemaphoreMessage(messageSize, ledky_5);
  } else if (mqttClient.messageTopic() == TOPIC_SEMAPHORE_6) {
    processSemaphoreMessage(messageSize, ledky_6);
  } else if (mqttClient.messageTopic() == TOPIC_SEMAPHORE_7) {
    processSemaphoreMessage(messageSize, ledky_7);
  }  

//  if (mqttClient.messageTopic() == TOPIC_BUTTON_ENABLE_0) {
//    processButtonEnableMessage(messageSize, 0);
//  } else if (mqttClient.messageTopic() == TOPIC_BUTTON_ENABLE_1) {
//    processButtonEnableMessage(messageSize, 1);
//  } else if (mqttClient.messageTopic() == TOPIC_BUTTON_ENABLE_2) {
//    processButtonEnableMessage(messageSize, 2);
//  } else if (mqttClient.messageTopic() == TOPIC_BUTTON_ENABLE_3) {
//    processButtonEnableMessage(messageSize, 3);
//  } else if (mqttClient.messageTopic() == TOPIC_BUTTON_ENABLE_4) {
//    processButtonEnableMessage(messageSize, 4);
//  } else if (mqttClient.messageTopic() == TOPIC_BUTTON_ENABLE_5) {
//    processButtonEnableMessage(messageSize, 5);
//  }
}
