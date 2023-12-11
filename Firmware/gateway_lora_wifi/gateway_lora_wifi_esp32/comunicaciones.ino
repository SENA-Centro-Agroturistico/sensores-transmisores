void setupWifi() {
  int n = WiFi.scanNetworks();
  byte coincide = 0;
  //Serial.print("Redes encontradas: ");
  //Serial.println(n);

  byte lectura;
  String ssid_alm = "";
  for (int i = 0; i < 50; i++) {
    lectura = EEPROM.read(i);
    if (lectura != 255) {
      ssid_alm += (char)lectura;
    }
  }
  //Serial.print(F("ssid_alm: "));
  //Serial.println(ssid_alm);
  byte lectura2;
  String pass_alm = "";
  for (int i = 50; i < 100; i++) {
    lectura2 = EEPROM.read(i);
    if (lectura2 != 255) {
      pass_alm += (char)lectura2;
    }
  }
  //Serial.print(F("pass_alm: "));
  //Serial.println(pass_alm);
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      String ssid_actual = WiFi.SSID(i);
      if (ssid_actual.equals(ssid_alm)) {
        coincide++;
        //Serial.print(F("Coincide con "));
        //Serial.println(ssid_alm);
        WiFi.mode(WIFI_STA);
        delay(10);
        int long tiempo = millis();
        WiFi.begin(ssid_alm.c_str(), pass_alm.c_str());
        while ((WiFi.status() != WL_CONNECTED) && (millis() - tiempo < TIEMPO_CONEXION_WIFI)) {
          Serial.print(F("."));
          parpadeo(1, 75);
          yield();
        }
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println(F("Conexión exitosa"));
          if (Ping.ping(remote_host)) {
            Serial.println("Hay conexión a internet");
            consulta_hora_internet();
            if (reconnect()) {
              Serial.println("MQTT Ok");
            } else {
              Serial.println("Falla MQTT");
            }
          }
        } else {
          Serial.println(F("Fallo de conexión Wifi"));
          diagnostico_wifi(WiFi.status());
        }
      }
    }
  } else {
    Serial.println(F("No se encontraron redes"));
  }
  if (coincide == 0) {
    Serial.println("No coincidencias");
  }
}

void diagnostico_wifi(int value) {
  switch (value) {
    case 0:
      Serial.println(F("WL_IDLE_STATUS - Wi-Fi is in process of changing between statuses"));
      break;
    case 1:
      Serial.println(F("WL_NO_SSID_AVAIL - SSID configurada no está disponible"));
      break;
    // case 3:
    //   Serial.println(F("WL_CONNECTED - Conexion establecida"));
    //   break;
    case 4:
      Serial.println(F("WL_CONNECT_FAILED - Password equivocado"));
      break;
    case 6:
      Serial.println(F("WL_DISCONNECTED - El modulo no esta configurado en STA"));
      break;
  }
}

bool reconnect() {
  Serial.println(F("Reconnect"));
  byte retries = 2;
  char id_mqtt[50];
  for (int i = 0; i < retries; i++) {
    yield();
    String clientId = "SENA-";
    clientId += String(random(0xffff), HEX);
    Serial.print(F("clientId: "));
    Serial.println(clientId);
    // if (client.connect(clientId.c_str(), usuarioMqtt, passMqtt)) {
      if (client.connect(clientId.c_str())) {
      return true;
    } else {
      int valueMqtt = client.state();
      diagnostico_mqtt(valueMqtt);
      Serial.println(F("intentando nuevamente..."));
      delay(2000);
      yield();
    }
    yield();
  }
  return false;
}

void diagnostico_mqtt(int value) {
  switch (value) {
    case -4:
      Serial.println(F("MQTT_CONNECTION_TIMEOUT - Servidor no responde dentro del tiempo \"keepalive\""));
      break;
    case -3:
      Serial.println(F("MQTT_CONNECTION_LOST - conexion de red rota"));
      break;
    case -2:
      Serial.println(F("MQTT_CONNECT_FAILED - fallo de conexion de red"));
      break;
    case -1:
      Serial.println(F("MQTT_DISCONNECTED - El cliente se desconecta limpiamente"));
      break;
    case 0:
      Serial.println(F("MQTT_CONNECTED - El cliente esta conectado"));
      break;
    case 1:
      Serial.println(F("MQTT_CONNECT_BAD_PROTOCOL - El servidor no soporta la version de MQTT request"));
      break;
    case 2:
      Serial.println(F("MQTT_CONNECT_BAD_CLIENT_ID - El servidor rechaza el identificador del cliente"));
      break;
    case 3:
      Serial.println(F("MQTT_CONNECT_UNAVAILABLE - El servidor no puede aceptar la conexion"));
      break;
    case 4:
      Serial.println(F("MQTT_CONNECT_BAD_CREDENTIALS - El usuario/password fue rechazado"));
      break;
    case 5:
      Serial.println(F("MQTT_CONNECT_UNAUTHORIZED - El cliente no estaba autorizado para conectarse"));
      break;
  }
}

void enviarMensaje() {
  client.loop();
  //Serial.println("");
  bool wifiOk = false;
  bool internetOk = false;
  bool mqttOk = false;
  if (WiFi.status() == WL_CONNECTED) wifiOk = true;
  if (Ping.ping(remote_host)) internetOk = true;
  if (client.connected()) mqttOk = true;
  if (wifiOk && internetOk && mqttOk) {
    Serial.println(F("Enviando..."));
    client.publish(topic, data_enviar.c_str());
  } else {
    Serial.println(F("Mensaje no enviado"));
    Serial.print(F("wifi ok: "));
    Serial.println(wifiOk);
    Serial.print(F("internet ok: "));
    Serial.println(internetOk);
    Serial.print(F("mqtt ok: "));
    Serial.println(mqttOk);
  }
}

void verificar_conexion() {
  Serial.println(F("Verificando conexión"));
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      reconnect();
    }
  } else {
    Serial.println(F("-- Desconectado --"));
    setupWifi();
  }
}