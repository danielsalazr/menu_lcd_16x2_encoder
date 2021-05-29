/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide/
*********/

// Import required libraries

#include <OneButton.h>
#include <Encoder.h>
#include <Arduino.h>
#include "WiFi.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include "FS.h"
#include <WebAuthentication.h>
#include <Wire.h> // This library is already built in to the Arduino IDE
#include <LiquidCrystal_I2C.h> //This library you can add via Include Library > Manage Library > 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include <ESPmDNS.h>

#include "time.h"

String horaaa, diaaa;
int ho, mi, se;

byte cont_error_clock;

int segundo;
int minuto;
int hora;
int diaa, dia;
int mes;
int ano;
int weekday;
long current;
struct tm timeinfo;
byte pedal=13;


unsigned long int tim=0, t_m;

int tope= 255, it=0 , t_cambio=0, velocidad_motor,
    enclave_vel, enclave_t, enclave_min;


bool ini = false, startup = false, fin= true ;


  const char* ntpServer = "de.pool.ntp.org";
const long  gmtOffset_sec = -6*3600;
const int   daylightOffset_sec = 3600;

String d [7]   =   {"lunes","Martes","Miercoles","jueves","Viernes","Sabado","Domingo"};
String messs[127]   =   {"Enero","Febrero","Marzo","Abril","Mayo","Junio","Julio""Agosto","Septiembre","Octubre","Noviembre","Diciembre"};

char Dias [7][12]   =   {"lunes","Martes","Miercoles","jueves","Viernes","Sabado","Domingo"};

bool pib_horas = false, pi_hora_inicio= false, conexion = false;
 


#include <ESPAsyncWiFiManager.h>

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#define TWDT_TIMEOUT_S          3
#define TASK_RESET_PERIOD_S     2
#define TRIGGER_PIN 16

bool trigger= false;

//------------------- Variables de lectura de puerto analogico -------------------//

const int potPin = 34;
int potValue = 0;

//------------------- Parametros de Pantalla LCD I2c -------------------//

//LiquidCrystal_I2C lcd(0x27, 20, 4);
  LiquidCrystal_I2C lcd(39, 20, 4);

byte cursor[8] = {
  0b01000,
  0b00100,
  0b00010,
  0b00001,
  0b00010,
  0b00100,
  0b01000,
  0b00000
};

//------------------- Parametros de encoder -------------------//
long oldPosition  = 0;
int old_enc;

int currentPosition = 0;
long newPosition;
int new_enc;

bool pib_mov= true;

int encoderDTpin = 27;
int encoderCLKpin = 26;

Encoder myEnc(encoderDTpin, encoderCLKpin);

int buttonPin = 14;

OneButton button0(buttonPin, true);



//------------------- Parametros de motor -------------------//

// Motor A
int motor1Pin1 = 25;
int motor1Pin2 = 33;
int enable1Pin = 32;

// Setting PWM properties
const int freq = 1000;  //60
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 255;

//------------------- Parametros de conexion wifi -------------------//

const char* ssid = "MANJARRES";
const char* password = "31997993";

//const char* ssid = "Nexxt_ADFCB0";
//const char* password = "NEXXTWifi2357";

//const char* ssid = "MotoG5";
//const char* password = "danielsalazr";

//const char* ssid = "CLARO_FE40";
//const char* password = "198225536";

#define HTTP_USER     "PortalSJMLE2515"
#define HTTP_PASS     "MLEWifi*2020"
#define HTTP_REALM    "realm"

//------------------- Datos de entrada desde el servidor  -------------------//

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
const char* PARAM_INPUT_3 = "submit";

//------------------- Parametros de maquina -------------------//

int pib_cont = 0;
int pib_vel = 100;
float pib_in = 3.0;

int cont = 0;
int velocidad = 100;
float inyeccion = 3.0;


//Comando para inicializacion de core #0

int dec;
int pos;
int ret = 0;
int cape=0;
bool pros = false;
byte est = 0;
byte dat = 0;


unsigned long int lt = 5000;

TaskHandle_t Task1;


int tome = 5000;

// current temperature & humidity, updated in loop()
float t = 29.0;
float h = 10.0;

//------------------- Variables de visualizacion en el servidor  -------------------//
String te = "100";
String hu = "3.0";

bool p = false;
bool q = false;
bool r = false;

//------------------- Variables de tiempos (control de millis() -------------------//

unsigned long ti = 0;
const long interval = 200;
unsigned long previousMillis = 0;

//------------------- Parametros de servidor -------------------//

AsyncWebServer server(80);
DNSServer dns;
AsyncWiFiManager wifiManager(&server,&dns);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

//------------------- Variables para leer Archivo Json -------------------//
String vs , iys, cs;


//------------------- Funcion de webSockets -------------------//

//IPAddress local_IP(192, 168, 0, 68);
//IPAddress gateway(192, 168, 0, 1);
//IPAddress subnet(255, 255, 255, 0);
//IPAddress primaryDNS(1, 1, 1, 1); //optional
//IPAddress secondaryDNS(8, 8, 4, 4); //optional


void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    //Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
    p = true;
    q = true; 
  } else if (type == WS_EVT_DISCONNECT) {
    //Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
    p = false;
    q = false;
  } else if (type == WS_EVT_ERROR) {
    //Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if (type == WS_EVT_PONG) {
    //Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");
  }
}

//   <span id="temperature">%TEMPERATURE%</span>

//------------------- Creacion de pagina Web en Html -------------------//



////------------------- Funcion de Uso interno para el servidor -------------------//

String processor(const String& var) {
  //Serial.println(var);
  if (var == "V") {
    return String(t);
  }
  else if (var == "I") {
    return String(h);
  }

  else if (var == "CONT") {
    return String(cont);
  }
  return String();
}


//------------------- Cargar configuracion almacenada en Spiffs -------------------//

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    //Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    //Serial.println("Config file size is too large");
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);

  // No usamos String aquí porque la biblioteca ArduinoJson requiere la entrada
  // buffer para ser mutable Si no usas ArduinoJson, también puedes
  // use configFile.readString en su lugar.

  char input[400];
  String line;
    
   while (configFile.available()) {

    line += configFile.readStringUntil('\n');
    //Serial.println(line);
    //Serial.write(configFile.read());
    //input = line;
  }

  //Serial.println(line);
  line.toCharArray(input, 400);

  
  const int capacity = JSON_OBJECT_SIZE(3) + 3 * JSON_OBJECT_SIZE(1);
  DynamicJsonDocument doc(capacity);
  DeserializationError err = deserializeJson(doc, input);

  if (err) {
    //Serial.print(F("deserializeJson() failed with code "));
    //Serial.println(err.c_str());
  }


  

  
  
  /*configFile.readBytes(buf.get(), size);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Error al analizar el archivo de configuración");
    return false;
  }
  */
  
  const char* contadorr  = doc["contador"];
  const char* velocidadd = doc["velocidad"];
  const char* inyeccionn = doc["inyeccion"];

  // La aplicación del mundo real almacenaría estos valores en algunas variables para
  // uso posterior.

  cs  = contadorr;
  vs = velocidadd;
  iys  = inyeccionn;

  //Serial.println("Variables numericas: ");
  //Serial.print("velocidad: ");
  //Serial.println(velocidadd);
  //Serial.print("inyeccion: ");
  //Serial.println(inyeccionn);
  //Serial.print("contador: ");
  //Serial.println(contadorr);
  
  return true;
}

//------------------- Almacenar configuracion en SPIFFS -------------------//

bool saveConfig() {
  
  //StaticJsonBuffer<200> jsonBuffer;
  char input_2[700];
  DynamicJsonDocument doc(1024);
  //JsonObject& json  = jsonBuffer.createObject();
  JsonObject json;
  json = doc.createNestedObject(); 
  json["velocidad"] = vs;
  json["inyeccion"] = iys;
  json["contador"]  = cs;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    //Serial.println("Error al abrir el archivo de configuración para escribir");
    return false;
  }
    
    char output[600];
  
   serializeJson(json, output);
   configFile.print(output);
   configFile.close();
  return true;
}

//------------------- Setup -------------------//

IPAddress _ip = IPAddress(192, 168, 1, 78);
                IPAddress _gw = IPAddress(192, 168, 1, 1);
                IPAddress _sn = IPAddress(255, 255, 255, 0);

char static_ip[16] = "192.168.1.56";
char static_gw[16] = "192.168.1.1";
char static_sn[16] = "255.255.255.0";

bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  //Serial.println("Should save config");
  shouldSaveConfig = true;
}

void savejson(){

      //Serial.println("saving config"); 
      char input_2[700];
      DynamicJsonDocument doc1(1024);
      
      JsonObject obj1;
  
      obj1 = doc1.createNestedObject();   
      obj1["ip"] = WiFi.localIP().toString();
      obj1["gateway"] = WiFi.gatewayIP().toString();
      obj1["subnet"] = WiFi.subnetMask().toString();
  
      File configFile = SPIFFS.open("/configWifi.json", "w");
      if (!configFile) {
        //Serial.println("failed to open config file for writing");
      }
  
      char output[600];
  
      serializeJson(doc1, output);

      configFile.print(output);
      configFile.close();


//      Serial.println(output);
//
//      Serial.println("local ip");
  //Serial.println(WiFi.localIP());
//  Serial.println(WiFi.gatewayIP());
//  Serial.println(WiFi.subnetMask());
//
//  Serial.println("Config_almacenada");

  
    
}
/*
void carga_ip()
{
           
    
}
*/
void configur_ip(){
    
    
    if ( digitalRead(pedal) == HIGH  ) {
        //Serial.println("Clave_1");
        lcd.setCursor (3, 1);
        lcd.print("Configuracion.");
        lcd.setCursor (9, 2);
        lcd.print("@");
        
        delay(5000);
        if (digitalRead(pedal) == HIGH)
        {     
            //Serial.println("Clave_2");
            lcd.setCursor (9, 2);
            lcd.print(">");
            delay(3000);
                   
            if(digitalRead(pedal) == LOW)
            {
                //Serial.println("Clave_3");
                lcd.setCursor (9, 2);
                lcd.print("<");
                delay(3000);
                if(digitalRead(pedal) == HIGH)
                {
                    lcd.setCursor (4, 2);
                    lcd.print("Accediendo.");
                    
                    trigger = true;
                    //Serial.println("Trigger activado");
                    //Serial.println("iniciando el gestor");        
                    
                    wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
                 
                    wifiManager.startConfigPortalModeless("DOSIFICANDO.COM", "12345678");
                    //Serial.println("connected...yeey :)");
                    //Serial.println("local ip");
                    //Serial.println(WiFi.localIP());
                }
            }
        }
    }
  
}




//------------------- Tarea del nucleo #0 -------------------//
void Task1code( void * pvParameters ) {

  //Nota: no incluir I2c aqui
  for (;;) {

        if (pros == true ){
            byte vel;

            if (ini == false)
            {   
                if (velocidad == 0){             
                    vel =0;
                }        
                else if (velocidad == 25){       
                    vel =125;
                }
                else if (velocidad == 50){    
                    vel =170;
                }
                else if (velocidad == 75){             
                    vel =215;
                }        
                else if (velocidad == 100){  
                    vel =255;
                }
            }    
        
            if ((digitalRead(16) == 1 || digitalRead(pedal) == 1) && cape == 0 )
            {              
                cape = 1;
                startup = true;
                lt = millis();                          
                //Serial.println("alto");
                //Serial.print("\t\t\t\t\t");
                //Serial.println(xTaskGetTickCount());

                delay(20); 
            }

            if (startup == true)
            {
                arranque(80, vel, inyeccion*1000);
            }

            if ( millis() - lt > inyeccion * 1000 && cape == 1 && fin == true)
            {
                //ledcWrite(pwmChannel, 0);
                cape = 0;
                //cont++;
                //Serial.print(cont);
                //Serial.println("Frenado....");
                //q=true;
                fin = false;
            }
        
    
    
        }
    
        potValue = analogRead(potPin);
        
        if (potValue < 4095 )
        {
        
          vs=  (String)velocidad;
          iys= (String)inyeccion;
          cs = (String)cont;
          //Serial.println(cs);
          
          saveConfig();
          //Serial.println(potValue);
          //Serial.println("Guardado");
          delay(300000);
        
        }
        
        
          vTaskDelay (10);
          //esp_task_wdt_reset();
          
          /*
          TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
          TIMERG0.wdt_feed=1;
          TIMERG0.wdt_wprotect=0;
          */
    
  }
  
}

//------------------- Loop  -------------------//

void printLocalTime()
{
  

  if(!getLocalTime(&timeinfo)){
    //Serial.println("Failed to obtain time");
    if (cont_error_clock <=2)
    {   
            lcd.setCursor (2, 0);
            lcd.print("                  ");
            lcd.setCursor (2, 0);
            lcd.print("Error de red");
            cont_error_clock++;
    }
    
    //Serial.println(cont_error_clock);
    return;
  }
  else 
  {
            segundo = timeinfo.tm_sec;
            minuto = timeinfo.tm_min;
            hora = timeinfo.tm_hour;
            dia= timeinfo.tm_mday;
            mes = timeinfo.tm_mon+1;
            ano = timeinfo.tm_year + 1900;
            //weekday = timeinfo.tm_wday +1;
            weekday = timeinfo.tm_wday;
          
            horaaa = d[weekday-1]+" "+ String(dia)+" de "+ messs[mes] + " "+ String(hora)+ ":" + String(minuto)+":" +String(segundo);

            //Serial.println(horaaa);
            bool pib_horas = false, pi_hora_inicio= false;
    
            if (pi_hora_inicio == false)
            {
                horas();
                pi_hora_inicio == true;
                
            }
    
            if ( segundo==0 && pib_horas == false)
            {
                horas();
                pib_horas = true;
            }
    
            else if ( segundo==55 && pib_horas == true)
            {
                pib_horas = false;
            }
  }
  //Serial.println(&timeinfo, "%A, %d %B %Y %H:%M:%S");
  //horaaa= (&timeinfo, "%A, %d %B %Y %H:%M:%S"); 
}



//------------------- Funcion de click -------------------//

void singleClick() {

    est ++;
    est %= 2;
  
    if ( newPosition == 1 && est == 1 )
    {
        dat = 1;
    
        lcd.setCursor (0 , newPosition);
        lcd.print("  ");
        lcd.setCursor (13, newPosition );
        lcd.write(byte(0));
        lcd.write(byte(0));
    }
  
    else if ( newPosition == 2 && est == 1 )
    {
        dat = 2;
        lcd.setCursor (0 , newPosition);
        lcd.print("  ");
        lcd.setCursor (13, newPosition );
        lcd.write(byte(0));
        lcd.write(byte(0));
  
    }
  
    else if ( newPosition == 3 && est == 1 )
    {
        dat = 3;
        lcd.setCursor (0 , newPosition);
        lcd.print("  ");
        lcd.setCursor (13, newPosition );
        lcd.write(byte(0));
        lcd.write(byte(0));
    }
  
    else if (est == 0) {
  
        //lcd.setCursor (13, 0 );
        //lcd.print("  ");
        lcd.setCursor (13, 1 );
        lcd.print("  ");
        lcd.setCursor (13, 2 );
        lcd.print("  ");
        lcd.setCursor (13, 3 );
        lcd.print("   ");
    
        lcd.setCursor (0, newPosition);
        lcd.write(byte(0));
        lcd.write(byte(0));
        dat = 0;
    }
}

void reini(){
  
   if (est == 1 && newPosition ==3)
   {
    //Serial.println("reiniciando");
    cont =0; 
    q=true;

    lcd.setCursor (13, newPosition);
    lcd.write(byte(0));
    lcd.write(byte(0));
   }
 }

 void horas(){

    if (conexion = true)
    {
        lcd.setCursor (2,0);
        lcd.print(hora/10);
        lcd.print(hora%10);
        lcd.print(":");
        lcd.print(minuto/10);
        lcd.print(minuto%10);
        //lcd.print(":");
        //lcd.print(segundo/10);
        //lcd.print(segundo%10);
        lcd.print("  ");  
        lcd.print(dia/10);
        lcd.print(dia%10);
        lcd.print("-");
        lcd.print(mes/10);
        lcd.print(mes%10);
        lcd.print("-");
        lcd.print(ano);
    }   
 }

void rotary_check() {

    old_enc = new_enc;
    new_enc = (myEnc.read()/4); // Esta es la trampa para   que nunca  falle el movimiento del cursor
    if (new_enc > old_enc && pib_mov == true && est == 0){
        newPosition ++;
        oldPosition=newPosition-1;
        q = true;
    }
    else if (new_enc < old_enc && pib_mov == true && est == 0){
  
        newPosition --;
        oldPosition=newPosition+1;
        q = true;
    }
    
    if(new_enc < 0)
    {
        myEnc.write(2002);
    }
  
    if (newPosition == 0 &&  oldPosition == 0 && est == 0)
    {   
        q = true;
        newPosition = 1;
        //Serial.println("");
        //Serial.print("Pos encoder:         ");
        //Serial.println(myEnc.read());
    }
  
    else if ( newPosition > 3 && oldPosition == 3 && est == 0)
    {   
        newPosition =1;
        //Serial.println("");
        //Serial.print("Pos encoder:         ");
        //Serial.println(myEnc.read());     
    }
  
    else if (newPosition == 0 && oldPosition == 1  && est == 0)
    {   
        newPosition = 3;
        //Serial.println("");
        //Serial.print("Pos encoder:         ");
        //Serial.println(myEnc.read());  
      }

    // IF the new position of the encoder is different then the old position
    if (new_enc != oldPosition) {
        /*
        Serial.println("");
        Serial.print("new encoder:         ");
        Serial.println(new_enc);
        Serial.print("old encoder:         ");
        Serial.println(old_enc);
        Serial.print("Pos encoder:         ");
        Serial.println(long(myEnc.read()));
        Serial.print("Encoder configurado:    ");
        Serial.println((myEnc.read() / 4));
        //Serial.println("Layer position " + String(currentPosition));
        Serial.println("new_posotion: " + String(newPosition));
        //Serial.println("current Select:  " + String(currentSelect));
        Serial.println("old position:  " + String(oldPosition));
        //Serial.println("\t\tTrue positin:  " + String(trposition));
        */
    }  
    if (est == 1 && dat == 1 ) {
  
      pib_mov = false;    
      pib_cont = cont;  
      pos = myEnc.read() / 4;

      if (pos != dec){
          if ( pos >  dec){
              velocidad += 25;  
          }
          if ( pos <  dec)
          {
              velocidad -= 25;
          }
          if (velocidad <= 25)
          {
               velocidad = 25;
          }
            
          velocidad = constrain(velocidad, 25, 100);
          pib_vel = velocidad;
    
          if (pib_vel == velocidad)
          {
              lcd.setCursor (15, 1);
              lcd.print("    ");
              lcd.setCursor (15, 1);
              lcd.print(velocidad);
              lcd.print("%");
          }

            te = String(velocidad);
        }
         dec = pos;
      }
  
      else if (est == 1 && dat == 2) {
  
          pib_mov = false;
      
          pos = myEnc.read() / 4;
      
          if (pos != dec)
          {
            lcd.setCursor (15, 2);
            lcd.print("     ");
      
            if ( pos >  dec)
            {
              inyeccion += 0.1;
            }
      
            if ( pos <  dec)
            {
              inyeccion -= 0.1;
            }
            
            if (inyeccion <0.5)
            {
                inyeccion = 0.5;
            }

            inyeccion = constrain(inyeccion, 0, 3);    
            lcd.setCursor (15, 2);
            lcd.print("     ");      
            lcd.setCursor (15, 2);
            lcd.print(inyeccion, 1);
            lcd.print("s");

            hu = String(inyeccion);
          }
          dec = pos;
    }
    else {
        pib_mov = true;       
    }
    if (q == true){
        if (est==0){
            lcd.setCursor (0, oldPosition);
            lcd.print("  "); 
            lcd.setCursor (0, newPosition);
            lcd.write(byte(0));
            lcd.write(byte(0));
        }

        if(cont_error_clock >=3)
        {
            lcd.setCursor (2, 0);
            lcd.print("                ");
            lcd.setCursor (2, 0);
            lcd.print("Wifi sin Internet");
            //Serial.println("Sin Internet"); 
        }
      
        if (est==1){
            lcd.setCursor (13, oldPosition);
            lcd.print("  ");        
            lcd.setCursor (13, newPosition);
            lcd.write(byte(0));
            lcd.write(byte(0));
        } 
        lcd.setCursor (15, 1);
        lcd.print("     ");
        lcd.setCursor (15, 1);
        lcd.print(velocidad);
        lcd.print("%");
        te = (String) velocidad;
      
        lcd.setCursor (15, 2);
        lcd.print("     ");
        lcd.setCursor (15, 2);
        lcd.print(inyeccion, 1);
        lcd.print("s");
      
        hu = (String) inyeccion;
      
        lcd.setCursor (10, 3);
        lcd.print(cont);
           
        lcd.setCursor (10, 3);
        lcd.print("   ");
        lcd.setCursor (10, 3);
        lcd.print(cont);
        //lcd.print("u");
      
        lcd.setCursor (16, 3);
        lcd.print("RST");
          q = false;
    }
 
}

void arranque(int minimo,int maximo,int t_max)
{
  
  if (ini == false)
  {
      //t_max = t_max *1000;
      t_m = millis();
      enclave_vel=maximo; enclave_t=t_max, enclave_min= minimo;

      //Serial.println(t_max);
      ini = true;
      velocidad_motor = minimo;
      t_cambio=0;
  }

  tim = millis();
  
  //--------------- arranque suave ---------------// 
  if( tim - t_m  <= 200 &&  ini == true)
  {
      
      //t_cambio++;
      //t_cambio%=2;

      digitalWrite(2,HIGH);

      if ( t_cambio == 0 && velocidad_motor <= enclave_vel)
      {
          velocidad_motor +=3;
      }
  }

  

  if( tim - t_m > 200 && tim - t_m <= 500 && ini == true)
  {
      
      //t_cambio++;
      //t_cambio%=1;
       
      if (velocidad_motor < enclave_vel)
      {
          velocidad_motor +=3;
      }
  }

  if( tim - t_m > 500 && tim - t_m <= enclave_t)
  {     
      velocidad_motor = enclave_vel;
  }


  //---------------  Fin de arranque suave ---------------// 

  if( tim - t_m > enclave_t &&  ini == true)
  {
      
      digitalWrite(2,LOW);
      if (velocidad_motor >= enclave_min - 20)
      {
          velocidad_motor = velocidad_motor- 3;
          //Serial.println("rampa de bajada");
        
      }

      if (velocidad_motor <= (enclave_min - 20) && digitalRead(16) == 0)
          {
              ini = false;
              startup = false;
              velocidad_motor = 0;
              cont++;
              q= true;
              fin = true;
              //Serial.println("Finalizado");
              cape = 0;
          }

      
    
  }
  
  ledcWrite(pwmChannel, velocidad_motor);
  //Serial.print(velocidad_motor); //Debug para comprobar velocidad
  //Serial.print("\t");
  //Serial.println(tim-t_m);
  
  
}

void clearMenu() {
    lcd.setCursor (0, 0);
    lcd.print("                    ");
    lcd.setCursor (0, 1);
    lcd.print("                    ");
    lcd.setCursor (0, 2);
    lcd.print("                    ");
    lcd.setCursor (0, 3);
    lcd.print("                    ");
}


void setup() {

    pinMode(TRIGGER_PIN, INPUT);
    pinMode(pedal, INPUT);

    Serial.begin(115200);

    lcd.begin (20, 4);
    Wire.begin(21, 22); //(SDA, SCL)
    lcd.init();  // initializing the LCD
    lcd.backlight(); // Enable or Turn On the backlight
    lcd.home();

    if (SPIFFS.begin()) {
      //Serial.println("mounted file system");
     
      configur_ip();
      if (SPIFFS.exists("/configWifi.json")) {   
          //Serial.println("reading config file");
          File configFile = SPIFFS.open("/configWifi.json", "r");
          
          if (configFile) {
            
                String line;
                line = "";
        
                while (configFile.available()) {
        
                    line += configFile.readStringUntil('\n');
                }
                char input_2[700];
                //Serial.println(line);
                
                line.toCharArray(input_2, 700);
                
                configFile.close();
                
                DynamicJsonDocument doc(1024);
                DeserializationError errr = deserializeJson(doc, input_2);
                if (errr) {
                      //Serial.print(F("deserializeJson() failed with code "));
                      //Serial.println(errr.c_str());
                }
                
                JsonObject repo1 = doc[0];
 
                //Serial.println("setting custom ip from config");
                
                strcpy(static_ip, repo1["ip"]);
                strcpy(static_gw, repo1["gateway"]);
                strcpy(static_sn, repo1["subnet"]);
                
                //Serial.println("Valores cargados");
                //Serial.println(static_ip);
                //Serial.println(static_gw);
                //Serial.println(static_sn);
            }

      else {
                //Serial.println("failed to load json config");
            }
      }    
      else{
          //Serial.println("no existe en el archivo");
      }

     }
    else {
        //Serial.println("failed to mount FS");
    }

    

    if (trigger == false)
    {

        if (!loadConfig()) {
            //Serial.println("Error al cargar la configuración");
        } 
        else {
            //Serial.println("Configuración cargada");
        }
        lcd.setCursor (0, 0);
        lcd.print("********************");
        lcd.setCursor (0, 1);
        lcd.print("*");
        lcd.setCursor (7, 1);
        lcd.print("SOMOS");
        lcd.setCursor (19, 1);
        lcd.print("*");
        lcd.setCursor (0, 2);
        lcd.print("*");
        lcd.setCursor (2, 2);
        lcd.print("DOSIFICANDO.COM");
        lcd.setCursor (19, 2);
        lcd.print("*");
        lcd.setCursor (0, 3);
        lcd.print("********************");
        delay(4000);
        clearMenu();

        lcd.setCursor (0, 0);
        lcd.print("********************");
        lcd.setCursor (0, 1);
        lcd.print("*");
        lcd.setCursor (7, 1);
        lcd.print("MAS QUE");
        lcd.setCursor (19, 1);
        lcd.print("*");
        lcd.setCursor (0, 2);
        lcd.print("*");
        lcd.setCursor (5, 2);
        lcd.print("MAQUINARIA");
        lcd.setCursor (19, 2);
        lcd.print("*");        
        lcd.setCursor (0, 3);
        lcd.print("********************");
        
        
        delay(2000);
        clearMenu();

        lcd.setCursor (0, 0);
        lcd.print("********************");
        lcd.setCursor (0, 1);
        lcd.print("*");
        lcd.setCursor (6, 1);
        lcd.print("PROCESOS");
        lcd.setCursor (19, 1);
        lcd.print("*");
        lcd.setCursor (0, 2);
        lcd.print("*");
        lcd.setCursor (4, 2);
        lcd.print("INTELIGENTES");
        lcd.setCursor (19, 2);
        lcd.print("*");        
        lcd.setCursor (0, 3);
        lcd.print("********************");
        delay(2000);
        clearMenu();

        velocidad = vs.toInt();
        inyeccion = iys.toFloat();
        cont      = cs.toInt();
    
        //Serial.print("vs:\t");
        //Serial.println(velocidad);
        //Serial.print("iys:\t");
        //Serial.println(inyeccion);
        //Serial.print("cs:\t");
        //Serial.println(cont );

        pinMode(motor1Pin1, OUTPUT);
        pinMode(motor1Pin2, OUTPUT);
        pinMode(enable1Pin, OUTPUT);
        pinMode(2, OUTPUT);
        
        digitalWrite(2,LOW);

        ledcSetup(pwmChannel, freq, resolution);
        ledcAttachPin(enable1Pin, pwmChannel);
      
        xTaskCreatePinnedToCore(
            Task1code,    // Function to implement the task
            "Task1",      // Name of the task
            10000,        // Stack size in words
            NULL,         // Task input parameter
            1,            // Priority of the task
            &Task1,       // Task handle.
          0);
      
        button0.attachClick(singleClick);
        button0.attachLongPressStop(reini);   
        button0.setDebounceTicks(80);
        button0.setPressTicks(4000);
      
        lcd.setCursor (1, 1);
        lcd.print("FARMACIA DROGUERIA");
        lcd.setCursor (5, 2);
        lcd.print("SAN JORGE");
        delay(3000);
        clearMenu();
        if (velocidad <= 25)
          {
               velocidad = 25;
          }

        if (inyeccion < 0.5)
          {
               inyeccion = 0.5;
          }
      
        //lcd.setCursor (2, 0);
        //lcd.print("Fecha:");
      
        lcd.setCursor (2, 1);
        lcd.print("Velocidad:");
        lcd.setCursor (2, 2);
        lcd.print("Inyeccion:");
        lcd.setCursor (2, 3);
        lcd.print("Conteo:");
      
        lcd.setCursor (0, oldPosition);
        lcd.print("  ");
      
        lcd.setCursor (15, 1);
        lcd.print(velocidad);
        lcd.print("%");
      
      
        lcd.setCursor (15, 2);
        lcd.print(inyeccion, 1);
        lcd.print("s");
      
        lcd.setCursor (10, 3);
        lcd.print(cont);
      
        lcd.setCursor (16, 3);
        lcd.print("RST");
      
      
        lcd.createChar(0, cursor);

      
        //wifiManager.setSaveConfigCallback(saveConfigCallback);
      
        _ip.fromString(static_ip);
        _gw.fromString(static_gw);
        _sn.fromString(static_sn);
        Serial.print("\nIp de ips\n");
        Serial.println(_ip);

        wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn, IPAddress(8,8,8,8),IPAddress(8,8,4,4));
      
        wifiManager.setTimeout(20);
        
      
        //wifiManager.autoConnect("DOSIFICANDO.COM");

        lcd.setCursor (3, 0);
        lcd.print("Buscando red...");

        if(!wifiManager.autoConnect("DOSIFICANDO.COM")) {

            lcd.setCursor (2, 0);
            lcd.print("sin conexion wifi");
            
            conexion= false;
            //Serial.println("Tiempo de espera excedido");
            //delay(3000);
            //Serial.println("Iniciando Loop");

            myEnc.write(2002);
      
      
            lcd.setCursor (0, oldPosition);
            lcd.print("  ");
          
            lcd.setCursor (0, newPosition);
            lcd.write(byte(0));
            lcd.write(byte(0));
          
            lcd.setCursor (15, 1);
            lcd.print(velocidad);
            lcd.print("%");
          
          
            lcd.setCursor (15, 2);
            lcd.print(inyeccion, 1);
            lcd.print("s");
          
            lcd.setCursor (10, 3);
            lcd.print(cont);
          
            
            lcd.setCursor (16, 3);
            lcd.print("RST");
            
            pros = true;
                
        }
      
        else 
        {
            ws.onEvent(onWsEvent);
            server.addHandler(&ws);
            server.addHandler(&events);
            
            lcd.setCursor (2, 0);
            lcd.print("conectando a wifi");
            delay(2000);
            conexion= true;
            //Serial.println("Se conecto, se conecto");

            //Serial.println("connected... sin utilizar el acceso :)");
            //Serial.println("local ip");
            //Serial.println(WiFi.localIP());
    
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
            printLocalTime();
    
            
            
           
            // Route for root / web page
            server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
                if (!request->authenticate(generateDigestHash(HTTP_USER, HTTP_PASS, HTTP_REALM).c_str())) {
                    return request->requestAuthentication(HTTP_REALM, true);
                }
                //request->send_P(200, "text/html", index_html, processor);
                //request->send_P(200, "text/html", index_html);
                //request->send(SPIFFS, "/Interfaz_san_jorge.html", String(), true);
                request->send(SPIFFS, "/Interfaz_san_jorge.html");
            });

            server.on("/MANUAL_DE_USUARIO_MLE2515", HTTP_GET, [](AsyncWebServerRequest * request) {
                if (!request->authenticate(generateDigestHash(HTTP_USER, HTTP_PASS, HTTP_REALM).c_str())) {
                    return request->requestAuthentication(HTTP_REALM, true);
                }
                request->send(SPIFFS, "/MANUAL_DE_USUARIO_MLE2515.pdf");
                //request->send(SPIFFS, "/MANUAL_DE_USUARIO_MLE2515.pdf", String(), false);
            });
          
            server.on("/sun", HTTP_GET, [](AsyncWebServerRequest * request) {
                request->send(SPIFFS, "/jorge2.png", "image/png");
            });
          
            server.on("/velocidad", HTTP_GET, [](AsyncWebServerRequest * request) {
                request->send_P(200, "text/plain", String(te).c_str());
          
            });
          
            server.on("/inyeccion", HTTP_GET, [](AsyncWebServerRequest * request) {
                request->send_P(200, "text/plain", String(hu).c_str());
            });
          
            server.on("/cont", HTTP_GET, [](AsyncWebServerRequest * request) {
                request->send_P(200, "text/plain", String(cont).c_str());      
            });
          
            server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
                String inputMessage;
                String inputParam;
          
                if (request->hasParam(PARAM_INPUT_1)) {
                    te = request->getParam(PARAM_INPUT_1)->value();
                    inputParam = PARAM_INPUT_1;
                    velocidad = te.toInt();
                    //Serial.print(velocidad);
                    q= true;
                }
                else if (request->hasParam(PARAM_INPUT_2)) {
                    hu = request->getParam(PARAM_INPUT_2)->value();
                    inputParam = PARAM_INPUT_2;
                    inyeccion = hu.toFloat();
                    //Serial.print("inyeccion:  ");
                    //Serial.print("\t\t");
                    //Serial.print(hu);
                    //Serial.print("\t\t");
                    //Serial.print(inyeccion);
                    q= true;
                }        
                // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
                else if (request->hasParam(PARAM_INPUT_3)) {
                    inputMessage = request->getParam(PARAM_INPUT_3)->value();
                    inputParam = PARAM_INPUT_3;
                    q= true;
                }
            
                else {
            
                  inputMessage = "No message sent";
                  inputParam = "none";
                  cont = 0;
                  q= true;
                }
             
                //request->send_P(200, "text/html", index_html);
                //request->send(200);
                request->send(SPIFFS, "/Interfaz_san_jorge.html", "text/html");
                
                //request->send(SPIFFS, "text/html", "/Interfaz_san_jorge.html");
                
            });
    
            
          
            server.begin();

            myEnc.write(2002);
      
      
            lcd.setCursor (0, oldPosition);
            lcd.print("  ");
          
            lcd.setCursor (0, newPosition);
            lcd.write(byte(0));
            lcd.write(byte(0));
          
            lcd.setCursor (15, 1);
            lcd.print(velocidad);
            lcd.print("%");
          
          
            lcd.setCursor (15, 2);
            lcd.print(inyeccion, 1);
            lcd.print("s");
          
            lcd.setCursor (10, 3);
            lcd.print(cont);
          
            
            lcd.setCursor (16, 3);
            lcd.print("RST");
            
            pros = true;

        /*if (MDNS.begin("MLE2515")) { //esp.local/
           Serial.println("MDNS responder started");*/
        }
                 
    }

    if (trigger == true){
        pros = false;     
    }
}

void loop() {

    if (trigger == true){          
        wifiManager.loop();
        if (digitalRead(TRIGGER_PIN) == HIGH){
            clearMenu();
            lcd.setCursor (4, 1);
            lcd.print("Almacenando");
            
            savejson();
            delay(2000);
            
            lcd.setCursor (4, 2);
            lcd.print("Finalizado");
            
            delay(5000);
            ESP.restart();
        }
    }
 
    /*if ( p == false)
    {
      if (q == false)
      {
          lcd.setCursor (18 , 0);
          lcd.print(" ");
          q=true;
      }
      
      
    }*/
  
    if (trigger == false){ 
        button0.tick();
        rotary_check();

        if ( WiFi.status() == 3 && cont_error_clock <= 3)
        {
            printLocalTime();
            ws.cleanupClients();
            
        }

        if(cont_error_clock ==3)
        {
            lcd.setCursor (2, 0);
            lcd.print("              ");
            lcd.setCursor (2, 0);
            lcd.print("Error de red");
            cont_error_clock++; 
        }

        if(WiFi.status() == 3 && cont_error_clock ==4)
        {
            cont_error_clock++;
            lcd.setCursor (2, 0);
            lcd.print("                ");
            lcd.setCursor (2, 0);
            lcd.print("Wifi sin Internet");
            ws.cleanupClients();
            
        }

        if ( WiFi.status() == 1)
        {
            conexion = false;
        }

        

        
        /*Serial.println();
      
        Serial.print(hora/10);
        Serial.print(hora%10);
        Serial.print(":");
        Serial.print(minuto/10);
        Serial.print(minuto%10);
        Serial.print(":");
        Serial.print(segundo/10);
        Serial.print(segundo%10);
        Serial.print(" ");
      
        Serial.print(dia/10);
        Serial.print(dia%10);
        Serial.print("-");
        Serial.print(mes/10);
        Serial.print(mes%10);
        Serial.print("-");
        Serial.println(ano);*/
  
  
  
        /*else if(p == true)
        {
          if (q == true)
          {
              lcd.setCursor (18 , 0);
              lcd.print("*");
              q=false;
          }
        }*/
      
        /*unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
          
        }
        */
        
    }

    if (Serial.available() >0)
    {
        int c =Serial.read();

        if( c == 'r')
        {   
            delay(1000);
            ESP.restart();
        }

        if (c == '7')
        {
            velocidad-=10;
            Serial.println(velocidad);
          
        }
        if (c == '9')
        {
            velocidad+=10;
            Serial.println(velocidad);
          
        }
        if (c == '4')
        {
            velocidad-=5;
            Serial.println(velocidad);
          
        }
        if (c == '6')
        {
            velocidad+=5;
            Serial.println(velocidad);
          
        }

        if (c == '1')
        {
            velocidad-=1;
            Serial.println(velocidad);
          
        }
        if (c == '3')
        {
            velocidad+=1;
            Serial.println(velocidad);
          
        }
        

        if (c == '2')
        {
            velocidad = 190;
            ledcWrite(pwmChannel, velocidad);
            Serial.println("motor al ruedo");
            Serial.println(velocidad);
          
        }
        if (c == '5')
        {         
            ledcWrite(pwmChannel, velocidad);
            Serial.println("Velocidad cambiada");
            Serial.println(velocidad);
          
        }

        if (c == '0')
        {
            
            velocidad = 0;
            ledcWrite(pwmChannel, velocidad);
            Serial.println("motor frenado");
            Serial.println(velocidad);
          
        }
        
    }
}
