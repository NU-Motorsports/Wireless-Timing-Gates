//Made by DJ Walsh in Fall 2022
//This is the gate-side code for an ESP-32 based Wireless Timing Gate System


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
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//Input Pins
const int gate_pin = 2;
const int mode_pin = 19;
const int select_pin = 27;
const int indicator_pin = 18;
const int error_pin = 4;

//Gate Variables
int gatenum = 0;

//Speed Detection Variables
float timervar = 0;
int stopwatch = 0;
bool wheelnum = 0;

//Gate Debounce Variables
int debounceDelay = 2000;           //in microseconds
int lastDebounceTime = 0;
bool gateReading = 0;
bool gateState = 0;
bool lastGateState = 0;

//Button Debounce Varbiables
int buttonDebounceDelay = 4000;     //in microseconds
int lastButtonDebounceTime = 0;
bool modeReading = 0;
bool selectReading = 0;
bool modeState = 0;
bool selectState = 0;
bool lastModeState = 0;
bool lastSelectState = 0;

//ESPNOW Variables
uint8_t broadcastAddress[] = {0x78, 0x21, 0x84, 0x7F, 0xFC, 0x84};

//Mode Variables
int modeStatus = 0;   //0:Gate Number 1:Speed Display 2:Broadcast Address 3:Lock 4:Speed Measurement type?


//Data Structure
typedef struct struct_message {
  int a;                            //Gate Number (0-9)
  bool b;                           //Gate Status (tripped (1) vs not tripped (0))
  int c;                            //Speed trap time (in microseconds) if speed is less than ~1mph will return ~1mph worth of microseconds
  bool d;                           //Speed Trap measurement status (successful=1 unsucessful=0)
} struct_message;

//Structured Object
struct_message myData;

//Peer Info
esp_now_peer_info_t peerInfo;



void setup() {
  //Initial Setup
  pinMode(gate_pin, INPUT);
  pinMode(mode_pin,INPUT);
  pinMode(select_pin,INPUT);
  pinMode(error_pin,OUTPUT);
  pinMode(indicator_pin, OUTPUT);

  digitalWrite(indicator_pin,LOW);
  digitalWrite(error_pin,LOW);
  
  WiFi.mode(WIFI_MODE_STA);
  Serial.begin(9600);
  
  
  //ESP-NOW Setup
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing Esp-Now");
    digitalWrite(error_pin,HIGH);
  }
  esp_now_register_send_cb(OnDataSent);
  //Register Peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt - false;
  //Add Peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    digitalWrite(error_pin,HIGH);
    return;
  }

  
  //Screen Initialization
  initScreen();
}



void loop() {
  inputread();
  
  
  //Gate Triggered
  if(gateReading !=lastGateState){                                //Detect Start of Press
    lastDebounceTime = esp_timer_get_time();
  }

  if((esp_timer_get_time()-lastDebounceTime)>debounceDelay){      //Continue if press remains longer than the debounce delay
    if(gateReading != gateState){                                 //Prevents looping for gate held down longer than 1 loop
      gateState = gateReading;
      if(gateState == HIGH){                                      //When gate triggered
        if(wheelnum == 0){
          timervar = esp_timer_get_time();                        //Record first press time
          digitalWrite(indicator_pin,HIGH);
          myData.b = 1;
          wheelnum = 1;
        }else if(wheelnum == 1){                                  //Record second gate hitting
          stopwatch = esp_timer_get_time() - timervar;
          myData.d = 1;
          senddata();
          wheelnum =0;
        }
      }
    }else if(((esp_timer_get_time() - timervar)>3000000) && (wheelnum == 1)){
        stopwatch = esp_timer_get_time() - timervar;
        myData.d = 0;
        senddata();
        wheelnum = 0;
      }
  }
  
  
  //Mode Triggered
  if(modeReading != lastGateState){
    lastButtonDebounceTime = esp_timer_get_time();
  }
  
  if((esp_timer_get_time()-lastButtonDebounceTime)>buttonDebounceDelay){
    if(modeReading != modeState){
      modeState = modeReading;
    }
  }
  
  
  //Gate Number Advance Button
  if(selectState==HIGH && gatenum<9) {
    gatenum = gatenum+1;
    updateDisplay();
  } else if(selectState==HIGH){
    gatenum = 0;
    updateDisplay();
  }

  lastModeState = modeReading;
  lastSelectState = selectReading;
  lastGateState = gateReading;
}







//////////////////////////////////FUNCTIONS////////////////////////////////////////

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


//Screen Update Gate Num
void updateDisplay_Gate(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25,0);
  display.println("NU Motorsports");
  display.setCursor(0,18);
  display.print("Timing Gate ");
  display.print(gatenum);
  display.display();
}


//Screen Update Broadcast Address
void updateDisplay_Address(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25,0);
  display.println("NU Motorsports");
  display.setCursor(13,18);
  display.println(WiFi.macAddress());
  display.display();
}


//Screen Update Lock


//Screen Update Gate Type


//Screen Initialization
void initScreen(){
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    digitalWrite(error_pin,HIGH);
    //for(;;); // Don't proceed, loop forever
  }
  
  updateDisplay_Gate();
}


//Read Inputs
void inputread(){
  gateReading = digitalRead(gate_pin);
  modeReading = digitalRead(mode_pin);
  selectReading = digitalRead(select_pin);
}


////Mode Button Debounce
//void modeDebounce(){
//  if(modeReading !=lastModeState){                                //Detect Start of Press
//    lastButtonDebounceTime = esp_timer_get_time();
//  }
//
//  if((esp_timer_get_time()-lastDebounceTime)>debounceDelay){      //Continue if press remains longer than the debounce delay
//    if(gateReading != gateState){                                 //Prevents looping for gate held down longer than 1 loop
//      gateState = gateReading;
//      if(gateState == HIGH){                                      //When gate triggered
//        if(wheelnum == 0){
//          timervar = esp_timer_get_time();                        //Record first press time
//          digitalWrite(indicator_pin,HIGH);
//          myData.b = 1;
//          wheelnum = 1;
//        }else if(wheelnum == 1){                                  //Record second gate hitting
//          stopwatch = esp_timer_get_time() - timervar;
//          myData.d = 1;
//          senddata();
//          wheelnum = 0;
//        }
//      }
//    }else if(((esp_timer_get_time() - timervar)>3000000) && (wheelnum == 1) && (gateState == HIGH)){
//      stopwatch = esp_timer_get_time() - timervar;
//      myData.d = 0;
//      myData.c = stopwatch;
//      wheelnum = 0;
//      senddata();
//      
//    }
//  }
//}


//Select Button Debounce


//Send Data
void senddata(){
  myData.c = stopwatch;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if(result == ESP_OK) {
      Serial.print("Sending Confirmed:    ");
      Serial.print(myData.a);
      Serial.print("    ");
      Serial.print(myData.b);
      Serial.print("    ");
      Serial.print(myData.c);
      Serial.print("    ");
      Serial.println(myData.d);
    } else  {
      Serial.println("Sending Error");
      digitalWrite(error_pin,HIGH);
    }
    digitalWrite(indicator_pin,LOW);
    myData.b = 0;
}
