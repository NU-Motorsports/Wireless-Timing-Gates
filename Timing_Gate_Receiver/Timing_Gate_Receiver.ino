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
#include <SD.h>
#include <CAN.h>


//Screen Setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Inupt Pins
const byte page_pin = 3;
const byte led_pin = 2;

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
int buttonDebounceDelay = 50000;
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
//5: Info Screen (MAC and Wheelbase)
byte num_pages = 5;

//CAN Variables



//Message Structure
typedef struct struct_message {
  byte a;                           //Gate Number
  int b;                            //Speed Trap Time (in microseconds) Will time out at 3s
  bool c;                           //Speed Trap Measurement Status (successful = 1 unsuccessful = 0)
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
  CAN.setPins(18, 19);    //(rx, tx)
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

    speedValue = (wheelbase/speedTimeReceived)*56820;       //Speed value in mph
  

  //On Data Received Actions
  if(messageReceivedStatus == 1){
    if(gateReceived == 0){
      lapStartTime = esp_timer_get_time()-speedTimeReceived;
    }else if(gateReceived == 8){
      lastLapDuration = ((esp_timer_get_time()-speedTimeReceived) - lapStartTime)/1000000.0;
    }else if(gateReceived == 9){
      lastLapDuration = ((esp_timer_get_time()-speedTimeReceived) - lapStartTime)/1000000.0;
      lapStartTime = esp_timer_get_time()-speedTimeReceived;
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

  updateDisplay();
}


//Screen Update
void updateDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("NU Motorsports");
  if(pageStatus == 0){
    updateDisplay_Trip();
  }else if(pageStatus == 1){
    updateDisplay_Sector();
  }else if(pageStatus == 2){
    updateDisplay_LapTime();
  }else if (pageStatus == 3){
    updateDisplay_Info();
  }else{
    display.print("Empty Mode Screen");
  }
  display.display();
}


//Screen Update Last Trip
void updateDisplay_Trip(){
  display.println("Last Gate Trip: ");
  display.print("Last Lap: ");
  display.println(lastLapDuration);
  display.print("Last Speed: ");
  display.println(speedValue);
  display.print("Last Sector: ");
  display.println("Empty");
}


//Screen Update Sector
void updateDisplay_Sector(){
  display.print("Last Sectors: ");
}


//Screen Update Sector
void updateDisplay_LapTime(){
  display.print("Last Lap: ");
}


//Screen Update Sector
void updateDisplay_Info(){
  display.println("Info: ");
  display.print("MAC:");
  display.println(WiFi.macAddress());
  display.print("WB:");
  display.println(wheelbase);
}


//Input Read
void inputread(){
  pageReading = digitalRead(page_pin);
}
