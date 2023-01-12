//Made by DJ Walsh in Fall 2022
//This is the car-side code for an ESP-32 based Wireless Timing Gate System
//My code is my documentation

//Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include<esp_now.h>


//Screen Setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Inupt Pins
const int button_pin = 23;
const int led_pin = 18;

//Speed Calc Variables
float wheelbase = 
bool buttonstate = 0;


//Message Structure
typedef struct struct_message {
  int a;
  bool b;
  int c;
  bool d;
} struct_message;

struct_message myData;

//Sync Structure

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {  
  memcpy(&myData, incomingData, sizeof(myData));
  
  Serial.print(myData.a);     //Gate Number
  Serial.print("     ");
  Serial.print(myData.b);     //Triggered?
  Serial.print("     ");
  Serial.print(myData.c);   //Speed (mph, will return 5 if less than 5mph
  Serial.print("     ");
  Serial.println(myData.d);   //Speed (mph, will return 5 if less than 5mph
  
  if(myData.b == 1){
    digitalWrite(led_pin, HIGH);
  } else  {
    digitalWrite(led_pin, LOW);
  }
}

void setup() {
  pinMode(button_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  WiFi.mode(WIFI_MODE_STA);
  Serial.begin(9600);

  initScreen();
  
  if  (esp_now_init() != ESP_OK)  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {


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
  display.setCursor(13,9);
  display.println(WiFi.macAddress());
  display.display();
  delay(2000);
}
