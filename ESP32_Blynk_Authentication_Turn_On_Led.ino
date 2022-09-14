/*************************************************************

  You’ll need:
   - Blynk IoT app (download from App Store or Google Play)
   - ESP32 board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)

  There is a bunch of great example sketches included to show you how to get
  started. Think of them as LEGO bricks  and combine them as you wish.
  For example, take the Ethernet Shield sketch and combine it with the
  Servo example, or choose a USB sketch and add a code from SendData
  example.
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLmbdI9t3f"
#define BLYNK_DEVICE_NAME "Drone Controller"
#define BLYNK_AUTH_TOKEN  "CP9e4Ja46xcDtQO39ayeuUZMlWU5hMmc"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Xfab-Visitor";
char pass[] = "xfab%79415";

WidgetLED led1(V1);
BlynkTimer timer;

void blinkLedWidget()  // function for switching off and on LED
{
  if (led1.getValue()) {
    led1.off();
    digitalWrite(LED_BUILTIN, LOW)
    Serial.println("LED on V1: off");
  } else {
    led1.on();
    digitalWrite(LED_BUILTIN, HIGH); 
    Serial.println("LED on V1: on");
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);  
  Blynk.begin(auth, ssid, pass);
  pinMode(LED_BUILTIN, OUTPUT);
  timer.setInterval(1000L, blinkLedWidget);

  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
