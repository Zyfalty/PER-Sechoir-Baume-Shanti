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

//Définitions des états du système
enum Etat { ARRET, MANUEL, CIRCUIT_OUVERT, CIRCUIT_FERME };

//Définitions et initialisation des paramètres du systèmes
struct Entrees {
  float T;
  float Text;
  float Tdes;
  float Tv1;
  float Tv2;
  bool  mode_manuel;
};

struct Sorties {
  bool desu, vent1, vent2, vt1, vt2, v3;
};

const float TSECHAGE_MIN = 20.0;
const float TSECHAGE_MAX = 35.0;
const float TLIM_DES     = 5.0;
const unsigned long TPS_MAX = 3600000UL; 

Etat etat = ARRET;
unsigned long entree_etat   = 0;
unsigned long dernier_cycle = 0;

float can_Tdes = 0.0;   // déshumidificateur
float can_Tv1  = 0.0;   // ventilateur 1
float can_Tv2  = 0.0;   // ventilateur 2
float can_Text = 0.0;   // extérieur

Entrees lire_capteurs() {
  Entrees e;
  e.Tdes = can_Tdes;
  e.Tv1  = can_Tv1;
  e.Tv2  = can_Tv2;
  e.Text = can_Text;
  e.T    = (e.Tdes + e.Tv1 + e.Tv2) / 3.0;
  return e;
}

// Fonction qui sert à changer d'états
void changer(Etat nouveau) {
  etat = nouveau; //on change d'état
  entree_etat = millis(); //on note l'heure d'entrée en millisecondes dans le nouvel état 
}

//On vérifie si la température est dans la plage de séchage
bool plage_ok(Entrees e) {
  if (e.T > TSECHAGE_MIN && e.T < TSECHAGE_MAX) {
    return true;
  } else {
    return false;
  }
}

// On vérifie s'il fait trop froid pour le deshumidificateur 
bool trop_froid(Entrees e) {
  if (e.T < TLIM_DES) {
    return true;
  } else {
    return false;
  }
}

// On vérifie si la durée maximale du cycle est dépassée
bool tps_ecoule() {
  if (millis() - entree_etat > TPS_MAX) {
    return true;
  } else {
    return false;
  }
}

// fonction qui renvoie la plus grande des trois valeurs
float max_des_3(float t1, float t2, float t3) {
  float resultat = t1;
  if (t2 > resultat) {
    resultat = t2;
  }
  if (t3 > resultat) {
    resultat = t3;
  }
  return resultat;
}

// fonction qui renvoie la plus petite des trois valeurs
float min_des_3(float t1, float t2, float t3) {
  float resultat = t1;
  if (t2 < resultat) {
    resultat = t2;
  }
  if (t3 < resultat) {
    resultat = t3;
  }
  return resultat;
}

//LECTURE DES BOUTONS
bool bouton(int pin) {
  return digitalRead(pin) == LOW;
}

//Machine à états
void fsm_update(Entrees e) {
  switch (etat) {

    case ARRET:
      if (e.mode_manuel && plage_ok(e) && !trop_froid(e)) {
        changer(MANUEL);
      } else if (!e.mode_manuel
                 && e.Text > max_des_3(e.Tdes, e.Tv1, e.Tv2)
                 && e.Text > TLIM_DES) {
        changer(CIRCUIT_OUVERT);
      }
      break;

    case MANUEL:
      if (!plage_ok(e) || trop_froid(e)) {
        changer(ARRET);
      }
      break;

    case CIRCUIT_OUVERT:
      if (trop_froid(e) || !plage_ok(e) || tps_ecoule()) {
        changer(ARRET);
      } else if (e.Text < min_des_3(e.Tdes, e.Tv1, e.Tv2)) {
        changer(CIRCUIT_FERME);
      }
      break;

    case CIRCUIT_FERME:
      if (trop_froid(e) || !plage_ok(e) || tps_ecoule()) {
        changer(ARRET);
      } else if (e.Text > maxi3(e.Tdes, e.Tv1, e.Tv2)
                 && e.Text > TLIM_DES) {
        changer(CIRCUIT_OUVERT);
      }
      break;
  }
}

//Fonction qui à sert rendre l’affichage compréhensible
const char* nom_etat(Etat e) {
  switch (e) {
    case ARRET:          return "ARRET";
    case MANUEL:         return "MANUEL";
    case CIRCUIT_OUVERT: return "CIRCUIT_OUVERT";
    case CIRCUIT_FERME:  return "CIRCUIT_FERME";
    default:             return "ETAT_INCONNU";
  }
}

//Sorties selon l'état du systèmes
Sorties sorties_selon_etat(Etat etat, Sorties manuel) {
  switch (etat) {
    //                            desu   vent1  vent2  vt1    vt2    v3
    case ARRET:          return { false, false, false, false, false, false };
    case MANUEL:         return manuel;
    case CIRCUIT_OUVERT: return { false, true,  true,  true,  false, true  };
    case CIRCUIT_FERME:  return { true,  true,  true,  false, true,  false };
    default: return { false, false, false, false, false, false };
  }
}

//Ecriture sur les broches
void appliquer(Sorties s) {

  if (s.desu) {
    digitalWrite(PIN_DESU, HIGH);
  } else {
    digitalWrite(PIN_DESU, LOW);
  }

  if (s.vent1) {
    digitalWrite(PIN_VENT1, HIGH);
  } else {
    digitalWrite(PIN_VENT1, LOW);
  }

  if (s.vent2) {
    digitalWrite(PIN_VENT2, HIGH);
  } else {
    digitalWrite(PIN_VENT2, LOW);
  }

  if (s.vt1) {
    digitalWrite(PIN_VT1, HIGH);
  } else {
    digitalWrite(PIN_VT1, LOW);
  }

  if (s.vt2) {
    digitalWrite(PIN_VT2, HIGH);
  } else {
    digitalWrite(PIN_VT2, LOW);
  }

  if (s.v3) {
    digitalWrite(PIN_V3, HIGH);
  } else {
    digitalWrite(PIN_V3, LOW);
  }
}

//Affichage
void afficher(Entrees e, Sorties s) {
  Serial.print("Etat: ");
  Serial.print(nom_etat(etat));
  Serial.print(" | T=");
  Serial.print(e.T, 1);
  Serial.print(" Text=");
  Serial.print(e.Text, 1);
  Serial.print(" | desu=");
  Serial.print(s.desu);
  Serial.print(" v1=");
  Serial.print(s.vent1);
  Serial.print(" v2=");
  Serial.print(s.vent2);
  Serial.print(" vt1=");
  Serial.print(s.vt1);
  Serial.print(" vt2=");
  Serial.print(s.vt2);
  Serial.print(" v3=");
  Serial.println(s.v3);
}

void setup() {
  Serial.begin(115200);

  // Initialisation des boutons
  pinMode(PIN_BTN_MANUEL, INPUT_PULLUP);
  pinMode(PIN_BTN_DESU,   INPUT_PULLUP);
  pinMode(PIN_BTN_VENT1,  INPUT_PULLUP);
  pinMode(PIN_BTN_VENT2,  INPUT_PULLUP);
  pinMode(PIN_BTN_VT1,    INPUT_PULLUP);
  pinMode(PIN_BTN_VT2,    INPUT_PULLUP);
  pinMode(PIN_BTN_V3,     INPUT_PULLUP);

  // Initialisation des actionneurs
  pinMode(PIN_DESU,  OUTPUT);
  pinMode(PIN_VENT1, OUTPUT);
  pinMode(PIN_VENT2, OUTPUT);
  pinMode(PIN_VT1,   OUTPUT);
  pinMode(PIN_VT2,   OUTPUT);
  pinMode(PIN_V3,    OUTPUT);

  // Etat initial des actionneurs
  digitalWrite(PIN_DESU,  LOW);
  digitalWrite(PIN_VENT1, LOW);
  digitalWrite(PIN_VENT2, LOW);
  digitalWrite(PIN_VT1,   LOW);
  digitalWrite(PIN_VT2,   LOW);
  digitalWrite(PIN_V3,    LOW);

  changer(ARRET);
}

void loop() {

  // Mesures reçues des noeuds Nano
  Entrees e = lire_capteurs();
  e.mode_manuel = bouton(PIN_BTN_MANUEL);

  // Commandes manuelles lues sur les boutons
  Sorties manuel;
  manuel.desu  = bouton(PIN_BTN_DESU);
  manuel.vent1 = bouton(PIN_BTN_VENT1);
  manuel.vent2 = bouton(PIN_BTN_VENT2);
  manuel.vt1   = bouton(PIN_BTN_VT1);
  manuel.vt2   = bouton(PIN_BTN_VT2);
  manuel.v3    = bouton(PIN_BTN_V3);

  // Décision
  fsm_update(e);

  // Application
  Sorties s = sorties_selon_etat(etat, manuel);
  appliquer(s);

  // Affichage
  afficher(e, s);

  // Un cycle par seconde
  delay(1000);
}
