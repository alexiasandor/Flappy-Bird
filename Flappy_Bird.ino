// Includem biblioteca pentru LCD-ul 20*4
#include<LiquidCrystal_I2C.h>
#include <Wire.h>
// seteaza adresa lcd-ului la '0x27' pentru lcd-ul cu 4 linii si 20 coloane
LiquidCrystal_I2C lcd(0x27, 20, 4);

//pinii pentru butoane 
#define BTN_PIN 2 //up
#define BTN_PIN1 3 // down

//pinii pentru senzorul de distanta
int TRIGGER_PIN = 11;
int  ECHO_PIN = 12;
#define DISTANCE_TRESHOLD 10
long duration, distance;

//pinii pentru senzorul de sunet
int SOUND_PIN = 7;
#define SOUND_THRESHOLD 50

//pinul pentru buzzer
int BUZZER_PIN = 4;
// biti corespunzatori pentru desenarea Flappy-ului
byte Flappy[] = {
  B00000,
  B01110,
  B01101,
  B00110,
  B11110,
  B01110,
  B01100,
  B00000
};

// bitii corespunzatori pentru desenarea stelutei
byte star[] = {
  B00000,
  B00100,
  B10101,
  B01110,
  B10101,
  B00100,
  B00000,
  B00000
};


//declaram variabile: loc liber, pozitie stea/moneda, pozitieFlappy,scor,butoane, variabile pentru batai din palme si distante

//variabilele pentru pozitia locului liber pentru trecerea Flappy
int holePosY;
int holePosX;

//variabilele pentru retinerea pozitiei stelutei
int moneyPosY;
int moneyPosX;

//variabilele pentru pozitia actuala a Flappy-ului
int flappyPosY ;
int flappyPosX ;

//retinerea scorului
int score = 0;

//variabila care se activeaza atunci cand jocul se termina
//(Flappy nu trece de obstacol)
bool isGameOver = false;

volatile bool buttonPushedUp = false;
volatile bool buttonPushedDown = false;

// varaibila pentru detectarea batailor din palme

int clapDetected = 0;

// variabile pentru detectarea a doua maini/ a unei maini
int consecutiveHands = 0; // numarul de maini consecutive detectate

//functia pentru intrerupere
void buttonPushUp(){
  buttonPushedUp = true;
}

void buttonPushDown(){
  buttonPushedDown = true;
}
void setup() {
  Serial.begin(9600);
  Wire.begin();
  // initializarea si activarea luminii lcd-ului
  lcd.init();
  lcd.backlight();

////////////////////////////////////////////////////////////////////////////////////DISTANTA/////////////////////////////////////////////////////////////////////////////////////
  // setam  pinul trigger ca iesire si pinul echo ca intrare 
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);

  ////////////////////////////////////////////////////////////////////////////////SUNET/////////////////////////////////////////////////////////////////////////////////////
  //initializarea senzorului de sunet
 

//////////////////////////////////////////////////////////////////////////////////////BUZZER/////////////////////////////////////////////////////////////////////////////////////////
  //initializarea pinului pentru buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  //crearea caracterului care reprezinta Flappy Bird-ul
  lcd.createChar(0, Flappy);
  lcd.createChar(1, star);

  //setam pozitia Flappy Bird-ului la (2,1)
  flappyPosY = 1;
  flappyPosX = 2;

  //setam pozitia initaiala a monedei la (3,17)
  moneyPosY = random(0, 4);
  moneyPosX = random(5, 20);

  //initializam pozitia locului liber
  holePosY = random(0,4);
  holePosX = 16;

}

int lastTime = 0;

void loop() {
  // put your main code here, to run repeatedly:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////// masuram valoarea sunetului
  int soundValue = digitalRead(SOUND_PIN);
  //daca valoarea sunetului este peste cea a pragului specificat si daca jocul nu s-a terminat
  Serial.println(soundValue);
 if (soundValue > SOUND_THRESHOLD && !isGameOver) {
    clapDetected++;
    if (clapDetected == 1) {
      flappyPosY--;
      clapDetected=0;
    } else if (clapDetected == 2) {
      flappyPosY++ ;
      clapDetected = 0;
    }
  }
  


// la fiecare 0.25 de milisecunde
  if (millis() % 250 == 0 && !isGameOver) {
    // verificam daca Flappy Bird-ul atinge obstacolul sau captureaza moneda

    // daca atinge obstacolul
    if (flappyPosY != holePosY && flappyPosX == holePosX) {
      isGameOver = true;
      return;
    } 
    
    //daca captureaza moneda
    if (flappyPosY == moneyPosY && flappyPosY == moneyPosX) {
      score += 20;
      lcd.setCursor(17, 0);
      lcd.print(score);
    }

    lcd.clear(); //curatam lcd-ul

// cand ajungem la marginea  ecranului fie cu obstacolul, fie cu moneda, trebuie sa generam noi pozitii
    if (holePosX == 0) {
      holePosY = random(0, 4);
      holePosX = random(15, 20);
      score++;

      moneyPosY = random(4);
      moneyPosX = random(11, 20);
    }
// afisam pozitiile cu obstacol si cu spatiu liber 
    for (int i = 0; i < 4; i++) {
      lcd.setCursor(holePosX, i);
      if (i == holePosY) {
        lcd.print(" ");  // spatiu pentru locul prin care trece Flappy Bird
      } else {
        lcd.print("X");
      }
    }
// la fiecare 0.25 de sec, obstacolul si banii se deplaseaza la stanga
    moneyPosX--;
    holePosX--;

//afisarea stelutei/monezii
    lcd.setCursor(moneyPosX, moneyPosY);
    lcd.write(byte(1));
    
//afisarea Flappy Bird-ului
    lcd.setCursor(flappyPosX, flappyPosY);
    lcd.write(byte(0));

// afisarea scorului in partea dreapta sus
    lcd.setCursor(17, 0);
    lcd.print(score);
  }

// daca buzzer-ul este activat 
//  if (millis() - buzzStartTime >= buzzDuration && buzzDuration > 0) {
//   digitalWrite(BUZZER_PIN, LOW);
//    buzzDuration = 0;

// daca jocul s-a terminat, afisam pe ecran un mesaj 
 if (isGameOver) {
  // activam buzzer-ul
  tone(BUZZER_PIN, 1000);
  delay(1000);
  noTone(BUZZER_PIN);
  delay(10);
    lcd.clear();
    lcd.setCursor(14, 0);
    lcd.print("Flappy");
    lcd.setCursor(16, 1);
    lcd.print("Bird");

    lcd.setCursor(0, 0);
    lcd.print("YOU LOST!");
    lcd.setCursor(0, 2);
    lcd.print("Your score: ");
    lcd.setCursor(0, 3);
    lcd.print(score);

    lastTime = millis();
    delay(10000);
  }
}
