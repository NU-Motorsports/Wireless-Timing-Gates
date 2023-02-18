#include <CAN.h>


void setup() {
  // put your setup code here, to run once:
  CAN.setPins(22,23);
    Serial.begin(115200);
  while (!Serial);

  Serial.println("CAN Sender");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500000)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }


}


void loop() {
  // try to parse packet
  int packetSize = CAN.parsePacket();
  Serial.print((char)CAN.read());
  if (packetSize || CAN.packetId() != -1) {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

      // only print packet data for non-RTR packets
      while (CAN.available()) {
        Serial.print((char)CAN.read());
      }
      Serial.println();
    }

    Serial.println();
  }
}
