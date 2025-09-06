#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int sensorPin = A0;  // Pino analógico A0
const int valvePin = 7;    // Pino digital para o relé
int sensorValue = 0;       // Variável para armazenar o valor lido

// Definição dos limites para conversão em porcentagem
const int minValue = 690; // Valor correspondente a 0% (seco)
const int maxValue = 0;   // Valor correspondente a 100% (molhado)

// Limite para acionar a válvula (exemplo: abaixo de 30% aciona a válvula)
const int threshold = 30;

// Endereço padrão do módulo I2C (pode variar, use um scanner I2C para confirmar)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);  // Inicia a comunicação serial
  lcd.init();          // Inicia o LCD
  lcd.backlight();     // Liga a luz de fundo do LCD

  pinMode(valvePin, OUTPUT);
  digitalWrite(valvePin, HIGH);  // Relé desligado inicialmente (HIGH = DESLIGADO)

  lcd.setCursor(0, 0);
  lcd.print("Umid:     Rele:");
}

void loop() {
  sensorValue = analogRead(sensorPin);  // Lê o valor do sensor

  // Calcula a porcentagem de umidade
  int humidityPercent = map(sensorValue, minValue, maxValue, 0, 100);
  humidityPercent = constrain(humidityPercent, 0, 100); // Limita a faixa entre 0% e 100%

  Serial.print("Umidade (%): ");
  Serial.println(humidityPercent);

  // Controle da válvula (Relé liga em LOW)
  if (humidityPercent < threshold) {
    digitalWrite(valvePin, LOW); // Aciona o relé (ativa a válvula)
    lcd.setCursor(11, 0);
    lcd.print("ON ");
  } else {
    digitalWrite(valvePin, HIGH);  // Desliga o relé (desativa a válvula)
    lcd.setCursor(11, 0);
    lcd.print("OFF");
  }

  // Exibe a umidade no LCD
  lcd.setCursor(6, 0);
  lcd.print(humidityPercent);
  lcd.print("%");

  delay(1000);  // Aguarda 1 segundo antes de ler novamente
}
