
#include <Wire.h>			// libreria de comunicacion por I2C
#include <LCD.h>			// libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C
#include <RTClib.h>

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

RTC_DS3231 rtc;

int A = 32; 			//variable A a pin digital 2 (DT en modulo)
int B = 33;  			//variable B a pin digital 4 (CLK en modulo)
int C = 25;
int aux=0;

int opcion = 0;
int menu = 0;


int ANTERIOR = 0;		// almacena valor anterior de la variable POSICION
int POSICION_A = 0;
volatile int POSICION = 0;	// variable POSICION con valor inicial de 50 y definida
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
  pinMode(C, INPUT_PULLUP);

  Serial.begin(9600);		// incializacion de comunicacion serie a 9600 bps

  attachInterrupt(digitalPinToInterrupt(A), encoder, FALLING);// interrupcion sobre pin A con
  attachInterrupt(digitalPinToInterrupt(C), push_b, FALLING);// interrupcion sobre pin A con

							  // funcion ISR encoder y modo LOW
  Serial.println("Listo");	// imprime en monitor serial Listo


}



void loop(){


  switch (aux) {
    case 1:
        lcd.clear();
        POSICION = 0;
        while(1){
        num_capas();  
        }
        aux=0;
        lcd.clear();
        break;
    case 2:
        lcd.clear();
        POSICION=0;
        aux = 0;
        while(1){
        
        
        switch(aux){
          case 1:

              break;
        
          case 2:

              while(1){
                nuevo_modo();
              }
              lcd.clear();
              aux=0;

              break;

          default:
              sub_menu();
        }
        }
        break;
    default:
          menu_inicial();
          hora();
  }





if (POSICION != ANTERIOR) {	// si el valor de POSICION es distinto de ANTERIOR
    Serial.println(POSICION);	// imprime valor de POSICION
    ANTERIOR = POSICION ;	// asigna a ANTERIOR el valor actualizado de POSICION
  }

}



void menu_inicial(){
  lcd.setCursor(1,0);
  lcd.print("Iniciar");
  lcd.setCursor(1,1);
  lcd.print("Configuracion");

  if (POSICION >=0 && POSICION <POSICION_A+10){
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,0);
    lcd.print("-");
    opcion = 1;

  } 
  
  if(POSICION >=POSICION_A+10 && POSICION <POSICION_A+20){
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print("-");
    opcion = 2;
  }

  if (POSICION == POSICION_A+20){
    POSICION_A=POSICION;
  }
  if (POSICION == POSICION_A-1){
    POSICION_A = POSICION-19;
  }

}

void sub_menu(){
  lcd.setCursor(1,0);
  lcd.print("CAMBIAR MODO");
  lcd.setCursor(1,1);
  lcd.print("MODO NUEVO");
  lcd.setCursor(1,3);
  lcd.print("CANCELAR");

  if (POSICION >=0 && POSICION <POSICION_A+10){
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,3);
    lcd.print(" ");

    lcd.setCursor(0,0);
    lcd.print("-");

  } 
  
  if(POSICION >=POSICION_A+10 && POSICION <POSICION_A+20){
    lcd.setCursor(0,0);
    lcd.print(" ");

    lcd.setCursor(0,3);
    lcd.print(" ");

    lcd.setCursor(0,1);
    lcd.print("-");
  }

  if(POSICION >=POSICION_A+20 && POSICION <POSICION_A+30){
    lcd.setCursor(0,1);
    lcd.print(" ");

    lcd.setCursor(0,0);
    lcd.print(" ");

    lcd.setCursor(0,3);
    lcd.print("-");
  }
  if (POSICION == POSICION_A+30){
    POSICION_A=POSICION;
  }
  if (POSICION == POSICION_A-1){
    POSICION_A = POSICION-29;
  }



}

void num_capas(){

  lcd.setCursor(1,0);
  lcd.print("#Capas: ");
  lcd.print(POSICION);
  lcd.setCursor(1,2);
  lcd.print("Aceptar");
  lcd.setCursor(1,3);
  lcd.print("Cancelar");

}


void nuevo_modo(){
  lcd.setCursor(1,0);
  lcd.print("NUEVO MODO");
  lcd.setCursor(1,2);
  lcd.print("EJEX (0-100): ");
  lcd.print(POSICION);
  lcd.setCursor(1,3);
  lcd.print("EJEY (0-100): ");
  lcd.print(POSICION);


}
  void hora(){
  DateTime fecha = rtc.now();

  lcd.setCursor(0, 3);		// ubica cursor en columna 0 y linea 1
  lcd.print("Hora: ");
  lcd.print(fecha.hour());		// funcion millis() / 1000 para segundos transcurridos
  lcd.print(":");
  lcd.print(fecha.minute());		// funcion millis() / 1000 para segundos transcurridos
  lcd.print(":");
  lcd.print(fecha.second());		// funcion millis() / 1000 para segundos transcurridos

  }


void push_b(){

  

  if (POSICION >=0 && POSICION <POSICION_A+10){
    Serial.println("OPCION1");
    aux = 1;
  } 
  
  if(POSICION >=POSICION_A+10 && POSICION <POSICION_A+20){
    Serial.println("OPCION2");
    aux = 2;
  } 


  if(POSICION >=POSICION_A+10 && POSICION <POSICION_A+20){
    Serial.println("OPCION2");
    aux = 2;
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

    if (POSICION >100){
      POSICION = 0;
      POSICION_A = 0;
    }
    if(POSICION <0){
      POSICION = 100;
      POSICION_A = 80;
    }

    //POSICION = min(55, max(50, POSICION));	// establece limite inferior de 0 y
						// superior de 100 para POSICION
    ultimaInterrupcion = tiempoInterrupcion;	// guarda valor actualizado del tiempo
  }						// de la interrupcion en variable static
}


int max(int num1, int num2) {
    if (num1 > num2) {
        return num1;
    } else {
        return num2;
    }
}


int min(int num1, int num2) {
    if (num1 > num2) {
        return num2;
    } else {
        return num1;
    }
}