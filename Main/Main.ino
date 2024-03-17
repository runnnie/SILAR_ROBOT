
#include <Wire.h>               // libreria de comunicacion por I2C
#include <LCD.h>                // libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>  // libreria para LCD por I2C
#include <RTClib.h>             // libraria para el modulo RTC, para medir el tiempo
#include "FS.h"                 // Librerias de para memoria SD
#include "SD.h"
#include "SPI.h"
#include <vector>               // Libreria para declarar vectores



LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);  // DIR, E, RW, RS, D4, D5, D6, D7

RTC_DS3231 rtc;


const int chipSelect = 5;  // Pin CS para la tarjeta SD
const char *nombreArchivo = "/pos.txt";

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


//Almacena el número de capas requeridas por el usuario
int NUM_CAPAS;

volatile int POS_B = 0;
volatile int AUX_POS_B = 0;
volatile int AUX_POS_B2 = 0;

int AUX_PRINT_B = 0;

int opcion = 0;
int menu = 0;


int aux = 0;   //almacena la opción que selecciona el usuario segun la interfaz que tenga
bool boleana = 0;
bool aux_submenu = 0;
volatile int AUX_STEPS_X = 0;
volatile int AUX_STEPS_Y = 0;


int pag_actual; //Almacena la página actual en la opción de sub_menu
int num_archivos; //Almacena el número de archivos en la memoria
String archivoSeleccionado = ""; //almacena el nombre del archivo seleccionado
struct Asociacion {//la estructura crea una relación entre cada nombre de archivo y un número
  int identificador;
  String nombreArchivo;
};

// Vector que contiene las asociaciones entre identificadores y nombres de archivo
std::vector<Asociacion> asociaciones;

/*
class Encoder {
  private: 
    volatile int POS_A = 0;      // variable POS_A con valor inicial de 50 y definida
    volatile int AUX_POS_A = 0;  //almacena el valor de pos_A
    volatile int AUX_POS_A2 = 0;
  public:

    void encoder1(){
      static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de // tiempo de interrupcion
      unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

      if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima  // pulsos menores a 5 mseg.
        if (digitalRead(DT_A) == HIGH)                    // si B es HIGH, sentido horario
        {
          POS_A++;  // incrementa POS_A en 1
          STEPSX = STEPSX + 150;
        } else {    // si B es LOW, senti anti horario
          POS_A--;  // decrementa POS_A en 1
          STEPSX = STEPSX - 150;
        }


    
      POS_A = max(0, POS_A);
      //POS_A = min(100, max(0, POS_A));      // establece limite inferior de 0 y
      STEPSX = min(15000, max(0, STEPSX));  // establece limite inferior de 0 y

      // superior de 100 para POS_A
      ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo
      }          
                                    // de la interrupcion en variable static

    }
    void encoder2(){
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




        AUX_POS_B = min(100, max(0, AUX_POS_B));  // establece limite inferior de 0 y
        STEPSY = min(10000, max(0, STEPSY));      // establece limite inferior de 0 y

        //savesteps();
        // superior de 100 para POS_A
        ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo


      }  // de la interrupcion en variable static
    }
    void push_a(){
      //Variable la cual hará que en determinado menú, nos regresemos al AUX_PRINT_A
      if (digitalRead(BUTTON_B) == LOW) {
        Serial.println("FUNCIONA1");
        boleana = 1;
        // Espera hasta que pase el intervalo
      }
    }
    void push_b(){
      if (POS_A >= 0 && POS_A < AUX_POS_A + 5) {
        Serial.println("OPCION1");
        aux = 1;
      }
    }

};

*/

class Interface{
  private:

  public:

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
    

    void hora() {
      DateTime fecha = rtc.now();

      lcd.setCursor(0, 3);  // ubica cursor en columna 0 y linea 1
      lcd.print("HORA: ");
      lcd.print(dosDigitos(fecha.hour()));  // funcion millis() / 1000 para segundos transcurridos
      lcd.print(":");
      lcd.print(dosDigitos(fecha.minute()));  // funcion millis() / 1000 para segundos transcurridos
      lcd.print(":");
      lcd.print(dosDigitos(fecha.second()));  // funcion millis() / 1000 para segundos transcurridos
    }


    String dosDigitos(int numero) {
      if (numero < 10) {
        return "0" + String(numero);
      } else {
        return String(numero);
      }
    }

};



void setup(){
  lcd.setBacklightPin(3, POSITIVE);  // puerto P3 de PCF8574 como positivo
  lcd.setBacklight(HIGH);            // habilita iluminacion posterior de LCD
  lcd.begin(20, 4);                  // 16 columnas por 2 lineas para LCD 1602A
  lcd.clear();    



  pinMode(CLK_A, INPUT_PULLUP);
  pinMode(DT_A, INPUT_PULLUP);
  pinMode(CLK_B, INPUT_PULLUP);
  pinMode(DT_B, INPUT_PULLUP);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_B), push_a, FALLING);  // interrupcion sobre pin A con
  attachInterrupt(digitalPinToInterrupt(BUTTON_A), push_b, FALLING);  // interrupcion sobre pin A con
  attachInterrupt(digitalPinToInterrupt(CLK_A), encoder1, FALLING);   // interrupcion sobre pin A con
  attachInterrupt(digitalPinToInterrupt(CLK_B), encoder2, FALLING);   // interrupcion sobre pin A con

  if (!rtc.begin()) {
    Serial.println("¡Modulo RTC no encontrado!");
    while (1)
      ;
  }


  //Se instancia un obteto de la clase Interface

}

Interface Inter;


void loop(){

  Inter.menu_inicial();

}


void encoder1() {
  static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de // tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima  // pulsos menores a 5 mseg.
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
    } */
  POS_A = max(0, POS_A);
  //POS_A = min(100, max(0, POS_A));      // establece limite inferior de 0 y
  STEPSX = min(15000, max(0, STEPSX));  // establece limite inferior de 0 y

  // superior de 100 para POS_A
  ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo
}          
                                // de la interrupcion en variable static

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
    }*/
    AUX_POS_B = min(100, max(0, AUX_POS_B));  // establece limite inferior de 0 y
    STEPSY = min(10000, max(0, STEPSY));      // establece limite inferior de 0 y

    //savesteps();
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