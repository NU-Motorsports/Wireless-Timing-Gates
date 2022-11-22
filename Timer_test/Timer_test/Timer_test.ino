const int button_pin = 10;
const int led_pin = 13;
bool gatestate = 0;
float timervar = 0;
float stopwatch = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(button_pin,INPUT);
  pinMode(led_pin,OUTPUT);
  Serial.begin(9600);
  digitalWrite(led_pin,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  gatestate = digitalRead(button_pin);
  
  if (gatestate == HIGH)  {
    gatestate = LOW;
    timervar = millis();
    digitalWrite(led_pin,HIGH);
    Serial.println("Timer Start");
    delay(100);
    
    
    do {
      gatestate = digitalRead(button_pin);
      delay(100);
      
    } while(gatestate == LOW);
    stopwatch = millis() - timervar;
    Serial.print("Timer stopped: ");
    Serial.print(stopwatch);
    Serial.println(" seconds");
    digitalWrite(led_pin,LOW);
    delay(100);
    
  }
}
