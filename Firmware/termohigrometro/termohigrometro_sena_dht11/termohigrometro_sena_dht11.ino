#include <DHT.h>

//-- LIBRERÍAS --
#include <SPI.h>
#include <RH_RF95.h>
#include "DHT.h"
#include <LowPower.h>
//#include <avr/wdt.h>

//-- CONSTANTES
#define ID_EQUIPO "THS0003"
#define CICLOS_AHORRO_ENERGIA 250
#define PIN_LED A1
#define RFM95_RST 2
#define PIN_SENSOR1 9
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
  Serial.println("\nTERMOHIGROMETRO SGPS11279 V1.0.2");
  //-- Inicializa pines digitales I/O
  pinMode(PIN_LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);

  dht.begin();

  delay(1000);

  //-- Inicializa sistema de comunicaciones
  if (!inicializa_lora()) Serial.println(F("Error inicializando LoRa"));

  delay(1000);
}

void loop() {
  yield();
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  data = "{\"id\":\"";
  data.concat(ID_EQUIPO);
  data.concat("\",\"bat\":");
  data.concat(medir_bateria());

  Serial.print("t: ");
  Serial.println(t);
  Serial.print("h: ");
  Serial.println(h);

  if (!isnan(h) && !isnan(t) && h > 0 && h < 100 && t > -10 && t < 100) {
    data.concat(",\"t\":");
    data.concat(String(t, 1));
    data.concat(",\"h\":");
    data.concat(String(h, 1));
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
  delay(1500);
}

void parpadeo(byte veces, int tiempo) {
  for (byte i = 0; i < veces; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(tiempo);
    digitalWrite(PIN_LED, LOW);
    delay(tiempo);
  }
}
