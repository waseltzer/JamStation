---------
MIDIUSB_common.h is the shared code for all the instruments.
There is a copy of this file in each specific instrument folder.

Unfortinately, Arduino does not support
#include "../MIDIUSB_common.h"
So, I made a copy of the shared code and instead use
#include "MIDIUSB_common.h"

Therefore, we need to manually keep MIDIUSB_common.h in sync across the folders until 
Arduino supports relative paths in include files.

---------
Changing the device name for the multiple Arduino Pro Micros.
The MIDI device name is "Sparkfun Pro Micro" for all of the boards.
This is confusing, so here's the hack to the Sparkfun "boards.txt" to change the name.

When uploading the code to the Pro Micro board, select the appropriate name
in the Arduino IDE Tools --> Board --> Sparkfun AVR Boards --> "<Instrument name> SparkFun Pro Micro"


boards.txt is the Sparkfun boards.txt
which lives in C:\Users\wayne\AppData\Local\Arduino15\packages\SparkFun\hardware\avr\1.1.12)
I copied the section beginning:
################################################################################
################################## Pro Micro ###################################
################################################################################
and changed: (for chimes, pipes, drums)

promicro.name=SparkFun Pro Micro
==>
chimespromicro.name=Chimes SparkFun Pro Micro

promicro.build.usb_product="SparkFun Pro Micro"
promicro.build.vid=0x1b4f
=>
chimespromicro.build.usb_product="Chimes SparkFun Pro Micro"
chimespromicro.build.vid=0xfff1


