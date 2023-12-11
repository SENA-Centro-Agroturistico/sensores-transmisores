//-- LIBRERÍAS --
#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>
#include <DHT.h>
#include "DHT.h"
#include <LowPower.h>
#include <BH1750.h>

//-- CONSTANTES
#define ID_EQUIPO "THX0004"
#define CICLOS_AHORRO_ENERGIA 250
#define PIN_LED A1
#define RFM95_RST 2
#define PIN_SENSOR_UV A0
#define PIN_SENSOR1 9
#define PIN_REF_3V3 A3
#define RFM95_CS 10
#define RFM95_INT 3
//      MOSI 11
//      MISO 12
//      SCK 13
#define RF95_FREQ 915.0
#define B_MAX 3300
#define B_MIN 2800
#define TIEMPO_MEDICION 600000  //-- 10 minutos x 60s x 1000ms. Toma una medida cada 10 min
#define INTENTOS_MEDICION 10
#define DHTTYPE DHT11

//-- VARIABLES
bool errorSonda1 = false;        //-- errores iniciales
bool errorMedidaSonda1 = false;  //-- errores de medición
String data;
bool enviadoOk = false;

DHT dht(PIN_SENSOR1, DHTTYPE);
BH1750 lightMeter;
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  Serial.begin(9600);
  Serial.println("\nTH LUX SGPS11279 V1.0.2");
  pinMode(PIN_LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  pinMode(PIN_SENSOR_UV, INPUT);
  pinMode(PIN_REF_3V3, INPUT);
  Wire.begin();
  delay(50);
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("lux ok"));
    //error_luz = false;
  }
  else {
    Serial.println(F("Error lux"));
    //error_luz = true;
  }
  dht.begin();
  delay(1000);

  //-- Inicializa sistema de comunicaciones
  if (!inicializa_lora()) Serial.println(F("Error inicializando LoRa"));

  delay(1000);
}

void loop() {
  yield();
  float lux = lightMeter.readLightLevel();
  Serial.println("Nivel Iluminación: " + String(lux)); //-- luxes
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  data = "{\"id\":\"";
  data.concat(ID_EQUIPO);
  data.concat("\",\"bat\":");
  data.concat(medir_bateria());

  if (!isnan(h) && !isnan(t) && h > 0 && h < 100 && t > -10 && t < 100) {
    data.concat(",\"t\":");
    data.concat(String(t, 1));
    data.concat(",\"h\":");
    data.concat(String(h, 1));
  }
  if (!isnan(lux) && lux > 0) {
    data.concat(",\"lux\":");
    data.concat(String(lux, 2));
  }
  data.concat("}");

  enviarMensaje();
  
  if (enviadoOk) {
    Serial.println(F("mensaje enviado con éxito"));
    parpadeo(1, 100);
  } else {
    Serial.println(F("Error enviando mensaje"));
    parpadeo(3, 100);
  }
  enviadoOk = false;
  for (byte i = 0; i < CICLOS_AHORRO_ENERGIA; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    delay(100);
  }
}

void parpadeo(byte veces, int tiempo) {
  for (byte i = 0; i < veces; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(tiempo);
    digitalWrite(PIN_LED, LOW);
    delay(tiempo);
  }
}
