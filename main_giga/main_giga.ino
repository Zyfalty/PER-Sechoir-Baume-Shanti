// ENIB 2026-2027
// PER Module
// Project Sechoir Baume Shanti
// intende for Arduino Giga

// Boutons
const int PIN_BTN_MANUEL = 22;
const int PIN_BTN_DESU  = 23;
const int PIN_BTN_VENT1 = 24;
const int PIN_BTN_VENT2 = 25;
const int PIN_BTN_VT1   = 26;
const int PIN_BTN_VT2   =  27;
const int PIN_BTN_V3    = 28;

// Actionneurs
const int PIN_DESU  = 30;
const int PIN_VENT1 = 31;
const int PIN_VENT2 = 32;
const int PIN_VT1   = 33;
const int PIN_VT2   = 34;
const int PIN_V3    = 35;

void setup() {

    // Initialisation des boutons
    pinMode(PIN_BTN_MANUEL, INPUT_PULLUP);
    pinMode(PIN_BTN_DESU, INPUT_PULLUP);
    pinMode(PIN_BTN_VENT1, INPUT_PULLUP);
    pinMode(PIN_BTN_VENT2, INPUT_PULLUP);
    pinMode(PIN_BTN_VT1, INPUT_PULLUP);
    pinMode(PIN_BTN_VT2, INPUT_PULLUP);
    pinMode(PIN_BTN_V3, INPUT_PULLUP);


    // Initialisation des actionneurs
    pinMode(PIN_DESU, OUTPUT);
    pinMode(PIN_VENT1, OUTPUT);
    pinMode(PIN_VENT2, OUTPUT);
    pinMode(PIN_VT1, OUTPUT);
    pinMode(PIN_VT2, OUTPUT);
    pinMode(PIN_V3, OUTPUT);


    // Etat initial des actionneurs
    digitalWrite(PIN_DESU, LOW);
    digitalWrite(PIN_VENT1, LOW);
    digitalWrite(PIN_VENT2, LOW);
    digitalWrite(PIN_VT1, LOW);
    digitalWrite(PIN_VT2, LOW);
    digitalWrite(PIN_V3, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

}
