#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <Servo.h>

#define LEDKOVY_PIN_0 13
#define LEDKOVY_PIN_1 12
#define LEDKOVY_PIN_2 8
#define LEDKOVY_PIN_3 7

#define SIRENE_PIN 3

#define MOTION_DETECTION_PIN_0 A0
#define MOTION_DETECTION_PIN_1 A1
#define MOTION_DETECTION_PIN_2 A2
#define MOTION_DETECTION_PIN_3 A3

#define BARRIER_0_PIN 10
#define BARRIER_1_PIN 9

char wifiSSID[] = "Karak";        // your network SSID (name)
char wifiPassword[] = "123qweASD";    // your network password (use for WPA, or use as key for WEP)

WiFiSSLClient wifiClient;
MqttClient mqttClient(wifiClient);

const char mqttBroker[]         = "d57a0d1c39d54550b147b58411d86743.s2.eu.hivemq.cloud";
const int  mqttPort             = 8883;
const char mqttBrokerUsername[] = "robot";
const char mqttBrokerPassword[] = "P@ssW0rd!";

#define TOPIC_LED_0 "/rail-crossing/led/13"
#define TOPIC_LED_1 "/rail-crossing/led/12"
#define TOPIC_LED_2 "/rail-crossing/led/8"
#define TOPIC_LED_3 "/rail-crossing/led/7"
#define TOPIC_SIRENE "/rail-crossing/sirene"
#define TOPIC_BARRIER_0 "/rail-crossing/barrier/10"
#define TOPIC_BARRIER_1 "/rail-crossing/barrier/9"
#define TOPIC_MOTION_DETECTION_0 "/rail-crossing/motion-detection/A0"
#define TOPIC_MOTION_DETECTION_1 "/rail-crossing/motion-detection/A1"
#define TOPIC_MOTION_DETECTION_2 "/rail-crossing/motion-detection/A2"
#define TOPIC_MOTION_DETECTION_3 "/rail-crossing/motion-detection/A3"
// #define TOPIC_MOTION_DETECTION_ENABLE_0 "/rail-crossing/motion-detection-enable/A0"
// #define TOPIC_MOTION_DETECTION_ENABLE_1 "/rail-crossing/motion-detection-enable/A1"
// #define TOPIC_MOTION_DETECTION_ENABLE_2 "/rail-crossing/motion-detection-enable/A2"
// #define TOPIC_MOTION_DETECTION_ENABLE_3 "/rail-crossing/motion-detection-enable/A3"

//boolean motionDetectionEnabled[4];

#define BARRIER_0_VYCHOZI_POZICE 45
#define BARRIER_0_STEP_SIZE 1
#define BARRIER_0_STEPS 50
#define BARRIER_1_VYCHOZI_POZICE 45
#define BARRIER_1_STEP_SIZE 1
#define BARRIER_1_STEPS 50

Servo servoBarrier0;
int servoBarrier0Position = BARRIER_0_VYCHOZI_POZICE;
int barrier0Motion = 0;

Servo servoBarrier1;
int servoBarrier1Position = BARRIER_1_VYCHOZI_POZICE;
int barrier1Motion = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }
  
  pinMode(LEDKOVY_PIN_0, OUTPUT);
  pinMode(LEDKOVY_PIN_1, OUTPUT);
  pinMode(LEDKOVY_PIN_2, OUTPUT);
  pinMode(LEDKOVY_PIN_3, OUTPUT);

  pinMode(MOTION_DETECTION_PIN_0, INPUT_PULLUP);
  pinMode(MOTION_DETECTION_PIN_1, INPUT_PULLUP);
  pinMode(MOTION_DETECTION_PIN_2, INPUT_PULLUP);
  pinMode(MOTION_DETECTION_PIN_3, INPUT_PULLUP);

  servoBarrier0.attach(BARRIER_0_PIN);
  servoBarrier0.write(servoBarrier0Position);

  servoBarrier1.attach(BARRIER_1_PIN);
  servoBarrier1.write(servoBarrier1Position);

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
  mqttClient.subscribe(TOPIC_SIRENE);
  mqttClient.subscribe(TOPIC_BARRIER_0);
  mqttClient.subscribe(TOPIC_BARRIER_1);
  //mqttClient.subscribe(TOPIC_MOTION_DETECTION_ENABLE_0);
  //mqttClient.subscribe(TOPIC_MOTION_DETECTION_ENABLE_1);
  //mqttClient.subscribe(TOPIC_MOTION_DETECTION_ENABLE_2);
  //mqttClient.subscribe(TOPIC_MOTION_DETECTION_ENABLE_3);
  Serial.println("Connected to the MQTT broker"); 
}

void monitorButton(int buttonPosition, int tlacitkovyPin, String topic) {
//  if (motionDetectionEnabled[buttonPosition] == false) {
//    return;
//  }
  if (digitalRead(tlacitkovyPin) == LOW) {
    Serial.print("Button pressed ");
    Serial.println(tlacitkovyPin);
    mqttClient.beginMessage(topic);
    mqttClient.print("ongoing");
    mqttClient.endMessage();
    delay(200);
    while (digitalRead(tlacitkovyPin) == LOW) {
      // wait
    }
  }
}

void loop() {
  mqttClient.poll();

  monitorButton(0, MOTION_DETECTION_PIN_0, TOPIC_MOTION_DETECTION_0);
  monitorButton(1, MOTION_DETECTION_PIN_1, TOPIC_MOTION_DETECTION_1);
  monitorButton(2, MOTION_DETECTION_PIN_2, TOPIC_MOTION_DETECTION_2);
  monitorButton(3, MOTION_DETECTION_PIN_3, TOPIC_MOTION_DETECTION_3);

  if (barrier0Motion != 0 || barrier1Motion != 0) {
    if (barrier0Motion > 0) {
      servoBarrier0.write(servoBarrier0Position);
      servoBarrier0Position += BARRIER_0_STEP_SIZE;
      barrier0Motion--;
    }
    if (barrier0Motion < 0) {
      servoBarrier0.write(servoBarrier0Position);
      servoBarrier0Position -= BARRIER_0_STEP_SIZE;
      barrier0Motion++;
    }
    if (barrier1Motion > 0) {
      servoBarrier1.write(servoBarrier1Position);
      servoBarrier1Position += BARRIER_1_STEP_SIZE;
      barrier1Motion--;
    }
    if (barrier1Motion < 0) {
      servoBarrier1.write(servoBarrier1Position);
      servoBarrier1Position -= BARRIER_1_STEP_SIZE;
      barrier1Motion++;
    }
    delay(100);
  }
}

void processLedMessage(int messageSize, int ledPin) {
  if (messageSize != 2 && messageSize != 3) {
    Serial.print("Invalid message of size ");
    Serial.println(messageSize);
    return;
  }

  int8_t enableCommand0 = (int8_t)mqttClient.read();
  int8_t enableCommand1 = (int8_t)mqttClient.read();
  if (messageSize == 2 && enableCommand0 == 'o' && enableCommand1 == 'n') {
    digitalWrite(ledPin, HIGH);
    return;
  } else if ((messageSize == 3) && (enableCommand0 == 'o') && (enableCommand1 == 'f') && (((int8_t)mqttClient.read()) == 'f')) {
    digitalWrite(ledPin, LOW);
    return;
  } else {
    Serial.println("Invalid message");
  }
}

void processSireneMessage(int messageSize) {
  if (messageSize != 4) {
    Serial.print("Invalid message of size ");
    Serial.println(messageSize);
    return;
  }

  int8_t c0 = (int8_t)mqttClient.read();
  int8_t c1 = (int8_t)mqttClient.read();
  int8_t c2 = (int8_t)mqttClient.read();
  int8_t c3 = (int8_t)mqttClient.read();
  if (c0 != 'b' || c1 != 'e' || c2 != 'e' || c3 != 'p') {
    Serial.print("Invalid message of size ");
    Serial.print((char)c0);
    Serial.print((char)c1);
    Serial.print((char)c2);
    Serial.println((char)c3);
    return;
  }

  tone(SIRENE_PIN, 1000, 200);
}

void processBarrier0Message(int messageSize) {
  if (messageSize != 4 && messageSize != 5) {
    Serial.print("Invalid message of size ");
    Serial.println(messageSize);
    return;
  }

  int8_t c0 = (int8_t)mqttClient.read();
  int8_t c1 = (int8_t)mqttClient.read();
  int8_t c2 = (int8_t)mqttClient.read();
  int8_t c3 = (int8_t)mqttClient.read();
  if (c0 == 'c' && c1 == 'l' && c2 == 'o' && c3 == 's') {
    if (servoBarrier0Position >= BARRIER_0_VYCHOZI_POZICE + BARRIER_0_STEPS) {
      barrier0Motion = -BARRIER_0_STEPS;
      Serial.println("Barrier 0 going down");
    }
    return;
  } else if (c0 == 'o' && c1 == 'p' && c2 == 'e' && c3 == 'n') {
    int8_t c4 = (int8_t)mqttClient.read();
    if (servoBarrier0Position <= BARRIER_0_VYCHOZI_POZICE) {
      barrier0Motion = BARRIER_0_STEPS;
      Serial.println("Barrier 0 going up");
    }
    return;
  } else {
    Serial.print("Invalid message of size ");
    Serial.print((char)c0);
    Serial.print((char)c1);
    Serial.print((char)c2);
    Serial.println((char)c3);
  }
}

void processBarrier1Message(int messageSize) {
  if (messageSize != 4 && messageSize != 5) {
    Serial.print("Invalid message of size ");
    Serial.println(messageSize);
    return;
  }

  int8_t c0 = (int8_t)mqttClient.read();
  int8_t c1 = (int8_t)mqttClient.read();
  int8_t c2 = (int8_t)mqttClient.read();
  int8_t c3 = (int8_t)mqttClient.read();
  if (c0 == 'c' && c1 == 'l' && c2 == 'o' && c3 == 's') {
    if (servoBarrier1Position >= BARRIER_1_VYCHOZI_POZICE + BARRIER_1_STEPS) {
      barrier1Motion = -BARRIER_1_STEPS;
      Serial.println("Barrier 1 going down");
    }
    return;
  } else if (c0 == 'o' && c1 == 'p' && c2 == 'e' && c3 == 'n') {
    int8_t c4 = (int8_t)mqttClient.read();
    if (servoBarrier1Position <= BARRIER_1_VYCHOZI_POZICE) {
      barrier1Motion = BARRIER_1_STEPS;
      Serial.println("Barrier 1 going up");
    }
    return;
  } else {
    Serial.print("Invalid message of size ");
    Serial.print((char)c0);
    Serial.print((char)c1);
    Serial.print((char)c2);
    Serial.println((char)c3);
  }
}

//void processButtonEnableMessage(int messageSize, int buttonPosition) {
//  if (messageSize != 2 && messageSize != 3) {
//    Serial.print("Invalid message of size ");
//    Serial.println(messageSize);
//    return;
//  }
//
//  int8_t enableCommand0 = (int8_t)mqttClient.read();
//  int8_t enableCommand1 = (int8_t)mqttClient.read();
//  if (messageSize == 2 && enableCommand0 == 'o' && enableCommand1 == 'n') {
//    motionDetectionEnabled[buttonPosition] = true;
//    return;
//  } else if ((messageSize == 3) && (enableCommand0 == 'o') && (enableCommand1 == 'f') && (((int8_t)mqttClient.read()) == 'f')) {
//    motionDetectionEnabled[buttonPosition] = false;
//    return;
//  } else {
//    Serial.println("Invalid message");
//  }
//}

void onMessage(int messageSize) {
  if (mqttClient.messageTopic() == TOPIC_LED_0) {
    processLedMessage(messageSize, LEDKOVY_PIN_0);
  } else if (mqttClient.messageTopic() == TOPIC_LED_1) {
    processLedMessage(messageSize, LEDKOVY_PIN_1);
  } else if (mqttClient.messageTopic() == TOPIC_LED_2) {
    processLedMessage(messageSize, LEDKOVY_PIN_2);
  } else if (mqttClient.messageTopic() == TOPIC_LED_3) {
    processLedMessage(messageSize, LEDKOVY_PIN_3);
  }

  if (mqttClient.messageTopic() == TOPIC_SIRENE) {
    processSireneMessage(messageSize);
  }

  if (mqttClient.messageTopic() == TOPIC_BARRIER_0) {
    processBarrier0Message(messageSize);
  } else if (mqttClient.messageTopic() == TOPIC_BARRIER_1) {
    processBarrier1Message(messageSize);
  }

//  if (mqttClient.messageTopic() == TOPIC_MOTION_DETECTION_ENABLE_0) {
//    processButtonEnableMessage(messageSize, 0);
//  } else if (mqttClient.messageTopic() == TOPIC_MOTION_DETECTION_ENABLE_1) {
//    processButtonEnableMessage(messageSize, 1);
//  } else if (mqttClient.messageTopic() == TOPIC_MOTION_DETECTION_ENABLE_2) {
//    processButtonEnableMessage(messageSize, 2);
//  } else if (mqttClient.messageTopic() == TOPIC_MOTION_DETECTION_ENABLE_3) {
//    processButtonEnableMessage(messageSize, 3);
//  }
}
