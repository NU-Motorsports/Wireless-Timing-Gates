#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

#define I2C_SDA 4
#define I2C_SCL 5
#define SCK 10
#define MISO 0
#define MOSI 6
#define CS 1

SPIClass spi = SPIClass(HSPI);

int mastervalue = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(I2C_SDA, I2C_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // Don't proceed, loop forever
  }
  
  updateDisplay();

  spi.begin(SCK, MISO, MOSI, CS);
  spi.setClockDivider(SPI_CLOCK_DIV8);
  digitalWrite(CS,HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(CS,LOW);
  spi.transfer(mastervalue);
  mastervalue = mastervalue +1;
  delay(100);
  

}

void updateDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25,0);
  display.println("NU Motorsports");
  display.setCursor(25,0);
  display.println("NU Motorsports");
  display.setCursor(0,18);
  display.print("Empty Mode Screen");
  display.display();
}
