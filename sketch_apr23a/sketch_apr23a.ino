// ============================================================
//   SMART GAS DETECTOR - Projet IoT
//   Composants : MQ-2, LCD 16x2, LED Rouge, LED Verte,
//                Buzzer, Servo (fenêtre), Virtual Terminal
//   Simulation Proteus 8 + Arduino IDE
// ============================================================

#include <LiquidCrystal.h>//pour controler l ecran LCD
#include <Servo.h>//pour controler le moteur

// --- Broches ---
#define GAS_PIN     A0   // Capteur MQ-2 sortie analogique
#define LED_ROUGE   12    // LED rouge = danger
#define LED_VERTE   13   // LED verte = normal
#define BUZZER      2    // Buzzer sonore
#define SERVO_PIN   11    // Servo = ouverture fenêtre

// LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(8, 7 , 6 , 5, 4, 3);

Servo servoFenetre;

// --- Seuil de détection ---
// Valeur analogique 0-1023. En dessous = normal, au dessus = gaz détecté
// Ajuste selon tes tests dans Proteus (entre 300 et 500 généralement)
const int SEUIL_GAZ = 400;

// --- Variables ---
int valeurGaz = 0;
bool etatAlerte = false;
unsigned long dernierBip = 0;
bool buzzerOn = false;

// ============================================================
void setup() {
  // Initialisation LCD
  lcd.begin(16, 2);
  lcd.clear();

  // Initialisation broches
  pinMode(LED_ROUGE, OUTPUT);
  pinMode(LED_VERTE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Initialisation servo - fenêtre fermée au démarrage
  servoFenetre.attach(SERVO_PIN);
  servoFenetre.write(0);   // 0° = fenêtre fermée

  // Communication série ? Virtual Terminal (Proteus)
  Serial.begin(9600);

  // Message de démarrage sur LCD
  lcd.setCursor(0, 0);
  lcd.print("  GAS DETECTOR  ");
  lcd.setCursor(0, 1);
  lcd.print("  Demarrage...  ");
  delay(2000);
  lcd.clear();

  // Message de démarrage sur Virtual Terminal
  Serial.println("=== Smart Gas Detector ===");
  Serial.println("Systeme pret. Surveillance en cours...");
  Serial.println("Seuil alerte: 400");
  Serial.println("==========================");

  // État initial = NORMAL
  etatNormal();
}

// ============================================================
void loop() {
  // Lire le capteur MQ-2
  valeurGaz = analogRead(GAS_PIN);

  if (valeurGaz >= SEUIL_GAZ) {
    // --- ALERTE GAZ DETECTE ---
    if (!etatAlerte) {
      etatAlerte = true;
      etatAlerteGaz();
    }
    // Buzzer qui bipe en continu (alternance ON/OFF)
    bipperBuzzer();

  } else {
    // --- ÉTAT NORMAL ---
    if (etatAlerte) {
      etatAlerte = false;
      etatNormal();
      Serial.println("[INFO] Gaz disperse. Retour a la normale.");
    }
  }

  delay(200); // Pause lecture
}

// ============================================================
//  Fonction : État NORMAL (pas de gaz)
// ============================================================
void etatNormal() {
  // LEDs
  digitalWrite(LED_VERTE, HIGH);   // Verte allumée
  digitalWrite(LED_ROUGE, LOW);    // Rouge éteinte

  // Buzzer éteint
  digitalWrite(BUZZER, LOW);
  buzzerOn = false;

  // Servo : ferme la fenêtre (0°)
  servoFenetre.write(0);

  // Affichage LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  ETAT NORMAL   ");
  lcd.setCursor(0, 1);
  lcd.print(" Air: OK  :)    ");

  // Message Virtual Terminal (simule SMS)
  Serial.println("----------------------------");
  Serial.println("[STATUS] AIR NORMAL");
  Serial.println("Pas de gaz detecte.");
  Serial.println("LED verte allumee.");
  Serial.println("Fenetre: FERMEE");
  Serial.println("----------------------------");
}

// ============================================================
//  Fonction : ALERTE - Gaz détecté
// ============================================================
void etatAlerteGaz() {
  // LEDs
  digitalWrite(LED_VERTE, LOW);    // Verte éteinte
  digitalWrite(LED_ROUGE, HIGH);   // Rouge allumée

  // Servo : ouvre la fenêtre (90°)
  servoFenetre.write(90);

  // Affichage LCD - Ligne 1
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("!!! DANGER !!!  ");
  lcd.setCursor(0, 1);
  lcd.print("GAZ DETECTE !   ");

  // Message Virtual Terminal (simule envoi SMS)
  Serial.println("****************************");
  Serial.println("[!!! ALERTE !!!]");
  Serial.println("GAZ DETECTE DANS L'AIR !");
  Serial.println(">> RENTREZ A LA MAISON !");
  Serial.println(">> Fenetre ouverte AUTO.");
  Serial.println(">> Buzzer active.");
  Serial.println(">> LED rouge allumee.");
  Serial.print("[Valeur capteur MQ-2: ");
  Serial.print(valeurGaz);
  Serial.println("]");
  Serial.println("****************************");
}

// ============================================================
//  Fonction : Bip du buzzer (sonnerie intermittente)
// ============================================================
void bipperBuzzer() {
  unsigned long maintenant = millis();

  // Bip toutes les 500ms
  if (maintenant - dernierBip >= 500) {
    dernierBip = maintenant;
    buzzerOn = !buzzerOn;
    digitalWrite(BUZZER, buzzerOn ? HIGH : LOW);

    // Mettre à jour LCD pour afficher la valeur en temps réel
    lcd.setCursor(0, 1);
    lcd.print("Niveau:");
    lcd.print(valeurGaz);
    lcd.print("    ");
  }
}
