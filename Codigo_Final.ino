// INCLUSÃO DE BIBLIOTECAS
#include <Btn.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <HX711_ADC.h>

// DEFINIÇÕES
#define pinRx 10 //Recepctor MP3
#define pinTx 11 //Transmissor MP3
#define volumeMP3 30
#define pinBuzzer 6
#define pinBtn1 4 //Botão Tara
#define pinBtn2 5 //Botão Resultado
#define pinHX_DT A0 //Azul
#define pinHX_SCK A1 //Branco

//VARIÁVIES
int gotas=0;
int gotasCALC=0;
float volume=0;
float volumeMED=0;
float volumeOLD=0;
float calibracao=5002.28;
unsigned long t = 0;

// INSTANCIANDO OBJETOS
SoftwareSerial playerMP3Serial(pinRx, pinTx);
DFRobotDFPlayerMini playerMP3;
HX711_ADC LoadCell(pinHX_DT, pinHX_SCK);

Btn bot1 = new Btn(pinBtn1); //Botão Start - Faz a tara e inicia a contagem
Btn bot2 = new Btn(pinBtn2); //Botão Resultado - Informa o Total


void setup() {

	Serial.begin(9600);
	playerMP3Serial.begin(9600);
 
  pinMode(pinBtn1, INPUT);
  pinMode(pinBtn2, INPUT);
  pinMode(pinBuzzer, OUTPUT);

  LoadCell.begin(); //Iniciando Balanca
  LoadCell.start(2000, true);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Falha ao iniciar balança - Verifique os pinos");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibracao);
    Serial.println("Balanca ligada");
  }
  Serial.println();

  Serial.println(F("Iniciando Módulo MP3...")); //Iniciando MP3
  if (!playerMP3.begin(playerMP3Serial)) {  // COMUNICAÇÃO REALIZADA VIA SOFTWARE SERIAL
    Serial.println(F("Falha ao iniciar MP3:"));
    Serial.println(F("1.Confira as conexões!"));
    Serial.println(F("2.Confira o cartão SD!"));
    while(true){
      delay(0);
    }
  }
  Serial.println(F("DFPlayer ligado!"));

  //Configurações iniciais
  playerMP3.setTimeOut(500); //Timeout serial 500ms
  playerMP3.volume(volumeMP3); // Volume (1-31)
  playerMP3.EQ(0); // Equalizacao normal

  playerMP3.playFolder(2, 6); //Iniciar Dosagem
  delay(2000);

}

void loop() {

  static boolean newDataReady = 0;  
  if (LoadCell.update()) newDataReady = true;

  if (newDataReady) {
      volumeMED = LoadCell.getData();
      Serial.println(volumeMED);
      newDataReady = 0;
  }

    if(volumeMED>(volumeOLD + 0.04)) {
      if(millis()>t+100) {
      tone(pinBuzzer,450);
      }
    volumeOLD = volumeMED;
    t=millis();
    }

  if(millis()> t+200) {
    noTone(pinBuzzer);
  }

  bot1.clique(tara);
  bot2.clique(resultado);
  
}

void tara () {
  gotas=0;
  volume=0;
  volumeOLD=0;
  LoadCell.tareNoDelay();
  playerMP3.playFolder(2, 6); // Iniciar Dosagem
  delay(1500);
}

void resultado () {

  gotas = volumeMED/0.05;
  volume = volumeMED*1.09;
  int inteiro = volume;
  int decimal = (volume-inteiro)*100;

  playerMP3.playFolder(2, 1); //Total
  delay(1500);
  playerMP3.playFolder(1, gotas); //Quantidade de Gotas
  delay(1750);
  playerMP3.playFolder(2, 3); // Gotas
  delay(1500);
  playerMP3.playFolder(2, 5); // E
  delay(1200);

  if (decimal==0){
    playerMP3.playFolder(1,inteiro);
    delay(1750);
  } else if (decimal<10) {
    playerMP3.playFolder(1,inteiro);
    delay(1750);
    playerMP3.playFolder(2,2); //Vírgula
    delay(1500);
    playerMP3.playFolder(1,0);
    delay(1500);
    playerMP3.playFolder(1,decimal);
    delay(1500);
  } else {
    playerMP3.playFolder(1,inteiro);
    delay(1750);
    playerMP3.playFolder(2,2); //Vírgula
    delay(1500);
    playerMP3.playFolder(1,decimal);
    delay(2000);
  }

  playerMP3.playFolder(2, 4); // ml
}

