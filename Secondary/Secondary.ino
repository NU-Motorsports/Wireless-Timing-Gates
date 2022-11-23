//Made by DJ Walsh in Fall 2022
//This is the gate-side code for an ESP-32 based Wireless Timing Gate System

//Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <esp_now.h>

//Screen Setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Input Pins
const int gate_pin = 23;
const int speed_pin = 2;
const int led_pin = 18;
const int advance_pin = 19;

//Global Variables
bool gatestate = 0;
bool gateadvance = 0;
int gatenum = 0;
bool ledstate = 0;
uint8_t broadcastAddress[] = {0x78, 0x21, 0x84, 0x7F, 0xFC, 0x84};


//Data Structure
typedef struct struct_message {
  int a;                            //Gate Number (0-9)
  bool b;                           //Gate Status (tripped (1) vs not tripped (0))
  float c;                          //Speed trap time (in ms) if speed is less than 1mph will return 0 seconds
} struct_message;

//Structured Object
struct_message myData;

//Peer Info
esp_now_peer_info_t peerInfo;



void setup() {
  //Initial Setup
  pinMode(gate_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  WiFi.mode(WIFI_MODE_STA);
  Serial.begin(115200);

  //ESP-NOW Setup
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing Esp-Now");
  }
  esp_now_register_send_cb(OnDataSent);

  //Register Peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt - false;

  //Add Peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  initScreen();
}



void loop() {
  //Read Inputs
  gatestate = digitalRead(gate_pin);
  gateadvance = digitalRead(advance_pin);

  

  myData.a = gatenum;
  myData.c = 0;

  if (gatestate == HIGH)  {                       //Gate Triggered
    digitalWrite(led_pin,HIGH);
    myData.b = 1;
    measureSpeedTime();
    
  } else  {                                       //Leave LED low and keep moving the rest of the time
    digitalWrite(led_pin, LOW);
    myData.b = 0;
  }

  if(gateadvance==HIGH && gatenum<9) {
    gatenum = gatenum+1;
    updateDisplay();
  } else if(gateadvance==HIGH){
    gatenum = 0;
    updateDisplay();
  }

  myData.a = gatenum;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  if(result == ESP_OK) {
    Serial.print("Sending Confirmed:    ");
    Serial.print(myData.a);
    Serial.print("    ");
    Serial.print(myData.b);
    Serial.print("    ");
    Serial.println(myData.c);
  } else  {
    Serial.println("Sending Error");
  }
  delay(100);

  }







//FUNCTIONS

//Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//Screen Update
void updateDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25,0);
  display.println("NU Motorsports");
  display.setCursor(0,18);
  display.print("Timing Gate ");
  display.print(gatenum);
  display.setCursor(13,45);
  display.println(WiFi.macAddress());
  display.display();
  delay(500);
}

//Screen Initialization
void initScreen(){
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  Serial.println(F("SSD1306 allocation failed"));
  for(;;); // Don't proceed, loop forever
  }
  
  updateDisplay();
}

void measureSpeedTime() {
  unsigned long speed_time = millis();
  
  for (float i=5;i<=millis()-speed_time;0)  {         //Set Min Recorded Speed (time)
    if (digitalRead(speed_pin) == HIGH) {
      speed_time = millis() - speed_time;         //Find Time Differential
        speed_value = speed_time;//17.6/speed_time;       //Calculate Speed in MPH
        myData.c = speed_value;
      }
    }
}
