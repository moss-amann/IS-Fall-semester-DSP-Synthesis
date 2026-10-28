#include <math.h>
   
    #include <Audio.h>
    #include <Wire.h>
    #include <SPI.h>
    #include <SD.h>
    #include <SerialFlash.h>
    #include <Bounce.h>
   
   // GUItool: begin automatically generated code
   AudioSynthWaveformDc     pitchAmt;            //xy=90,133
   AudioSynthWaveform       LFO;      //xy=90,194
   AudioSynthWaveformDc     filterAmt; //xy=90,255
   AudioEffectMultiply      multiply;      //xy=247,160
   AudioEffectMultiply      multiply1; //xy=250,233
   AudioSynthWaveformModulated oscillator;   //xy=396,166
   AudioFilterStateVariable filter;        //xy=541,224
   AudioOutputI2S           i2s;            //xy=707,208
   AudioConnection          patchCord1(pitchAmt, 0, multiply, 0);
   AudioConnection          patchCord2(LFO, 0, multiply, 1);
   AudioConnection          patchCord3(LFO, 0, multiply1, 0);
   AudioConnection          patchCord4(filterAmt, 0, multiply1, 1);
   AudioConnection          patchCord5(multiply, 0, oscillator, 0);
   AudioConnection          patchCord6(multiply1, 0, filter, 1);
   AudioConnection          patchCord7(oscillator, 0, filter, 0);
   AudioConnection          patchCord8(filter, 0, i2s, 0);
   AudioConnection          patchCord9(filter, 0, i2s, 1);
   AudioControlSGTL5000     sgtl5000;       //xy=618,140
   // GUItool: end automatically generated code
   
    #define PITCH_POT A13
    #define CUTOFF_POT A12
    #define RATE_POT A11   
    #define AMOUNT_POT A10
    #define RESONANCE_POT A0
   
   //#define LED 3
    #define PITCH_OR_FILTER 0
    #define LEAD_WAVE 1
    #define LFO_WAVE 2

    Bounce pitch_filter = Bounce(PITCH_OR_FILTER, 15);
    Bounce lead_wave = Bounce(LEAD_WAVE, 15);
    Bounce lfo_wave = Bounce(LFO_WAVE, 15);

    int temp_f = 0;



    int shapes[] = {
       WAVEFORM_SINE, WAVEFORM_TRIANGLE, WAVEFORM_SAWTOOTH, 
       WAVEFORM_SAWTOOTH_REVERSE, WAVEFORM_SQUARE, WAVEFORM_SAMPLE_HOLD
    };
   
    #define N_TYPES 6
      
    int pitch = 220;
    int cutoff = 2000;
    int rate = 0;
    int amount = 0;
    bool isPitch = true;
    float resonance = 0.0;
    bool f_capture = true; 

    unsigned long currentMillis = 0;
    unsigned long previousMillis = 0;
    unsigned long interval = 10;



    int waveIndex_lead = 0;
    int waveIndex_lfo = 0;
   
    float inputToPitch(int input){
       int n = map(input, 0, 1023, 21, 108);
       return 440 * pow(2, (n - 69) / 12.0);
    }
   
    void updateFrequency(){
        float glide = 10.0f;

        float target_freq = inputToPitch(temp_f);
  
        float diff = target_freq - pitch;
        if(abs(diff) <= 10.0f){
            f_capture = true;
        } else {
            pitch += (diff > 0) ? glide : (0 - glide);
        }

        if(f_capture){
            pitch = target_freq;
        } 

        oscillator.frequency(inputToPitch(pitch));
    }


   // input is from 0 to 127
    void setCutoff(int u){
       float co = 50 * exp(5.481 * u / 127.0);
       filter.frequency(co);
       filter.octaveControl(log2f(12000.0 / (float)co));
    }

    void setResonance(float r){
        r = r / 150.0;   //will yield a maximum of 0.8; 
        filter.resonance(r);
    }
   
    void setLFORate(int u){
       float f = exp(u / 25.0) / 4.0;
       LFO.frequency(f);
    }
   
    void updateLFO(){
       pitchAmt.amplitude(isPitch ? amount / 127.0 : 0);
       filterAmt.amplitude(isPitch ? 0 : amount / 127.0);
    }

    void updateWaveLead(){
        oscillator.begin(shapes[waveIndex_lead]);
    }    

    void updateWaveLFO(){
        LFO.begin(shapes[waveIndex_lfo]);
    }    

    void readKnobs() {
        //read freq
        int raw_f = analogRead(PITCH_POT);
        temp_f = map(raw_f, 0, 1023, 25, 80); 

    }


   
   `void setup() {
       AudioMemory(20);
      
       pinMode(PITCH_OR_FILTER, INPUT_PULLUP);
       pinMode(LEAD_WAVE, INPUT_PULLUP);
       pinMode(LFO_WAVE, INPUT_PULLUP);
       updateLFO();
   
       oscillator.amplitude(0.5);
       oscillator.frequency(pitch);
       oscillator.begin(shapes[waveIndex_lead]);
       oscillator.frequencyModulation(1);
   
       setLFORate(rate);
       LFO.amplitude(1);
       LFO.begin(shapes[waveIndex_lfo]);
    }
   
  ` void loop()  {
        pitch_filter.update();
        lead_wave.update();
        lfo_wave.update();

        if(pitch_filter.fallingEdge()){
            isPitch = !isPitch;
            if(isPitch){
                Serial.println("Doing pitch mod\n");
            } else {
                Serial.println("Doing filter mod\n");
            }
            updateLFO();
        }

        if(lead_wave.fallingEdge()){
            waveIndex_lead = (waveIndex_lead + 1) % N_TYPES;
            updateWaveLead();
            Serial.println("lead wave change");

        }

        if(lfo_wave.fallingEdge()){
            waveIndex_lfo = (waveIndex_lfo + 1) % N_TYPES;
            updateWaveLFO();
            Serial.println("lfo wave change");
            
        }

        currentMillis = millis();

        if((currentMillis - previousMillis) >= interval){
            previousMillis = currentMillis;
            readKnobs();
        }
        if(abs(temp_f - pitch) > 3){
            f_capture = false;
            updateFrequency();
        }

       // read the pitch pot position
        int newpitch = analogRead(PITCH_POT);
        if (newpitch != pitch) {
           pitch = newpitch;
           oscillator.frequency(inputToPitch(newpitch));
        }
       
        int newcutoff = analogRead(CUTOFF_POT) >> 3;
        if (newcutoff != cutoff){
           cutoff = newcutoff;
           setCutoff(cutoff);
        }

        int newresonance = analogRead(RESONANCE_POT) >> 3;
        if(newresonance != resonance){
            resonance = newresonance;
            setResonance(resonance);
        }
       
        int newrate = analogRead(RATE_POT) >> 3;
         if (newrate != rate){
           //  if it has
           rate = newrate;
           setLFORate(rate);
        }
       
       int newamount = analogRead(AMOUNT_POT) >> 3;
       if (newamount != amount) {
           amount = newamount;
           updateLFO();
       }
   }