#include <SPI.h>

#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5

SPIClass spi = SPIClass(VSPI);

volatile boolean received;

volatile int Slavevalue;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(MISO,OUTPUT);                   //Sets MISO as OUTPUT (Have to Send data to Master IN 
  SPCR |= _BV(SPE);                       //Turn on SPI in Slave Mode
  received = false;
  spi.attachInterrupt();
    
  ISR(SPI_STC_vect)                        //Inerrrput routine function 
  
  {
  
    Slavevalue = SPDR;         // Value received from master if store in variable slavereceived
  
    received = true;                        //Sets received as True 
  
  }


}

//ISR(SPI_STC_vect){
//
//  Slavevalue = SPDR;         // Value received from master if store in variable slavereceived
//
//  received = true;                        //Sets received as True 
//
//}

void loop() {
  // put your main code here, to run repeatedly:
  if(received){
    Serial.println(Slavevalue);
  }

}
