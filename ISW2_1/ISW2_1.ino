//Independent Study Week 2: Oscillators
    // Project 1 
    // Getting familiar with Teeny Audio Library. 2 oscillatos with adjustable waveforms and frequencies. 
    // Includes a mute buttons

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=283.1999969482422,307
AudioSynthWaveform       waveform2;      //xy=283.1999969482422,376
AudioOutputI2S           i2s1;           //xy=810.2000389099121,311.
AudioConnection          patchCord1(waveform1, 0, i2s1, 0);
AudioConnection          patchCord2(waveform2, 0, i2s1, 1);
// GUItool: end automatically generated code

int current_waveform = 0;

Bounce button0 = Bounce(0, 15);
Bounce button1 = Bounce(1, 15);
Bounce button2 = Bounce(2, 15);

bool mute = 0; 


void setup() {
  Serial.begin(9600);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  
  AudioMemory(10);

  waveform1.frequency(220);
  waveform2.frequency(440);
  waveform1.amplitude(0.3);
  waveform2.amplitude(0.3);

  current_waveform = WAVEFORM_TRIANGLE;

  waveform1.begin(WAVEFORM_SINE);
  waveform2.begin(WAVEFORM_SINE);
}


void loop() {
  //changes waveform
  button0.update();
  button1.update();


  //replace this
  float knob_A2 = (float)analogRead(A2) / 1023.0;
  float knob_A3 = (float)analogRead(A3) / 1023.0;

  AudioNoInterrupts();
  waveform1.frequency(100.0 + knob_A3 * 900.00);
  waveform2.frequency(100.0 + knob_A2 * 900.00);
  AudioInterrupts();


  if (button0.fallingEdge()) {
    switch (current_waveform) {
      case WAVEFORM_SINE:
        current_waveform = WAVEFORM_SAWTOOTH;
        Serial.println("Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH:
        current_waveform = WAVEFORM_SAWTOOTH_REVERSE;
        Serial.println("Reverse Sawtooth");
        break;
      case WAVEFORM_SAWTOOTH_REVERSE:
        current_waveform = WAVEFORM_SQUARE;
        Serial.println("Square");
        break;
      case WAVEFORM_SQUARE:
        current_waveform = WAVEFORM_TRIANGLE;
        Serial.println("Triangle");
        break;
      case WAVEFORM_TRIANGLE:
        current_waveform = WAVEFORM_PULSE;
        Serial.println("Pulse");
        break;
      case WAVEFORM_PULSE:
        current_waveform = WAVEFORM_SINE;
        Serial.println("Sine");
        break;
    }

    AudioNoInterrupts();
    waveform1.begin(current_waveform);
    waveform2.begin(WAVEFORM_SINE);
    AudioInterrupts();

    
  }

    //possibly include a mute button
  if(button1.fallingEdge()) {
    mute = !mute;
    
    AudioNoInterrupts();
    if(mute) {
      waveform1.amplitude(0.00);
      waveform2.amplitude(0.00);
    }
    else {
      waveform1.amplitude(0.3);
      waveform2.amplitude(0.30);
    }
    AudioInterrupts();

    if (mute) {
      Serial.println("MUTED");
    } else {
      Serial.println("Playing");
    }

  }
}
