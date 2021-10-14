/*****************************************
   Voorbeeld TestCode
   Voor circuit met 2 leds en 2 knoppen
   Breid de code zelf uit
   zodat je deze kunt gebruiken om te testen
   of alle componenten (nog) goed
   zijn aangesloten op je Arduino
   Emmauscollege
   v20210210GEE
 *****************************************/

/*****************************************
   variabelen die je gebruikt maken
 *****************************************/
// gebruikte pinnen
const int pinLedA   = 12; // pin van LED A
const int pinLedB   = 11; // pin van LED B
const int pinKnopA  = 2; // pin van knop A
const int pinKnopB  = 3; // pin van knop B

/*****************************************
   setup()
 *****************************************/
void setup() {
  // enable console en stuur opstartbericht
  Serial.begin(9600);
  Serial.println("Test start");

  // zet pinmode voor leds
  pinMode(pinLedA, OUTPUT);
  pinMode(pinLedB, OUTPUT);

  // zet pinmode voor knoppen
  pinMode(pinKnopA, INPUT);
  pinMode(pinKnopB, INPUT);

  // zet LEDs aan
  digitalWrite(pinLedA, HIGH);
  digitalWrite(pinLedB, HIGH);
  // wacht seconde zodat je kunt zien dat de LEDs het doen
  delay(1000);
}

/*****************************************
   loop()
 *****************************************/
void loop() {
  // zet LedA aan/uit afhankelijk van stand van knopA
  if (digitalRead(pinKnopA) == LOW) {
    digitalWrite(pinLedA, LOW);
  } else {
    digitalWrite(pinLedA, HIGH);
  }

  // zet LedB aan/uit afhankelijk van stand van nopB
  if (digitalRead(pinKnopB) == LOW) {
    digitalWrite(pinLedB, LOW);
  } else {
    digitalWrite(pinLedB, HIGH);
  }

  // kleine vertraging, 100 keer per seconde loopen is genoeg
  delay(10);
}
