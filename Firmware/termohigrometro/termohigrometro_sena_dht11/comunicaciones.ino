void enviarMensaje() {
  yield();
  //wdt_reset();
  Serial.println("\nenviando mensaje:");
  char mensaje[80];
  data.toCharArray(mensaje, 80);
  Serial.println(mensaje);
  rf95.send((uint8_t*)mensaje, 80);
  rf95.waitPacketSent(3500);
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (rf95.waitAvailableTimeout(2500)) {
    yield();
    if (rf95.recv(buf, &len)) {
      String data_ajustar = (char*)buf;
      Serial.print(F("data_ajustar: "));
      Serial.println(data_ajustar);
      if (data_ajustar.equals(ID_EQUIPO)) {
        enviadoOk = true;
      }
      else {
        enviadoOk = false;
      }
    }
    else {
      Serial.println(F("R-Fallo"));
      enviadoOk = false;
    }
  }
  else {
    Serial.println(F("No rta LORA"));
    enviadoOk = false;
  }
}
