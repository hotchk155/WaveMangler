// Based on Teensy Audio demo by PJRC

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "WavMangler.h"

WavMangler               mangler;       //xy=154,78
AudioOutputI2S           i2s1;           //xy=334,89
AudioConnection          patchCord1(mangler, 0, i2s1, 0);
AudioConnection          patchCord2(mangler, 1, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=240,153

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

  // A brief delay for the library read WAV info
  delay(5);

  mangler.slice(2000, 3000);
  
  // Simply wait for the file to finish playing.
  while (mangler.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);
  }
}


void loop() {
  playFile("gwb.raw");
}

