#define INSTRUMENT "drums"

#define LEDS_PLAY false //should the instrument's LEDs light up when the piezo sensors are triggered?


// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 0

#define KEYCOUNT 2  // how many keys does the instrument have
int LEDS_REVERSED = 0;       // 0 if LEDs are left to right, 1 if reversed (PVC pipes)

int key_midi_channels[] = {  // an array of MIDI channels for the keys
  3,3       
};

int key_midi_notes[] = {  // an array of MIDI note numbers for the keys
  100, 101                //  
};

byte key_colors[KEYCOUNT][3] = { // an array of LED colors for the keys
//  {255, 255, 255},
//  {255, 255, 255},
//  {255, 255, 255},
//  {255, 255, 255},
  {255, 255, 255}
};

int dim = 4; // Dim factor for LEDs. (Divide key_color[] by dim for dim level.)

int piezothreshhold = 110; //piezo sensor threshhold for hit detection

int piezoPins[] = {
  A0, A1
  // A0, A1, A2, A3, A6
  // A0, A1, A2, A3
};       // an array of pin numbers to which the piezo sensors are attached

#include "MIDIUSB_common.h"
