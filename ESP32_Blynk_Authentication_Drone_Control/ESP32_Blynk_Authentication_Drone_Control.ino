/*************************************************************

  You’ll need:
   - Blynk IoT app (download from App Store or Google Play)
   - ESP32 board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)
     
  
  This configuration is based on FlySky Fs-i6 Mode 1 as follow:
  CH1 = Aileron (Roll) 
  CH2 = Elevator (Pitch)
  CH3 = Throttle
  CH4 = Rudder (Yaw)

 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings

// For schematic - Connect ESP pin 14 to Pixhawk Signal, Gnd to Gnd and Powersupply to Vin since we're powering ESP with Pixhawk Power

#define PPM_FRAME_LENGTH 22500
#define PPM_PULSE_LENGTH 300
#define PPM_CHANNELS 6
#define DEFAULT_CHANNEL_VALUE 1500
#define OUTPUT_PIN 14

uint16_t channelValue[PPM_CHANNELS]={1500, 1500, 1050, 1500, 1500, 1500};

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

enum ppmState_e
{
    PPM_STATE_IDLE,
    PPM_STATE_PULSE,
    PPM_STATE_FILL,
    PPM_STATE_SYNC
};


#define BLYNK_TEMPLATE_ID "TMPLmbdI9t3f"
#define BLYNK_DEVICE_NAME "Drone Controller"
#define BLYNK_AUTH_TOKEN "CP9e4Ja46xcDtQO39ayeuUZMlWU5hMmc"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "WLAN1-0762EH";
char pass[] = "Lukas@92";



//Arm
BLYNK_WRITE(V0) {
  int arm = param.asInt();
  if (arm == 1)
  {
    channelValue[1] = 2002;
    channelValue[2] = 1057;
    channelValue[3] = 2002;
    digitalWrite(LED_BUILTIN, arm);

  }

  else {
    channelValue[1] = 1500;
    channelValue[3] = 1500;
    channelValue[2] = 1057;
  }
  Serial.print("Copter Armed"); 
  Serial.println();

}

// Disarm
BLYNK_WRITE(V5) {
  int disarm = param.asInt();
  if (disarm == 1)
  {
    channelValue[1] = 2002;
    channelValue[2] = 1057;
    channelValue[3] = 1000;
    digitalWrite(LED_BUILTIN, 0);
  }

  else {
    channelValue[1] = 1500;
    channelValue[3] = 1500;
    channelValue[2] = 1057;
  }
  Serial.print("Copter Disarmed");
  Serial.println();
}

// Alt Hold
BLYNK_WRITE(V7) {
  int althold = param.asInt();
  if (althold == 1)
  {
    channelValue[4] = 1265;
  }
}

// Stabilize
BLYNK_WRITE(V12) {
  int stabilize = param.asInt();
  if (stabilize == 1)
  {
    channelValue[4] = 1105;
  }
}

// Loiter
BLYNK_WRITE(V8) {
  int loiter = param.asInt();
  if (loiter == 1)
  {
    channelValue[4] = 1426;
  }
}

// Guided
BLYNK_WRITE(V9) {
  int guided = param.asInt();
  if (guided == 1)
  {
    channelValue[4] = 1902;
  }
}

// RTL
BLYNK_WRITE(V11) {
  int rtl = param.asInt();
  if (rtl == 1)
  {
    channelValue[4] = 1748;
  }
}

// Land
BLYNK_WRITE(V10) {
  int land = param.asInt();
  if (land == 1)
  {
    channelValue[4] = 1582;
  }
}

// Roll Input
BLYNK_WRITE(V1) {
  //int pinvalueA = param.asInt();
  channelValue[0] =  param.asInt();
  Serial.print("CH1:");
  Serial.print(channelValue[0]);
  Serial.println();
}

// Pitch Input
BLYNK_WRITE(V2) {
  int pinvalueB = param.asInt();
  channelValue[1] = map(pinvalueB,2000,1000,1000,2000);
  Serial.print("CH2:");
  Serial.print(channelValue[1]);
  Serial.println();
}

// Throttle
BLYNK_WRITE(V3) {
  //int pinValueC= param.asInt();
  //channelValue[2]  = map(pinValueC,1000,2000,1050,1900);
  channelValue[2] = param.asInt();
  Serial.print("CH3:");
  Serial.print(channelValue[2]);
  Serial.println();
}

// Yaw Input
BLYNK_WRITE(V4) {
  //int pinvalueD = param.asInt();
  channelValue[3] = param.asInt();
  Serial.print("CH4:");
  Serial.print(channelValue[3]);
  Serial.println();
  //digitalWrite(LED_BUILTIN, pinvalueD);
}

void IRAM_ATTR onPpmTimer()
{

    static uint8_t ppmState = PPM_STATE_IDLE;
    static uint8_t ppmChannel = 0;
    static uint8_t ppmOutput = LOW;
    static int usedFrameLength = 0;
    int currentChannelValue;

    portENTER_CRITICAL(&timerMux);

    if (ppmState == PPM_STATE_IDLE)
    {
        ppmState = PPM_STATE_PULSE;
        ppmChannel = 0;
        usedFrameLength = 0;
        ppmOutput = LOW;
    }

    if (ppmState == PPM_STATE_PULSE)
    {
        ppmOutput = HIGH;
        usedFrameLength += PPM_PULSE_LENGTH;
        ppmState = PPM_STATE_FILL;

        timerAlarmWrite(timer, PPM_PULSE_LENGTH, true);
    }
    else if (ppmState == PPM_STATE_FILL)
    {
        ppmOutput = LOW;
        currentChannelValue = channelValue[ppmChannel];

        ppmChannel++;
        ppmState = PPM_STATE_PULSE;

        if (ppmChannel >= PPM_CHANNELS)
        {
            ppmChannel = 0;
            timerAlarmWrite(timer, PPM_FRAME_LENGTH - usedFrameLength, true);
            usedFrameLength = 0;
        }
        else
        {
            usedFrameLength += currentChannelValue - PPM_PULSE_LENGTH;
            timerAlarmWrite(timer, currentChannelValue - PPM_PULSE_LENGTH, true);
        }
    }
    portEXIT_CRITICAL(&timerMux);
    digitalWrite(OUTPUT_PIN, ppmOutput);
}


void setup()
{
  // Debug console
  Serial.begin(115200);  
  Blynk.begin(auth, ssid, pass);

  pinMode(OUTPUT_PIN, OUTPUT);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onPpmTimer, true);
  timerAlarmWrite(timer, 12000, true);
  timerAlarmEnable(timer);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  Blynk.run();
  //PPM_width_values();
  // timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
