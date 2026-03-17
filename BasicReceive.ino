#include <RFHexSniffer.h>

RFHexSniffer rf;

void setup() {
  Serial.begin(115200);

  rf.begin(2);
  rf.setShortLongThreshold(700);
  rf.setFrameGap(5000);
  rf.setBitOrderMSB(true);

  Serial.println("Escuchando...");
}

void loop() {
  rf.update();

  if (rf.available()) {
    char hexBuffer[129];
    rf.readHex(hexBuffer, sizeof(hexBuffer));

    Serial.print("HEX: ");
    Serial.println(hexBuffer);

    rf.clear();
  }
}
