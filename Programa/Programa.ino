
#include <Wire.h>			// libreria de comunicacion por I2C
#include <LCD.h>			// libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C
#include <RTClib.h>

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

RTC_DS3231 rtc;

int A = 32; 			//variable A a pin digital 2 (DT en modulo)
int B = 33;  			//variable B a pin digital 4 (CLK en modulo)

int ANTERIOR = 50;		// almacena valor anterior de la variable POSICION

volatile int POSICION = 50;	// variable POSICION con valor inicial de 50 y definida
				// como global al ser usada en loop e ISR (encoder)

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

  pinMode(A, INPUT_PULLUP);		// A como entrada
  pinMode(B, INPUT_PULLUP);		// B como entrada

  Serial.begin(9600);		// incializacion de comunicacion serie a 9600 bps

  attachInterrupt(digitalPinToInterrupt(A), encoder, FALLING);// interrupcion sobre pin A con
							  // funcion ISR encoder y modo LOW
  Serial.println("Listo");	// imprime en monitor serial Listo


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

if (POSICION != ANTERIOR) {	// si el valor de POSICION es distinto de ANTERIOR
    Serial.println(POSICION);	// imprime valor de POSICION
    ANTERIOR = POSICION ;	// asigna a ANTERIOR el valor actualizado de POSICION
  }

}





void encoder()  {
  static unsigned long ultimaInterrupcion = 0;	// variable static con ultimo valor de
						// tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();	// variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {	// rutina antirebote desestima
							// pulsos menores a 5 mseg.
    if (digitalRead(B) == HIGH)			// si B es HIGH, sentido horario
    {
      POSICION++ ;				// incrementa POSICION en 1
    }
    else {					// si B es LOW, senti anti horario
      POSICION-- ;				// decrementa POSICION en 1
    }

    //POSICION = min(100, max(0, POSICION));	// establece limite inferior de 0 y
						// superior de 100 para POSICION
    ultimaInterrupcion = tiempoInterrupcion;	// guarda valor actualizado del tiempo
  }						// de la interrupcion en variable static
}