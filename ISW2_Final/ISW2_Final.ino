//Independent Study Week 2: Oscillators
    // Project 3 (Final)
    // Uses potentiometers, buttons and a joystick to control 3 waveforms. Specifically their frequency, 
    // amplitude, phase, waveform. When the frequency of waveform 1 is adjusted, waveform 2 and 3 are 
    // adjusted accordingly 

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=274.1999969482422,315
AudioSynthWaveform       waveform2;      //xy=283.1999969482422,376
AudioSynthWaveform       waveform3;      //xy=283.1999969482422,459
AudioMixer4              mixer1;         //xy=553.1999969482422,327
AudioOutputI2S           i2s1;           //xy=810.2000389099121,311.0000057220459
AudioConnection          patchCord1(waveform1, 0, mixer1, 0);
AudioConnection          patchCord2(waveform2, 0, mixer1, 1);
AudioConnection          patchCord3(waveform3, 0, mixer1, 2);
AudioConnection          patchCord4(mixer1, 0, i2s1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 1);
// GUItool: end automatically generated code


//-----------------WAVEFORM STRUCT TEMPLATE-------//
struct oscillator {
  int pin_num;
  int current_waveform;
  float amplitude;
};

/*
typedef enum {
  SINE, TRIANGLE, SAWTOOTH, SQUARE
} wavetype;
   */
int wavetypes[] = {
  WAVEFORM_SINE, WAVEFORM_TRIANGLE, WAVEFORM_SAWTOOTH, WAVEFORM_SQUARE
};


struct oscillator osc_1 = {1, 0, 0.35 };
struct oscillator osc_2 = {3, 0, 0.25};


//--------BUTTONS------------
  Bounce mute = Bounce(0, 15);
  Bounce wave1 = Bounce(1, 15);
  Bounce wave2 = Bounce(3, 15);
  //Bounce osc2Interval = 


/*------------KNOB DECLARATIONS----------- */
#define FREQ_KNOB A2
#define PHASE_X_PIN A0
#define VOL_OFFSET_Y_PIN A1

float midiToFreq(int midi){
  return 440 * pow(2, (midi - 69) / 12.0);
}

int freqToMidi(float freq){
  return (int) ((12 * log(freq / 220.0) / log(2.0) ) + 57.01);
}


int phase_wave_2;
int whosturn = 0;
int osc1_midi; 
bool muted = false;


/*  --------READ JOYSTICK  ---------------*/
void analogReadJoystick(){
  phase_wave_2 = (analogRead(PHASE_X_PIN) - 512) * (150.0 / 512.0);
  if(phase_wave_2 < 0){
    phase_wave_2 +=360;
  }
  float vol_offset_2 = (analogRead(VOL_OFFSET_Y_PIN) - 512) * (0.2 / 512.0);
  //osc_2.amplitude += vol_offset_2;
  osc_2.amplitude = constrain(0.2 + vol_offset_2, 0.0, 1.0); //ai suggest
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(0, INPUT_PULLUP); //MUTE
  pinMode(1, INPUT_PULLUP); //osc1
  pinMode(3, INPUT_PULLUP); //osc2

  AudioMemory(15); //not quite sure what this does

  mixer1.gain(0, 0.33); 
  mixer1.gain(1, 0.33);
  mixer1.gain(2, 0.33);

  waveform1.frequency(midiToFreq(72));
  waveform2.frequency(midiToFreq(76));
  waveform3.frequency(midiToFreq(48));

  waveform1.amplitude(osc_1.amplitude);
  waveform2.amplitude(osc_2.amplitude);
  waveform3.amplitude(0.10);

  waveform1.begin(wavetypes[osc_1.current_waveform]);
  waveform2.begin(wavetypes[osc_2.current_waveform]);
  waveform3.begin(WAVEFORM_SAWTOOTH);
    

}



void loop() {
  
  //UPDATE BUTTONS
  mute.update();
  wave1.update();
  wave2.update();

  //MUTE CHUNK
  if(mute.fallingEdge()) {
    muted = !muted;
    
    AudioNoInterrupts();
    if(muted) {
      waveform1.amplitude(0.00);
      waveform2.amplitude(0.00);
      waveform3.amplitude(0.00);
    }
    else {
      waveform1.amplitude(osc_1.amplitude);
      waveform2.amplitude(osc_2.amplitude);
      waveform3.amplitude(0.10);
    }
    AudioInterrupts();
  }

  //CHECK WAVES
  if(wave1.fallingEdge()) {
    osc_1.current_waveform = (osc_1.current_waveform + 1) % 4;
    AudioNoInterrupts();
    waveform1.begin(wavetypes[osc_1.current_waveform]);
    AudioInterrupts();
  }

  if(wave2.fallingEdge()) {
    osc_2.current_waveform = (osc_2.current_waveform + 1) % 4;
    AudioNoInterrupts();
    waveform2.begin(wavetypes[osc_2.current_waveform]);
    AudioInterrupts();
  }

  switch(whosturn){
    case 0: //FREQUENCY
      osc1_midi = map(analogRead(FREQ_KNOB), 0, 1023, 24, 80);
      AudioNoInterrupts();
      waveform1.frequency(midiToFreq(osc1_midi));
      waveform2.frequency(midiToFreq(osc1_midi + 5));
      waveform3.frequency(midiToFreq(osc1_midi - 12));
      AudioInterrupts();
      break;
    case 1: 
      AudioNoInterrupts();
      analogReadJoystick();
      if (muted == true) {
        waveform2.amplitude(0.00);
      } else {      
        waveform2.amplitude(osc_2.amplitude);
      }
      waveform2.phase(phase_wave_2);
    
      AudioInterrupts();
      break;
    case 2: //MIX 1, ignore for now
      break;
    case 3: //MIX 2, ignore for now
      break;
  }
  whosturn = (whosturn + 1) % 4;

}
