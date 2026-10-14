//==================requires testing ===========================//

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthNoiseWhite     noise1;         //xy=68.99999618530273,331.49999618530273
AudioSynthWaveform       osc1;      //xy=85.19999694824219,168.1999969482422
AudioSynthWaveform       osc2;      //xy=100.19999694824219,253.19999313354492
AudioSynthWaveformDc     filterDC;            //xy=124.19999694824219,444.1999969482422
AudioMixer4              mixer;         //xy=331.50001335144043,286.50000953674316
AudioEffectEnvelope      filterEnv;      //xy=353.1999969482422,452.1999969482422
AudioFilterStateVariable filter;        //xy=550.2500762939453,372.75001525878906
AudioEffectEnvelope      ampEnv;      //xy=762.9500389099121,362.950026512146
AudioOutputI2S           i2s1;           //xy=957.5,367.74999618530273
AudioConnection          patchCord1(noise1, 0, mixer, 2);
AudioConnection          patchCord2(osc1, 0, mixer, 0);
AudioConnection          patchCord3(osc2, 0, mixer, 1);
AudioConnection          patchCord4(filterDC, filterEnv);
AudioConnection          patchCord5(mixer, 0, filter, 0);
AudioConnection          patchCord6(filterEnv, 0, filter, 1);
AudioConnection          patchCord7(filter, 0, ampEnv, 0);
AudioConnection          patchCord8(ampEnv, 0, i2s1, 0);
AudioConnection          patchCord9(ampEnv, 0, i2s1, 1);
// GUItool: end automatically generated code


#define CUTOFF_KNOB A0
#define FREQ_KNOB A1
//#define NOISE_KNOB

float last_cutoff = -1.0f;

bool new_freq_capture;

float osc1_freq;
float osc2_freq;

float osc1_new_freq;
float osc2_new_freq;

int main_midi;
int sub_midi;

float cutoff_freq = 830.61;


IntervalTimer noteTimer;
int ampEnvTime = 1600;

//can use myTimer.update() when



void readKnobs(){
  //include timer function in this somehow
  int rawFreq = analogRead(FREQ_KNOB);
  int rawCutoff = analogRead(CUTOFF_KNOB);

  //map freq + cutoff to temp var
  osc1_new_freq = map(rawFreq, 0, 1023, 130.0f, 2093.0f); //C3 - C7
  cutoff_freq = expf((float)rawCutoff / 150.0) * 10.0 + 80.0;

}

float midiToFreq(float note) {
    return 440.0f * powf(2.0f, (note - 69.0f) / 12.0f);
}

float freqToMidi(float freq) {
    return 69.0f + 12.0f * (logf(freq / 440.0f) / logf(2.0f));
}

void glideFreq(){
  float glide_param = 5.0f;
//increment or dec freq until target reach
	float diff = osc1_new_freq - osc1_freq; 
	if(abs(diff) <= 10.0f) {
		osc1_freq = osc1_new_freq;
		new_freq_capture = true; 
	} else {
		osc1_freq += (diff > 0) ? glide_param : (0 - glide_param); //make this parameter adjustable
	}
	osc1.frequency(osc1_freq);
	osc2.frequency(midiToFreq(freqToMidi(osc1_freq) - 12.0f));
}


void noteTrigger(){
	if(filterEnv.isActive()){
		filterEnv.noteOff();
		ampEnv.noteOff();	//try without this see if anything fun happens
  } else {
    filterEnv.noteOn();
    ampEnv.noteOn();
  }
}

void setup(){
  Serial.begin(9600);

  AudioMemory(15);

  //mixer
  mixer.gain(0, 0.3);
  mixer.gain(1, 0.3);
  mixer.gain(2, 0.1);

  main_midi = 60;
  sub_midi = main_midi - 12;
  osc1_freq = midiToFreq(main_midi);
  osc2_freq = midiToFreq(sub_midi);

  osc1.frequency(osc1_freq);
  osc1.amplitude(0.3);
  osc1.begin(WAVEFORM_SINE);

  osc2.frequency(osc2_freq);
  osc2.amplitude(0.3);
  osc2.begin(WAVEFORM_SINE);

  noise1.amplitude(0.1);

  ampEnv.attack(500);
  ampEnv.decay(300);
  ampEnv.sustain(0.4);
  ampEnv.release(700);


  filterDC.amplitude(0.1);

  filter.frequency(cutoff_freq);

  filterEnv.attack(800);
  filterEnv.decay(300);
  filterEnv.sustain(0.4);
  filterEnv.release(500);

	noteTimer.begin(noteTrigger, ampEnvTime * 1000); 
}



void loop(){
  readKnobs();
  if(abs(osc1_freq - osc1_new_freq) > 20.0f) {
    new_freq_capture = false;
    glideFreq();
  }

  if (abs(cutoff_freq - last_cutoff) > 2.0f) {
    filter.frequency(cutoff_freq);
    last_cutoff = cutoff_freq;
  }



}
