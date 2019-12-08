/* To-do:
   Implement indicator for when two instruments are collaborating together
   Fix so doesn't freak out after win
   Collaborate after win 
   Decay rate fast makes you want to play faster, we need to make it so more and harder does not equate to winning
*/

#include <FastLED.h>
#include <MIDIUSB.h>
#include "pitchToFrequency.h"
// How many leds in your strip?
#define NUM_LEDS 64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define DATA_PIN1 5 // led data pins
#define DATA_PIN2 6
#define DATA_PIN3 7
#define DATA_PIN4 8
#define CHANNEL 0 // MIDI channel 1 = 0x00
CRGB leds[NUM_LEDS];

uint8_t gHue = 0; // rainbow shit

// use this for keep track of time instead of delay()
//Delay is ok in setup() though, since it's a 1-time dealio.
const int period = 50;
unsigned long midi_effects_on_time = 0;
unsigned long winner_on_time = 0;
const long midi_effects_timer = 250;
const long winner_period = 3000;

int midi_effects_start = 32; //Where on the pixel strip we're starting for midi effects
int midi_effects_counter = 0; // count whe an effect happens, register it here
int midi_effects_register = 0;

/*
  MIDI Assignments
  Instrument  Channel     Midi Note(s)
  Chimes      Channel 1   80, 85, 89, 92
  PVC Pipes   Channel 2   85, 89, 92, 97
  Base Drum   Channel 3   100
  Snare Drum  Channel 3   101
  Expressive  Channel 4

  Note: Most midi programs start with Channel 1. Arduino/MidiUSB starts with Channel 0.
  So everything is N-1 here.
*/

// assign midi controller channel values;
int chimes  = 0;
int pvc     = 1;
int drum    = 2;
int snare   = 2;
int midi    = 3;

/*
   Now we gettin cray.
   Table for MIDI Notes on specific channels referenced above
      BB   PVC   WC    Exp
  BB    --   100   101   102
  PVC   100  --    103   104
  WC    101  103   --    105
  Exp   102  104   105   --

*/
// more interesting lighting effects down below
int same_time       = 4;      // Weight 1; When PD detects the same note is being struck at some regular interval, it sends on this channel;
int same_note_time  = 5; // Weight 1; When PD detects two instruments playing at the same tempo, it sends this channel
int same_tempo      = 6;     // Weight 2; When PD detects two instruments at the same tempo
int same_division   = 7;  // Weight 4; When PD does something that Torin figured out
int same_melody     = 8;    // Weight 6; When PD does something that Torin figured out


// midi library stuff
const char* pitch_name(byte pitch) {
  static const char* names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
  return names[pitch % 12];
}

int pitch_octave(byte pitch) {
  return (pitch / 12) - 1;
}

void noteOn(byte channel, byte pitch, byte velocity) {

  Serial.print("Receive Note On: ");
  Serial.print(pitch_name(pitch));
  Serial.print(pitch_octave(pitch));
  Serial.print(", ch=");
  Serial.print(channel);
  Serial.print(", vel=");
  Serial.print(velocity);
  Serial.println("");

  // Drum or Snare
  // Dum
  if (channel == drum && pitch == 48) {
    for (int i = 0; i < 8; i ++) {
      leds[i] = CHSV(160, 255, 255);
    }
  }
  // Snare
  if (channel == drum && pitch == 50) {
    for (int i = 0; i < 8; i ++) {
      leds[i] = CHSV(0, 255, 255);
    }
  }

  // PVC
  if (channel == pvc && pitch == 85) {
    for (int i = 8; i < 16; i ++) {
      leds[i] = CHSV(0, 255, 255);
    }
  }

  if (channel == pvc && pitch == 89) {
    for (int i = 8; i < 16; i ++) {
      leds[i] = CHSV(32, 255, 255);
    }
  }

  if (channel == pvc && pitch == 92) {
    for (int i = 8; i < 16; i ++) {
      leds[i] = CHSV(160, 255, 255);
    }
  }

  if (channel == pvc && pitch == 97) {
    for (int i = 8; i < 16; i ++) {
      leds[i] = CHSV(0, 255, 255);
    }
  }

  // Start the chimes yo
  if (channel == chimes && pitch == 80) {
    for (int i = 16; i < 24; i ++) {
      leds[i] = CHSV(160, 255, 255);
    }
  }

  if (channel == chimes && pitch == 85) {
    for (int i = 16; i < 24; i ++) {
      leds[i] = CHSV(0, 255, 255);
    }
  }

  if (channel == chimes && pitch == 89) {
    for (int i = 16; i < 24; i ++) {
      leds[i] = CHSV(32, 255, 255);
    }
  }

  if (channel == chimes && pitch == 92) {
    for (int i = 16; i < 24; i ++) {
      leds[i] = CHSV(160, 255, 255);
    }
  }

  // Midi Expressive Mania
  if (pitch >= 36 <= 72) {
    for (int i = 24; i < 32; i ++) {
      leds[i] = CHSV(196, 255, 255);
    }
  }


  if (pitch == 60) {
    midi_effects_register = 1;
    midi_effects_counter += 1;
 //   midi_effects();
  }

  if (pitch == 62) {
    midi_effects_register = 1;
    midi_effects_counter += 1;
 //   midi_effects();
  }

  if (channel == same_tempo) {
    midi_effects_register = 1;
    midi_effects_counter += 2;
 //   midi_effects();
  }

  if (pitch == 62) {
    midi_effects_register = 1;
    midi_effects_counter += 4;
//    midi_effects();
  }

  if (pitch == 60) {
    midi_effects_register = 1;
    if (midi_effects_counter < NUM_LEDS - 6) {
       midi_effects_counter += 6;
    }
  }
}

//int same_time       = 4;      // Weight 1; When PD detects the same note is being struck at some regular interval, it sends on this channel;
//int same_note_time  = 5; // Weight 1; When PD detects two instruments playing at the same tempo, it sends this channel
//int same_tempo      = 6;     // Weight 2; When PD detects two instruments at the same tempo
//int same_division   = 7;  // Weight 4; When PD does something that Torin figured out
//int same_melody     = 8;    // Weight 6; When PD does something that Torin figured out

void noteOff(byte channel, byte pitch, byte velocity) {
  Serial.print("Receive Note Off: ");
  Serial.print(pitch_name(pitch));
  Serial.print(pitch_octave(pitch));
  Serial.print(", ch=");
  Serial.print(channel);
  Serial.print(", vel=");
  Serial.println(velocity);
}

void controlChange(byte channel, byte control, byte value) {
  Serial.print("Receive Control change: control=");
  Serial.print(control);
  Serial.print(", value=");
  Serial.print(value);
  Serial.print(", channel=");
  Serial.println(channel);
}

// This will fade out to a lower brightness value, not completely black
void fadeall() {
  for (int i = 0; i < 32; i++) {
    leds[i].nscale8(230);
  }
}

void fadeall_midi_effects() {
  for (int i = midi_effects_start; i < NUM_LEDS; i++) {
    leds[i].nscale8(230);
  }
}

// This will fade out completely black
void fadeall_black() {
  for (int i = 0  ; i < NUM_LEDS; i++) {
    leds[i].nscale8(230);
  }
}

void setup() {
  Serial.begin(57600);
  Serial.println("resetting");

  FastLED.addLeds<LED_TYPE, DATA_PIN1, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, DATA_PIN2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, DATA_PIN3, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, DATA_PIN4, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

  LEDS.setBrightness(84);
  initial_color();
  delay(1000);
  effects_initialize();

  delay(1000);
  FastLED.show();
}

void loop() {
  if ( midi_effects_counter > NUM_LEDS/2 + 10 ) {
    midi_effects_counter = NUM_LEDS/2 + 10;
  }
  midiEventPacket_t rx = MidiUSB.read();
  switch (rx.header) {
    case 0:
      break; //No pending events

    case 0x9:
      noteOn(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;

    case 0x8:
      noteOff(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;

    case 0xB:
      controlChange(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //control
        rx.byte3         //value
      );
      break;

    default:
      Serial.print("Unhandled MIDI message: ");
      Serial.print(rx.header, HEX);
      Serial.print("-");
      Serial.print(rx.byte1, HEX);
      Serial.print("-");
      Serial.print(rx.byte2, HEX);
      Serial.print("-");
      Serial.println(rx.byte3, HEX);
  }

  fadeall();
  fadeall_midi_effects();
  midi_effects();
  winner_winner();
  midi_effects_decay();
  FastLED.show();
}

void initial_color() {
  //drum
  for (int i = 0; i < 4; i ++) {
    leds[i] = CHSV(160, 255, 255);
  }
  //snare
  for (int i = 4; i < 8; i ++) {
    leds[i] = CHSV(0, 255, 255);
  }
  //PVC
  for (int i = 8; i < 10; i ++) {
    leds[i] = CHSV(96, 255, 255);
  }
  for (int i = 10; i < 12; i ++) {
    leds[i] = CHSV(96, 255, 255);
  }
  for (int i = 12; i < 14; i ++) {
    leds[i] = CHSV(96, 255, 255);
  }
  for (int i = 14; i < 16; i ++) {
    leds[i] = CHSV(96, 255, 255);
  }
  //Chimes
  for (int i = 16; i < 18; i ++) {
    leds[i] = CHSV(42, 255, 255);
  }
  for (int i = 18; i < 20; i ++) {
    leds[i] = CHSV(42, 255, 255);
  }
  for (int i = 20; i < 22; i ++) {
    leds[i] = CHSV(42, 255, 255);
  }
  for (int i = 22; i < 24; i ++) {
    leds[i] = CHSV(32, 255, 255);
  }
  //MIDI
  //MIDI Expressive
  for (int i = 24; i < 32; i ++) {
    leds[i] = CHSV(196, 255, 255);
  }
}

void effects_initialize() {
  for (int i = 32; i < NUM_LEDS - 1; i++) {
    leds[i] = CHSV(160, 0, 255);
    delay(25);
    FastLED.show();
  }
  for (int i = NUM_LEDS - 1; i > 31; i--) {
    leds[i] = CHSV(0, 0, 0);
    delay(25);
    Serial.println("Why I no worky");
    FastLED.show();
  }
}

void midi_effects() {
  if ( midi_effects_counter + midi_effects_start >= NUM_LEDS) {
    winner_on_time = millis();
    }
  for (int i = midi_effects_start; i < midi_effects_start + midi_effects_counter && i < NUM_LEDS; i ++) {
    leds[i] = CHSV(0, 0, 255);
  }
}


void midi_effects_decay() {
  if (midi_effects_counter < 0) {
    midi_effects_counter = 0;
  }
  if (midi_effects_register == 1) {
    if (millis() - midi_effects_on_time > midi_effects_timer) {
      midi_effects_counter -= 1;
      midi_effects_on_time = millis();
    }
  }
}

void winner_winner() {
  // FastLED's built-in rainbow generator
  if (millis() - winner_on_time < winner_period) {
    fill_rainbow( &(leds[31]), NUM_LEDS /2 , gHue, 7);
    EVERY_N_MILLISECONDS( 5 ) {
      gHue++;  // slowly cycle the "base color" through the rainbow
    }
    FastLED.show();
  }
}
