//-- Funcion para inicializar sonda 1
bool inicializa_sonda1() {
  for (byte i = 0; i < 10; i++) {
    //wdt_disable();
    Serial.print(F("S_1 intento # ")); Serial.println(i + 1);
    sensor_1.begin();

    if ((sensor_1.getDeviceCount() < 1) || (!sensor_1.getAddress(dir_sensor1, 0))) {
      Serial.println(F("Error sensor1"));
      errorSonda1 = true;
      return false;
    }
    else {
      Serial.println(F("sensor1 Ok"));
      sensor_1.setResolution(dir_sensor1, 12);
      return true;
    }
  }
}

//-- Funcion para inicializar sonda 2
bool inicializa_sonda2() {
  for (byte i = 0; i < 10; i++) {
    //wdt_disable();
    Serial.print(F("S_2 intento # ")); Serial.println(i + 1);
    sensor_2.begin();

    if ((sensor_2.getDeviceCount() < 1) || (!sensor_2.getAddress(dir_sensor2, 0))) {
      Serial.println(F("Error sensor2"));
      errorSonda2 = true;
      return false;
    }
    else {
      Serial.println(F("sensor2 Ok"));
      sensor_2.setResolution(dir_sensor2, 12);
      return true;
    }
  }
}

//-- Funcion para inicializar sonda 3
bool inicializa_sonda3() {
  for (byte i = 0; i < 10; i++) {
    //wdt_disable();
    Serial.print(F("S_3 intento # ")); Serial.println(i + 1);
    sensor_3.begin();

    if ((sensor_3.getDeviceCount() < 1) || (!sensor_3.getAddress(dir_sensor3, 0))) {
      Serial.println(F("Error sensor3"));
      errorSonda3 = true;
      return false;
    }
    else {
      Serial.println(F("sensor3 Ok"));
      sensor_3.setResolution(dir_sensor3, 12);
      return true;
    }
  }
}

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
