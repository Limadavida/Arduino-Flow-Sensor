/*
Target: A simple Snake game.

**Date: 04/12/2019
**Author: devlimadavid
**linkedin.com/in/devdavidlima/
*/

#include <Arduino.h>

// ------- BIGNUMBERS LCD 16x2 --------- 
#include <LiquidCrystal.h>
#include <BigNumbers.h>

LiquidCrystal lcd(12,11,5,4,3,2); // construct LCD object
BigNumbers bigNum(&lcd);         // construct BigNumbers object, passing to it the name of our LCD object

// -------------- ADS1115 Analog Digital Converter ------
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;       /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

// -------- Filter to Analog Digital Converter ---------
#include "MegunoLink.h"
#include "Filter.h"
ExponentialFilter<long> ADCFilter(10, 0);

// ---- SETTING SENSOR 02 -------
// - SPAN -
float oxygen = 0;

int refOxygen = 0;
int valMax, valMin, valMax8, valMin8, valMin4, valMax4,  valMin2, valMax2;
int minmV = 10;
int maxmV = 14;

#define GANHO_DOIS
//#define GANHO_QUATRO
//#define GANHO_OITO

#ifdef GANHO_DOIS
#define valMax 530 // 100%
#define valMin 120 // 21%
#endif

#ifdef GANHO_QUATRO
#define valMax 970
#define valMin 315
#endif

#ifdef GANHO_OITO
#define valMax 3137
#define valMin 428
#endif

// ---------- - Checking  -----------------------------------------
#if defined(GANHO_DOIS) && defined(GANHO_QUATRO) && defined(GANHO_OITO)
#error "SOMENTE UM TIPO DE GANHO PODE SER DEFINIDO"
#elif defined(GANHO_DOIS)
#pragma message "GANHO DOIS ATIVADO"
#elif defined(GANHO_QUATRO)
#pragma message "GANHO QUATRO ATIVADO"
#elif defined(GANHO_OITO)
#pragma message "GANHO OITO ATIVADO"
#else
#error "NENHUM GANHO ATIVADO!"
#endif

// ------------------- LCD ----------------------------------
#include <LiquidCrystal.h>
 
//Define os pinos que serão utilizados para ligação ao display
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{

  // --------- BIG NUMBERS ----------
TCCR1B = TCCR1B & 0b11111000 | 0x01; // use for Arduino Uno
  lcd.begin(16,2); // setup LCD rows and columns
  bigNum.begin(); // set up BigNumbers
  lcd.clear(); // clear display
  
  //----- Menu / AutoCalib / pegando dados -----
  Serial.begin(115200);
  
  Serial.println ("CALIBRAÇÃO (Y - 1) / (N - 2)");
  int calibra = Serial.parseInt();
 if (calibra = 1){
  Serial.print("entrou no modo calibraçao");
 }else{
  Serial.println("INICIANDO O SISTEMA");
  Serial.println(" O2 B ");

 }
  
  
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV


#ifdef GANHO_DOIS
  ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
#endif
#ifdef GANHO_QUATRO
  ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
#endif
#ifdef GANHO_OITO
  ads.setGain(GAIN_EIGHT);         // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
#endif
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
   ads.begin();
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  
  for (int x = 0; x < 60; x++) {
    int16_t adc0;

    adc0 = ads.readADC_SingleEnded(0);
    Serial.print(adc0); Serial.print(",");

    int RawValue = adc0;
    ADCFilter.Filter(RawValue);

    Serial.println(ADCFilter.Current());
    //oxygen = map(ADCFilter.Current(), 640, 896, 0, 100);
    delay(50);

  }
  
  //Define o número de colunas e linhas do LCD
  //lcd.begin(16, 2);
}
 
void loop()
{
  //ADS1115
  int16_t adc0, adc1, adc2, adc3;

  adc0 = ads.readADC_SingleEnded(0);

  Serial.print(adc0); Serial.print(",");

  int RawValue = adc0;
  ADCFilter.Filter(RawValue);

  Serial.print(ADCFilter.Current()); Serial.print("\n");

  oxygen = map(ADCFilter.Current(), valMin, valMax, 21, 100);

// ----------- LCD ---------------------------------
    //Limpa a tela
    //lcd.clear();
    //Posiciona o cursor na coluna 3, linha 0;
   lcd.setCursor(0, 0);
    //Envia o texto entre aspas para o LCD
    lcd.print("[02%]");
    //lcd.setCursor(3, 1);
    //lcd.print(oxygen);

if (oxygen < 101) //Sensor OUT
{
   lcd.clear();
  bigNum.displayLargeInt(oxygen,0, 3, false); //saida do display com bignumbers
  lcd.setCursor(9, 1);
  lcd.print("%");
  lcd.print(" [02] ");   
    delay(400);
}else{
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print(" [02] "); 
  lcd.setCursor(0, 1);
  lcd.print("SENSOR OUT ERROR");
    delay(300);
}

}