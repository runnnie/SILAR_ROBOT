
#include <Wire.h>               // libreria de comunicacion por I2C
#include <LCD.h>                // libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>  // libreria para LCD por I2C
#include <RTClib.h>


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);  // DIR, E, RW, RS, D4, D5, D6, D7

RTC_DS3231 rtc;

//Stepper motor
//Counters for movements of stepper motors
int STEPSY = 0;
int STEPSX = 0;
int AUX_STEPSX;
int AUX_STEPSY;
//Counter of steps in a routine
int NUMERO_PASO = 0;
//Steps of the routine
volatile int X[20];
volatile int Y[20];
int MINUTOS[40];
int SEGUNDOS[40];


// Variables for encoders
int CLK_A = 33;
int DT_A = 32;
int BUTTON_A = 39;
int CLK_B = 35;
int DT_B = 34;
int BUTTON_B = 36;

//Variables for step motors
int DIRX = 26;
int PULX = 25;
int DIRY = 14;
int PULY = 27;
int VEL = 300;







volatile int POS_A = 0;      // variable POS_A con valor inicial de 50 y definida
volatile int AUX_POS_A = 0;  //almacena el valor de pos_A
volatile int AUX_POS_A2 = 0;

int AUX_PRINT_A = 0;  // almacena valor AUX_PRINT_A de la variable POS_A para luego imprimirla

int NUM_CAPAS;

volatile int POS_B = 0;
volatile int AUX_POS_B = 0;
volatile int AUX_POS_B2 = 0;

int AUX_PRINT_B = 0;

int opcion = 0;
int menu = 0;


int aux = 0;
bool boleana = 0;
bool aux_submenu = 0;
volatile int AUX_STEPS_X = 0;
volatile int AUX_STEPS_Y = 0;

int tiempo1 = 10;
int tiempo2 = 5;
int tiempo3 = 10;
int tiempo4 = 5;

void setup() {
  lcd.setBacklightPin(3, POSITIVE);  // puerto P3 de PCF8574 como positivo
  lcd.setBacklight(HIGH);            // habilita iluminacion posterior de LCD
  lcd.begin(20, 4);                  // 16 columnas por 2 lineas para LCD 1602A
  lcd.clear();                       // limpia pantalla

  //Pines of encoders
  pinMode(CLK_A, INPUT_PULLUP);
  pinMode(DT_A, INPUT_PULLUP);
  pinMode(CLK_B, INPUT_PULLUP);
  pinMode(DT_B, INPUT_PULLUP);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);

  //Pines of stepper motors
  pinMode(DIRX, OUTPUT);
  pinMode(DIRY, OUTPUT);
  pinMode(PULX, OUTPUT);
  pinMode(PULY, OUTPUT);


  attachInterrupt(digitalPinToInterrupt(BUTTON_B), push_a, FALLING);  // interrupcion sobre pin A con
  attachInterrupt(digitalPinToInterrupt(BUTTON_A), push_b, FALLING);  // interrupcion sobre pin A con
  attachInterrupt(digitalPinToInterrupt(CLK_A), encoder1, FALLING);   // interrupcion sobre pin A con
  attachInterrupt(digitalPinToInterrupt(CLK_B), encoder2, FALLING);   // interrupcion sobre pin A con


  Serial.begin(115200);
  Serial.println("Listo");  // imprime en monitor serial Listo

  if (!rtc.begin()) {
    Serial.println("¡Modulo RTC no encontrado!");
    while (1)
      ;
  }
  //rtc.adjust(DateTime(2023,08,10,11,17))
  //rtc.adjust(DateTime(__DATE__,__TIME__));
}



void loop() {
  //Menú principal

  if (digitalRead(BUTTON_A) == LOW) {
    delay(200);
  }
  switch (aux) {

    //Opción 1 = Iniciar
    case 1:
      lcd.clear();
      AUX_PRINT_A = 0;
      POS_A = 0;
      AUX_POS_A = 0;

      while (true) {
        num_capas();

        if (digitalRead(BUTTON_A) == LOW) {
          lcd.clear();
          DateTime fecha = rtc.now();

          for (int i = 0; i < NUM_CAPAS; i++) {

            lcd.setCursor(1, 0);
            lcd.print("*PROCESO EN CURSO*");
            lcd.setCursor(1, 1);
            lcd.print("PASO: ");

            lcd.setCursor(11, 1);
            lcd.print("CAPA: ");

            lcd.setCursor(1, 2);
            lcd.print("T PASO: ");

            lcd.setCursor(1, 4);
            lcd.print("T TOTAL: ");


            //Impresión del tiempo total


            for (int j = 0; j < NUMERO_PASO; j++) {

              STEPSX = X[j] * 150;
              STEPSY = Y[j] * 100;



              motor_movement();
              delay(500);
              //Primero se mueven los motores, una vez que estén en su lugar empieza a contar el tiempo del paso

              DateTime inicio = rtc.now();                                      // Momento en que comienza el período
              DateTime fin = inicio + TimeSpan(0, 0, MINUTOS[j], SEGUNDOS[j]);  // Calcula el momento en que terminará el período
              while (rtc.now() <= fin) {
                DateTime ahora = rtc.now();
                TimeSpan Transcurrido = ahora - inicio;
                TimeSpan Tiempo_total = ahora - fecha;

                //Impresión del paso en el que va
                lcd.setCursor(7, 1);
                lcd.print(String(j));



                //Impresión del número de la capa en la que va
                lcd.setCursor(16, 1);
                lcd.print(String(i));



                //impresión del tema dle paso
                lcd.setCursor(9, 2);
                lcd.print(Transcurrido.minutes());  // funcion millis() / 1000 para segundos transcurridos
                lcd.print(":");
                lcd.print(Transcurrido.seconds());  // funcion millis() / 1000 para segundos transcurridos
                lcd.print("  ");



                lcd.setCursor(10, 4);
                lcd.print(Tiempo_total.minutes());
                lcd.print(":");
                lcd.print(Tiempo_total.seconds());
                lcd.print("  ");
              }
            }
          }

          if (boleana == 1) {
            boleana = 0;
            aux = 0;
            AUX_PRINT_A = 0;
            POS_A = 0;
            AUX_POS_A = 0;
            break;
          }
          lcd.clear();
        }
      }
      lcd.clear();
      break;

    //Opción 2 = Configuración
    case 2:
      lcd.clear();
      AUX_PRINT_A = 0;
      POS_A = 0;
      AUX_POS_A = 0;
      aux = 0;
      aux_submenu = 1;
      while (1) {


        if (digitalRead(BUTTON_A) == LOW) {
          delay(200);
        }
        if (digitalRead(BUTTON_B) == LOW) {
          delay(200);
        }
        //Menú de configuración
        switch (aux) {

          //Opción 1 de configuración = Cambiar modo
          case 1:
            lcd.clear();
            AUX_PRINT_A = 0;
            POS_A = 0;
            AUX_POS_A = 0;

            while (1) {
              lcd.setCursor(0, 0);
              lcd.print("MODOS:");
              lcd.setCursor(1, 1);


              if (POS_A != AUX_PRINT_A) {                                            // si el valor de POS_A es distinto de AUX_PRINT_A
                Serial.println("x: " + String(POS_A) + " y: " + String(AUX_POS_B));  // imprime valor de POS_A
                AUX_PRINT_A = POS_A;                                                 // asigna a AUX_PRINT_A el valor actualizado de POS_A
              }

              if (boleana == 1) {
                boleana = 0;
                aux = 0;
                AUX_PRINT_A = 0;
                POS_A = 0;
                AUX_POS_A = 0;
                lcd.clear();
                break;
              }
            }

            break;

            //Opción 2 Modificar tiempos

          case 2:
            lcd.clear();
            AUX_PRINT_A = 0;
            POS_A = 0;
            AUX_POS_A = 0;

            AUX_PRINT_B = 0;
            AUX_POS_B = 0;
            POS_B = 0;
            while (1) {
              if (digitalRead(BUTTON_B) == LOW) {
                delay(200);
              }
              sub_menu_tiempo();

              if (POS_A != AUX_PRINT_A) {                                            // si el valor de POS_A es distinto de AUX_PRINT_A
                Serial.println("x: " + String(POS_A) + " y: " + String(AUX_POS_B));  // imprime valor de POS_A
                AUX_PRINT_A = POS_A;                                                 // asigna a AUX_PRINT_A el valor actualizado de POS_A
              }

              if (AUX_POS_B != AUX_PRINT_B) {                                        // si el valor de POS_A es distinto de AUX_PRINT_A
                Serial.println("x: " + String(POS_A) + " y: " + String(AUX_POS_B));  // imprime valor de POS_A
                AUX_PRINT_B = AUX_POS_B;                                             // asigna a AUX_PRINT_A el valor actualizado de POS_A
              }

              if (boleana == 1) {
                boleana = 0;
                aux = 0;
                AUX_PRINT_A = 0;
                POS_A = 0;
                AUX_POS_A = 0;
                lcd.clear();
                break;
              }
            }

            break;
            //Opción 3 de cofiguración = Modo Nuevo

          case 3:
            lcd.clear();
            POS_A = 0;
            POS_B = 0;
            AUX_POS_A = 0;
            AUX_POS_B = 0;

            AUX_PRINT_A = 0;
            AUX_PRINT_B = 0;

            NUMERO_PASO = 0;
            AUX_STEPS_X = 0;
            AUX_STEPS_Y = 0;
            STEPSX = 0;
            STEPSY = 0;


            while (1) {
              if (digitalRead(BUTTON_B) == LOW) {
                delay(200);
              }

              nuevo_modo();
              print_vainterm();


              // opción para salir del menú
              if (boleana == 1) {
                boleana = 0;
                aux = 0;
                AUX_PRINT_A = 0;
                POS_A = 0;
                AUX_POS_A = 0;
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


        if (POS_A != AUX_PRINT_A) {                                            // si el valor de POS_A es distinto de AUX_PRINT_A
          Serial.println("x: " + String(POS_A) + " y: " + String(AUX_POS_B));  // imprime valor de POS_A
          AUX_PRINT_A = POS_A;                                                 // asigna a AUX_PRINT_A el valor actualizado de POS_A
        }

        if (AUX_POS_B != AUX_PRINT_B) {                                        // si el valor de POS_A es distinto de AUX_PRINT_A
          Serial.println("x: " + String(POS_A) + " y: " + String(AUX_POS_B));  // imprime valor de POS_A
          AUX_PRINT_B = AUX_POS_B;                                             // asigna a AUX_PRINT_A el valor actualizado de POS_A
        }



        if (boleana == 1) {
          boleana = 0;
          aux = 0;
          aux_submenu = 0;
          AUX_PRINT_A = 0;
          POS_A = 0;
          AUX_POS_A = 0;
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





  if (POS_A != AUX_PRINT_A) {                                            // si el valor de POS_A es distinto de AUX_PRINT_A
    Serial.println("x: " + String(POS_A) + " y: " + String(AUX_POS_B));  // imprime valor de POS_A
    AUX_PRINT_A = POS_A;                                                 // asigna a AUX_PRINT_A el valor actualizado de POS_A
  }
  if (AUX_POS_B != AUX_PRINT_B) {                                        // si el valor de POS_A es distinto de AUX_PRINT_A
    Serial.println("x: " + String(POS_A) + " y: " + String(AUX_POS_B));  // imprime valor de POS_A
    AUX_PRINT_B = AUX_POS_B;                                             // asigna a AUX_PRINT_A el valor actualizado de POS_A
  }
}



void menu_inicial() {
  lcd.setCursor(1, 0);
  lcd.print("INICIAR");
  lcd.setCursor(1, 1);
  lcd.print("CONFIGURACION");

  if (POS_A >= 0 && POS_A < AUX_POS_A + 5) {
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(0, 0);
    lcd.print("-");
  }

  if (POS_A >= AUX_POS_A + 5 && POS_A < AUX_POS_A + 10) {
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("-");
  }

  if (POS_A == AUX_POS_A + 10) {
    AUX_POS_A = POS_A;
  }
  if (POS_A == AUX_POS_A - 1) {
    AUX_POS_A = POS_A - 9;
  }
}

void sub_menu() {
  lcd.setCursor(1, 0);
  lcd.print("CAMBIAR MODO");
  lcd.setCursor(1, 1);
  lcd.print("MODIFICAR TIEMPOS");
  lcd.setCursor(1, 2);
  lcd.print("MODO NUEVO");



  if (POS_A >= 0 && POS_A < AUX_POS_A + 5) {
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(0, 2);
    lcd.print(" ");

    lcd.setCursor(0, 0);
    lcd.print("-");
  }

  if (POS_A >= AUX_POS_A + 5 && POS_A < AUX_POS_A + 10) {
    lcd.setCursor(0, 0);
    lcd.print(" ");

    lcd.setCursor(0, 2);
    lcd.print(" ");

    lcd.setCursor(0, 1);
    lcd.print("-");
  }

  if (POS_A >= AUX_POS_A + 10 && POS_A < AUX_POS_A + 15) {
    lcd.setCursor(0, 1);
    lcd.print(" ");

    lcd.setCursor(0, 0);
    lcd.print(" ");

    lcd.setCursor(0, 2);
    lcd.print("-");
  }
  if (POS_A == AUX_POS_A + 15) {
    AUX_POS_A = POS_A;
  }
  if (POS_A == AUX_POS_A - 1) {
    AUX_POS_A = POS_A - 14;
  }

  Serial.println("x: " + String(POS_A) + " y: " + String(AUX_POS_B));
}

void sub_menu_tiempo() {


  lcd.setCursor(1, 0);
  lcd.print("Paso 1: ");
  lcd.print(tiempo1);
  lcd.print(" min    ");


  lcd.setCursor(1, 1);
  lcd.print("Paso 2: ");
  lcd.print(tiempo2);
  lcd.print(" min    ");

  lcd.setCursor(1, 2);
  lcd.print("Paso 3: ");
  lcd.print(tiempo3);
  lcd.print(" min    ");

  lcd.setCursor(1, 3);
  lcd.print("Paso 4: ");
  lcd.print(tiempo4);
  lcd.print(" min    ");

  AUX_POS_B = tiempo1;
  while (POS_A >= 0 && POS_A < AUX_POS_A + 5) {

    lcd.setCursor(1, 0);
    lcd.print("Paso 1: ");
    lcd.print(tiempo1);
    lcd.print(" min    ");

    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(0, 2);
    lcd.print(" ");
    lcd.setCursor(0, 3);
    lcd.print(" ");

    lcd.setCursor(0, 0);
    lcd.print("-");



    tiempo1 = AUX_POS_B;
    if (boleana == 1) {
      break;
    }
  }

  AUX_POS_B = tiempo2;
  while (POS_A >= AUX_POS_A + 5 && POS_A < AUX_POS_A + 10) {


    lcd.setCursor(1, 1);
    lcd.print("Paso 2: ");
    lcd.print(tiempo2);
    lcd.print(" min    ");

    lcd.setCursor(0, 0);
    lcd.print(" ");

    lcd.setCursor(0, 2);
    lcd.print(" ");

    lcd.setCursor(0, 1);
    lcd.print("-");

    lcd.setCursor(0, 3);
    lcd.print(" ");


    tiempo2 = AUX_POS_B;
    if (boleana == 1) {
      break;
    }
  }
  AUX_POS_B = tiempo3;
  while (POS_A >= AUX_POS_A + 10 && POS_A < AUX_POS_A + 15) {

    lcd.setCursor(1, 2);
    lcd.print("Paso 3: ");
    lcd.print(tiempo3);
    lcd.print(" min    ");

    lcd.setCursor(0, 1);
    lcd.print(" ");

    lcd.setCursor(0, 0);
    lcd.print(" ");

    lcd.setCursor(0, 2);
    lcd.print("-");

    lcd.setCursor(0, 3);
    lcd.print(" ");

    tiempo3 = AUX_POS_B;
    if (boleana == 1) {
      break;
    }
  }


  AUX_POS_B = tiempo4;
  while (POS_A >= AUX_POS_A + 15 && POS_A < AUX_POS_A + 20) {

    lcd.setCursor(1, 3);
    lcd.print("Paso 4: ");
    lcd.print(tiempo4);
    lcd.print(" min    ");

    lcd.setCursor(0, 1);
    lcd.print(" ");

    lcd.setCursor(0, 0);
    lcd.print(" ");

    lcd.setCursor(0, 2);
    lcd.print(" ");

    lcd.setCursor(0, 3);
    lcd.print("-");

    tiempo4 = AUX_POS_B;



    if (boleana == 1) {
      break;
    }
  }

  if (POS_A == AUX_POS_A + 20) {
    AUX_POS_A = POS_A;
  }
  if (POS_A == AUX_POS_A - 1) {
    AUX_POS_A = POS_A - 19;
  }
}


void num_capas() {

  lcd.setCursor(2, 1);
  lcd.print("NUMERO DE CAPAS:");
  lcd.setCursor(0, 2);
  lcd.print("         ");
  lcd.print(POS_A);
  lcd.print("    ");
  NUM_CAPAS = POS_A;
}


void nuevo_modo() {

  // Movimiento de motor x


  motor_movement();

  if (digitalRead(BUTTON_A) == LOW) {


    X[NUMERO_PASO] = POS_A;
    Y[NUMERO_PASO] = AUX_POS_B;
    AUX_POS_A2 = POS_A;
    AUX_POS_B2 = AUX_POS_B;

    AUX_STEPSX = STEPSX;
    AUX_STEPSY = STEPSY;

    delay(200);
    time_of_steps();


    POS_A = AUX_POS_A2;
    AUX_POS_B = AUX_POS_B2;

    STEPSX = AUX_STEPSX;
    STEPSY = AUX_STEPSY;
    AUX_STEPS_X = AUX_STEPSX;
    AUX_STEPS_Y = AUX_STEPSY;


    NUMERO_PASO++;
  }
  lcd.setCursor(4, 0);
  lcd.print("*NUEVO MODO*");
  lcd.setCursor(1, 1);
  lcd.print("PASO: ");
  lcd.print(NUMERO_PASO);
  lcd.setCursor(1, 2);
  lcd.print("EJEX (0-100): ");
  lcd.print(POS_A);
  lcd.print("  ");
  lcd.setCursor(1, 3);
  lcd.print("EJEY (0-100): ");
  lcd.print(AUX_POS_B);
  lcd.print("  ");
}
void hora() {
  DateTime fecha = rtc.now();

  lcd.setCursor(0, 3);  // ubica cursor en columna 0 y linea 1
  lcd.print("HORA: ");
  lcd.print(fecha.hour());  // funcion millis() / 1000 para segundos transcurridos
  lcd.print(":");
  lcd.print(fecha.minute());  // funcion millis() / 1000 para segundos transcurridos
  lcd.print(":");
  lcd.print(fecha.second());  // funcion millis() / 1000 para segundos transcurridos
}













void encoder1() {
  static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de
                                                // tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
                                                      // pulsos menores a 5 mseg.
    if (digitalRead(DT_A) == HIGH)                    // si B es HIGH, sentido horario
    {
      POS_A++;  // incrementa POS_A en 1
      STEPSX = STEPSX + 150;
    } else {    // si B es LOW, senti anti horario
      POS_A--;  // decrementa POS_A en 1
      STEPSX = STEPSX - 150;
    }


    /*
    if (POS_A >100){
      POS_A = 0;
      AUX_POS_A = 0;
    }
    if(POS_A <0){
      POS_A = 100;
      AUX_POS_A = 80;
    }
*/
    POS_A = min(100, max(0, POS_A));      // establece limite inferior de 0 y
    STEPSX = min(15000, max(0, STEPSX));  // establece limite inferior de 0 y

    // superior de 100 para POS_A
    ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo
  }                                           // de la interrupcion en variable static
}





void encoder2() {
  static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de
                                                // tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
                                                      // pulsos menores a 5 mseg.
    if (digitalRead(DT_B) == HIGH)                    // si B es HIGH, sentido horario
    {
      AUX_POS_B++;  // incrementa POS_A en 1
      STEPSY = STEPSY + 100;
    } else {        // si B es LOW, senti anti horario
      AUX_POS_B--;  // decrementa POS_A en 1
      STEPSY = STEPSY - 100;
    }



    /*if (AUX_POS_B >100){
      AUX_POS_B = 0;
      POS_B = 0;
    }
    if(AUX_POS_B <0){
      AUX_POS_B = 100;
      POS_B = 80;
    }
*/
    AUX_POS_B = min(100, max(0, AUX_POS_B));  // establece limite inferior de 0 y
    STEPSY = min(10000, max(0, STEPSY));      // establece limite inferior de 0 y

    // superior de 100 para POS_A
    ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo


  }  // de la interrupcion en variable static
}

void push_a() {
  //Variable la cual hará que en determinado menú, nos regresemos al AUX_PRINT_A
  if (digitalRead(BUTTON_B) == LOW) {
    Serial.println("FUNCIONA1");
    boleana = 1;
    // Espera hasta que pase el intervalo
  }
}

void push_b() {
  if (POS_A >= 0 && POS_A < AUX_POS_A + 5) {
    Serial.println("OPCION1");
    aux = 1;
  }

  if (POS_A >= AUX_POS_A + 5 && POS_A < AUX_POS_A + 10) {
    Serial.println("OPCION2");
    aux = 2;
  }


  if (POS_A >= AUX_POS_A + 10 && POS_A < AUX_POS_A + 15 && aux_submenu == 1) {
    Serial.println("OPCION2");
    aux = 3;
  }
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


void print_vainterm() {

  if (POS_A != AUX_PRINT_A) {                                            // si el valor de POS_A es distinto de AUX_PRINT_A
    Serial.println("x: " + String(POS_A) + " y: " + String(AUX_POS_B));  // imprime valor de POS_A
    AUX_PRINT_A = POS_A;                                                 // asigna a AUX_PRINT_A el valor actualizado de POS_A
  }


  if (AUX_POS_B != AUX_PRINT_B) {                                        // si el valor de POS_A es distinto de AUX_PRINT_A
    Serial.println("x: " + String(POS_A) + " y: " + String(AUX_POS_B));  // imprime valor de POS_A
    AUX_PRINT_B = AUX_POS_B;                                             // asigna a AUX_PRINT_A el valor actualizado de POS_A
  }
}


void time_of_steps() {
  lcd.clear();
  POS_A = 0;
  POS_B = 0;
  AUX_POS_A = 0;
  AUX_POS_B = 0;

  while (true) {

    lcd.setCursor(1, 0);
    lcd.print("*TIEMPO DEL PASO*");
    lcd.setCursor(1, 1);
    lcd.print("PASO: ");
    lcd.print(NUMERO_PASO);
    lcd.setCursor(1, 2);
    lcd.print("MINUTOS: ");
    lcd.print(POS_A);
    lcd.print("  ");
    lcd.setCursor(1, 3);
    lcd.print("SEGUNDOS: ");
    lcd.print(AUX_POS_B);
    lcd.print("  ");

    if (digitalRead(BUTTON_A) == LOW) {
      lcd.clear();
      MINUTOS[NUMERO_PASO] = POS_A;
      SEGUNDOS[NUMERO_PASO] = AUX_POS_B;
      delay(200);
      break;
    }
  }
}

void motor_movement() {

  //Movimiento de motores y

  while (AUX_STEPS_Y < STEPSY) {


    digitalWrite(DIRY, HIGH);

    for (int i = 0; i < 100; i++) {
      digitalWrite(PULY, HIGH);
      delayMicroseconds(VEL);
      digitalWrite(PULY, LOW);
      delayMicroseconds(VEL);
    }

    AUX_STEPS_Y += 100;
  }

  while (AUX_STEPS_Y > STEPSY) {


    digitalWrite(DIRY, LOW);

    for (int i = 0; i < 100; i++) {
      digitalWrite(PULY, HIGH);
      delayMicroseconds(VEL);
      digitalWrite(PULY, LOW);
      delayMicroseconds(VEL);
    }
    AUX_STEPS_Y -= 100;
  }
  while (AUX_STEPS_X < STEPSX) {


    digitalWrite(DIRX, HIGH);


    for (int i = 0; i < 150; i++) {
      digitalWrite(PULX, HIGH);
      delayMicroseconds(VEL);
      digitalWrite(PULX, LOW);
      delayMicroseconds(VEL);
    }
    AUX_STEPS_X += 150;
  }

  while (AUX_STEPS_X > STEPSX) {




    digitalWrite(DIRX, LOW);

    for (int i = 0; i < 150; i++) {
      digitalWrite(PULX, HIGH);
      delayMicroseconds(VEL);
      digitalWrite(PULX, LOW);
      delayMicroseconds(VEL);
    }

    AUX_STEPS_X -= 150;
  }
}
