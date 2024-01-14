
#include <Wire.h>			// libreria de comunicacion por I2C
#include <LCD.h>			// libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C                                                   
#include <RTClib.h>

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

RTC_DS3231 rtc;

//int A = 32; 			//variable A a pin digital 2 (DT en modulo)
//int B = 33;  			//variable B a pin digital 4 (CLK en modulo)
//int C = 25;
//int D = 26;

//int E = 27;
//int F = 14;


int A = 15; 			//variable A a pin digital 2 (DT en modulo)
int B = 2;  			//variable B a pin digital 4 (CLK en modulo)
int C = 16;


int E = 0;
int F = 4;
int D = 17;







int NUMERO_PASO;

int aux=0;
bool boleana = 0;
bool aux_submenu = 0;


int opcion = 0;
int menu = 0;


int ANTERIOR_A = 0;		// almacena valor ANTERIOR_A de la variable POSICION_1
volatile int POSICION_A = 0;
volatile int POSICION_1 = 0;	// variable POSICION_1 con valor inicial de 50 y definida
				// como global al ser usada en loop e ISR (encoder1)


int ANTERIOR_B = 0;
volatile int POSICION_B=0;
volatile int POSICION_2=0;

int tiempo1=10;
int  tiempo2=5;
int  tiempo3=10;
int  tiempo4=5;

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
  pinMode(D, INPUT_PULLUP);
  pinMode(E, INPUT_PULLUP);
  pinMode(F, INPUT_PULLUP);


  Serial.begin(9600);		// incializacion de comunicacion serie a 9600 bps

  attachInterrupt(digitalPinToInterrupt(A), encoder1, FALLING);// interrupcion sobre pin A con
  attachInterrupt(digitalPinToInterrupt(E), encoder2, FALLING);// interrupcion sobre pin A con

  attachInterrupt(digitalPinToInterrupt(D), push_a, FALLING);// interrupcion sobre pin A con
  attachInterrupt(digitalPinToInterrupt(C), push_b, FALLING);// interrupcion sobre pin A con

							  // funcion ISR encoder1 y modo LOW
  Serial.println("Listo");	// imprime en monitor serial Listo


}



void loop(){

  //Menú principal

  if(digitalRead(C) == LOW){
    delay(200);
  }
  switch (aux) {

    //Opción 1 = Iniciar
    case 1:
        lcd.clear();
        ANTERIOR_A=0;
        POSICION_1 = 0;
        POSICION_A=0;
        while(1){
        num_capas();  

        if(digitalRead(C)==LOW){
          lcd.clear();
          while(1){
            lcd.setCursor(1,0);
            lcd.print("*PROCESO EN CURSO*");
            lcd.setCursor(1,2);
            lcd.print("#CAPA: ");
            lcd.setCursor(1,3);
            lcd.print("TIEMPO: ");
            
          }
        }



        if (POSICION_1 != ANTERIOR_A) {	// si el valor de POSICION_1 es distinto de ANTERIOR_A
          Serial.println(POSICION_1);	// imprime valor de POSICION_1
          ANTERIOR_A = POSICION_1 ;	// asigna a ANTERIOR_A el valor actualizado de POSICION_1

        }

        if (boleana == 1){
          boleana = 0;
          aux = 0;
          ANTERIOR_A = 0;
          POSICION_1=0;
          POSICION_A=0;
          break;
        }

        }
        lcd.clear();
        break;

    //Opción 2 = Configuración
    case 2:
        lcd.clear();
        ANTERIOR_A = 0;
        POSICION_1=0;
        POSICION_A=0;
        aux = 0;
        aux_submenu = 1;
        while(1){


        if(digitalRead(C) == LOW){
          delay(200);
        }
        if(digitalRead(D) == LOW){
          delay(200);
        }
        //Menú de configuración
        switch(aux){

          //Opción 1 de configuración = Cambiar modo
          case 1:
              lcd.clear();
              ANTERIOR_A = 0;
              POSICION_1 = 0;
              POSICION_A=0;

              while(1){
                lcd.setCursor(0,0);
                lcd.print("MODOS:");

                if (POSICION_1 != ANTERIOR_A) {	// si el valor de POSICION_1 es distinto de ANTERIOR_A
                  Serial.println(POSICION_1);	// imprime valor de POSICION_1
                  ANTERIOR_A = POSICION_1 ;	// asigna a ANTERIOR_A el valor actualizado de POSICION_1
                }

                if (boleana == 1){
                  boleana = 0;
                  aux = 0;
                  ANTERIOR_A = 0;
                  POSICION_1 = 0;
                  POSICION_A=0;
                  lcd.clear();
                  break;
                }
              }

              break;

          //Opción 2 Modificar tiempos

          case 2:
              lcd.clear();
              ANTERIOR_A = 0;
              POSICION_1 = 0;
              POSICION_A=0;

              ANTERIOR_B = 0;
              POSICION_2 = 0;
              POSICION_B=0;
              while(1){
                if(digitalRead(D) == LOW){
                  delay(200);
                }
                sub_menu_tiempo();

                if (POSICION_1 != ANTERIOR_A) {	// si el valor de POSICION_1 es distinto de ANTERIOR_A
                  Serial.println(POSICION_1);	// imprime valor de POSICION_1
                  ANTERIOR_A = POSICION_1 ;	// asigna a ANTERIOR_A el valor actualizado de POSICION_1
                }

                if (POSICION_2 != ANTERIOR_B) {	// si el valor de POSICION_1 es distinto de ANTERIOR_A
                    Serial.println(POSICION_2);	// imprime valor de POSICION_1
                    ANTERIOR_B = POSICION_2 ;	// asigna a ANTERIOR_A el valor actualizado de POSICION_1
                  }

                if (boleana == 1){
                  boleana = 0;
                  aux = 0;
                  ANTERIOR_A = 0;
                  POSICION_1 = 0;
                  POSICION_A=0;
                  lcd.clear();
                  break;
                }

              }
              
          break;
          //Opción 3 de cofiguración = Modo Nuevo

          case 3:
              lcd.clear();
              ANTERIOR_A = 0;
              POSICION_1 = 0;
              POSICION_A = 0;
              ANTERIOR_B = 0;
              POSICION_2 = 0;
              POSICION_B = 0;
              NUMERO_PASO = 0;

              while(1){
                if(digitalRead(D) == LOW){
                  delay(200);
                }

                nuevo_modo();

                if (POSICION_1 != ANTERIOR_A) {	// si el valor de POSICION_1 es distinto de ANTERIOR_A
                    Serial.println(POSICION_1);	// imprime valor de POSICION_1
                    ANTERIOR_A = POSICION_1 ;	// asigna a ANTERIOR_A el valor actualizado de POSICION_1
                  }


                if (POSICION_2 != ANTERIOR_B) {	// si el valor de POSICION_1 es distinto de ANTERIOR_A
                    Serial.println(POSICION_2);	// imprime valor de POSICION_1
                    ANTERIOR_B = POSICION_2 ;	// asigna a ANTERIOR_A el valor actualizado de POSICION_1
                  }
                if (boleana == 1){
                  boleana = 0;
                  aux = 0;
                  ANTERIOR_A = 0;
                  POSICION_1 = 0;
                  POSICION_A=0;
                  lcd.clear();
                  break;
                }
              }

              break;

          //En caso de que no se haya seccionado ninguna opción, se imprime el menú de 
          //la opción de configuración
          default:
              sub_menu();

        }


        if (POSICION_1 != ANTERIOR_A) {	// si el valor de POSICION_1 es distinto de ANTERIOR_A
            Serial.println(POSICION_1);	// imprime valor de POSICION_1
            ANTERIOR_A = POSICION_1 ;	// asigna a ANTERIOR_A el valor actualizado de POSICION_1
          }

        if (POSICION_2 != ANTERIOR_B) {	// si el valor de POSICION_1 es distinto de ANTERIOR_A
            Serial.println(POSICION_2);	// imprime valor de POSICION_1
            ANTERIOR_B = POSICION_2 ;	// asigna a ANTERIOR_A el valor actualizado de POSICION_1
          }

  

        if (boleana == 1){
        boleana = 0;
        aux=0;
        aux_submenu = 0;
        ANTERIOR_A = 0;
        POSICION_1 = 0;
        POSICION_A=0;
        lcd.clear();
        break;
        }


        }
        break;
    //En caso de que no se haya seleccionado nada, se imprime el menú principal
    default:
          menu_inicial();
          hora();
  }





if (POSICION_1 != ANTERIOR_A) {	// si el valor de POSICION_1 es distinto de ANTERIOR_A
    Serial.println(POSICION_1);	// imprime valor de POSICION_1
    ANTERIOR_A = POSICION_1 ;	// asigna a ANTERIOR_A el valor actualizado de POSICION_1
  }
if (POSICION_2 != ANTERIOR_B) {	// si el valor de POSICION_1 es distinto de ANTERIOR_A
    Serial.println(POSICION_2);	// imprime valor de POSICION_1
    ANTERIOR_B = POSICION_2 ;	// asigna a ANTERIOR_A el valor actualizado de POSICION_1
  }

  
  

}



void menu_inicial(){
  lcd.setCursor(1,0);
  lcd.print("INICIAR");
  lcd.setCursor(1,1);
  lcd.print("CONFIGURACION");

  if (POSICION_1 >=0 && POSICION_1 <POSICION_A+5){
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,0);
    lcd.print("-");

  } 
  
  if(POSICION_1 >=POSICION_A+5 && POSICION_1 <POSICION_A+10){
    lcd.setCursor(0,0);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.print("-");
  }

  if (POSICION_1 == POSICION_A+10){
    POSICION_A=POSICION_1;
  }
  if (POSICION_1 == POSICION_A-1){
    POSICION_A = POSICION_1-9;
  }

}

void sub_menu(){
  lcd.setCursor(1,0);
  lcd.print("CAMBIAR MODO");
  lcd.setCursor(1,1);
  lcd.print("MODIFICAR TIEMPOS");
  lcd.setCursor(1,2);
  lcd.print("MODO NUEVO");

  

  if (POSICION_1 >=0 && POSICION_1 <POSICION_A+5){
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,2);
    lcd.print(" ");

    lcd.setCursor(0,0);
    lcd.print("-");

  } 
  
  if(POSICION_1 >=POSICION_A+5 && POSICION_1 <POSICION_A+10){
    lcd.setCursor(0,0);
    lcd.print(" ");

    lcd.setCursor(0,2);
    lcd.print(" ");

    lcd.setCursor(0,1);
    lcd.print("-");
  }

  if(POSICION_1 >=POSICION_A+10 && POSICION_1 <POSICION_A+15){
    lcd.setCursor(0,1);
    lcd.print(" ");

    lcd.setCursor(0,0);
    lcd.print(" ");

    lcd.setCursor(0,2);
    lcd.print("-");
  }
  if (POSICION_1 == POSICION_A+15){
    POSICION_A=POSICION_1;
  }
  if (POSICION_1 == POSICION_A-1){
    POSICION_A = POSICION_1-14;
  }

  Serial.println(POSICION_1);

}

void sub_menu_tiempo(){


  lcd.setCursor(1,0);
  lcd.print("Paso 1: ");
  lcd.print(tiempo1);
  lcd.print(" min    ");


  lcd.setCursor(1,1);
  lcd.print("Paso 2: ");
  lcd.print(tiempo2);
  lcd.print(" min    ");
  
  lcd.setCursor(1,2);
  lcd.print("Paso 3: ");
  lcd.print(tiempo3);
    lcd.print(" min    ");
  
  lcd.setCursor(1,3);
  lcd.print("Paso 4: ");
  lcd.print(tiempo4);
    lcd.print(" min    ");

  POSICION_2=tiempo1;
  while (POSICION_1 >=0 && POSICION_1 <POSICION_A+5){

    lcd.setCursor(1,0);
    lcd.print("Paso 1: ");
    lcd.print(tiempo1);
    lcd.print(" min    ");

    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,2);
    lcd.print(" ");
    lcd.setCursor(0,3);
    lcd.print(" ");

    lcd.setCursor(0,0);
    lcd.print("-");



    tiempo1=POSICION_2;
    if (boleana == 1){
    break;
  } 
  }

  POSICION_2=tiempo2;
  while(POSICION_1 >=POSICION_A+5 && POSICION_1 <POSICION_A+10){


    lcd.setCursor(1,1);
    lcd.print("Paso 2: ");
    lcd.print(tiempo2);
    lcd.print(" min    ");
  
    lcd.setCursor(0,0);
    lcd.print(" ");

    lcd.setCursor(0,2);
    lcd.print(" ");

    lcd.setCursor(0,1);
    lcd.print("-");

    lcd.setCursor(0,3);
    lcd.print(" ");


    tiempo2=POSICION_2;
    if (boleana == 1){
    break;
  }
  }
  POSICION_2=tiempo3;
  while(POSICION_1 >=POSICION_A+10 && POSICION_1 <POSICION_A+15){

    lcd.setCursor(1,2);
    lcd.print("Paso 3: ");
    lcd.print(tiempo3);
    lcd.print(" min    ");

    lcd.setCursor(0,1);
    lcd.print(" ");

    lcd.setCursor(0,0);
    lcd.print(" ");

    lcd.setCursor(0,2);
    lcd.print("-");

    lcd.setCursor(0,3);
    lcd.print(" ");

    tiempo3=POSICION_2;
    if (boleana == 1){
    break;
  }
  }


POSICION_2=tiempo4;
while(POSICION_1 >=POSICION_A+15 && POSICION_1 <POSICION_A+20){

    lcd.setCursor(1,3);
    lcd.print("Paso 4: ");
    lcd.print(tiempo4);
    lcd.print(" min    ");

    lcd.setCursor(0,1);
    lcd.print(" ");

    lcd.setCursor(0,0);
    lcd.print(" ");

    lcd.setCursor(0,2);
    lcd.print(" ");

    lcd.setCursor(0,3);
    lcd.print("-");

    tiempo4=POSICION_2;



    if (boleana == 1){
    break;
    }
  }

  if (POSICION_1 == POSICION_A+20){
    POSICION_A=POSICION_1;
  }
  if (POSICION_1 == POSICION_A-1){
    POSICION_A = POSICION_1-19;
  }


}


void num_capas(){

  lcd.setCursor(2,1);
  lcd.print("NUMERO DE CAPAS:");
  lcd.setCursor(0,2);
  lcd.print("         ");
  lcd.print(POSICION_1);
  lcd.print("    ");



}


void nuevo_modo(){

  if(digitalRead(C)==LOW){
    delay(200);
    NUMERO_PASO++;
    lcd.clear();
    ANTERIOR_A = 0;
    POSICION_1 = 0;
    POSICION_A = 0;
    ANTERIOR_B = 0;
    POSICION_2 = 0;
    POSICION_B = 0;

  }
  lcd.setCursor(4,0);
  lcd.print("*NUEVO MODO*");
  lcd.setCursor(1,1);
  lcd.print("PASO: ");
  lcd.print(NUMERO_PASO);
  lcd.setCursor(1,2);
  lcd.print("EJEX (0-100): ");
  lcd.print(POSICION_1);
  lcd.print("  ");
  lcd.setCursor(1,3);
  lcd.print("EJEY (0-100): ");
  lcd.print(POSICION_2);
  lcd.print("  ");


}
  void hora(){
  DateTime fecha = rtc.now();

  lcd.setCursor(0, 3);		// ubica cursor en columna 0 y linea 1
  lcd.print("HORA: ");
  lcd.print(fecha.hour());		// funcion millis() / 1000 para segundos transcurridos
  lcd.print(":");
  lcd.print(fecha.minute());		// funcion millis() / 1000 para segundos transcurridos
  lcd.print(":");
  lcd.print(fecha.second());		// funcion millis() / 1000 para segundos transcurridos

  }


void push_a(){

    

    //Variable la cual hará que en determinado menú, nos regresemos al ANTERIOR_A
  if(digitalRead(D) == LOW){
    Serial.println("FUNCIONA1");
    boleana = 1;
    // Espera hasta que pase el intervalo
  }



  


}

void push_b(){

  
  if (POSICION_1 >=0 && POSICION_1 <POSICION_A+5){
    Serial.println("OPCION1");
    aux = 1;
  } 
  
  if(POSICION_1 >=POSICION_A+5 && POSICION_1 <POSICION_A+10){
    Serial.println("OPCION2");
    aux = 2;
  } 


  if(POSICION_1 >=POSICION_A+10 && POSICION_1 <POSICION_A+15 && aux_submenu==1){
    Serial.println("OPCION2");
    aux = 3;
  } 
  }



void encoder1()  {
  static unsigned long ultimaInterrupcion = 0;	// variable static con ultimo valor de
						// tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();	// variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {	// rutina antirebote desestima
							// pulsos menores a 5 mseg.
    if (digitalRead(B) == HIGH)			// si B es HIGH, sentido horario
    {
      POSICION_1++ ;				// incrementa POSICION_1 en 1
    }
    else {					// si B es LOW, senti anti horario
      POSICION_1-- ;				// decrementa POSICION_1 en 1
    }



    if (POSICION_1 >100){
      POSICION_1 = 0;
      POSICION_A = 0;
    }
    if(POSICION_1 <0){
      POSICION_1 = 100;
      POSICION_A = 80;
    }

    //POSICION_1 = min(55, max(50, POSICION_1));	// establece limite inferior de 0 y
						// superior de 100 para POSICION_1
    ultimaInterrupcion = tiempoInterrupcion;	// guarda valor actualizado del tiempo
  }						// de la interrupcion en variable static
}





void encoder2()  {
  static unsigned long ultimaInterrupcion = 0;	// variable static con ultimo valor de
						// tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();	// variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {	// rutina antirebote desestima
							// pulsos menores a 5 mseg.
    if (digitalRead(F) == HIGH)			// si B es HIGH, sentido horario
    {
      POSICION_2++ ;				// incrementa POSICION_1 en 1
    }
    else {					// si B es LOW, senti anti horario
      POSICION_2-- ;				// decrementa POSICION_1 en 1
    }



    if (POSICION_2 >100){
      POSICION_2 = 0;
      POSICION_B = 0;
    }
    if(POSICION_2 <0){
      POSICION_2 = 100;
      POSICION_B = 80;
    }

    //POSICION_1 = min(55, max(50, POSICION_1));	// establece limite inferior de 0 y
						// superior de 100 para POSICION_1
    ultimaInterrupcion = tiempoInterrupcion;	// guarda valor actualizado del tiempo
 
  Serial.println(POSICION_2);

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