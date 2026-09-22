//Independent Study Week 2: Oscillators
    // Project 2
    // Experimenting with noise, mixing, amplitude and frequency

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=274.1999969482422,315
AudioSynthWaveform       waveform2;      //xy=283.1999969482422,376
AudioSynthNoisePink      pink1;          //xy=307.1999969482422,447
AudioMixer4              mixer1;         //xy=553.1999969482422,327
AudioOutputI2S           i2s1;           //xy=810.2000389099121,311.
AudioConnection          patchCord1(waveform1, 0, mixer1, 0);
AudioConnection          patchCord2(waveform2, 0, mixer1, 1);
AudioConnection          patchCord3(pink1, 0, mixer1, 2);
AudioConnection          patchCord4(mixer1, 0, i2s1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 1);
// GUItool: end automatically generated code

const float noteFrequencies[12] = {
  // Octave 4 (C4 to B4)
  261.63, // 0: C4
  277.18, // 1: C#4 / Db4
  293.66, // 2: D4
  311.13, // 3: D#4 / Eb4
  329.63, // 4: E4
  349.23, // 5: F4
  369.99, // 6: F#4 / Gb4
  392.00, // 7: G4
  415.30, // 8: G#4 / Ab4
  440.00, // 9: A4
  466.16, // 10: A#4 / Bb4
  493.88, // 11: B4
};

int currentNoteIndex = 0;

//mute button
Bounce button0 = Bounce(0, 15); //up
Bounce button1 = Bounce(1, 15); //down
//bool mute = 0; 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

  AudioMemory(10);

  waveform1.frequency(220);
  waveform2.frequency(440);
  waveform1.amplitude(0.3);
  waveform2.amplitude(0.3);
  pink1.amplitude(0.05);

  waveform1.begin(WAVEFORM_SINE);
  waveform2.begin(WAVEFORM_SAWTOOTH);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  button0.update();
  button1.update();

  //will not return a value over 0.682
  float w2_a = (float)analogRead(A2) / 1500.0; 
  //will not return a value over 0.51
  float p_a = (float)analogRead(A3) / 2000.0;
  float amp_sum = w2_a + p_a;

  //AudioNoInterupts();
  if(amp_sum > 0.7) {
    Serial.println("EXCEEDS AMPLITUDE");
  } else {
    AudioNoInterrupts();
    waveform2.amplitude(w2_a);
    pink1.amplitude(p_a);
    AudioInterrupts();
  }
  
  if(button0.fallingEdge()){
    if(currentNoteIndex < 11){
      AudioNoInterrupts();
      currentNoteIndex += 1; 
      waveform1.frequency(noteFrequencies[currentNoteIndex]);
      AudioInterrupts();
    }
  }

  if(button1.fallingEdge()){
    if(currentNoteIndex >= 0) {
      AudioNoInterrupts();
      currentNoteIndex -= 1;
      waveform1.frequency(noteFrequencies[currentNoteIndex]);
      AudioInterrupts();
    }
  }
}


