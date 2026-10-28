#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h> 

#define WHITE ILI9341_WHITE
#define RED ILI9341_RED
#define BLACK ILI9341_BLACK
#define BLUE ILI9341_BLUE
#define YELLOW ILI9341_YELLOW


#include "SPI.h"
#include "ILI9341_t3.h"
#include "font_Arial.h"

#define TFT_DC  9
#define TFT_CS 10

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);


/*==========TO-DO ===================/
  - clean up code 
  - add interrupt so if you press a new button the old sample stops !
  - incorporate the screen!
  - try playing a waveform underneath ? 
*/


AudioPlaySdWav           playWav1;
AudioOutputI2S           i2s1;

AudioConnection          patchCord1(playWav1, 0, i2s1, 0);
AudioConnection          patchCord2(playWav1, 1, i2s1, 1);

#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

int sample_number = 0;

unsigned long cM = 0, cM2 = 0;
unsigned long pM = 0, pM2 = 0;
unsigned long interval = 10;


Bounce s0 = Bounce(0, 15);
Bounce s1 = Bounce(1, 15);
Bounce s2 = Bounce(2, 15);
Bounce s3 = Bounce(3, 15);
Bounce s4 = Bounce(4, 15);

void setup() {
  pinMode(0, INPUT_PULLUP);
    pinMode(1, INPUT_PULLUP);
      pinMode(2, INPUT_PULLUP);
        pinMode(3, INPUT_PULLUP);
          pinMode(4, INPUT_PULLUP);

  AudioMemory(8);

  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.println("Waiting for Arduino Serial Monitor...");

  Serial.begin(9600);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);

  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  tft.setRotation(3);
  screen_main();
}

void playFile(const char *filename) {
  cM = millis();
  Serial.println(cM);
  Serial.print("Playing file: ");
  Serial.println(filename);
  /*if((currentMillis - previousMillis) > interval){
    update_screen();
    previousMillis = currentMillis;
  }*/
  
  //update_screen();

  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(25);

  /*if(!playWav1.isPlaying()){
    screen_main();
  }*/
  //while(playWav1.isPlaying()){
    //if((playWav1.lengthMillis() - playWav1.positionMillis()) >= 20) {
      //screen_main();
      //Serial.println(currentMillis);
    //}
  //}

  //while(playWav1.isPlaying()){
    if((cM - pM) >= interval){
      //pM = cM;
      if((playWav1.lengthMillis() - playWav1.positionMillis()) <= 40){
        Serial.println("screenmain");
        screen_main();
      } else {
        update_screen();
      }
      pM = cM;
    }

  //} 
}


void loop() {
  s0.update();
  s1.update();
  s2.update();
  s3.update();
  s4.update();
  cM2 = millis();

  //screenText();

  if(s0.fallingEdge()){
    playFile("BELLALUNA.WAV");  // filenames are always uppercase 8.3 format
    sample_number = 0;
  }
  if(s1.fallingEdge()){
    playFile("KUNGFU.WAV");  // filenames are always uppercase 8.3 format
    sample_number = 1;
  }
  if(s2.fallingEdge()){
    playFile("SHOOTME.WAV");  // filenames are always uppercase 8.3 format
    sample_number = 2;
  }
  if(s3.fallingEdge()){
    playFile("DEVILWEARS.WAV");  // filenames are always uppercase 8.3 format
    sample_number = 3;
  }
  if(s4.fallingEdge()){
    playFile("VICHU.WAV");  // filenames are always uppercase 8.3 format
    sample_number = 4; 
  }

  cM2 = millis();
  
  if((cM2 - pM2) > 50){
    if(!playWav1.isPlaying() ){
      screen_main();
    }
    pM2 = cM2;
  }

  
  
}

unsigned long screen_main() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();

  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.println("MOSS' SAMPLER");

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("   1. Bella Luna");
  tft.println("   2. Kung fu");
  tft.println("   3. Shoot me");
  tft.println("   4. Devil wears");
  tft.println("   5. Vic Hu");
  return micros() - start;
}

unsigned long update_screen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.println("MOSS' SAMPLER");

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
      
  unsigned long start = micros();
  switch(sample_number) {
    case 0: 
      tft.setTextColor(ILI9341_BLUE, ILI9341_RED);
      tft.println("   1. Bella Luna");
      tft.setTextColor(WHITE, BLACK);
      tft.println("   2. Kung fu");
      tft.println("   3. Shoot me");
      tft.println("   4. Devil wears");
      tft.println("   5. Vic Hu");
      break;
    case 1: 
      tft.println("   1. Bella Luna");
      tft.setTextColor(ILI9341_BLUE, ILI9341_RED);
      tft.println("   2. Kung fu");
      tft.setTextColor(WHITE, BLACK);
      tft.println("   3. Shoot me");
      tft.println("   4. Devil wears");
      tft.println("   5. Vic Hu");
      break;
    case 2:
      tft.setCursor(0, 0);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(3);
      tft.println("MOSS' SAMPLER");
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(2);
      tft.println("   1. Bella Luna");
      tft.println("   2. Kung fu");
      tft.setTextColor(ILI9341_BLUE, ILI9341_RED);
      tft.println("   3. Shoot me");
      tft.setTextColor(WHITE, BLACK);
      tft.println("   4. Devil wears");
      tft.println("   5. Vic Hu");
      break;
    case 3:
      tft.println("   1. Bella Luna");
      tft.println("   2. Kung fu");
      tft.println("   3. Shoot me");
      tft.setTextColor(ILI9341_BLUE, ILI9341_RED);
      tft.println("   4. Devil wears");
      tft.setTextColor(WHITE, BLACK);
      tft.println("   5. Vic Hu");
      break;
    case 4:
      tft.println("   1. Bella Luna");
      tft.println("   2. Kung fu");
      tft.println("   3. Shoot me");
      tft.println("   4. Devil wears");
      tft.setTextColor(ILI9341_BLUE, ILI9341_RED);
      tft.println("   5. Vic Hu");
      tft.setTextColor(WHITE, BLACK);
      break;
  
  }
  return micros() - start;

}

