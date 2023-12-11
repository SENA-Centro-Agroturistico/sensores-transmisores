//-- Inicializa el RFM95W LoRa --
bool inicializa_lora() {
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  for (byte i = 0; i < 3; i++) {
    if (!rf95.init()) {
      return false;
    }
    else {
      i = 3;
      rf95.setFrequency(RF95_FREQ);
      rf95.setTxPower(23, false); //-- Establece potencia en 23dBm
      return true;
    }
  }
}
