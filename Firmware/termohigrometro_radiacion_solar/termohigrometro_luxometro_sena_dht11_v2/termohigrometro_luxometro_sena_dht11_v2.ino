#include <DHT.h>

//-- LIBRERÍAS --
#include <SPI.h>
#include <RH_RF95.h>
#include "DHT.h"
#include <LowPower.h>
//#include <avr/wdt.h>

//-- CONSTANTES
#define ID_EQUIPO "THX0003"
#define CICLOS_AHORRO_ENERGIA 150
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
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  Serial.begin(9600);
  Serial.println("\nTERMOHIGROMETRO SGPS11279 V0.0.1");
  //-- Inicializa pines digitales I/O
  pinMode(PIN_LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  pinMode(PIN_SENSOR_UV, INPUT);
  pinMode(PIN_REF_3V3, INPUT);
  dht.begin();
  delay(1000);

  //-- Inicializa sistema de comunicaciones
  if (!inicializa_lora()) Serial.println(F("Error inicializando LoRa"));

  delay(1000);
}

void loop() {
  yield();
  // lectura de
  int lecturaUv = promedioAnalogico();
  int nivel_referencia = promedioAnalogico_();

  float voltaje = 3.3 / nivel_referencia * lecturaUv;
  float intensidadUV = mapFloat(voltaje, 0.99, 2.8, 0.0, 15.0);

  Serial.println("\nlecturaUv: " + String(lecturaUv));
  Serial.println("nivel_referencia: " + String(nivel_referencia));
  Serial.println("voltaje: " + String(voltaje));
  Serial.println("intensidadUV: " + String(intensidadUV));  //-- Se expresa en mW/cm^2


  float t = dht.readTemperature();
  float h = dht.readHumidity();
  data = "{\"id\":\"";
  data.concat(ID_EQUIPO);
  data.concat("\",\"bat\":");
  data.concat(medir_bateria());

  //Serial.print("t: ");
  //Serial.println(t);
  //Serial.print("h: ");
  //Serial.println(h);

  if (!isnan(h) && !isnan(t) && h > 0 && h < 100 && t > -10 && t < 100) {
    data.concat(",\"t\":");
    data.concat(String(t, 1));
    data.concat(",\"h\":");
    data.concat(String(h, 1));
  }

  data.concat(",\"uv\":");
  data.concat(String(intensidadUV,2));
  //data.concat(",\"iuv\":");
  //data.concat(String(indice));

  data.concat("}");

  //Serial.println(data);

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

float promedioAnalogico() {
  byte lecturas = 8;
  unsigned int la_lectura = 0;
  for (byte i = 0; i < lecturas; i++) {
    Serial.print(".");
    la_lectura += analogRead(PIN_SENSOR_UV);
    delay(50);
  }
  Serial.println(".");

  return la_lectura / lecturas;
}

float promedioAnalogico_() {
  byte lecturas = 8;
  unsigned int la_lectura = 0;
  for (byte i = 0; i < lecturas; i++) {
    Serial.print(".");
    la_lectura += analogRead(PIN_REF_3V3);
    delay(50);
  }
  Serial.println(".");

  return la_lectura / lecturas;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void parpadeo(byte veces, int tiempo) {
  for (byte i = 0; i < veces; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(tiempo);
    digitalWrite(PIN_LED, LOW);
    delay(tiempo);
  }
}
