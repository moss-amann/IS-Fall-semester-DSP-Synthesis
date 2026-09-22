//Independent Study Week 3: Envelopes
    // Project 1 
    // Using envelopes and noise to try to make drums sound more real

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioSynthSimpleDrum     snare;          //xy=104.69999694824219,133.20000457763672
AudioSynthSimpleDrum     kick;          //xy=109.20000076293945,79.20000171661377
AudioSynthSimpleDrum     closed_hh;          //xy=109.11111450195312,335.7778129577637
AudioSynthNoiseWhite     s_noise;         //xy=110.2222188313802,189.1111077202691
AudioSynthNoiseWhite     c_noise;         //xy=110.22221755981445,382.4444408416748
AudioSynthSimpleDrum     tom;          //xy=115.52777099609375,242.44446182250977
AudioSynthNoiseWhite     o_noise;         //xy=114.66666327582465,521.3333299424913
AudioSynthSimpleDrum     open_hh;          //xy=120.2222188313802,450.2222188313802
AudioEffectEnvelope      kickEnv;      //xy=292.950008392334,86.45000267028809
AudioEffectEnvelope      snareEnv_noise;      //xy=299.6999931335449,208.4499912261963
AudioEffectEnvelope      snareEnv_amp;      //xy=312.1999969482422,142.1999969482422
AudioEffectEnvelope      tomEnv;      //xy=322.9500274658203,261.700008392334
AudioEffectEnvelope      openEnv_noise;      //xy=329.0000114440918,526.499988541007
AudioEffectEnvelope      openEnv_amp;      //xy=336.49999618530273,470.24999618530273
AudioEffectEnvelope      closeEnv_noise;      //xy=348.99999618530273,407.74999618530273
AudioEffectEnvelope      closeEnv_amp;      //xy=361.50001525878906,351.50001525878906
AudioMixer4              mixer1;         //xy=578.8889999389648,161.44446182250977
AudioMixer4              mixer2;         //xy=617.7499961853027,383.99999618530273
AudioMixer4              mixer3;         //xy=863.9999694824219,257.74999618530273
AudioOutputI2S           i2s1;           //xy=1103.9999961853027,251.49999618530273
AudioConnection          patchCord1(snare, snareEnv_amp);
AudioConnection          patchCord2(kick, kickEnv);
AudioConnection          patchCord3(closed_hh, closeEnv_amp);
AudioConnection          patchCord4(s_noise, snareEnv_noise);
AudioConnection          patchCord5(c_noise, closeEnv_noise);
AudioConnection          patchCord6(tom, tomEnv);
AudioConnection          patchCord7(o_noise, openEnv_noise);
AudioConnection          patchCord8(open_hh, openEnv_amp);
AudioConnection          patchCord9(kickEnv, 0, mixer1, 0);
AudioConnection          patchCord10(snareEnv_noise, 0, mixer1, 2);
AudioConnection          patchCord11(snareEnv_amp, 0, mixer1, 1);
AudioConnection          patchCord12(tomEnv, 0, mixer1, 3);
AudioConnection          patchCord13(openEnv_noise, 0, mixer2, 3);
AudioConnection          patchCord14(openEnv_amp, 0, mixer2, 2);
AudioConnection          patchCord15(closeEnv_noise, 0, mixer2, 1);
AudioConnection          patchCord16(closeEnv_amp, 0, mixer2, 0);
AudioConnection          patchCord17(mixer1, 0, mixer3, 0);
AudioConnection          patchCord18(mixer2, 0, mixer3, 1);
AudioConnection          patchCord19(mixer3, 0, i2s1, 0);
AudioConnection          patchCord20(mixer3, 0, i2s1, 1);
// GUItool: end automatically generated code


Bounce b1 = Bounce(0, 15);
Bounce b2 = Bounce(1, 15);
Bounce b3 = Bounce(3, 15);
Bounce b4 = Bounce(4, 15);
Bounce bState = Bounce(5, 15);

#define TEMPO_KNOB A0


typedef enum {
  PLAY,
  SET1,
  SET2 
} state;

state present = PLAY;

typedef enum {
  KICK, 
  SNARE, 
  CLOSE_HH, 
  OPEN_HH, 
  REST
} drumType;

//                         0     1     2     3     4     5     6     7
drumType drumSequence[] = {KICK, KICK, KICK, KICK, KICK, KICK, KICK, KICK};
int note_index = 0; 

void printDrum(drumType drum){
  switch(drum){
    case KICK: 
      Serial.println("Kick");
      break;
    case SNARE: 
      Serial.println("Snare");
      break;
    case CLOSE_HH: 
      Serial.println("Closed hi hat");
      break;
    case OPEN_HH: 
      Serial.println("Open hi hat");
      break;
    case REST: 
      Serial.println("Rest");
      break;
  }
}

void playDrum(drumType drum){ //not entierly positive this is how you pass an enum
  switch(drum){
    case KICK:
      kick.noteOn();
      kickEnv.noteOn();
      break;
    case SNARE: 
      snare.noteOn();
      snareEnv_amp.noteOn();
      snareEnv_noise.noteOn();
      break;
    case CLOSE_HH:
      closed_hh.noteOn();
      closeEnv_amp.noteOn();
      closeEnv_noise.noteOn();
      break;
    case OPEN_HH:
      open_hh.noteOn();
      openEnv_amp.noteOn();
      openEnv_noise.noteOn();
      break;
    case REST: 
      break;
  }
}



unsigned long beat_time = 500;
unsigned long previousMillis = 0;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  
  AudioMemory(15);

  //mixer1 gain
  mixer1.gain(0, 0.40);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.15);
  mixer1.gain(3, 0.20);

  //mixer2 gain
  mixer2.gain(0, 0.25);
  mixer2.gain(1, 0.25);
  mixer2.gain(2, 0.25);
  mixer2.gain(3, 0.25);

  //mixer3 gain
  mixer3.gain(0, 0.25);
  mixer3.gain(1, 0.25);

  //noise amplitudes
  s_noise.amplitude(0.4);
  c_noise.amplitude(0.2);
  o_noise.amplitude(0.5);

  //KICK
  kick.frequency(50.0);
  kick.length(300);
  kick.pitchMod(0.6);

  kickEnv.attack(0);
  kickEnv.decay(100);
  kickEnv.sustain(0);

  //SNARE
  snare.frequency(200.0);
  snare.length(178);
  snare.pitchMod(0.35);

  snareEnv_amp.attack(0);
  snareEnv_amp.decay(160);
  snareEnv_amp.sustain(0);

  snareEnv_noise.attack(0);
  snareEnv_noise.decay(150);
  snareEnv_noise.sustain(0);
  
  //TOM - add in later
  //CLOSED HH

  closed_hh.frequency(400.0);
  closed_hh.length(40);
  closed_hh.pitchMod(0.0);

  closeEnv_amp.attack(15);
  closeEnv_amp.decay(150);
  closeEnv_amp.sustain(0);

  closeEnv_noise.attack(20);
  closeEnv_noise.decay(100);
  closeEnv_noise.sustain(0);

  //OPEN HH
  open_hh.frequency(1600.0);
  open_hh.length(700);
  open_hh.pitchMod(0.5);

  openEnv_amp.attack(15);
  openEnv_amp.decay(700);
  openEnv_amp.sustain(0);
  
  openEnv_noise.attack(30.5);
  openEnv_noise.decay(600);
  openEnv_noise.sustain(0);



}

float tempo;

void loop() {
  // put your main code here, to run repeatedly:
  b1.update();
  b2.update();
  b3.update();
  b4.update();
  bState.update();
  tempo = map(analogRead(TEMPO_KNOB), 0, 1023, 80, 700);
  unsigned long currentMillis = millis();

  //check state update
  if(bState.fallingEdge()){
    present = (state)((present + 1) % 3);

    if (present == PLAY) {
      previousMillis = millis(); 
      note_index = 0; 
      Serial.println("State: PLAYING");
    } else {
      Serial.println("State: SETTING");
    }
  }

  //do according to state
  switch(present) {
    case PLAY: 
      if(currentMillis - previousMillis >= beat_time){
        previousMillis = currentMillis;
        playDrum(drumSequence[note_index]); 
        note_index = (note_index + 1) % 8;
      }
      break;
    case SET1: 
      AudioNoInterrupts();
      if(b1.fallingEdge()){
        drumSequence[0] = (drumType)((drumSequence[0] + 1) % 5);
        Serial.print("Beat 1: ");
        //Serial.printf("Current: %s\n", drumStrings[0]);
        printDrum(drumSequence[0]);        

      }
      if(b2.fallingEdge()){
        //drumSequence[1] = drumSequence[1] + 1; //no idea if this works how i think it will
        drumSequence[1] = (drumType)((drumSequence[1] + 1) % 5);        
        Serial.print("Beat 2: ");
        //Serial.printf("Current: %s\n", drumStrings[1]);
        printDrum(drumSequence[1]);
      }
      if(b3.fallingEdge()){
        //drumSequence[2] = drumSequence[2] + 1; //no idea if this works how i think it will
        drumSequence[2] = (drumType)((drumSequence[2] + 1) % 5);        
        Serial.print("Beat 3: ");
        //Serial.printf("Current: %s\n", drumStrings[2]);
        printDrum(drumSequence[2]);
      }
      if(b4.fallingEdge()){
        //drumSequence[3] = drumSequence[3] + 1; //no idea if this works how i think it will
        drumSequence[3] = (drumType)((drumSequence[3] + 1) % 5);        
        Serial.print("Beat 4: ");
        //Serial.printf("Current: %s\n", drumStrings[3]);
        printDrum(drumSequence[3]);
      }
      AudioInterrupts();
      break;
    case SET2:
      AudioNoInterrupts();
      if(b1.fallingEdge()){
        drumSequence[4] = (drumType)((drumSequence[4] + 1) % 5);        
        Serial.print("Beat 5: ");
        //Serial.printf("Current: %s\n", drumStrings[0]);
        printDrum(drumSequence[4]);
      }
      if(b2.fallingEdge()){
        //drumSequence[1] = drumSequence[1] + 1; //no idea if this works how i think it will
        drumSequence[5] = (drumType)((drumSequence[5] + 1) % 5);        
        Serial.print("Beat 6: ");
        //Serial.printf("Current: %s\n", drumStrings[1]);
        printDrum(drumSequence[5]);
      }
      if(b3.fallingEdge()){
        //drumSequence[2] = drumSequence[2] + 1; //no idea if this works how i think it will
        drumSequence[6] = (drumType)((drumSequence[6] + 1) % 5);        
        Serial.print("Beat 7: ");
        //Serial.printf("Current: %s\n", drumStrings[2]);
        printDrum(drumSequence[6]);
      }
      if(b4.fallingEdge()){
        //drumSequence[3] = drumSequence[3] + 1; //no idea if this works how i think it will
        drumSequence[7] = (drumType)((drumSequence[7] + 1) % 5);        
        Serial.print("Beat 8: ");
        //Serial.printf("Current: %s\n", drumStrings[3]);
        printDrum(drumSequence[7]);
      }
      AudioInterrupts();
      break;

  }

  AudioNoInterrupts();
  beat_time = tempo;
  AudioInterrupts();


}
