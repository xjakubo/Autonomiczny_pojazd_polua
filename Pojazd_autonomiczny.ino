#include <Adafruit_NeoPixel.h>
#include<Servo.h>

#define LED_PIN    7
#define LED_COUNT 14

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


#define L_PWM 5           
#define L_DIR 4
#define R_PWM 6
#define R_DIR 9

Servo Oczy;

int Trig = 13;              // pin 13 Arduino połączony z pinem Trigger czujnika
int Echo = 12;              // pin 12 Arduino połączony z pinem Echo czujnika
int CM;                     // odległość w cm
long CZAS;                  // długość powrotnego impulsu w uS
int center = 90;            // Tutaj przetrzymujemy sobie pozycję serwa "na wprost"
int lewo =  180;            // Tutaj przetrzymujemy sobie pozycję serwa "na lewo"
int prawo =   0;            // Tutaj przetrzymujemy sobie pozycję serwa "na prawo"
int odlprawo = 0;           // Odległość na prawo
int odllewo =  0;           // Odległość na lewo
int odljazda = 50;           // Odległość do przodu
int vmax = 105;             // Prędkość pojazdu
int tobr = 300;             // Czas obrotu pojazdu
int pos = 90;               // Zmienna pozycji 0 serwa
void setup()
{
  strip.begin();                               // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();                                // Pokaż stan pixeli
  strip.setBrightness(250);                    // Ustaw jasność pixeli na 250 (max = 255)
  pinMode(8, OUTPUT);                         // Ustaw pin A1 jako wyjście
  digitalWrite(8, HIGH);                      // Ustaw stan pinu A1 na wysoki
  Oczy.attach(11);                             // Do którego pinu podpięliśmy serwo? Ano do 11
  Oczy.write(center);                          // Od razu ustawiamy serwo w pozycji "na wprost"
  Serial.begin(9600);                          // inicjalizaja monitora szeregowego
  pinMode(Trig, OUTPUT);                       // ustawienie pinu 2 w Arduino jako wyjście
  pinMode(Echo, INPUT);                        // ustawienie pinu 3 w Arduino jako wejście
   
  Serial.println("Test czujnika odleglosci");
    for(int i=0; i<strip.numPixels(); i++) {   //  Dla każdego pixela...
    strip.setPixelColor(i, 0,0,255);           //  Ustaw kolor pixeli
    strip.show();                              //  Pokaż stan pixeli
    delay(50);                                 //  Zatrzymaj się na 50ms
  }
      for(int i=0; i<strip.numPixels(); i++) {    //  Dla każdego pixela...
    strip.setPixelColor(i, 0,255,0);              //  Ustaw kolor pixeli
    strip.show();                                 //  Pokaż stan pixeli
    delay(40);                                    //  Zatrzymaj się na 50ms
  }
        for(int i=0; i<strip.numPixels(); i++) {  //  Dla każdego pixela...
    strip.setPixelColor(i, 255,0,0);              //  Ustaw kolor pixeli
    strip.show();                                 //  Pokaż stan pixeli
    delay(30);                                    //  Zatrzymaj się na 30ms
  }

  digitalWrite(L_DIR, 0);         //  Ustawienie kierunku 'przód' na lewych kołach
  digitalWrite(R_DIR, 0);         //  Ustawienie kierunku 'przód' na prawych kołach
  analogWrite(L_PWM, vmax);       //  Ustawienie predkosci na lewych kołach
  analogWrite(R_PWM, vmax);       //  Ustawienie predkosci na prawych kołach
}
  
void loop()
{

     for( pos = 180; pos >= 0 ;pos -=30){          //  
       Oczy.write(pos);                        //  
       odljazda = pomiar_odleglosci();        //  Pomiar odległości 
          if (odljazda <= 30){                      //  Jeżeli odległość jest mniejsza od 30cm...
          analogWrite(L_PWM,0);                   //  Koła stop
          analogWrite(R_PWM,0);                   //  Koła stop
          delay(200);
          rozgladaj();                            //  Rozglądaj się
          if(odllewo>odlprawo){                   //  Jeżeli odległość z lewej jest większa od prawej...
          obrotL();}                               //  Obróć się w lewo
          else {
          obrotR();
          }
                                        //  Obróć się w prawo
          }
       
    Serial.println(odllewo);                //  Pokaż odległość w lewo (Wysyłka danych przez port szeregowy)
    Serial.println(odlprawo);               //  Pokaż odległość w prawo (Wysyłka danych przez port szeregowy)
    delay(40);                              //  Zatrzymaj się na 40ms
  }
  
     for( pos = 0; pos <= 180 ;pos +=30){       //  
       Oczy.write(pos);                        //  
       odljazda = pomiar_odleglosci();        //  Pomiar odległości 
          if (odljazda <= 30){                      //  Jeżeli odległość jest mniejsza od 30cm...
          analogWrite(L_PWM,0);                   //  Koła stop
          analogWrite(R_PWM,0);                   //  Koła stop
          delay(200);
          rozgladaj();                            //  Rozglądaj się
          if(odllewo>odlprawo){                   //  Jeżeli odległość z lewej jest większa od prawej...
          obrotL();}                               //  Obróć się w lewo
          else {
          obrotR();
          }
          
          }  
        
    Serial.println(odllewo);                //  Pokaż odległość w lewo (Wysyłka danych przez port szeregowy)
    Serial.println(odlprawo);               //  Pokaż odległość w prawo (Wysyłka danych przez port szeregowy)
    delay(40);                              //  Zatrzymaj się na 40ms
  }
}

void jedzprosto(){                //  Funkcja jazdy prosto

  digitalWrite(L_DIR, 0);         //  Ustawienie kierunku 'przód' na lewych kołach
  digitalWrite(R_DIR, 0);         //  Ustawienie kierunku 'przód' na prawych kołach
  analogWrite(L_PWM, vmax);       //  Ustawienie predkosci na lewych kołach
  analogWrite(R_PWM, vmax);       //  Ustawienie predkosci na prawych kołach
}
int pomiar_odleglosci ()          //  Funckja pomiaru odległości
{
  digitalWrite(Trig, LOW);        //  Ustawienie stanu wysokiego na 2 uS - impuls inicjalizujacy - patrz dokumentacja
  delayMicroseconds(2);           
  digitalWrite(Trig, HIGH);       //  Ustawienie stanu wysokiego na 15 uS - impuls inicjalizujacy - patrz dokumentacja
  delayMicroseconds(15);
  digitalWrite(Trig, LOW);
  digitalWrite(Echo, HIGH); 
  CZAS = pulseIn(Echo, HIGH);
  CM = CZAS / 58;  
  return CM;                                  //  szerokość odbitego impulsu w uS podzielone przez 58 to odleglosc w cm - patrz dokumentacja
}

void rozgladaj(){                             //  Funkcja rozglądania się            
  delay(200);   
  digitalWrite(8, LOW);                      //  Ustaw stan pinu A1 na niski
  delay(200);                                 //  Zatrzymaj się na 200ms
  digitalWrite(8, HIGH);                     //  Ustaw stan pinu A1 na wysoki
  int R = random(0,255);                      //  Losuj natężenie koloru czerwonego (od 0 do 255)
  int G = random(0,255);                      //  Losuj natężenie koloru zielonego (od 0 do 255)
  int B = random(0,255);                      //  Losuj natężenie koloru niebieskiego (od 0 do 255)
  for(int i=0; i<strip.numPixels(); i++) {    //  Dla każdego pixela...
   
    strip.setPixelColor(i, R,G,B);            //  Set pixel's color (in RAM)
    strip.show();                             //  Update strip to match
    delay(50);      }                         //  Pause for a moment
  analogWrite(L_PWM, 0);                      //  Ustawienie predkosci na lewych kołach
  analogWrite(R_PWM, 0);                      //  Ustawienie predkosci na prawych kołach
  delay(200);                                 //  Zatrzymaj się na 200ms
  digitalWrite(L_DIR, 1);                     //  Ustawienie kierunku 'przód' na lewych kołach
  digitalWrite(R_DIR, 1);                     //  Ustawienie kierunku 'przód' na prawych kołach
  analogWrite(L_PWM, vmax);                   //  Ustawienie predkosci na lewych kołach
  analogWrite(R_PWM, vmax);                   //  Ustawienie predkosci na prawych kołach
  delay(200);                                 //  Zatrzymaj się na 200ms
  analogWrite(L_PWM, 0);                      //  Ustawienie predkosci na lewych kołach
  analogWrite(R_PWM, 0);                      //  Ustawienie predkosci na prawych kołach
  Oczy.write(lewo);                           //  Popatrz się na lewo
  delay(100);
  odllewo = pomiar_odleglosci();              //  Zmierz odległość w lewo
  delay(400);                                 //  Zatrzymaj się na 400ms
  Oczy.write(center);                         //  Popatrz się prosto
  delay(400);                                 //  Zatrzymaj się na 400ms
  Oczy.write(prawo);                          //  Popatrz się na prawo
  delay(100);
  odlprawo = pomiar_odleglosci();             //  Zmierz odległość w prawo
  delay(400);                                 //  Zatrzymaj się na 400ms
  Oczy.write(center);                         //  Popatrz się prosto...
  pos = 90;                                   //  W pozycji 90*
  pomiar_odleglosci();                        //  Zmierz odległość
  delay(200);                                 //  Zatrzymaj się na 200ms
}

void obrotL(){                //  Funkcja obrotu w lewo
  Serial.println("Jadę w lewo");
  digitalWrite(L_DIR, 0);     //  Ustawienie kierunku 'tyl' na lewych kołach
  digitalWrite(R_DIR, 1);     //  Ustawienie kierunku 'przód' na prawych kołach
  analogWrite(L_PWM, 250);    //  Ustawienie predkosci na lewych kołach
  analogWrite(R_PWM, 250);    //  Ustawienie predkosci na prawych kołach
  delay(tobr);
  analogWrite(L_PWM, 0);      //  Koła stop
  analogWrite(R_PWM, 0);      //  Koła stop
  odllewo = 0;                 
  odlprawo = 0;
  delay(100);
  digitalWrite(L_DIR, 0);         //  Ustawienie kierunku 'przód' na lewych kołach
  digitalWrite(R_DIR, 0);         //  Ustawienie kierunku 'przód' na prawych kołach
  analogWrite(L_PWM, vmax);       //  Ustawienie predkosci na lewych kołach
  analogWrite(R_PWM, vmax);       //  Ustawienie predkosci na prawych kołach
}

void obrotR(){                //  Funkcja obrotu w prawo
   Serial.println("Jadę w prawo");
  digitalWrite(L_DIR, 1);     //  Ustawienie kierunku 'przod' na lewych kołach
  digitalWrite(R_DIR, 0);     //  Ustawienie kierunku 'tyl' na prawych kołach
  analogWrite(L_PWM, 250);    //  Ustawienie predkosci na lewych kołach
  analogWrite(R_PWM, 250);    //  Ustawienie predkosci na prawych kołach
  delay(tobr);
  analogWrite(L_PWM, 0);      //  Koła stop
  analogWrite(R_PWM, 0);      //  Koła stop
  odllewo = 0;
  odlprawo = 0;
   delay(100);
  digitalWrite(L_DIR, 0);         //  Ustawienie kierunku 'przód' na lewych kołach
  digitalWrite(R_DIR, 0);         //  Ustawienie kierunku 'przód' na prawych kołach
  analogWrite(L_PWM, vmax);       //  Ustawienie predkosci na lewych kołach
  analogWrite(R_PWM, vmax);       //  Ustawienie predkosci na prawych kołach
  
}
