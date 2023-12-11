//-- LIBRERÍAS --
#include <SPI.h>
#include <RH_RF95.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LowPower.h>
//#include <avr/wdt.h>

//-- CONSTANTES
#define ID_EQUIPO "TM3S002"
#define CICLOS_AHORRO_ENERGIA 250  // EN PRODUCCIÓN SERÁN 75
#define PIN_LED A1
#define RFM95_RST 2
#define PIN_SENSOR1 8
#define PIN_SENSOR2 7
#define PIN_SENSOR3 6
#define RFM95_CS 10
#define RFM95_INT 3
//      MOSI 11
//      MISO 12
//      SCK 13
#define RF95_FREQ 915.0
#define B_MAX 3300
#define B_MIN 2800
#define TIEMPO_MEDICION 600000     //-- 10 minutos x 60s x 1000ms. Toma una medida cada 10 min
#define INTENTOS_MEDICION 10

//-- VARIABLES
bool errorSonda1 = false; //-- errores iniciales
bool errorSonda2 = false; //-- errores iniciales
bool errorSonda3 = false; //-- errores iniciales
bool errorMedidaSonda1 = false; //-- errores de medición
bool errorMedidaSonda2 = false; //-- errores de medición
bool errorMedidaSonda3 = false; //-- errores de medición
String data;
bool enviadoOk = false;
//unsigned long millisMedidas;
//bool primera_medida = true;

OneWire oneWire_1(PIN_SENSOR1);
OneWire oneWire_2(PIN_SENSOR2);
OneWire oneWire_3(PIN_SENSOR3);

DallasTemperature sensor_1(&oneWire_1);
DallasTemperature sensor_2(&oneWire_2);
DallasTemperature sensor_3(&oneWire_3);

DeviceAddress dir_sensor1, dir_sensor2, dir_sensor3;

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  Serial.begin(9600);
  Serial.println("\nTERMÓMETRO 3 SONDAS SGPS11279 V0.0.2");
  //-- Inicializa pines digitales I/O
  pinMode(PIN_LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);

  //-- inicializa las sondas de temperatura
  if (!inicializa_sonda1()) Serial.println(F("Error inicializando sonda 1"));
  if (!inicializa_sonda2()) Serial.println(F("Error inicializando sonda 2"));
  if (!inicializa_sonda3()) Serial.println(F("Error inicializando sonda 3"));

  //-- Inicializa sistema de comunicaciones
  if (!inicializa_lora()) Serial.println(F("Error inicializando LoRa"));


  //millisMedidas = millis();
  delay(1000);
}

void loop() {
  yield();
  //if ((millis() - millisMedidas > TIEMPO_MEDICION) || primera_medida) {
  //primera_medida = false;

  float t1, t2, t3;
  data = "{\"id\":\"";
  data.concat(ID_EQUIPO);
  data.concat("\",\"bat\":");
  data.concat(medir_bateria());

  //-- Inicio de mediciones
  for (byte i = 0; i < INTENTOS_MEDICION; i++) {
    t1 = tomar_medida(1);
    if (!errorMedidaSonda1) i = INTENTOS_MEDICION;
  }
  for (byte i = 0; i < INTENTOS_MEDICION; i++) {
    t2 = tomar_medida(2);
    if (!errorMedidaSonda2) i = INTENTOS_MEDICION;
  }
  for (byte i = 0; i < INTENTOS_MEDICION; i++) {
    t3 = tomar_medida(3);
    if (!errorMedidaSonda3) i = INTENTOS_MEDICION;
  }

  if (!errorMedidaSonda1) {
    data.concat(",\"t1\":");
    data.concat(String(t1, 1));
  }

  if (!errorMedidaSonda2) {
    data.concat(",\"t2\":");
    data.concat(String(t2, 1));
  }

  if (!errorMedidaSonda3) {
    data.concat(",\"t3\":");
    data.concat(String(t3, 1));
  }

  data.concat("}");
  //Serial.println(data);

  enviarMensaje();
  if (enviadoOk) {
    Serial.println(F("mensaje enviado con éxito"));
    parpadeo(1, 100);
  } else {
    Serial.println(F("Error enviando mensaje"));
    parpadeo(3, 100);
    //millisMedidas = millis();
  }
  //millisMedidas = millis();
  enviadoOk = false;
  
  for (byte i = 0; i < CICLOS_AHORRO_ENERGIA; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    Serial.print(".");
    Serial.flush();
    delay(100);
  }
  //}
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
