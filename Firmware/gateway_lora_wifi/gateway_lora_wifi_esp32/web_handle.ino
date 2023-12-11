void paginaConfig() {
  Serial.println("paginaConfig");
  server.send(200, "text/html", pagina + mensaje + paginafin);
}

void guardarConfig() {
  Serial.println("guardarConfig");


  Serial.println(server.arg("s"));
  Serial.println(server.arg("p"));

  grabarEEPROM(0, server.arg("s"), 50);
  grabarEEPROM(50, server.arg("p"), 50);
  mensaje = "Configuracion Guardada... Reiniciando";
  paginaConfig();
  long int pausaGuardar = millis();
  while (millis() - pausaGuardar < 1500) {
  }
  ESP.restart();
}

void escanear() {
  Serial.println("escanear");
  //mensaje = "Buscando redes... Un momento por favor";
  //paginaConfig();
  int n = WiFi.scanNetworks();
  Serial.println("Escaneo terminado");
  mensaje = "";
  if (n == 0) {
    Serial.println("No se encontraron redes inalámbricas");
    mensaje = "No se encontraron redes inalámbricas";
  } else {
    Serial.print(n);
    Serial.println("redes inalámbricas encontradas");
    mensaje = "<p>Redes encontradas</p><br>";
    if (n > 5) {
      n = 5;
    }
    for (int i = 0; i < n; i++) {
      // agrega al STRING "mensaje" la información de las redes encontradas
      //mensaje = (mensaje) + "<p>" + String(i + 1) + ": " + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") </p> \r\n";
      mensaje = mensaje + "<div style='margin-bottom: 10px;'><a href='#' onclick='c(this)' data-ssid='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + " dBm)</a></div>";
      delay(10);
    }
  }
  paginaConfig();
}

void handle_NotFound() {
  Serial.println("handle_NotFound");
  //server.send(404, "text/plain", "Not found");
}