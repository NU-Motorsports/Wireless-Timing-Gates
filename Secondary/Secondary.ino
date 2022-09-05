//Made by DJ Walsh in Fall 2022
//This is the gate-side code for an ESP-32 based Wireless Timing Gate System

//Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

//Screen Setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Misc Setup
const int button_pin = 23;
const int led_pin = 18;
bool buttonstate = 0;



void setup() {
  pinMode(button_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  WiFi.mode(WIFI_MODE_STA);

  initScreen();
}



void loop() {
  buttonstate = digitalRead(button_pin);
  
  if (buttonstate == HIGH)  {
    digitalWrite(led_pin,HIGH);
    delay(500);
    
    do  {
      buttonstate = digitalRead(button_pin);
    } while(buttonstate == LOW);
    
    digitalWrite(led_pin, LOW);
    delay(2000);

  }
}



void initScreen(){
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  Serial.println(F("SSD1306 allocation failed"));
  for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25,0);
  display.println(F("NU Motorsports"));
  display.setCursor(0,18);
  display.println(F("Timing Gate Secondary"));
  display.setCursor(13,45);
  display.println(WiFi.macAddress());
  display.display();
  delay(2000);
}
