#include "esp_timer.h"

const int button_pin = 15;
const int led_pin = 13;
bool gateState = 0;
bool lastGateState = 0;
float timervar = 0;
int debounceDelay = 4000; //in microseconds
float stopwatch = 0;
bool wheelnum = 0;
int lastDebounceTime = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(button_pin,INPUT);
  pinMode(led_pin,OUTPUT);
  Serial.begin(115200);
  digitalWrite(led_pin,LOW);
  Serial.write("Serial Test");
}

void loop() {
  bool gateReading = digitalRead(button_pin);

  //Detect start of a press
  if (gateReading != lastGateState){
    lastDebounceTime = esp_timer_get_time();
  }

  //Continue if press remains longer then the debounce delay
  if((esp_timer_get_time()-lastDebounceTime)>debounceDelay){
    if(gateReading != gateState){                   //
      gateState = gateReading;
      if(gateState == HIGH){
        if(wheelnum == 0){
          timervar = esp_timer_get_time()/1000000.00;
          digitalWrite(led_pin,HIGH);
          Serial.println("Timer Start ");
          wheelnum = 1;
        }else if(wheelnum == 1){
          stopwatch = esp_timer_get_time()/1000000.000 - timervar;
          digitalWrite(led_pin,LOW);
          Serial.print("Timer stopped: ");
          Serial.print(stopwatch);
          Serial.print(" seconds");
          wheelnum = 0;
        }
      }
    }
  }

  lastGateState = gateState;
}

//functions
//record speed time 
//void recordtime(){
//  
//}
