#include "esp_timer.h"

const int button_pin = 4;
const int led_pin = 2;
bool gateState = 0;
bool lastGateState = 0;
float timervar = 0;
int debounceDelay = 2000; //in microseconds
float stopwatch = 0;
bool wheelnum = 0;
int lastDebounceTime = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(button_pin,INPUT);
  pinMode(led_pin,OUTPUT);
  Serial.begin(9600);
  digitalWrite(led_pin,LOW);
  Serial.write("Serial Test");
}

void loop() {
  bool gateReading = digitalRead(button_pin);
//  Serial.print(esp_timer_get_time());
//  Serial.print("   ");
//  Serial.print(esp_timer_get_time()-lastDebounceTime);
//  Serial.print("   ");
//  Serial.println(gateReading);

  //Detect start of a press
  if(gateReading != lastGateState){
    lastDebounceTime = esp_timer_get_time();
  }

  //Continue if press remains longer then the debounce delay
  if((esp_timer_get_time()-lastDebounceTime)>debounceDelay){
    if(gateReading != gateState){
      gateState = gateReading;
      if(gateState == HIGH){
        if(wheelnum == 0){
          timervar = esp_timer_get_time();
          digitalWrite(led_pin,HIGH);
          //Serial.println("Timer Start ");
          wheelnum = 1;
        }else if(wheelnum == 1){
          stopwatch = (esp_timer_get_time() - timervar)/1000000.000;
          digitalWrite(led_pin,LOW);
          Serial.print("Timer stopped: ");
          Serial.print(stopwatch);
          Serial.println(" seconds");
          wheelnum = 0;
        }
      }
    }
  }

  lastGateState = gateReading;
}

//functions
//record speed time 
//void recordtime(){
//  
//}
