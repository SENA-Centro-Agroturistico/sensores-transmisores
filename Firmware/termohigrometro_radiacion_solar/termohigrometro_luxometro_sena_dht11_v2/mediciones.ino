//-- Medicion de la bateria
byte medir_bateria() {
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC));
  uint8_t low = ADCL;
  uint8_t high = ADCH;
  long result = (high << 8) | low;
  result = 1125300L / result; // Calcula Vcc (en mV); 1125300 = 1.1*1023*1000
  //Serial.print(F("vBat: ")); Serial.print(result);
  if (result < B_MIN) result = B_MIN;
  if (result > B_MAX) result = B_MAX;
  byte bateria = map(result, B_MIN, B_MAX, 0, 100);
  //Serial.print(F(" - bateria: ")); Serial.println(bateria);
  return bateria; // Vcc en porcentaje 0 - 100% 3000 - 3300
}
