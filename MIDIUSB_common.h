#include <MIDIUSB.h>
#include "pitchToFrequency.h"
#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    7

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void LEDset(int ledNum, uint32_t color) {
  int physLED = ledNum;
  if (ledNum > (LED_COUNT - 1)) {
    //Serial.println("Invalid ledNum");
  } else {
    //Serial.print("ledNum: ");
    //Serial.println(ledNum);
    if (LEDS_REVERSED) {
      physLED = (LED_COUNT - 1) - ledNum;
    }
    //Serial.print("physLED: ");
    //Serial.println(physLED);
    strip.setPixelColor(physLED, color);
    strip.show();
  }
}

void LEDsetKey(int i) {
  // make the LED brighter:
  LEDset(i, strip.Color(key_colors[i][0], key_colors[i][1], key_colors[i][2]));
  delay(200);
  // make the LED dim again
  LEDset(i, strip.Color(key_colors[i][0] / dim, key_colors[i][1] / dim, key_colors[i][2] / dim));
}

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
  for (int i = 0; i < KEYCOUNT; i++) { // For each key
    if (pitch == key_midi_notes[i]) {
      LEDset(i, strip.Color(key_colors[i][0], key_colors[i][1], key_colors[i][2]));
      Serial.print(" key: ");
      Serial.print(i);
    }
  }
  Serial.println("");
}

void noteOff(byte channel, byte pitch, byte velocity) {
  for (int i = 0; i < KEYCOUNT; i++) { // For each key
    if (pitch == key_midi_notes[i]) {
      LEDset(i, strip.Color(key_colors[i][0] / dim, key_colors[i][1] / dim, key_colors[i][2] / dim));
    }
  }
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

void sendNoteOn(byte pitch, byte channel ) {
  midiEventPacket_t tx;
  //note on
  tx = {0x09, 0x90 | channel, pitch, 127};
  MidiUSB.sendMIDI(tx);
  // send MIDI now
  MidiUSB.flush();
}

void sendNoteOff(byte pitch, byte channel) {
  midiEventPacket_t tx;
  //note off
  tx = {0x08, 0x80 | channel, pitch, 127};
  MidiUSB.sendMIDI(tx);
  // send MIDI now
  MidiUSB.flush();
}

char s[100];

void setup() {
  Serial.begin(115200);
  delay(1000);
  //  while (!Serial) {
  //    ; // wait for serial port to connect. Needed for native USB
  //  }
  sprintf(s, "Instrument: %s\n", INSTRUMENT);
  Serial.print(s);

  for (int i = 0; i < KEYCOUNT; i++) {
    pinMode(piezoPins[i], INPUT);
  }

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.clear();
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)

  // test the LEDS
  for (int i = 0; i < LED_COUNT; i++) { // For each pixel in strip...
    LEDset(i, strip.Color(0, 0, 255));
    delay(1000);
    LEDset(i, strip.Color(0, 0, 0));             //  off
  }
  // turn on the LEDS for each key
  for (int i = 0; i < KEYCOUNT; i++) { // For each pixel in strip...
    LEDset(i, strip.Color(key_colors[i][0] / dim, key_colors[i][1] / dim, key_colors[i][2] / dim));
  }
}

void loop() {
  // poll for key strikes
  int piezovalue;
  for (int i = 0; i < KEYCOUNT; i++) {
    piezovalue = analogRead(piezoPins[i]);
    if (piezovalue > piezothreshhold) {
      sendNoteOn(key_midi_notes[i], key_midi_channels[i]);
      LEDsetKey(i);
      sprintf(s, "%s key#:%d piezovalue:%d MIDI channel:%d note:%d\n", INSTRUMENT, i, piezovalue, key_midi_channels[i], key_midi_notes[i]);
      Serial.print(s);
      sendNoteOff(key_midi_notes[i], key_midi_channels[i]);
    }
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
}
