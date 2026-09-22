////Independent Study Week 2: Oscillators
    // Bonus Projecyt
    // I wanted to mess around with the drum objects in the Teensy Audio Library

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioSynthKarplusStrong  string1;        //xy=277.1999969482422,525
AudioSynthSimpleDrum     drum2;          //xy=283.1999969482422,368
AudioSynthSimpleDrum     drum3;          //xy=283.1999969482422,427
AudioSynthSimpleDrum     drum1;          //xy=290,305
AudioMixer4              mixer1;         //xy=516,351
AudioOutputI2S           i2s1;           //xy=693,347
AudioConnection          patchCord1(string1, 0, mixer1, 3);
AudioConnection          patchCord2(drum2, 0, mixer1, 1);
AudioConnection          patchCord3(drum3, 0, mixer1, 2);
AudioConnection          patchCord4(drum1, 0, mixer1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 1);
// GUItool: end automatically generated code
// GUItool: end automatically generated code
// GUItool: end automatically generated code

Bounce d1 = Bounce(0, 15);
Bounce d2 = Bounce(1, 15);
Bounce d3 = Bounce(3, 15);

void setup() {
  // put your setup code here, to run once:
  pinMode(0, INPUT_PULLUP); //d3 button
  pinMode(1, INPUT_PULLUP); //d2 but
  pinMode(3, INPUT_PULLUP);
  //later have a duration pot
  AudioMemory(15);

  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.15);

  drum1.frequency(60);
  drum1.length(1500);
  drum1.secondMix(0.0);
  drum1.pitchMod(0.55);

  drum2.frequency(250);
  drum2.length(300);
  drum2.secondMix(0.0);
  drum2.pitchMod(0.55);

  drum3.frequency(400);
  drum3.length(100);
  drum3.secondMix(0.0);
  drum3.pitchMod(0.55);


}

const unsigned long TIME_INTERVAL = 450; // 1 second
unsigned long previousMillis = 0;
int current_note = 0;

const float stringNotes[4] = {
  130.81, //  C3
  146.83, //  D3
  123.47, //  F3
  196.00  //  G3
};

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= TIME_INTERVAL){
    previousMillis = currentMillis;
    string1.noteOn(stringNotes[current_note], 0.6);
    current_note = (current_note + 1) % 4;
  }
  d1.update();
  d2.update();
  d3.update();  

  if(d1.fallingEdge()){
    drum1.noteOn();
  }
  if(d2.fallingEdge()){
    drum2.noteOn();
  }
  if(d3.fallingEdge()){
    drum3.noteOn();
  }
  
}
