/*******************************************************************
 *  Push Notificaiton Demo                                         *
 *  Using IFTTT and Telegram to send alerts to your phone          *
 *                                                                 *
 *  Telegram Library:                                              *
 *  https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot *
 *                                                                 *
 *  IFTTT Maker Library:                                           *
 *  https://github.com/witnessmenow/arduino-ifttt-maker            *
 *                                                                 *
 *  By Brian Lough                                                 *
 *******************************************************************/

//Including the two libraries
#include <UniversalTelegramBot.h>
#include <IFTTTMaker.h>

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//------- WiFi Settings -------
char ssid[] = "xxx";       // your network SSID (name)
char password[] = "yyyy";  // your network key

#define TELEGRAM_BUTTON_PIN D5
#define IFTTT_BUTTON_PIN D6


// ------- IFTTT Maker config --------
#define KEY "zzzzzzzzzzzzzzzzzzzzzzz"  // Get it from this page https://ifttt.com/services/maker/settings
#define EVENT_NAME "button_pressed" // Name of your event name, set when you are creating the applet

// ------- Telegram config --------
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)
#define CHAT_ID "-128380507" // Chat ID of where you want the message to go (You can use MyIdBot to get the chat ID)

// SSL client needed for both libraries
WiFiClientSecure client;

IFTTTMaker ifttt(KEY, client);
UniversalTelegramBot bot(BOT_TOKEN, client);

String ipAddress = "";

volatile bool telegramButtonPressedFlag = false;
volatile bool iftttButtonPressedFlag = false;

void setup() {

  Serial.begin(115200);

  // Initlaze the buttons
  pinMode(TELEGRAM_BUTTON_PIN, INPUT);
  pinMode(IFTTT_BUTTON_PIN, INPUT);

  // NOTE:
  // It is important to use interupts when making network calls in your sketch
  // if you just checked the status of te button in the loop you might
  // miss the button press.
  attachInterrupt(TELEGRAM_BUTTON_PIN, telegramButtonPressed, RISING);
  attachInterrupt(IFTTT_BUTTON_PIN, iftttButtonPressed, RISING);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  ipAddress = ip.toString();

}

void telegramButtonPressed() {
  Serial.println("telegramButtonPressed");
  int button = digitalRead(TELEGRAM_BUTTON_PIN);
  if(button == HIGH)
  {
    telegramButtonPressedFlag = true;
  }
  return;
}


void iftttButtonPressed() {
  Serial.println("iftttButtonPressed");
  int button = digitalRead(IFTTT_BUTTON_PIN);
  if(button == HIGH)
  {
    iftttButtonPressedFlag = true;
  }
  return;
}

void triggerIftttEvent() {
  if(ifttt.triggerEvent(EVENT_NAME, ssid, ipAddress)){
    Serial.println("IFTTT Successfully sent");
  }
  iftttButtonPressedFlag = false;
}

void sendTelegramMessage() {
  String message = "SSID:  ";
  message.concat(ssid);
  message.concat("\n");
  message.concat("IP: ");
  message.concat(ipAddress);
  message.concat("\n");
  if(bot.sendMessage(CHAT_ID, message, "Markdown")){
    Serial.println("TELEGRAM Successfully sent");
  }
  telegramButtonPressedFlag = false;
}

void loop() {
  if ( iftttButtonPressedFlag ) {
    triggerIftttEvent();
  }

  if ( telegramButtonPressedFlag ) {
    sendTelegramMessage();
  }
}
