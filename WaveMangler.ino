// Based on Teensy Audio demo by PJRC

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "WavMangler.h"

typedef struct {
  uint32_t startPos;  
  float play_rate;
  int play_time;
} Slice;


#define MAX_SLICES 10

Slice slices[MAX_SLICES];


WavMangler               mangler;       //xy=154,78
AudioOutputI2S           i2s1;           //xy=334,89
AudioConnection          patchCord1(mangler, 0, i2s1, 0);
AudioConnection          patchCord2(mangler, 1, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=240,153

void init_slices(uint32_t max_sample) {
  int i;
  for(i=0; i<MAX_SLICES; ++i) {
    slices[i].startPos = random(max_sample);
    slices[i].play_rate = (1+random(100))/(1+random(100));
    slices[i].play_time = 100 + random(1000);
  }
  boolean sorted = false;
  while(!sorted) {
    sorted = true;
    for(i=1; i<MAX_SLICES; ++i) {
      if(slices[i-1].startPos > slices[i].startPos) {
        uint32_t t = slices[i-1].startPos;
        slices[i-1].startPos = slices[i].startPos;
        slices[i].startPos = t;
        sorted = false;
      }
    }    
  }
  slices[0].startPos = 0;
}

void setup() {
  Serial.begin(9600);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(5);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  SPI.setMOSI(7);
  SPI.setSCK(14);
  if (!(SD.begin(10))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  mangler.open(filename);
  
  uint32_t max_sample = mangler.get_sample_count();
  init_slices(max_sample);

  // A brief delay for the library read WAV info
  delay(5);
for(;;) {
  for(int s = 0; s < MAX_SLICES; ++s) {
    if(s < MAX_SLICES - 1) {
      mangler.slice(slices[s].startPos, slices[s+1].startPos);
    }
    else {
      mangler.slice(slices[s].startPos, max_sample);
    }
    for(int i=0; i<slices[s].play_time; ++i) {
      float rate_bias = analogRead(1)/100.0;
      int v = analogRead(2);
      float play_rate = (v/1024.0) * rate_bias + ((1023-v)/1024.0) * slices[s].play_rate;      
      mangler.set_play_rate(play_rate);
      mangler.set_bit_filter(analogRead(1)/64);
      delay(1);
    }
  }
}    
}


void loop() {
  playFile("a.raw");
}

