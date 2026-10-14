/*
  THIS IS GUI TOOL WITHOUT AMPLITUDE ENVELOPE FOR SOME REASON
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       osc2;      //xy=73.94999694824219,218.19999313354492
AudioSynthWaveform       osc3;      //xy=84,299.0000145435333
AudioSynthWaveform       osc1;      //xy=85.19999694824219,168.1999969482422
AudioSynthWaveformDc     filterDC;            //xy=88.20000457763672,449.2000045776367
AudioSynthNoiseWhite     noise;         //xy=91.49999618530273,356.49999618530273
AudioEffectEnvelope      filterEnv;      //xy=268.20000076293945,451.2000036239624
AudioMixer4              osc_mix;         //xy=271.4999771118164,276.49997329711914
AudioFilterStateVariable filter;        //xy=418.2000045776367,365.2000026702881
AudioMixer4              filter_mix;         //xy=678.2000370025635,291.20000076293945
AudioOutputI2S           i2s1;           //xy=982.1999893188477,291.20000171661377
AudioConnection          patchCord1(osc2, 0, osc_mix, 1);
AudioConnection          patchCord2(osc3, 0, osc_mix, 2);
AudioConnection          patchCord3(osc1, 0, osc_mix, 0);
AudioConnection          patchCord4(filterDC, filterEnv);
AudioConnection          patchCord5(noise, 0, osc_mix, 3);
AudioConnection          patchCord6(filterEnv, 0, filter, 1);
AudioConnection          patchCord7(osc_mix, 0, filter, 0);
AudioConnection          patchCord8(filter, 0, filter_mix, 0);
AudioConnection          patchCord9(filter, 1, filter_mix, 1);
AudioConnection          patchCord10(filter, 2, filter_mix, 2);
AudioConnection          patchCord11(filter_mix, 0, i2s1, 0);
AudioConnection          patchCord12(filter_mix, 0, i2s1, 1);
// GUItool: end automatically generated code
*/


/*******THIS IS GUI WITH */
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       osc2;           //xy=393.1999816894531,348.99998474121094
AudioSynthWaveform       osc3;           //xy=404.1999816894531,429.99998474121094
AudioSynthWaveform       osc1;           //xy=405.1999816894531,298.99998474121094
AudioSynthWaveformDc     filterDC;       //xy=408.1999816894531,579.9999847412109
AudioSynthNoiseWhite     noise;          //xy=411.1999816894531,486.99998474121094
AudioEffectEnvelope      filterEnv;      //xy=588.1999816894531,581.9999847412109
AudioMixer4              osc_mix;        //xy=591.1999816894531,406.99998474121094
AudioFilterStateVariable filter;         //xy=738.1999816894531,495.99998474121094
AudioMixer4              filter_mix;     //xy=998.1999816894531,421.99998474121094
AudioEffectEnvelope      ampEnv;      //xy=1145.3435067912103,414.96182507231066
AudioOutputI2S           i2s2; //xy=1302.1999816894531,421.99998474121094
AudioConnection          patchCord1(osc2, 0, osc_mix, 1);
AudioConnection          patchCord2(osc3, 0, osc_mix, 2);
AudioConnection          patchCord3(osc1, 0, osc_mix, 0);
AudioConnection          patchCord4(filterDC, filterEnv);
AudioConnection          patchCord5(noise, 0, osc_mix, 3);
AudioConnection          patchCord6(filterEnv, 0, filter, 1);
AudioConnection          patchCord7(osc_mix, 0, filter, 0);
AudioConnection          patchCord8(filter, 0, filter_mix, 0);
AudioConnection          patchCord9(filter, 1, filter_mix, 1);
AudioConnection          patchCord10(filter, 2, filter_mix, 2);
AudioConnection          patchCord11(filter_mix, ampEnv);
AudioConnection          patchCord12(ampEnv, 0, i2s2, 0);
AudioConnection          patchCord13(ampEnv, 0, i2s2, 1);
// GUItool: end automatically generated code


//==================variables=====================//
/*
#define FREQ_KNOB A0
#define ATTACK_KNOB A1
#define DECAY_KNOB A2
#define RELEASE_KNOB A3
#define CUTOFF_KNOB A4
*/

#define FREQ_KNOB A13
#define ATTACK_KNOB A11
#define DECAY_KNOB A10
#define RELEASE_KNOB A0
#define CUTOFF_KNOB A14


#define ENV_PIN 0
#define WAVE_PIN 1
#define MUTE_PIN 2
#define MIX_PIN 3

Bounce enve = Bounce(ENV_PIN, 15);
Bounce wave = Bounce(WAVE_PIN, 15);
Bounce mute = Bounce(MUTE_PIN, 15);
Bounce mixt = Bounce(MIX_PIN, 15);

byte filter_type = 0;
bool f_capture = true; 
bool muted = false; 

float cut_f;
float cut_last;

enum ENVELOPE_EFFECT {
  AMPLITUDE,
  FILTER
};

enum WAVE {
  //THROW IN SOME MORE WAVEFORMS

  SINE, 
  TRIANGLE, 
  SQUARE
};

class OSCILLATOR {
  public:
    float fre;
    float amp;
    WAVE wav;

    OSCILLATOR(float f, float a, WAVE w){
      fre = f;
      amp = a;
      wav = w;
    }

    void updateWaveform() {
      switch(wav){
        case SINE:
          wav = TRIANGLE; 
          Serial.println("waveform = triangle");
          break;
        case TRIANGLE:
          wav = SQUARE;
          Serial.println("waveform = square");
          break;
        case SQUARE:
          wav = SINE;
          Serial.println("waveform = sine");
          break;
      }
    }

    short getTeensyWaveform() {
      switch(wav) {
        case SINE: return WAVEFORM_SINE;
        case TRIANGLE: return WAVEFORM_TRIANGLE;
        case SQUARE: return WAVEFORM_SQUARE;
        default: return WAVEFORM_SINE;
      }
    }

};

OSCILLATOR w1(440.0f, 0.4, SINE);
OSCILLATOR w2(440.0f, 0.4, SINE);
OSCILLATOR w3(440.0f, 0.4, SINE);


ENVELOPE_EFFECT effecting = AMPLITUDE;  //start with amplitude

int temp_a = 0;
int temp_d = 0;
int temp_r = 0;
int temp_f = 0;

int amp_time = 300;
int fil_time = 300;
IntervalTimer ampTimer;
IntervalTimer filTimer;


unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long interval = 10;

//===================functions=================//

void ampTrigger(){
  if(ampEnv.isActive()){
    ampEnv.noteOff();
  } else { 
    ampEnv.noteOn();
  }
}

void filTrigger(){
  if(filterEnv.isActive()){
    filterEnv.noteOff();
  } else {
    filterEnv.noteOn();
  }
}

void changeWaveform();

float midiToFreq(float note) {
    return 440.0f * powf(2.0f, (note - 69.0f) / 12.0f); //try wt float but i have a feeling its returning an error
}

float freqToMidi(float freq) {
    return 69.0f + 12.0f * (logf(freq / 440.0f) / logf(2.0f));
}

void readKnobs() {
  //read freq
  int raw_f = analogRead(FREQ_KNOB);
  temp_f = map(raw_f, 0, 1023, 25, 80); 

  //read envelope
  int raw_a = analogRead(ATTACK_KNOB);
  int raw_d = analogRead(DECAY_KNOB);
  int raw_r = analogRead(RELEASE_KNOB);
  
  temp_a = map(raw_a, 0, 1023, 10, 500); //maps from 0.01s to 0.5s
  temp_d = map(raw_d, 0, 1023, 10, 1000); //maps from 0.01s to 1s
  temp_r = map(raw_r, 0, 1023, 10, 1000);   //maps from 0.01s to 1.5s
  
  //read cutoff
  int raw_c = analogRead(CUTOFF_KNOB);//change this
  cut_f = expf((float)raw_c / 150.0) * 10.0 + 80.0;

}

void changeWaveform(){
  AudioNoInterrupts();
  osc1.begin(w1.getTeensyWaveform());
  osc2.begin(w2.getTeensyWaveform());
  osc3.begin(w3.getTeensyWaveform());
  AudioInterrupts();  

}

void updateAmplitudeEnvelope(){
  ampEnv.attack(temp_a);
  ampEnv.decay(temp_d);
  ampEnv.release(temp_r);
  amp_time = temp_a + temp_d + temp_r;

  updateTimerA();
}

void updateTimerA(){ ampTimer.update(amp_time * 1000); }

void updateTimerF(){ filTimer.update(fil_time * 1000); }

void updateFilterEnvelope(){
  filterEnv.attack(temp_a);
  filterEnv.decay(temp_d);
  filterEnv.release(temp_r);
  fil_time = temp_a + temp_d + temp_r;

  updateTimerF();

}

void updateFilterMix(){
  if(filter_type == 0){
    filter_mix.gain(0, 1.0);
    filter_mix.gain(1, 0.0);
    filter_mix.gain(2, 0.0);
    Serial.println("LOWPASS FILTER");

  } else if(filter_type == 1){
    filter_mix.gain(0, 0.0);
    filter_mix.gain(1, 1.0);
    filter_mix.gain(2, 0.0);
    Serial.println("BANDPASS FILTER");

  } else {
    filter_mix.gain(0, 0.0);
    filter_mix.gain(1, 0.0);
    filter_mix.gain(2, 1.0);
    Serial.println("HIGHPASS FILTER");

  }
}

void updateFrequency(){
  float glide = 10.0f;

  float target_freq = midiToFreq(temp_f);
  //float target_sub = midiToFreq(temp_f - 12);
  //float target_harm = midiToFreq(temp_f + 6);

  float target_sub = target_freq * 0.496f;
  float target_harm = target_freq * 1.501f;

  float diff = target_freq - w1.fre;
  if(abs(diff) <= 10.0f){
    f_capture = true;
  } else {
    w1.fre += (diff > 0) ? glide : (0 - glide);
    w2.fre += (diff > 0) ? glide : (0 - glide);
    w3.fre += (diff > 0) ? glide : (0 - glide);
  }

  if(f_capture){
    //osc2.frequency(target_sub);
    //osc3.frequency(target_harm);

    w1.fre = target_freq;
    w2.fre = target_sub;
    w3.fre = target_harm;

    //return;
  } 
  osc1.frequency(w1.fre);
  osc2.frequency(w2.fre);
  osc3.frequency(w3.fre);

  
  //return;
}

/*chunck update
void updateFrequency() {
  float base_freq = midiToFreq(temp_f);
  
  w1.fre = base_freq;         // Root note
  w2.fre = base_freq * 0.49f;  // Sub-octave (-12 semitones)
  w3.fre = base_freq * 1.506f;  // Fifth (+7 semitones)

  osc1.frequency(w1.fre);
  osc2.frequency(w2.fre);
  osc3.frequency(w3.fre);
}*/

void muteAmplitude() {
  //affects oscillator
  if(muted) {
    osc1.amplitude(0.0); //class will hold prior amplitude level
    osc2.amplitude(0.0);
    osc3.amplitude(0.0);
    Serial.println("Audio Muted");
  } else {
    osc1.amplitude(0.4);
    osc2.amplitude(0.4);
    osc3.amplitude(0.4);
    Serial.println("Audio unmuted");
  }
}


void setup() {


  /*==========INITIALIZING EVERYTHING=====================*/
  osc_mix.gain(0, 0.23);
  osc_mix.gain(1, 0.23);
  osc_mix.gain(2, 0.3);
  osc_mix.gain(3, 0.1);

  filterDC.amplitude(0.1);

  filterEnv.attack(100);
  filterEnv.decay(100);
  filterEnv.sustain(0.6);
  filterEnv.release(100);

  filter.frequency(800.0f);
  //filter.resonance(0.7);

  filter_mix.gain(0, 1.0);  //defaults to lowpass
  filter_mix.gain(1, 0.0);
  filter_mix.gain(2, 0.0);  

  ampEnv.attack(100);
  ampEnv.decay(100);
  ampEnv.sustain(0.5);
  ampEnv.release(1000);

  /*
  osc1.begin(w1.amp, w1.fre, w1.getTeensyWaveform());
  osc2.begin(w2.amp, w2.fre, w2.getTeensyWaveform());
  osc3.begin(w3.amp, w3.fre, w3.getTeensyWaveform());
  */
  osc1.begin(WAVEFORM_SINE);
  osc1.amplitude(0.3);
  osc1.frequency(440.0f);

  osc2.begin(WAVEFORM_SINE);
  osc2.amplitude(0.3);
  osc2.frequency(220.0f);

  osc3.begin(WAVEFORM_SINE);
  osc3.amplitude(0.3);
  osc3.frequency(659.25f);

  noise.amplitude(0.3);


  //=========================INITIAL OVER=====================//

  Serial.begin(9600);
  AudioMemory(20);

  pinMode(ENV_PIN, INPUT_PULLUP);
  pinMode(WAVE_PIN, INPUT_PULLUP);
  pinMode(MUTE_PIN, INPUT_PULLUP);
  pinMode(MIX_PIN, INPUT_PULLUP);

  
  ampTimer.begin(ampTrigger, amp_time * 1000);
  filTimer.begin(filTrigger, fil_time * 1000);

}



void loop(){
  currentMillis = millis();
  //read buttons
  enve.update();  //just controlls what is being adjusted, needs to change wt p
  if(enve.fallingEdge()){
    switch (effecting) {
      case AMPLITUDE: 
        Serial.println("Adjusting amplitude envelope");
        //updateAmplitudeEnvelope();
        effecting = FILTER;
        break;
      case FILTER:
        Serial.println("Adjusting filter envelope");
        //updateFilterEnvelope();
        effecting = AMPLITUDE;
        break;
    }
  }

  wave.update();
  if(wave.fallingEdge()){ //affects the class 
    w1.updateWaveform();  //class fucntion calls other fucntion?
    w2.updateWaveform();
    w3.updateWaveform();
    changeWaveform();
  }
    
  mute.update();
  if(mute.fallingEdge()){
    muted = !muted;
    muteAmplitude();
  }

  mixt.update();    //set filter mix
  if(mixt.fallingEdge()){
    filter_type = (filter_type + 1) % 3;
    updateFilterMix();
  }

  //some sort of timer here too

  if((currentMillis - previousMillis) >= interval){
    previousMillis = currentMillis;
    readKnobs();

  }
  if(abs(temp_f - freqToMidi(w1.fre)) > 3){
    f_capture = false;
    updateFrequency();
  }

  //some sort of timer here 
  if(effecting == AMPLITUDE){
    updateAmplitudeEnvelope();
  } else if(effecting == FILTER) {
    updateFilterEnvelope();
  }

  if(abs(cut_f - cut_last) > 2.0f) {
    filter.frequency(cut_f);
    cut_last = cut_f;
  }
      
}
