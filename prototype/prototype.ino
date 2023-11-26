#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <TimeLib.h>
#include <NTPClient.h>
#include <vector>

const int trigPin[] = {18, 13, 17, 20, 11, 15, 35, 7, 6};
const int echoPin[] = {19, 12, 16, 21, 10, 14, 36, 8, 5};
const int numOfSensors = 9;
const int numMeasurements = 100;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String formattedDate;
String dayStamp;
String timeStamp;

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

std::vector<double> collectSensorData() {
  std::vector<double> distances;
  double duration;
  for (int i = 0; i < numOfSensors; i++) {
    double distance = 0;
    Serial.println(i);
    for (int j = 0; j < numMeasurements; j++) {
      digitalWrite(trigPin[i], LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin[i], HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin[i], LOW);
      duration = pulseIn(echoPin[i], HIGH);
      Serial.print((duration * 0.0343) / 2);
      Serial.print(" ");
      distance += ((duration * 0.0343) / 2)/numMeasurements;
      delay(10);
    }
    Serial.println();
    Serial.print("Mean: ");
    Serial.println(distance);

    if (distance > 30) {
      distance = 30;
    }

    distances.push_back(30 - distance);
  }
  return distances;
}

const int buttonPin = 2;
int buttonState = 0;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 9; i++) {
    pinMode(trigPin[i], OUTPUT);
    pinMode(echoPin[i], INPUT);
  }

  pinMode(buttonPin, INPUT);

  WiFi.begin("NETWORK", "NETWORK PASSWORD");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  config.api_key = "KEY";
  auth.user.email = "USER EMAIL";
  auth.user.password = "USER PASSWORD";

  Firebase.reconnectNetwork(true);

  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  // Initialize an NTPClient to get time
  timeClient.begin();
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    if (Firebase.ready() && buttonState == HIGH) {
      Serial.println("Button pressed! Collecting sensor data...");
      std::vector<double> distances = collectSensorData();
      timeClient.update();

      // Get the epoch time (seconds since January 1, 1970)
      unsigned long epochTime = timeClient.getEpochTime();
      Serial.println(epochTime);


      FirebaseJson content;
      String documentPath = "sensorData/" + String(epochTime);

      for (int i = 0; i < 9; i++) {
        String dataPath = "fields/sensors/arrayValue/values/[" + String(i) + "]/doubleValue";
        content.set(dataPath, String(distances[i]));
      }

      if (Firebase.Firestore.createDocument(&fbdo, "elsysgk", "", documentPath.c_str(), content.raw()))
        Serial.println("Data uploaded successfully");
      else
        Serial.println("Failed to upload data");

      delay(1000);
    }

    delay(100);
  }
}
