#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ------------ Pinos ------------
const int sensor1Pin = A0;   // Sensor 1 (analógico)
const int sensor2Pin = A1;   // Sensor 2 (analógico)
const int buzzerPin  = 11;   // Buzzer ativo (HIGH = apita)

// ------------ LCD -------------
LiquidCrystal_I2C lcd(0x27, 16, 2); // Se não aparecer, testar 0x3F

// ------ Calibração por sensor ------
const int min1 = 690;  // S1 ~0% (seco)    -> AJUSTAR
const int max1 = 0;    // S1 ~100% (molhado)
const int min2 = 690;  // S2 ~0% (seco)    -> AJUSTAR
const int max2 = 0;    // S2 ~100% (molhado)

// ------ Regras p/ algodão ----------
const int thresholdAlert = 30; // alerta quando <30%
const int hysteresisUp   = 35; // silencia quando >=35%
const int idealLow       = 40; // faixa ideal 40–60%
const int idealHigh      = 60;

// ------ Buzzer (padrões) -----------
// Beep curto: 200 ms ON / 200 ms OFF entre beeps do mesmo ciclo.
// Pausa entre ciclos: 1500 ms.
const unsigned long beepOnMs   = 200;
const unsigned long beepGapMs  = 200;
const unsigned long cyclePause = 1500;

enum AlarmMode { NONE, S1_ONLY, S2_ONLY, BOTH };

// Estado do padrão
AlarmMode currentMode = NONE;
AlarmMode lastMode    = NONE;
unsigned long tMark   = 0;
int beepsTarget = 0;    // 1 (S1), 2 (S2), 3 (BOTH)
int beepsDone   = 0;    // quantos beeps já tocaram no ciclo
bool buzzerOn   = false;

// ---------- Utilidades ----------
int averageRead(int pin, byte samples = 10, int delayMs = 5) {
  long acc = 0;
  for (byte i = 0; i < samples; i++) { acc += analogRead(pin); delay(delayMs); }
  return (int)(acc / samples);
}

int toPercent(int raw, int minV, int maxV) {
  long p = map(raw, minV, maxV, 0, 100);
  if (p < 0)   p = 0;
  if (p > 100) p = 100;
  return (int)p;
}

void buzzerLOW() { digitalWrite(buzzerPin, LOW); buzzerOn = false; }
void buzzerHIGH(){ digitalWrite(buzzerPin, HIGH); buzzerOn = true;  }

void resetPatternForMode(AlarmMode m) {
  lastMode   = m;
  currentMode= m;
  tMark      = millis();
  beepsDone  = 0;
  buzzerLOW();

  switch (m) {
    case NONE:    beepsTarget = 0; break;
    case S1_ONLY: beepsTarget = 1; break;
    case S2_ONLY: beepsTarget = 2; break;
    case BOTH:    beepsTarget = 3; break;
  }
}

// Toca 1/2/3 beeps curtos conforme o modo; pausa longa e repete enquanto o modo persistir.
void runBeepPattern() {
  if (currentMode == NONE) { buzzerLOW(); return; }

  unsigned long now = millis();

  // Caso especial: acabou o ciclo (fez todos os beeps), espera a pausa longa e reinicia
  if (beepsDone >= beepsTarget) {
    if (now - tMark >= cyclePause) {
      beepsDone = 0;
      tMark = now;
    }
    return;
  }

  // Dentro do ciclo de beeps:
  if (!buzzerOn) {
    // BEEPER OFF -> aguarda gap, então liga
    if (now - tMark >= beepGapMs) {
      buzzerHIGH();
      tMark = now;
    }
  } else {
    // BEEPER ON -> aguarda onMs, então desliga e conta um beep
    if (now - tMark >= beepOnMs) {
      buzzerLOW();
      beepsDone++;
      tMark = now;
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  buzzerLOW();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0); lcd.print("S1:    Estado:");
  lcd.setCursor(0, 1); lcd.print("S2:    Estado:");
  delay(800);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Algodao - Faixa");
  lcd.setCursor(0, 1); lcd.print("ideal: 40% a 60%");
  delay(1200);
  lcd.clear();

  resetPatternForMode(NONE);
}

void loop() {
  // ---- Leituras cruas ----
  int raw1 = averageRead(sensor1Pin);
  int raw2 = averageRead(sensor2Pin);

  // ---- Conversao p/ % ----
  int p1 = toPercent(raw1, min1, max1);
  int p2 = toPercent(raw2, min2, max2);

  // ---- Logs ----
  Serial.print("Raw1: "); Serial.print(raw1);
  Serial.print("  p1(%): "); Serial.print(p1);
  Serial.print("  | Raw2: "); Serial.print(raw2);
  Serial.print("  p2(%): "); Serial.println(p2);

  // ---- Histerese por sensor ----
  static bool s1Alarm = false;
  static bool s2Alarm = false;

  if (p1 < thresholdAlert) s1Alarm = true;
  else if (p1 >= hysteresisUp) s1Alarm = false;

  if (p2 < thresholdAlert) s2Alarm = true;
  else if (p2 >= hysteresisUp) s2Alarm = false;

  // ---- Define modo atual ----
  AlarmMode newMode = NONE;
  if (s1Alarm && s2Alarm)      newMode = BOTH;
  else if (s1Alarm)            newMode = S1_ONLY;
  else if (s2Alarm)            newMode = S2_ONLY;
  else                         newMode = NONE;

  if (newMode != currentMode) {
    resetPatternForMode(newMode);
  }

  // ---- Atualiza LCD ----
  // Linha 0 - S1
  lcd.setCursor(0, 0); lcd.print("S1:");
  lcd.setCursor(3, 0);
  if (p1 < 10) lcd.print("  ");
  else if (p1 < 100) lcd.print(" ");
  lcd.print(p1); lcd.print("% ");

  lcd.setCursor(11, 0);
  if (p1 < thresholdAlert)              lcd.print("AL ");
  else if (p1 >= idealLow && p1 <= idealHigh) lcd.print("OK ");
  else if (p1 > idealHigh)              lcd.print("UM ");
  else                                  lcd.print("UM "); // 30–39%

  // Linha 1 - S2
  lcd.setCursor(0, 1); lcd.print("S2:");
  lcd.setCursor(3, 1);
  if (p2 < 10) lcd.print("  ");
  else if (p2 < 100) lcd.print(" ");
  lcd.print(p2); lcd.print("% ");

  lcd.setCursor(11, 1);
  if (p2 < thresholdAlert)              lcd.print("AL ");
  else if (p2 >= idealLow && p2 <= idealHigh) lcd.print("OK ");
  else if (p2 > idealHigh)              lcd.print("UM ");
  else                                  lcd.print("UM ");

  // ---- Toca o padrão (se houver alarme) ----
  runBeepPattern();

  delay(30); // loop leve
}
