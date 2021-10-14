/*****************************************
 * Auto Startcode                        *
 * met toestansdiagrammen                *
 * Emmauscollege                         *
 * v20201117GEE                          *
 *****************************************/
 
// libraries die je gebruikt
#include <LiquidCrystal.h>

/*****************************************
 * variabelen die je gebruikt            *
 *****************************************/

// initialize het display
// de helderheid van het display regel je met de potmeter op de auto, 
// daarvoor is geen code nodig
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// gebruikte pinnen
const int pinAfstandTrigM = A4; // afstandssensor midden
const int pinAfstandEchoM = A5; // afstandssensor midden
const int pinAfstandTrigR = A2; // afstandssensor rechts
const int pinAfstandEchoR = A3; // afstandssensor rechts
const int pinAfstandTrigL = A0; // afstandssensor links
const int pinAfstandEchoL = A1; // afstandssensor links
const int pinMotorSnelheidR = 11; // motor rechts
const int pinMotorSnelheidL = 10; // motor links

// variabelen om waarden van sensoren en actuatoren te onthouden
long afstandR = 0;
long afstandL = 0;
long afstandM = 0;
int snelheidR = 0; 
int  snelheidL = 0;
String regelBoven = "";
String regelOnder = "";

// variabelen voor de toestanden maken
// toestanden:
const int TEST = 1;
const int STOP = 2;
int toestand = TEST;
unsigned long toestandStartTijd = 0;
// subtoestanden van TEST:
const int RECHTSAF = 1;
const int LINKSAF = 2;
const int VOORUIT = 3;
const int WACHT = 4;
int testToestand = RECHTSAF;
unsigned long testToestandStartTijd = 0;

/*****************************************
 * functies die je gebruikt              *
 *****************************************/

// functie om afstandssensor uit te lezen
long readDistance(int triggerPin, int echoPin)
{
  long echoTime = 0;
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  // timeout after 30.000 microseconds (around 5 meters)
  echoTime = pulseIn(echoPin, HIGH, 30000); 
  if (echoTime == 0) {
    echoTime = 30000;
  }
  return echoTime;
}

void testLoop() {
  // lees afstandssensoren uit
  // dit is nodig voor alle test toestanden
  // omrekenen naar centimeters = milliseconden / 29 / 2
  afstandR = readDistance(pinAfstandTrigR, pinAfstandEchoR) / 29 / 2; 
  afstandL = readDistance(pinAfstandTrigL, pinAfstandEchoL) / 29 / 2; 
  afstandM = readDistance(pinAfstandTrigM, pinAfstandEchoM) / 29 / 2; 

  // bepaal toestand
  if (testToestand == RECHTSAF) {
    if (millis() - testToestandStartTijd > 1000) {
      testToestandStartTijd = millis();
      testToestand = LINKSAF;
    }
  }
  if (testToestand == LINKSAF) {
    if (millis() - testToestandStartTijd > 1000) {
      testToestandStartTijd = millis();
      testToestand = VOORUIT;
    }
  }
  if (testToestand == VOORUIT) {
    if (millis() - testToestandStartTijd > 1000) {
      testToestandStartTijd = millis();
      testToestand = WACHT;
    }
  }

  // bepaal snelheid afhankelijk van toestand
  // snelheid kan 0 t/m 255 zijn
  // bij lage getallen (ongeveer onder 100) heeft de motor 
  // te weinig kracht om te rijden
  if (testToestand == RECHTSAF) {
    snelheidR = 0;
    snelheidL = 128;
  }
  if (testToestand == LINKSAF) {
    snelheidR = 128;
    snelheidL = 0;
  }
  if (testToestand == VOORUIT) {
    snelheidR = 128;
    snelheidL = 128;
  }
  if (testToestand == WACHT) {
    snelheidR = 0;
    snelheidL = 0;
  }

  // zet waarden voor acturatoren, voor alle testToestanden
  // zet motorsnelheid
  analogWrite(pinMotorSnelheidR, snelheidR);
  analogWrite(pinMotorSnelheidL, snelheidL);
  // zet tekst op display
  regelBoven = String(afstandL) + "   " + 
               String(afstandM) + "   " + 
               String(afstandR) + "   ";
  regelOnder = String(snelheidL) + 
               " TEST" + String(testToestand) + " " + 
               String(snelheidR) + "      ";
  lcd.setCursor(0, 0); // zet cursor op het begin van de bovenste regel
  lcd.print(regelBoven);
  lcd.setCursor(0, 1); // zet cursor op het begin van de onderste regel
  lcd.print(regelOnder);
  // zet debug info op de seriële monitor
  Serial.print(regelBoven);
  Serial.print("--");
  Serial.println(regelOnder);
}

/*****************************************
 * setup() en loop()                     *
 *****************************************/

void setup() {
  // pinnen voor afstandssensor worden
  // voor elke meting in readDistance()
  // in de goede mode gezet

  // zet pinmode voor motor aansturing via PWM
  pinMode(pinMotorSnelheidL, OUTPUT);
  pinMode(pinMotorSnelheidR, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // enable console
  Serial.begin(9600);

  // opstart bericht op console en seriële monitor
  lcd.setCursor(0, 0); // zet cursor op het begin van de bovenste regel
  lcd.print("Auto v20201021");
  lcd.setCursor(0, 1); // zet cursor op het begin van de onderste regel
  lcd.print("SETUP");// print demo bericht
  Serial.println("Auto start");
  delay(2000); // wachttijd om het display te lezen en de auto neer te zetten

  // zet toestanden en in beginstand
  toestand = TEST;
  toestandStartTijd = millis();
  testToestand = RECHTSAF;
  testToestandStartTijd = millis();
}

void loop()
{
  // toestand bepalen
  if (toestand == TEST) {
    if (millis() - toestandStartTijd > 10000) {
      toestandStartTijd = millis();
      toestand = STOP;
    }
  }
  if (toestand == STOP) {
    // de auto blijft in de toestand STOP
  }

  // de dingen doen die per toestand gedaan worden
  if (toestand == TEST) {
    testLoop();
  }
  if (toestand == STOP) {
    // zet motoren stil
    analogWrite(pinMotorSnelheidR, 0);
    analogWrite(pinMotorSnelheidL, 0);
    // zet tekst op display
    regelBoven = "                ";
    regelOnder = "      STOP      ";
    lcd.setCursor(0, 0); // zet cursor op het begin van de bovenste regel
    lcd.print(regelBoven);
    lcd.setCursor(0, 1); // zet cursor op het begin van de onderste regel
    lcd.print(regelOnder);
    Serial.println("STOP");
  }

  // vertraging om te zorgen dat de seriële monitor de berichten bijhoudt
  delay(100);
}
