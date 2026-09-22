//Independent Study Week 3: Envelopes
    // Project 2 
    // Knobs control attack, decay, relase times and sustain level. A random note is generated based on 
    // some probability to make it more interesting

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=405.2935371398926,488.53424072265625
AudioEffectEnvelope      envelope1;      //xy=701.2829971313477,475.4015598297119
AudioMixer4              mixer1;         //xy=957.8745040893555,498.63625144958496
AudioOutputI2S           i2s1;           //xy=1143.5005073547363,402.2045283317566
AudioConnection          patchCord1(waveform1, envelope1);
AudioConnection          patchCord2(envelope1, 0, mixer1, 0);
AudioConnection          patchCord3(mixer1, 0, i2s1, 0);
AudioConnection          patchCord4(mixer1, 0, i2s1, 1);
// GUItool: end automatically generated code

#define ATTACK_KNOB A0
#define DECAY_KNOB A1
#define SUSTAIN_KNOB A2
#define RELEASE_KNOB A3

float midiToFreq(int midi){
  return 440 * pow(2, (midi - 69) / 12.0);
}

/*INTERVALS
  0th, 5th, 6th, 10th 
  50%  25%  15%  10%

  base note 

*/

int updateInterval(int interval){ //0 - 99
  if(interval < 50){   //0 - 49
    //base note
    return 48;
  } else if (interval < 75) {//50 - 74
    //5th 
    return 52;
  } else if (interval < 85) { //75 - 89
    //6th
    return 53;
  } else if (interval < 93) {
    //10th
    return 57;
  } else {
    return 60;
  }
}

const int base_note = 48;
int midi = 48;

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(A14));
  Serial.begin(9600);
  pinMode(0, INPUT_PULLUP); 
  
  AudioMemory(15); //not quite sure what this does

  mixer1.gain(0, 1); 
  
  waveform1.frequency(440.0f);
  
  waveform1.amplitude(0.3);
 
  waveform1.begin(WAVEFORM_SINE);
 
  envelope1.attack(0);
  envelope1.decay(0);
  envelope1.sustain(0.0);
  envelope1.release(0);

}

long time_period = 0;
long sustain_ms = 30;
unsigned long last_millis = 0;

int attack;
int decay;
float sustain;
int release;

bool half_note = false; 

void loop() {
  // put your main code here, to run repeatedly:
  int int_random = random(100);
  midi = updateInterval(int_random); 

  if(int_random < 70) {
    half_note = false;
  } else {
    half_note = true;
  }


  attack = map(analogRead(ATTACK_KNOB), 0, 1023, 11, 2000);
  decay = map(analogRead(DECAY_KNOB), 0, 1023, 35, 2000);
  sustain = map(analogRead(SUSTAIN_KNOB), 0, 1023, 0.0, 1.0);
  release = map(analogRead(RELEASE_KNOB), 0, 1023, 300, 4000);

  AudioNoInterrupts();
  envelope1.attack(attack);
  envelope1.decay(decay);
  envelope1.sustain(sustain);
  envelope1.release(release);
  AudioInterrupts();

  time_period = attack + decay + sustain_ms + release; 

  unsigned long currentMillis = millis();

  if(currentMillis - last_millis >= time_period) {
    last_millis = currentMillis;
    AudioNoInterrupts();
    waveform1.frequency(midiToFreq(midi));
    AudioInterrupts();
    envelope1.noteOn();
  }

  


}
