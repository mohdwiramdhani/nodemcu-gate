#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Servo.h>

#define FIREBASE_HOST "skripsi-ba-parkir-99-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "leG7Sro0Irph334F4rSayx5I6rJOgdAUhFhLLHZ1"
#define WIFI_SSID "nt"
#define WIFI_PASSWORD "abogoboga"

Servo entranceServo;
Servo exitServo;
int entranceServoPosition = 1;
int exitServoPosition = 90;

int entranceInfraredSensorPin = D2;
int exitInfraredSensorPin = D3;

const int entranceGreenLEDPin = D4;
const int exitGreenLEDPin = D5;

const String ALLOWED_UID = "h08P4LepcqgAy0OmVROZxI0ppIw1";

void setup() {
  Serial.begin(9600);

  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected to WiFi. IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to Firebase");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.println("Connected to Firebase");

  digitalWrite(entranceGreenLEDPin, LOW);
  digitalWrite(exitGreenLEDPin, LOW);

  pinMode(entranceGreenLEDPin, OUTPUT);
  pinMode(exitGreenLEDPin, OUTPUT);

  pinMode(entranceInfraredSensorPin, INPUT);
  pinMode(exitInfraredSensorPin, INPUT);

  entranceServo.attach(D0);
  exitServo.attach(D1);
}

void loop() {

  int entranceRequestedPosition = Firebase.getInt("entranceGate/" + ALLOWED_UID + "/value");
  entranceRequestedPosition = constrain(entranceRequestedPosition, 1, 90);

  Serial.print("Value Entrance: ");
  Serial.println(entranceRequestedPosition);

  int entranceInfraredSensorValue = digitalRead(entranceInfraredSensorPin);
  Serial.print("EntranceInfrared Sensor Value: ");
  Serial.println(entranceInfraredSensorValue == LOW ? "LOW" : "HIGH");

  if (entranceRequestedPosition != entranceServoPosition) {

    Serial.print("Connecting Entrance");
    entranceServoPosition = entranceRequestedPosition;
    entranceServo.write(entranceServoPosition);

    if (entranceRequestedPosition == 1) {
      digitalWrite(entranceGreenLEDPin, HIGH);
      Serial.println("Entrance LED ON");
    } else {
      digitalWrite(entranceGreenLEDPin, LOW);
      Serial.println("Entrance LED OFF");
    }

  }

  if (entranceInfraredSensorValue == LOW) {
    Firebase.remove("entranceGate/" + ALLOWED_UID);
    Firebase.setInt("entranceGate/" + ALLOWED_UID + "/value", 90);
    Serial.println("Detect Entrance");
  }

  int exitRequestedPosition = Firebase.getInt("exitGate/" + ALLOWED_UID + "/value");
  exitRequestedPosition = constrain(exitRequestedPosition, 1, 90);

  Serial.print("Value Exit: ");
  Serial.println(exitRequestedPosition);

  int exitInfraredSensorValue = digitalRead(exitInfraredSensorPin);
  Serial.print("ExitInfrared Sensor Value: ");
  Serial.println(exitInfraredSensorValue == LOW ? "LOW" : "HIGH");

  if (exitRequestedPosition != exitServoPosition) {

    Serial.print("Connecting Exit");
    exitServoPosition = exitRequestedPosition;
    exitServo.write(exitServoPosition);

    if (exitRequestedPosition == 90) {
      digitalWrite(exitGreenLEDPin, HIGH);
      Serial.println("Exit LED ON");
    } else {
      digitalWrite(exitGreenLEDPin, LOW);
      Serial.println("Exit LED OFF");
    }

  }

  if (exitInfraredSensorValue == LOW) {
    Firebase.remove("exitGate/" + ALLOWED_UID);
    Firebase.setInt("exitGate/" + ALLOWED_UID + "/value", 1);
    Serial.println("Detect Exit");
  }
  
  delay(1000);
}
