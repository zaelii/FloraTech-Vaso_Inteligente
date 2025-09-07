# 🌱 FloraTech – Vaso Inteligente com Arduino

Este projeto faz parte da oficina **FloraTech**, que une **tecnologia, robótica educacional e sustentabilidade**.  
A proposta é construir um **vaso inteligente** para monitoramento de **algodão** usando **sensores de umidade do solo** e **display LCD I2C**, com alerta sonoro via **buzzer**.

---

## 🚀 Funcionalidades

- Leitura de **2 sensores de umidade do solo** (A0 e A1).  
- Exibição em **LCD I2C 16x2** da porcentagem de umidade e estado de cada sensor:
  - `AL` → solo seco (alarme).  
  - `OK` → solo dentro da faixa ideal (40–60%).  
  - `UM` → solo úmido (>60%).  
- **Buzzer no GPIO 11** para alertar quando a umidade cair abaixo de 30%.  
  - 1 beep curto = Sensor 1 seco.  
  - 2 beeps curtos = Sensor 2 seco.  
  - 3 beeps curtos = Ambos secos.  

---

## 🛠️ Materiais utilizados

- **Arduino Uno R3**  
- **2 sensores de umidade do solo analógicos**  
- **Display LCD 16x2 com módulo I2C** (endereço 0x27 ou 0x3F)  
- **Buzzer ativo (digital)**  
- Jumpers, protoboard e cabos USB  

---

## 📌 Conexões (GPIOs usados)

| Componente         | Pino Arduino |
|--------------------|--------------|
| Sensor de umidade 1 | A0           |
| Sensor de umidade 2 | A1           |
| Buzzer             | D11          |
| LCD SDA            | A4           |
| LCD SCL            | A5           |

---

## ⚙️ Calibração

Cada sensor pode variar. Antes de usar:
1. Coloque a sonda no **solo seco** e anote o valor cru → configure como `minX`.  
2. Coloque a sonda no **solo bem molhado** e anote o valor cru → configure como `maxX`.  
3. Ajuste as constantes `min1/max1` e `min2/max2` no código.  

---

## 🖥️ Código

O código principal está no arquivo **`floraTech.ino`**.  
- Biblioteca usada para LCD: [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C).  
- A lógica já vem com **histerese** para evitar chiado próximo ao limite.  

---

## 🌾 Contexto educacional

Este projeto foi desenvolvido para **oficinas práticas com alunos**, permitindo:  
- Aprender sobre sensores e Arduino.  
- Refletir sobre **uso consciente da água**.  
- Valorizar o **algodão nordestino** como cultura local.  

---

## 📷 Fotos / Demonstrações

*(Adicione aqui fotos do protótipo e prints do LCD)*

---

## ✨ Créditos

- Desenvolvido por **Disraeli Michelangelo Rafael da Costa Filho** no âmbito da oficina **FloraTech**.  
- Inspirado em práticas de **robótica educacional** e **agricultura de precisão**.  

---

## 📄 Licença

Este projeto é open-source sob a licença **MIT**.  
Sinta-se livre para usar, modificar e compartilhar.
