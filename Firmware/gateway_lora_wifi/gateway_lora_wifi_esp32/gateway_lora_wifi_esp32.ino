#include <ArduinoJson.h>
#include "variables.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <DNSServer.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <RTClib.h>
#include <PubSubClient.h>
#include <EEPROM.h>
WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
RH_RF95 rf95(RFM95_CS, RFM95_INT);
RTC_DS1307 rtc;

//----------------------------------------------------------
//------------------------- SETUP --------------------------
//----------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.print("\nGateway LoRa/Wifi - ESP32 V");
  Serial.println(VERSION);
  EEPROM.begin(256);

  //-- inicializa puertos i/o
  pinMode(PIN_LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);

  //-- inicializa I2C
  Wire.begin(PIN_SDA, PIN_SCL);

  if (!rtc.begin()) {
    Serial.println(F("No se encuentra RTC"));
  } else {
    Serial.println(F("RTC ok"));
  }

  //-- inicializa LoRa
  if (inicializa_lora()) {
    //Serial.println(F("LoRa ok"));
  } else {
    Serial.println(F("\nFallo LoRa General"));
  }

  //-- Hora actual --
  tiempo_actual = consultar_hora_rtc();
  Serial.print(F("Fecha y hora: "));
  Serial.println(tiempo_actual);

  byte config_wifi = digitalRead(PIN_BUTTON);
  if (config_wifi == 0) {
    digitalWrite(PIN_LED, HIGH);
    WiFi.softAP("SENA_AP");
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    server.on("/", paginaConfig);
    server.on("/guardarConfig", guardarConfig);  //función para guardar configuración en la EEPROM
    server.on("/escanear", escanear);
    server.onNotFound(handle_NotFound);
    server.begin();
    Serial.println("HTTP server started");
    while (true) {
      yield();
      server.handleClient();
    }
  }
  client.setServer(servidorMqtt, 13883);
  digitalWrite(PIN_LED, LOW);
  setupWifi();
  millisReset = millis();
  wifiMillis = millis();
  statMillis = millis();
}


//----------------------------------------------------------
//-------------------------- LOOP --------------------------
//----------------------------------------------------------
void loop() {
  client.loop();

  //-- Verificación Led Vivo
  if (millis() - statMillis > T_VIVO) {
    if (WiFi.status() == WL_CONNECTED) {
      parpadeo(1, 50);
    } else {
      parpadeo(2, 50);
    }
    statMillis = millis();
  }

  if (millis() - wifiMillis > T_VERIFICAR) {
    verificar_conexion();
    wifiMillis = millis();
  }

  if (rf95.available()) {
    digitalWrite(PIN_LED, HIGH);
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      msg = (char*)buf;
      //Serial.println(msg);
      //Serial.println(len);
      sMsg = String(msg);

      if (len > 20 && len < 201) {
        if (msg[0] == '{') {
          elId = parseMensaje();
        }
        byte longId_dec = elId.length() + 1;
        char rtaLoRa[longId_dec];
        elId.toCharArray(rtaLoRa, longId_dec);
        rf95.send((uint8_t*)rtaLoRa, sizeof(rtaLoRa));
        rf95.waitPacketSent();
      } else {
        Serial.println(F("too small"));
      }
    } else {
      Serial.println(F("recv failed"));
    }
    digitalWrite(PIN_LED, LOW);
  }

  if (msg_valido) {
    msg_valido = false;
    byte longitud = sMsg.length();
    String fec_data = ",\"f\":\"" + consultar_hora_rtc() + "\"}";
    data_enviar = sMsg.substring(0, longitud - 1) + fec_data;
    Serial.println(data_enviar);
    enviarMensaje();
  }
}
