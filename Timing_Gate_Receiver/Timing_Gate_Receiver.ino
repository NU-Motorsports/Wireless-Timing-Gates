//Made by DJ Walsh in Fall 2022
//This is the car-side code for an ESP-32 based Wireless Timing Gate System
//My code is my documentation

//Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_timer.h>


//Screen Setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Inupt Pins
const byte page_pin = 23;
const byte led_pin = 18;

//Speed Calc Variables
float wheelbase = 55.0;       //inches
float speedValue = 0;          //mph

//Received Data Variables
bool messageReceivedStatus = 0;
byte gateReceived = 0;
int speedTimeReceived = 0;
bool speedStatusReceived = 0;

//Lap Timing Variables
int lapStartTime = 0;
float lastLapDuration = 0;
float fastestLapDuration = 0;

//Button Debounce Variables
bool buttonstate = 0;
int buttonDebounceDelay = 16000;
int lastButtonDebounceTime = 0;
bool pageReading = 0;
bool lastPageState = 0;
bool pageState = 0;

//Page Variables
byte pageStatus = 0;
//1: Recent Entry Overview
//2: All Recent Sectors
//3: Fastest Lap and Sectors
//4: Top Speed
//5: Broadcast Address
byte num_pages = 5;


//Message Structure
typedef struct struct_message {
  byte a;
  int b;
  bool c;
} struct_message;

struct_message myData;


//Data Received Callback function
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {  
  memcpy(&myData, incomingData, sizeof(myData));

  gateReceived = myData.a;
  speedTimeReceived = myData.b;
  speedStatusReceived = myData.c;

  messageReceivedStatus = 1;

  Serial.print("Gate: ");
  Serial.print(myData.a);
  Serial.print("     Speed Time: ");
  Serial.println(speedTimeReceived);
  
}

void setup() {
  pinMode(page_pin, INPUT);
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
  inputread();

    speedValue = (wheelbase/speedTimeReceived)/56820;       //Speed value in mph
  

  //On Data Received Actions
  if(messageReceivedStatus == 1){
    if(gateReceived == 0){
      lapStartTime = esp_timer_get_time();
      Serial.print("Message Received!    Lap Start Time: ");
      Serial.println(lapStartTime);
      
    }else if(gateReceived == 8){
      lastLapDuration = (esp_timer_get_time() - lapStartTime)/1000000.0;
      Serial.print("Message Received with 8    Lap End time: ");
      Serial.println(lastLapDuration);
    }else if(gateReceived == 9){
      lastLapDuration = (esp_timer_get_time() - lapStartTime)/1000000.0;
      lapStartTime = esp_timer_get_time();
    }
    messageReceivedStatus = 0;
    //Serial.println(lastLapDuration);
    updateDisplay();
  }
  

  
  //Page Triggered
  if(pageReading != lastPageState){
    lastButtonDebounceTime = esp_timer_get_time();
  }
  
  if((esp_timer_get_time()-lastButtonDebounceTime)>buttonDebounceDelay){
    if(pageReading != pageState){
      pageState = pageReading;
      if(pageState == HIGH){
        if(pageStatus<(num_pages-1)){
          pageStatus = pageStatus+1;
          updateDisplay();
        }else{
          pageStatus = 0;
          updateDisplay();
        }
      }
    }
  }

  lastPageState = pageReading;
}



//////////////////////////////////FUNCTION//////////////////////////////////////
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
  display.print("Device Address");
  display.setCursor(0,36);
  display.println(WiFi.macAddress());
  display.display();
}


//Screen Update
void updateDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  if(pageStatus == 0){
    updateDisplay_Info();
  }else{
    display.setCursor(25,0);
    display.println("NU Motorsports");
    display.setCursor(0,18);
    display.print("Empty Mode Screen");
  }
  display.display();
}


//Screen Update Information
void updateDisplay_Info(){
  display.setCursor(25,0);
  display.println("NU Motorsports");
  display.setCursor(0,18);
  display.print("Recent Information: ");
  display.setCursor(0,36);
  display.print("Last Lap: ");
  display.print(lastLapDuration);
}


//Input Read
void inputread(){
  pageReading = digitalRead(page_pin);
}
