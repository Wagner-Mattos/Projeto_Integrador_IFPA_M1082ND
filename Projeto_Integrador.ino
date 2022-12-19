/*
  Código C++

  Projeto Integrador - IFPA - Belém
  Medidor de Energia Elétrica
  Curso: Técnico em Eletrotécnica

  Alunos: FELIPE GARCIA DE LIMA
          JESIEL NOVAES DE OLIVEIRA
          LEONARDO LIMA COSTA MESQUITA
          MARCONI ARAUJO MAIA
          MATEUS DE OLIVEIRA MARTINS
          PATRICK SARGES DOS SANTOS
          WAGNER AUGUSTO PALHETA DE MATTOS

  Orientadora: Profa. Dra. EMILIANE ADVINCULA MALHEIROS

  Ano: 2022
*/

//################# Constantes ################

const float SENSE = 0.066;  // Sensibilidade do sensor ACS712 66 miliVolts para 1 Ampere

//################# Bibliotecas #################

#include <LiquidCrystal.h>

//########## Mapeamento de Hardware #############

#define SENSOR A0  // ACS712
#define RS 12      // Pino RS do LCD no Pino D12 do Arduino
#define E 11       // Pino E do LCD no Pino D11 do Arduino
#define D4 7       // Pino D4 do LCD no Pino D7 do Arduino
#define D5 6       // Pino D5 do LCD no Pino D6 do Arduino
#define D6 5       // Pino D6 do LCD no Pino D5 do Arduino
#define D7 4       // Pino D7 do LCD no Pino D4 do Arduino

//########## Declaração de Objetos #############

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);  //inicia a biblioteca

//########## Protótipo de Funções e Procedimentos#############

float mediaRMS();
void menu_1();
void menu_2();
void menu_3();
void menu_4();

//########## Variáveis Globais#############

double energia = 0.0;

byte linha = 1;
byte coluna = 0;
byte menu = 1;

boolean modo = 0;  // Padrão AC
boolean flag = 0;

float tensao = 0.0;  // Tensão de Operação ( tensão eficaz)

//############### Configurações Iniciais #################
void setup() {
  lcd.begin(20, 4);
  Serial.begin(9600);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), botaoEscolha, FALLING);
  attachInterrupt(digitalPinToInterrupt(2), botaoEnter, FALLING);

  menu_1();
}
//################ Loop #######################
void loop() {

  unsigned long fim = 0;
  unsigned long inicio = millis();

  float corrente, potencia, mediaTensao;

  mediaTensao = mediaRMS();

  if (abs(mediaTensao) < (5.0 / 1023.0)) {
    if (flag == 1) {
      menu_4();
      flag = 0;
    }
    corrente = 0.0;
    potencia = 0.0;
  } else if (abs(mediaTensao) > 1.98) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("  SOBRE CORRENTE!!");
    flag = 1;
  } else {
    if (flag == 1) {
      menu_4();
      flag = 0;
    }
    corrente = mediaTensao / SENSE;
    potencia = abs(tensao * corrente);
  }

  Serial.println(mediaTensao);

  if (tensao != 0.0) {
    lcd.setCursor(10, 1);
    lcd.print(corrente);
    lcd.print(" A ");
    lcd.setCursor(10, 2);
    lcd.print(potencia);
    lcd.print(" W ");
    lcd.setCursor(10, 3);

    fim = millis();
    energia += potencia * (((fim - inicio) / 1000.0) / 3600.0) / 1000.0;
    lcd.print(energia, 3);
    lcd.print(" kWh ");
  }
}
//################ Funções #######################

float mediaRMS() {

  float soma = 0.0;
  int x = 511;
  if (modo == 1) {  // modo DC
    x = 0;
    for (int i = 0; i < 1000; i++) {
      soma += pow(((analogRead(SENSOR) - x) * 5.0 / 1023.0), 2);
      delay(1);
    }
    float media = sqrt(soma / 1000.0);
    return media - 2.5;
  }else{
    for (int i = 0; i < 1000; i++) {
      soma += pow(((analogRead(SENSOR) - x) * 5.0 / 1023.0), 2);
      delay(1);
    }
    float media = sqrt(soma / 1000.0);
    return media;
  }

}
//################ Procedimentos #######################
void botaoEscolha() {
  if (tensao > 0) tensao = 0.0;
  linha++;
  switch (menu) {
    case 1:
      menu_1();
      break;
    case 2:
      menu_2();
      break;
    case 3:
      menu_3();
      break;
  }
}
void botaoEnter() {

  switch (menu) {
    case 1:
      if (linha == 1) {
        menu_2();
        menu = 2;
      } else {
        menu_3();
        menu = 3;
      }
      break;
    case 2:
      switch (coluna) {
        case 0:  // coluna 0
          switch (linha) {
            case 1:
              modo = 1;
              tensao = 12.0;
              menu_4();
              break;
            case 2:
              modo = 1;
              tensao = 22.0;
              menu_4();
              break;
            default:
              modo = 1;
              tensao = 50.0;
              menu_4();
          }
          break;
        case 7:  // coluna 7
          switch (linha) {
            case 1:
              modo = 1;
              tensao = 16.0;
              menu_4();
              break;
            case 2:
              modo = 1;
              tensao = 24.0;
              menu_4();
              break;
            default:
              modo = 1;
              tensao = 100.0;
              menu_4();
          }
          break;
        default:  // coluna 14
          switch (linha) {
            case 1:
              modo = 1;
              tensao = 19.0;
              menu_4();
              break;
            case 2:
              modo = 1;
              tensao = 48.0;
              menu_4();
              break;
            default:
              menu_3();
              menu = 3;
          }
      }
      break;  // fim case 2
    case 3:
      switch (linha) {
        case 1:
          modo = 0;
          tensao = 127.0;
          menu_4();
          break;
        case 2:
          modo = 0;
          tensao = 220.0;
          menu_4();
          break;
        default:
          menu_2();
          menu = 2;
      }
      break;
  }
}
void menu_1() {
  if (linha >= 3) linha = 1;
  lcd.clear();
  lcd.print("MODO:");
  lcd.setCursor(6, 1);
  lcd.print("DC");
  lcd.setCursor(6, 2);
  lcd.print("AC");
  lcd.setCursor(4, linha);
  lcd.print("->");
}
void menu_2() {
  if (linha >= 4) {
    switch (coluna) {
      case 0:
        coluna = 7;
        linha = 1;
        break;
      case 7:
        coluna = 14;
        linha = 1;
        break;
      default:
        coluna = 0;
        linha = 1;
    }
  }
  lcd.clear();
  lcd.print("TENSAO DC:");
  lcd.setCursor(1, 1);
  lcd.print(" 12V");
  lcd.print("    16V");
  lcd.print("    19V");
  lcd.setCursor(1, 2);
  lcd.print(" 22V");
  lcd.print("    24V");
  lcd.print("    48V");
  lcd.setCursor(1, 3);
  lcd.print(" 50V");
  lcd.print("    100V");
  lcd.print("    AC");
  lcd.setCursor(coluna, linha);
  lcd.print("->");
}
void menu_3() {
  if (linha >= 4) linha = 1;
  lcd.clear();
  lcd.print("TENSAO AC:");
  lcd.setCursor(6, 1);
  lcd.print("127V");
  lcd.setCursor(6, 2);
  lcd.print("220V");
  lcd.setCursor(6, 3);
  lcd.print("DC");
  lcd.setCursor(4, linha);
  lcd.print("->");
}
void menu_4() {
  lcd.clear();
  if (modo == 1) {
    lcd.print("TensaoDC: ");
  } else {
    lcd.print("TensaoAC: ");
  }
  lcd.print(tensao);
  lcd.print(" V");
  lcd.setCursor(0, 1);
  lcd.print("Corrente:");
  lcd.setCursor(0, 2);
  lcd.print("Potencia:");
  lcd.setCursor(0, 3);
  lcd.print("Energia :");
}