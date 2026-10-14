#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Arduino.h>

AudioSynthWaveform       waveform1;      //xy=680.6666564941406,720.6666564941406
AudioMixer4              mixer1;         //xy=919.7142900739398,723.7142726353237
AudioEffectEnvelope      envelope1;      //xy=1112.5714329310827,726.5714154924666
AudioOutputI2S           i2s1;           //xy=1286.8571472167969,717.999986921038
AudioConnection          patchCord1(waveform1, 0, mixer1, 0);
AudioConnection          patchCord2(mixer1, envelope1);
AudioConnection          patchCord3(envelope1, 0, i2s1, 0);
AudioConnection          patchCord4(envelope1, 0, i2s1, 1);

//PUT IN A FILTER
//ADD TIMED FUNCTION FOR POTENTIOMETER READINGS

#define NOTE_PIN 1
#define DEBOUNCE 50

volatile unsigned long lastInterruptTime = 0;
volatile bool buttonPressed = false;
volatile unsigned long pressStartTime = 0;

void buttonISR() {
    unsigned long now = millis();

    // Debounce check
    if (now - lastInterruptTime < DEBOUNCE) {
        return;
    }
    lastInterruptTime = now;

    // Read pin state
    bool currentState = digitalRead(NOTE_PIN) == LOW; // LOW means pressed if using pull-up

    if (currentState && !buttonPressed) {
        // Button pressed
        buttonPressed = true;
        pressStartTime = now;
        handleNoteTrigger();
    } else if (!currentState && buttonPressed) {
        // Button released
        buttonPressed = false;
        handleNoteRelease();
        
    }
}

void handleNoteTrigger(){
  envelope1.noteOn();
}

void handleNoteRelease(){
  envelope1.noteOff();
}



void setup() {
  // put your setup code here, to run once:
  pinMode(NOTE_PIN, INPUT_PULLUP);

  AudioMemory(15);

  attachInterrupt(digitalPinToInterrupt(NOTE_PIN), buttonISR, CHANGE);

  mixer1.gain(0, 0.5);

  waveform1.frequency(440.0f);
  waveform1.amplitude(0.5);
  waveform1.begin(WAVEFORM_SINE);

  envelope1.attack(40);
  envelope1.decay(150);
  envelope1.sustain(0.6);
  envelope1.release(500);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
