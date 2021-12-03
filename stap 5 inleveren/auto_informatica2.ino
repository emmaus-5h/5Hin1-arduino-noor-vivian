#include <LiquidCrystal.h>
/* meer info over de library voor het LCD0-scherm is hier:
    https://www.arduino.cc/en/Reference/LiquidCrystal
*/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

/*****************************************
   variabelen die je gebruikt
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
int snelheidR = 2;
int  snelheidL = 2;
String regelBoven = "";
String regelOnder = "";
int buzzerPin = 12;

// variabelen voor de toestanden maken
// toestanden:
const int RECHTSAF = 1;
const int LINKSAF = 2;
const int VOORUIT = 3;
const int WACHT = 4;
int toestand = RECHTSAF;
unsigned long toestandStartTijd = 0;

/*****************************************
   functies die je gebruikt
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

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

/*****************************************
   setup() en loop()
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
  lcd.clear(); // wis LCD
  lcd.setCursor(0, 0); // zet cursor op het begin van de bovenste regel
  lcd.print("Auto v20201021");
  lcd.setCursor(0, 1); // zet cursor op het begin van de onderste regel
  lcd.print("SETUP");// print demo bericht
  Serial.println("Auto start");
  delay(2000); // wachttijd om het display te lezen en de auto neer te zetten

  // zet toestanden en in beginstand
  toestand = VOORUIT;
  toestandStartTijd = millis();

  int thisNote = 0;
};

void loop() {
  // lees afstandssensoren uit
  // dit is nodig voor alle test toestanden
  // omrekenen naar centimeters = milliseconden / 29 / 2

  afstandR = readDistance(pinAfstandTrigR, pinAfstandEchoR) / 29 / 2;
  afstandL = readDistance(pinAfstandTrigL, pinAfstandEchoL) / 29 / 2;
  afstandM = readDistance(pinAfstandTrigM, pinAfstandEchoM) / 29 / 2;

  // bepaal toestand

  if (toestand == VOORUIT) {
    if (millis() - toestandStartTijd > 1000 && afstandM < 20 && afstandR < 20 && afstandL > 20) {
      toestandStartTijd = millis();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LINKS");
      Serial.println("LINKS");
      toestand = LINKSAF;
    }
  }

  if (toestand == VOORUIT) {
    if (millis() - toestandStartTijd > 1000 && afstandM < 20 && afstandL < 20 && afstandR > 20) {
      toestandStartTijd = millis();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("RECHTS");
      Serial.println("RECHTS");
      toestand = RECHTSAF;
    }
  }


  if (toestand == LINKSAF) {
    if (millis() - toestandStartTijd > 1000 && afstandM > 30) {
      toestandStartTijd = millis();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("VOORUIT");
      Serial.println("VOORUIT");
      toestand = VOORUIT;
    }
  }

  if (toestand == RECHTSAF) {
    if (millis() - toestandStartTijd > 1000 && afstandM > 30) {
      toestandStartTijd = millis();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("VOORUIT");
      Serial.println("VOORUIT");
      toestand = VOORUIT;
    }
  }



  if (toestand == VOORUIT || toestand == LINKSAF || toestand == RECHTSAF) {
    if (millis() - toestandStartTijd > 1000 && afstandL < 20 && afstandR < 20 && afstandM < 20) {
      toestandStartTijd = millis();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WACHTEN");
      Serial.println("WACHTEN");
      for (int thisNote = 0; thisNote < 8; thisNote++) {

        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, et
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(8, melody[thisNote], noteDuration);
        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(8);
      }
      toestand = WACHT;
    }
  }


  // bepaal snelheid afhankelijk van toestand
  // snelheid kan 0 t/m 255 zijn
  // bij lage getallen (ongeveer onder 100) heeft de motor
  // te weinig kracht om te rijden
  if (toestand == RECHTSAF) {
    snelheidR = 0;
    snelheidL = 255;
  }
  if (toestand == LINKSAF) {
    snelheidR = 255;
    snelheidL = 0;
  }
  if (toestand == VOORUIT) {
    snelheidR = 128;
    snelheidL = 128;
  }
  if (toestand == WACHT) {
    snelheidR = 0;
    snelheidL = 0;
  }

  // zet waarden voor acturatoren, voor alle toestanden
  // zet motorsnelheid
  analogWrite(pinMotorSnelheidR, snelheidR);
  analogWrite(pinMotorSnelheidL, snelheidL);

  // vertraging om te zorgen dat de seriële monitor de berichten bijhoudt
  delay(100);

}
