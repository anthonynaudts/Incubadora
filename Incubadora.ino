//#include <VarSpeedServo.h>
#include <pt.h>
#include <RTClib.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTTYPE DHT11
#define BUTTOM 13
#define BOMBILLO 8
#define ABANICO 6
#define GIRO1 3
#define GIRO2 2
#define PIN_LM35 A0
#define DHTPin 5
DHT dht(DHTPin, DHTTYPE);
LiquidCrystal_I2C lcd(0x3f,16,2);
RTC_DS1307 RTC;
void(* resetFunc) (void) = 0;

long tiempoReseteo = 86400000 * 30; //30 días     // 1 día = 86400000
int lecturaADC = 0;
int buttonState = 0;
double voltajeLM35 = 0.0;
double TemperaturaLM35 = 0.0;
double promedioTemperatura = 0;

static struct pt pt1, pt2, pt3, pt4, pt5;

static int temperaturaYHumedad(struct pt *pt)
{
  static unsigned long lastTimeBlink = 0;
  PT_BEGIN(pt);
  
  while(1) {
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 1000);
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    lcd.setCursor(0,0);
    lcd.print("T" + String(t) + " H" + String(h) + "%");
    lastTimeBlink = millis();
  }
  PT_END(pt);
}

static int pantallaLCD(struct pt *pt)
{
  static unsigned long lastTimeBlink = 0;
  static unsigned long data = 0;
  DateTime now = RTC.now();
  PT_BEGIN(pt);
  while(1) {
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 2500);


        lcd.setCursor(0,0);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,0);
        lcd.print("Proceso:");
        lcd.setCursor(0,1);
        buttonState = digitalRead(BUTTOM);
        if(buttonState == LOW){
            lcd.print("Nacimiento");
         }else{
            lcd.print("Incubacion");
         }
        

        lastTimeBlink = millis();
        PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 2500);
    
//      if(data == 0){
        lcd.setCursor(0,0);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("                ");
        
        lcd.setCursor(0,0);
        lcd.print("Fecha ");
        lcd.print(now.day(), DEC);
        lcd.print("/");
        lcd.print(now.month(), DEC);
        lcd.print("/");
        lcd.print(now.year(), DEC);
        lcd.setCursor(0,1);
        lcd.print("Hora ");
        lcd.print(now.twelveHour(), DEC);
        lcd.print(":");
        lcd.print(now.minute(), DEC);
        lcd.print(" ");
        if(now.isPM() == 0){
          lcd.print("AM");
        }else{
          lcd.print("PM");
        }
//        data = 1;
//      }

      lastTimeBlink = millis();
      PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 2500);
      
//      if(data == 1){
        lecturaADC = analogRead(A0); 
        voltajeLM35 = ((double)lecturaADC/1023)*5;
        TemperaturaLM35 = voltajeLM35/0.01;
        
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        lcd.setCursor(0,0);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,0);
        lcd.print("Temp " + String(t) + " " + String(TemperaturaLM35));
        lcd.setCursor(0,1);
        lcd.print("Humedad " + String(h) + "%");
//        data = 0;
//      }

      if(millis() > tiempoReseteo){
        resetFunc();
      }
  }
  PT_END(pt);
}


static int reloj(struct pt *pt)
{
  static unsigned long lastTimeBlink = 0;
  PT_BEGIN(pt);
  while(1) {
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 1000);
        DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC
        lcd.setCursor(0,0);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("                ");
        
        lcd.setCursor(0,0);
        lcd.print("Fecha ");
        lcd.print(now.day(), DEC);
        lcd.print("/");
        lcd.print(now.month(), DEC);
        lcd.print("/");
        lcd.print(now.year(), DEC);
        lcd.setCursor(0,1);
        lcd.print("Hora ");
        lcd.print(now.twelveHour(), DEC);
        lcd.print(":");
        lcd.print(now.minute(), DEC);
        lcd.print(" ");
        if(now.isPM() == 0){
          lcd.print("AM");
        }else{
          lcd.print("PM");
        }
  }
  PT_END(pt);
}


static int motor(struct pt *pt)
{
  static unsigned long lastTimeBlink = 0;
  static unsigned long permitir = 1;
  int reloj = 0;
  int minuto = 1;
  DateTime now = RTC.now();
  PT_BEGIN(pt);
  while(1) {
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 1000);
    reloj = now.minute();
    
    if(reloj == minuto && permitir == 1){
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 3000);
        digitalWrite(GIRO2, LOW);
        Serial.println("Giro del motor");

    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 1300);
        digitalWrite(GIRO2, HIGH);
        
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 15000);
        digitalWrite(GIRO1, LOW);
        Serial.println("Giro del motor");
        
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 1300);
        digitalWrite(GIRO1, HIGH);
        permitir = 0;
    }

    if(reloj == minuto + 2){
        permitir = 1;
     }

      if(permitir == 1){Serial.println("Giro permitido");}else{Serial.println("Giro restringido");}

  }
  PT_END(pt);
}

static int bombillo(struct pt *pt)
{
  static unsigned long lastTimeBlink = 0;
  static unsigned long cont = 0;
  
  PT_BEGIN(pt);
  while(1) {
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 1000);
    float h = dht.readHumidity();
        if(cont >= 10){
          promedioTemperatura = promedioTemperatura / cont;
          Serial.print("Temperatura: ");
          Serial.print(promedioTemperatura);
          Serial.print("      ");
          Serial.print("Humedad: ");
          Serial.print(h);
          Serial.println("");
           
          if(promedioTemperatura >= 40){  //38
            digitalWrite(BOMBILLO, HIGH);
            Serial.println("¡Bombillos apagados!");
          }
          if(promedioTemperatura <= 37.50){
            digitalWrite(BOMBILLO, LOW);  
            Serial.println("¡Bombillos encendidos!");
          }




           if(TemperaturaLM35 >= 37){
            buttonState = digitalRead(BUTTOM);
            if(buttonState == LOW){
                 Serial.println("Proceso de nacimiento");
                  if(h <= 63){
                      digitalWrite(ABANICO, HIGH);
                      Serial.println("¡Abanico humedad apagado!");
                   }
                   if(h >= 65){
                      digitalWrite(ABANICO, LOW);
                      Serial.println("¡Abanico humedad encendido!");
                   }
             }else{
                Serial.println("Proceso de incubacion");
                  if(h <= 55){
                      digitalWrite(ABANICO, HIGH);
                      Serial.println("¡Abanico humedad apagado!");
                   }
                   if(h >= 60){
                      digitalWrite(ABANICO, LOW);
                      Serial.println("¡Abanico humedad encendido!");
                   }
             }
          }else{
            digitalWrite(ABANICO, HIGH);
            Serial.println("¡Abanico humedad apagado!");
          }


          

          promedioTemperatura = 0;
          cont = 0;
        }else{
            cont = cont + 1;
            lecturaADC = analogRead(A0); 
            voltajeLM35 = ((double)lecturaADC/1023)*5;
            TemperaturaLM35 = voltajeLM35/0.01;
            promedioTemperatura = promedioTemperatura + TemperaturaLM35;
            Serial.print("Valor ");
            Serial.print(TemperaturaLM35);
            Serial.println("");
        }
        
    
  }
  PT_END(pt);
}


void setup() {
  RTC.begin();
  //RTC.adjust(DateTime(__DATE__, __TIME__)); //Hala la hora de la computadora
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  pinMode(BUTTOM, INPUT);
  pinMode(BOMBILLO, OUTPUT);
  digitalWrite(BOMBILLO, HIGH);
  pinMode(ABANICO, OUTPUT);
  digitalWrite(ABANICO, HIGH);
  pinMode(GIRO1 ,OUTPUT);
  pinMode(GIRO2 ,OUTPUT);
  digitalWrite(GIRO1, HIGH);
  digitalWrite(GIRO2, HIGH);
  PT_INIT(&pt1);
  PT_INIT(&pt2);
  PT_INIT(&pt3);
  PT_INIT(&pt4);
  PT_INIT(&pt5);
}

void loop() {
//  temperaturaYHumedad(&pt1);
  pantallaLCD(&pt2);
//  reloj(&pt3);
  motor(&pt4);
  bombillo(&pt5);
}
