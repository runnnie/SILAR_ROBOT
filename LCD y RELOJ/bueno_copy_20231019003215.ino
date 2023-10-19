
#include <Wire.h>			// libreria de comunicacion por I2C
#include <LCD.h>			// libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C
#include <RTClib.h>

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

RTC_DS3231 rtc;


void setup()
{
  lcd.setBacklightPin(3,POSITIVE);	// puerto P3 de PCF8574 como positivo
  lcd.setBacklight(HIGH);		// habilita iluminacion posterior de LCD
  lcd.begin(20,4);			// 16 columnas por 2 lineas para LCD 1602A
  lcd.clear();			// limpia pantalla

  if(! rtc.begin()){
    Serial.println("¡Modulo RTC no encontrado!");
    while(1);
  }
  //rtc.adjust(DateTime(2023,08,10,11,17))
  //rtc.adjust(DateTime(__DATE__,__TIME__));
}



void loop()
{

  DateTime fecha = rtc.now();

  lcd.setCursor(0, 0);		// ubica cursor en columna 0 y linea 0
  lcd.print("Hola, la hora es:");	// escribe el texto
  lcd.setCursor(0, 1);		// ubica cursor en columna 0 y linea 1
  lcd.print(fecha.hour());		// funcion millis() / 1000 para segundos transcurridos
  lcd.print(":");
  lcd.print(fecha.minute());		// funcion millis() / 1000 para segundos transcurridos
  lcd.print(":");
  lcd.print(fecha.second());		// funcion millis() / 1000 para segundos transcurridos



}