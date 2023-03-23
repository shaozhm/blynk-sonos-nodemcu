/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on NodeMCU.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right NodeMCU module
  in the Tools -> Board menu!

  For advanced settings please follow ESP examples :
   - ESP8266_Standalone_Manual_IP.ino
   - ESP8266_Standalone_SmartConfig.ino
   - ESP8266_Standalone_SSL.ino

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
//#define BLYNK_TEMPLATE_ID   "YourTemplateID"


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "QaW4U6_jzOKbthNeebh1mG2vGw_GStw_";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Zhimin_NG";
char pass[] = "gti21#Oracle";

char WiFi_hostname[] = "NSUB";

const int builtinLedPin = 16; //[output] D0 pin on NodeMCU - the builtin LED
const int musicPin = 5; //[output] D1 pin on NodeMCU - the music relay
const int relayPin = 15; //[output] D8 pin on NodeMCU - the power relay
const int controlPin = 13; //[input] D7 pin on NodeMCU, the signal comes from raspi, to control the music power 14.85V relay switch on D1 musicPin;
const int inputPin = 12; //[input] D6 pin on NodeMCU, the signal comes from raspi, to control the 5V power relay switch on D8 relayPin;

int ledState = HIGH;
int relayState = LOW;
int musicState = LOW;

BlynkTimer timer;
void checkPhysicalInput();

// Every time we connect to the cloud...
BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(V2); //refer to builtinLed D0 pin
  Blynk.syncVirtual(V3); //refer to relay D8 pin - 5V power relay
  Blynk.syncVirtual(V4); //refer to relay D1 pin - 14.85V music power relay

  // Alternatively, you could override server state using:
  //Blynk.virtualWrite(V2, ledState);
  //Blynk.virtualWrite(V3, relayState);
  //Blynk.virtualWrite(V4, musicState);
}

// When App button is pushed - switch the state
BLYNK_WRITE(V2) {
  ledState = param.asInt();
  digitalWrite(builtinLedPin, ledState);
}

// When App button is pushed - switch the state
BLYNK_WRITE(V3) {
  relayState = param.asInt();
  digitalWrite(relayPin, relayState);
}

// When App button is pushed - switch the state
 BLYNK_WRITE(V4) {
  musicState = param.asInt();
  digitalWrite(musicPin, musicState);
}

void checkPhysicalInput()
{
  if (relayState == HIGH) {
    if (musicState != digitalRead(controlPin)) {
      // Toggle relay state
      musicState = !musicState;
      digitalWrite(musicPin, musicState);

      // Update input Widget
      Blynk.virtualWrite(V4, musicState);
    }
    if (HIGH == digitalRead(inputPin)) {

      // clear musicState
      musicState = LOW;
      digitalWrite(musicPin, musicState);

      // Update input Widget
      Blynk.virtualWrite(V4, musicState);
      
      // Toggle relay state
      relayState = LOW;
      digitalWrite(relayPin, relayState);

      // Update input Widget
      Blynk.virtualWrite(V3, relayState);
    }
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  
  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  Blynk.begin(auth, ssid, pass, "sonospi.local", 8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  WiFi.hostname(WiFi_hostname);
  
  pinMode(builtinLedPin, OUTPUT);
  digitalWrite(builtinLedPin, ledState); //initialize the builtin LED to turn off

  pinMode(musicPin, OUTPUT);
  digitalWrite(musicPin, musicState); //initialize the music relay to turn off
  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayState); //initialize the power relay to turn off

  pinMode(controlPin, INPUT);
  pinMode(inputPin, INPUT);

  // Setup a function to be called every 100 ms
  timer.setInterval(100L, checkPhysicalInput);
}

void loop()
{
  Blynk.run();
  timer.run();
}
