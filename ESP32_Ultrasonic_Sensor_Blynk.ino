#define BLYNK_TEMPLATE_ID "TMPLkQmDQ6-s"
#define BLYNK_DEVICE_NAME "Ultrasonic Blynk"
#define BLYNK_AUTH_TOKEN "YOvCvQQOF1BT6wL2Jqng-ruu6rAjRM6q"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "WLAN1-0762EH";
char pass[] = "Lukas@92";

BlynkTimer timer;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

const int trigPin = 5;
const int echoPin = 18;

long duration;
float distanceCm;
float distanceInch;

void ultrasonic()
{
  // Clear the trigger Pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Set the TrigPin on High State for 10us 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;

  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  Blynk.virtualWrite(V5, distanceCm);
}

void setup() {
  Serial.begin(115200);
  
  Blynk.begin(auth, ssid, pass);
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  timer.setInterval(1000L, ultrasonic);

}

void loop() {
  Blynk.run();
  timer.run(); 
}
