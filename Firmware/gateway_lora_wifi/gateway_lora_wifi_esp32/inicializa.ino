bool inicializa_lora() {
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  for (byte i = 0; i < INTENTOS_LORA_INI; i++) {
    digitalWrite(PIN_LED, HIGH);
    if (!rf95.init()) {
      Serial.println(F("Fallo inicilo LoRa"));
    } else {
      i = INTENTOS_LORA_INI;
      if (!rf95.setFrequency(RF95_FREQ)) {
        Serial.println(F("Fallo set Frecuencia"));
      }
      rf95.setTxPower(23, false);
      digitalWrite(PIN_LED, LOW);
      return true;
    }
  }
  return false;
}

void parpadeo(byte veces, int duracion) {
  for (byte i = 0; i < veces; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(duracion);
    digitalWrite(PIN_LED, LOW);
    delay(duracion);
  }
}