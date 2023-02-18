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


//I/O Pins
const byte gate_pin = 1;
const byte page_pin = 3;
const byte select_pin = 8;
const byte indicator_pin = 7;
const byte error_pin = 2;

//Gate Variables
byte gatenum = 0;

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
int buttonDebounceDelay = 50000;     //in microseconds
int lastButtonDebounceTime = 0;
bool modeReading = 0;
bool selectReading = 0;
bool modeState = 0;
bool selectState = 0;
bool lastModeState = 0;
bool lastSelectState = 0;

//ESPNOW Variables
uint8_t broadcastAddress[] = {0x94, 0x3C, 0xC6, 0x33, 0xA5, 0x5C};

//Mode Variables
byte modeStatus = 0;   //0:Gate Number 1:Speed Display 2:Broadcast Address 3:Lock 4:Speed Measurement type?
byte num_modes = 3;


//Data Structure
typedef struct struct_message {
  byte a;                            //Gate Number (0-9)
  int b;                            //Speed trap time (in microseconds) if speed is less than ~1mph will return ~1mph worth of microseconds
  bool c;                           //Speed Trap measurement status (successful=1 unsucessful=0)
} struct_message;

//Structured Object
struct_message myData;

//Peer Info
esp_now_peer_info_t peerInfo;



void setup() {
  //Initial Setup
  pinMode(gate_pin, INPUT);
  pinMode(page_pin,INPUT);
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
          myData.c = 1;
          senddata();
          wheelnum =0;
        }
      }
    }else if(((esp_timer_get_time() - timervar)>3000000) && (wheelnum == 1)){
        stopwatch = esp_timer_get_time() - timervar;
        myData.c = 0;
        senddata();
        wheelnum = 0;
      }
  }
  
  
  //Mode Triggered
  if(modeReading != lastModeState){
    lastButtonDebounceTime = esp_timer_get_time();
  }
  
  if((esp_timer_get_time()-lastButtonDebounceTime)>buttonDebounceDelay){
    if(modeReading != modeState){
      modeState = modeReading;
      if(modeState == HIGH){
        if(modeStatus<(num_modes-1)){
          modeStatus = modeStatus+1;
          updateDisplay();
        }else{
          modeStatus = 0;
          updateDisplay();
        }
      }
    }
  }


  //Select Triggered
  if(selectReading != lastSelectState){
    lastButtonDebounceTime = esp_timer_get_time();
    if(selectReading != selectState){
      if(modeState == 0){
        if(gatenum<9){
          gatenum = gatenum+1;
          updateDisplay();
        }else{
          gatenum = 0;
          updateDisplay();
        }
      }
    }
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
  if(modeStatus == 0){
    updateDisplay_Gate();
  }else if(modeStatus == 1){
    updateDisplay_Address();
  }else if(modeStatus == 2){
    updateDisplay_Lock();
  }else{
    display.setCursor(25,0);
    display.println("NU Motorsports");
    display.setCursor(0,18);
    display.print("Empty Mode Screen");
  }
  display.display();
}


//Screen Update Gate Num
void updateDisplay_Gate(){
  display.setCursor(0,18);
  display.print("Timing Gate:");
  display.setCursor(0,36);
  if(gatenum == 0){
    display.print("Start Line");
  }if(gatenum == 8){
    display.print("Finish Line");
  }if(gatenum == 9){
    display.print("Start/Finish Line");
  }else if(gatenum!=0 && gatenum!=8 && gatenum!=9){
    display.print("Sector ");
    display.print(gatenum);
  }
  
}


//Screen Update Broadcast Address
void updateDisplay_Address(){
  display.setCursor(0,18);
  display.print("Broadcast Address:");
  display.setCursor(0,36);
  display.println(WiFi.macAddress());
}


//Screen Update Lock
void updateDisplay_Lock(){
  display.setCursor(0,18);
  display.println("Lock?");
}



//Screen Initialization
void initScreen(){
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    digitalWrite(error_pin,HIGH);
    //for(;;); // Don't proceed, loop forever
  }
  
  updateDisplay();
}


//Read Inputs
void inputread(){
  gateReading = digitalRead(gate_pin);
  modeReading = digitalRead(page_pin);
  selectReading = digitalRead(select_pin);
}



//Send Data
void senddata(){
  myData.a = gatenum;
  myData.b = stopwatch;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if(result == ESP_OK) {
      Serial.print("Sending Confirmed:    ");
      Serial.print(myData.a);
      Serial.print("    ");
      Serial.print(myData.b);
      Serial.print("    ");
      Serial.print(myData.b);
      Serial.print("    ");
      Serial.println(myData.c);
    } else  {
      Serial.println("Sending Error");
      digitalWrite(error_pin,HIGH);
    }
    digitalWrite(indicator_pin,LOW);
    myData.b = 0;
}
