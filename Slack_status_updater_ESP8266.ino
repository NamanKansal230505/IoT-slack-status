/*******************************************************************
    Sets a custom status on your slack account.
    
    By Becky Stern
    Based on Brian Lough's ArduinoSlackAPI library examples
    https://github.com/witnessmenow/arduino-slack-api

    You will need a Slack bearer token, see library readme for more details

    You will also need to be on version 2.5 or higher of the ESP8266 core

 *******************************************************************/

// ----------------------------
// Standard Libraries
// ----------------------------

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <ArduinoSlack.h>
// Library for connecting to the Slack API

// Install from Github
// https://github.com/witnessmenow/arduino-slack-api

#include <ArduinoJson.h>
// Library used for parsing Json from the API responses

// Search for "Arduino Json" in the Arduino Library manager
// https://github.com/bblanchon/ArduinoJson

//------- Replace the following! ------

char ssid[] = "SSID";         // your network SSID (name)
char password[] = "password"; // your network password

#define SLACK_ACCESS_TOKEN "AAAAAAAAAABBBBBBBBBBBCCCCCCCCCCCDDDDDDDDDDD"


//------- ---------------------- ------

WiFiClientSecure client;
ArduinoSlack slack(client, SLACK_ACCESS_TOKEN);

unsigned long delayBetweenRequests = 15000; // Time between requests
unsigned long requestDueTime;               // time when request due

// these constants won't change:
const int  switchPin[] = {13, 12, 14, 4, 5, 16};    // the pins that the switch is attached to
int pinCount = 6;           // the number of pins (i.e. the length of the array)

// Variables will change:
int switchState[] = {0,0,0,0,0,0};         // current state of the switch
int lastSwitchState[] = {0,0,0,0,0,0};     // previous state of the switch

bool changeStatus = false;
int whichStatus = 0;

void setup() {
  // initialize the switch pins as inputs:
  for (int i = 0; i < pinCount; i++) {
    pinMode(switchPin[i], INPUT_PULLUP);
  }
 
  
  Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was Previously
    // connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);

    client.setFingerprint(SLACK_FINGERPRINT);

    // If you want to enable some extra debugging
    // uncomment the "#define SLACK_ENABLE_DEBUG" in ArduinoSlack.h
}

void displayProfile(SlackProfile profile)
{
    if (!profile.error)
    {
        Serial.println("--------- Profile ---------");

    
        Serial.print("Display Name: ");
        Serial.println(profile.displayName);

        Serial.print("Status Text: ");
        Serial.println(profile.statusText);

        Serial.print("Status Emoji: ");
        Serial.println(profile.statusEmoji);

        Serial.print("Status Expiration: ");
        Serial.println(profile.statusExpiration);

        Serial.println("------------------------");
    } else {
        Serial.println("error getting profile");
    }
}

void loop() {

for (int i = 0; i < pinCount; i++) {
    // read the pushbutton input pin:
  switchState[i] = digitalRead(switchPin[i]);
}
for (int j = 0; j < pinCount; j++) {  
  // compare the switchState to its previous state
  if (switchState[j] != lastSwitchState[j]) { // if the state has changed
    if (switchState[j] == HIGH) {
      // if the current state is HIGH then the switch is not connected
      //Serial.print(switchPin[j]);
      //Serial.println(" is not connected");
    } else {
      // if the current state is LOW then the switch is connected to the common ground pin
      Serial.print(switchPin[j]);
      Serial.println(" is connected");
      whichStatus = j;
      changeStatus = true;
      requestDueTime = millis() + 2000; // add time to move the rotary switch to another position before updating status
    }
    // Delay a little bit to avoid bouncing
    delay(20);
  }
  // save the current state as the last state, for next time through the loop
  lastSwitchState[j] = switchState[j];

}
if (millis() > requestDueTime && changeStatus)
        {
          SlackProfile profile;
          switch (whichStatus) {
            case 0:
             profile = slack.setCustomStatus("I am the first status", ":sparkling_heart:");
             break;
            case 1:
              profile = slack.setCustomStatus("I am the second status", ":v:");
              // There is an optional third parameter which takes a Unix timestamp for
              // when this custom status expires:
              // slack.setCustomStatus("I am the second status", ":v:", 1532627506);
              break;
            case 2:
              profile = slack.setCustomStatus("I am the third status", ":sunglasses:");
              break;
            case 3:
              profile = slack.setCustomStatus("I am the fourth status", ":dog2:");
              break;
            case 4:
              profile = slack.setCustomStatus("I am the fifth status", ":palm_tree:");
              break;
            case 5:
              profile = slack.setCustomStatus("I am the sixth status", ":soon:");
              break;
          }
            displayProfile(profile);
            requestDueTime = millis() + delayBetweenRequests;
            changeStatus = false;
        }

}
