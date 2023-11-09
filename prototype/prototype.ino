#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <TimeLib.h>
#include <NTPClient.h>
#include <vector>

const int trigPin[] = {18, 13, 17, 20, 11, 15, 35, 7, 6};
const int echoPin[] = {19, 12, 16, 21, 10, 14, 36, 8, 5};
const int numOfSensors = 9;
double duration, distance;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String formattedDate;
String dayStamp;
String timeStamp;

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    // Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

std::vector<double> collectSensorData() {
  std::vector<double> distances;
  for (int i = 0; i < numOfSensors; i++) {
    digitalWrite(trigPin[i], LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin[i], LOW);
    duration = pulseIn(echoPin[i], HIGH);
    distance = (duration * 0.0343) / 2;

    if (distance > 30) {
      distance = 30;
    }

    distances.push_back(30 - distance);
    delay(100);
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

  WiFi.begin("Myhres", "krem1234");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Serial.println("Connecting to WiFi...");
  }

  // Serial.println("Connected to WiFi");

  config.api_key = "AIzaSyA0g3v93bv8A6aWR1ND2C5dcRxz2aRNRXg";
  auth.user.email = "esp@32.com";
  auth.user.password = "123456";

  Firebase.reconnectNetwork(true);

  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  // Initialize an NTPClient to get time
  timeClient.begin();
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if (Firebase.ready() && buttonState == HIGH) {
    // Serial.println("Button pressed! Collecting sensor data...");
    std::vector<double> distances = collectSensorData();
    timeClient.update();

    // Get the epoch time (seconds since January 1, 1970)
    unsigned long epochTime = timeClient.getEpochTime();
    // Serial.println(epochTime);


    FirebaseJson content;
    String documentPath = "sensorData/" + String(epochTime);

    for (int i = 0; i < 9; i++) {
      String dataPath = "fields/sensors/arrayValue/values/[" + String(i) + "]/doubleValue";
      content.set(dataPath, String(distances[i]));
    }

    // if (Firebase.Firestore.createDocument(&fbdo, "elsysgk", "", documentPath.c_str(), content.raw()))
    //   Serial.println("Data uploaded successfully");
    // else
    //   Serial.println("Failed to upload data");

    delay(1000);
  }

  delay(100);
}
