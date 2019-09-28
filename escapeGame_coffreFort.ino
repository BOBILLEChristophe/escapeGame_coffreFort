#include <Bounce2.h>

#define DEBUG                 // Commenter en production
#define NBRE_CODES 9
#define NBRE_TRAPPES 2
#define DEBONCE_INTERVAL 10   // en ms

//  combinaisons
typedef struct Combinaisons {
  byte originale[NBRE_CODES] = { 6, 5, 4, 2, 8, 5, 9, 3, 6 };
  byte saisie[NBRE_CODES] = { 0 };
  bool ok = false;
};
Combinaisons comb;


// 2 trappes sur des sorties sur les broches
const byte trapPin[] = { A8, A9};

// Affectation des pins pour les LEDs et le boutons
const byte ledPin[] = {47, 45, 43, 41, 39, 37, 35, 33, 31};
const byte btnPin[] = {2, 3, 4, 5, 6, 7, 8, 9, 10};

Bounce *btn = new Bounce[NBRE_CODES];    // instance de Bonce

// Activation du beep
const byte tonePin = 7;

byte lastBtn = 255;

/*********************************************************************/
/************************** setup ************************************/
/*********************************************************************/

void setup() {

  Serial.begin(9600);

  for (byte i = 0; i < NBRE_CODES; i++) {
    btn[i].attach(btnPin[i], INPUT_PULLUP); // affectation et commutation des pins pour les boutons
    btn[i].interval(DEBONCE_INTERVAL);      // debounce interval in ms
    pinMode(ledPin[i], OUTPUT);             // affectation des pins pour les LEDs
    digitalWrite(ledPin[i], LOW);           // commutation des pins pour les LEDs
  }

  for (byte i = 0; i < NBRE_TRAPPES; i++) {
    pinMode(trapPin[i], OUTPUT);             // Affectation des pins pour les trappes
    digitalWrite(trapPin[i], LOW);           // Commutation des pins pour les trappes
  }


  /****************************** debut du test ****************************/

  byte pos = 0;

  while (!comb.ok) {                                      // tant que la variable "combinaison Ok" est "false"
    for (byte i = 0; i < NBRE_CODES; i++) {
      btn[i].update();
      if (btn[i].fell() && lastBtn != i)  {               // un nouveau bouton est enfoncé
        beep(440, 200);                                   // bruit pendant 200 ms (La fonction tone() est non bloquante)
        if (compare(i, pos) == 0) {                       // le bouton enfonce est-il le bon ?
          digitalWrite (ledPin[pos], HIGH);               // on allume la LED correspondante
          pos++;                                          // incrementation du compteur de position
          if (pos == NBRE_CODES)                          // s'il s'agit du dernier bouton
            comb.ok = true;                               // la variable "comb.ok" est mise a "true"
          else {
#ifdef DEBUG
            comb.saisie[pos] = i;                           // On affecte a "comb.saisie[pos]" le n° de la pin du bouton appuye
            AfficheTab(pos);                                // on affiche les differents codes deja saisis
#endif
          }
        }
        else {                                            // le bouton enfonce n'est pas le bon
          reset(pos);                                     // on efface toutes les combinaisons déjà saisies et on eteind toutes les LEDs allumées
          pos = 0;                                        // reinitialisation de pos
          beep(100, 200);                                 // bruit pendant 200 ms
#ifdef DEBUG
          AfficheTab(pos);                                // on affiche le code efface
#endif
        }
        lastBtn = i;
      }



      
    } // end for
  } // end while

  /****************************** La combinaison est bonne, on sort du test ****************************/

  digitalWrite(trapPin[0], HIGH);
  digitalWrite(trapPin[1], HIGH);
  beep(800, 200);
}

/*********************************************************************/
/************************** loop *************************************/
/*********************************************************************/

void loop() {}

/*********************************************************************/
/************************** fonctions ********************************/
/*********************************************************************/

int compare(byte i, byte pos) {
  if (i == comb.originale[pos])  // comparaison du code
    return 0;  // bon
  else
    return -1; // pas bon
}

void beep(int timbre, unsigned long duration) {
  tone(tonePin, timbre, duration);
}

void reset(byte pos) {
  for (byte i = 0; i < pos; i++) {
    digitalWrite(ledPin[i], LOW); // LED eteintes
#ifdef DEBUG
    comb.saisie[i] = 0;
#endif
  }
}

void AfficheTab(byte pos) {
  Serial.print("Combinaison saisie : ");
  for (byte i = 0; i < pos; i++) {
    Serial.print(comb.saisie[i]);
    Serial.print(" ");
  }
  Serial.print("\n");
  Serial.print("-------------------------------\n");
}
