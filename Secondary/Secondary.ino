#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const int button_pin = 23;
const int led_pin = 18;

bool buttonstate = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(button_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  Serial.begin(115200);

  WiFi.mode(WIFI_MODE_STA);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  Serial.println(F("SSD1306 allocation failed"));
  for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
//  display.display();
//  delay(2000); // Pause for 2 seconds
//
//  // Clear the buffer
//  display.clearDisplay();
//
//  // Draw a single pixel in white
//  display.drawPixel(10, 10, SSD1306_WHITE);
//
//  // Show the display buffer on the screen. You MUST call display() after
//  // drawing commands to make them visible on screen!
//  display.display();
//  delay(2000);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25,0);
  display.println(F("NU Motorsports"));
  display.setCursor(10,18);
  display.println(F("Timing Gate System"));
  display.setCursor(13,45);
  display.println(WiFi.macAddress());
  display.display();


}

void loop() {
  // put your main code here, to run repeatedly:
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
