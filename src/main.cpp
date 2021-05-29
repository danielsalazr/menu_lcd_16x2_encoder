/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>
#include <Arduino.h>
#include "lcdConf.h"

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(2, 3);

//   avoid using pins with LEDs attached
long oldPosition  = 64000;


long posicionMenu (byte max){
  long newPosition = myEnc.read()/4;
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
  }
  return newPosition % max;
}

long posicion(byte max){
  long newPosition = myEnc.read()/4;
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
  }
  Serial.println(newPosition);
}

void cursor(byte posicion, byte maximaFila){
  //posicion es la entrada de la posicion donde se imprimira el cursor
  //maximaFila es la cantidad maxima de filas fisicas en la pantalla ej LCd 16x2 = 2, LCD 20x4 =4

  for (byte i = 0 ; i < maximaFila; i++) {
    lcd.setCursor (0, i);
    if (posicion == i){
      lcd.write(0);
    } else {
      lcd.print(" ");
    }
  }
  
  
}

void impresionPantalla(byte casilla){
  
}

void setup() {
  myEnc.write(oldPosition);
  inicializarLcd();
  

  lcd.setCursor (2, 0);
  lcd.print("Configuracion.");

  

  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");

  
}



void loop() {
  //Serial.println(posicionMenu(4));
  long posicion  = posicionMenu(maxFila);

  cursor(posicion, maxFila);


  lcd.setCursor (15, 1);
  lcd.print(posicion);
  
}

