# Embedded-Arduino-CW2022
Embedded module CW 21COA202

1 Introduction
This coursework forms 100% of the assessment for this module.
There are several parts to the coursework. A main exercise (known as BASIC) and several additional
parts to allow you to demonstrate more knowledge (EXTENSIONS). These are all detailed below.
If anything is not clear as you read please read through to the end of the document, it may be clear later.
If still not, then post the question into the support forum on LEARN.

2 Data Monitor
The aim of this exercise is to program the Arduino as a Data Monitor and Displayer. It will read data
values over the Serial Monitor, process them and then display them on the screen. The Arduino’s buttons
will allow for interaction with the numbers displayed on the LCD.
First we will introduce the concepts of Data and Channels, then how things should be displayed on the
Arduino Screen and the protocol that will be implemented across the Serial Monitor, with some examples.

2.1 Data and Channels
The Data Monitor receives data values over the Serial Interface in a series of channels. These are
defined as follows:
• After an initialisation phase, each message is a text string sent as a sequence of characters
terminated by an LF. These are sent over the serial interface either through the Serial Monitor or
from a separate host program.
• Data values will be integers in the range 0-255 presented as text.
• We associate each data value with a channel with the channel id being a letter in the range A-Z.
Each channel has a description text.
• Each channel has a maximum and minimum value. These values are provided across the serial
monitor and special displays should be used when the received data values exceed the range.
The protocol for communication is detailed later in the document, but first look at what is to be displayed.

2.2 Arduino Display
The Arduino presents data values on its screen and provides an interface via the buttons for the user to
examine the most recent values. We describe this interface in this section.
• The display has two lines and shows the value of two channels providing an up-down scrolling
facility to see others.
• The display must be of the form:
+--------+
|ˆ1XXX   |
|v2YYY   |
+--------+
ˆ
\-- note no space between channel letters and value display
Note the exact positioning of the characters.
• In this example, 1 and 2 and are replaced by the channel letter and XXX and YYY by the most recent
value. The value must be right justified, if the value is not 3 digits, then move it so the units columns
line in the right column. e.g.:
+-------+
|ˆA146  |
|vW 9   |
+-------+
• Program the UP and DOWN buttons to move up through the channel list in alphabetical order. On
reaching the first or last channel remove the ˆ or v, to indicate there are no more channels to show.
• If the input numbers on any channel fall outside this range then the backlight must change colour:
red, if there is a value on a channel above the maximum; green, if there is one below the minimum;
yellow if both are true. If all recent data values are in range on every channel then set the backlight
to white.
– (New for v2) If a minimum value is received that is larger than the maximum value it must be
stored, but not included in the determination of the backlight colour in the paragraph above.
• Pressing and holding the SELECT button for longer than one second clears the screen, sets the
backlight to purple and displays simply your ID number and nothing else. Releasing it returns the
display to normal. During this time your program must continue to read lines from the serial monitor
and process them.

2.3 The protocol in more detail
• Synchronisation phase:
– After your Arduino has booted it must set the backlight to purple and repeatedly send the
character Q with no following NL or CR character to the Serial interface at a frequency of once
per second.
– After sending, the Arduino must monitor for an incoming character and when an X is received
synchronisation completes and the main program loop can start up.
– No newline or carriage return should be sent or expected.
• After synchronisation happens your program must send the string BASIC followed by a new line,
then set the backlight to white and start the main phase.
• Main phase:
– The host sends messages to the Arduino over the serial interface. The first character will be
one of the following:
– C — this indicates a new channel. It will be followed by the channel letter and a String
description. This could be up to 15 characters and extra characters until the LF must be
ignored. If the host sends a C message an existing channel, the new description must be
used.
– V — this indicates a value. It will be followed by a letter, indicating the channel and an integer
number: the value. The value will be expressed as text and in the range 0 to 255. Data values
received on a channel that has not been created must be ignored.
– (changed for v2) X — specifies the maximum value for a channel. If no X line has been
received, then assume 255 as default.
– N — specifies the minimum value for a channel. Here assume 0 as default.
– Both X and N are followed by the channel letter and value in the same form as the V message,
with no spaces or other characters. e.g.
XA100
NW100
– Any lines not conforming to the protocol must be ignored. However, the text ERROR: followed
by the ignored string should be written to the Serial Monitor. This will aid debugging.
*
(New for v2) Lines such as XA with no value are invalid.
– In addition, you may send any text beginning with DEBUG: to the Serial monitor; it will be
ignored by the assessment testing program, but will help your debugging.
– There must be no whitespace characters in the messages, except in the channel description.
• Unless you implement the EEPROM extension (see below), then on initialisation no channels
should be assumed defined and consequently all received data values must be ignored until a
suitable C message arrives.

2.4 Protocol Examples
Some An example uses of the protocol:
This first example sets up two channels A and B and gives them the names Main and Secondary. Data
values 100, 200, 201, 202, 203 and 204 are sent over channel A and 5 and 4 over channel B.
CAMain
CBSecondary
VA100
VA200
VA201
VB5
VA202
VA203
VA204
VB4
The following example sets up a single channel A, sets maximum to 100, minimum to 50 and sends data
values 90, 100 and 110.
CAMain
XA100
NA50
VA90
VA100
VA110
A python program for running on a host and conforming to the specification will be provided in week 5.
You are welcome to modify this for your testing purposes. A version of this program will be used to test
your implementations, although the numbers will change.

2.5 Requirements
• You must implement your code using a Finite State Machine (FSM). Include a picture of this
machine, listing the states and the state transitions in your documentation. See lab sheet 4.
• Make sure comment your code appropriately. Ideally code should be mostly self commenting
through sensible choice of variable and function names and use of macros. However, there will be
times when things are less comprehensive, so be sure to highlight theses.
• In your documentation:
– Write a description of your states, what is being waited for, and the actions take during
transition between states.
– Write 200–500 words of reflection on your code. Include those things that don’t work as well
as you would like and how you would fix them.

2.6 Assessment
Achieving the above implementation is worth up to 50 marks, if properly documented (20 marks) and
coded (30 marks).

3 Extension features
The remaining marks are distributed across the extension features. You may implement as many as you
wish. Assume about 40% of each extension’s marks will be based on any documentation.
The following extensions allow you to demonstrate more knowledge and in return are work additional
marks indicated. If you have implemented any of these then the string BASIC sent after synchronisation
must be replaced by a list of the extension names separated by commas and followed by a new line. For
example:
RECENT,FREERAM,HCI,SCROLL,EEPROM,UDCHARS

3.1 UDCHARS
This shows your ability to define your own character forms. [5 marks]
• Define some characters to replace the v and ˆ with nicer looking arrows.
• In your documentation indicate which lines of the code define the characters.

3.2 FREERAM
This show your ability to read the free SRAM in an Arduino. [5 marks]
• Modify what happens when pressing the SELECT button. In addition to the ID number, display the
amount of free SRAM.
• In your documentation indicate the parts of the code to display the free SRAM.

3.3 HCI
This shows your ability to select particular values from an array of values. [5 marks]
• When the user presses and releases the RIGHT button the display must only display channels
where the current value is beyond the maximum. (changed for v2) If none match this criterion, then
display nothing.
• When the user presses and releases the LEFT button the display must only display channels where
the current value is beyond the minimum.
• If in either of the above two states repressing and releasing the same button returns to displaying
all the channels.
• If no channels match the criteria, then nothing should be shown. The v and ˆ arrows should appear
based on the subset of channels to display.
• In your documentation show the lines of code and thinking behind the mechanism to display subsets
of the list of channels.

3.4 EEPROM
This shows your ability read/write from EEPROM. [5 marks]
• Store the channel letters and descriptions with their maximum and minimum values in the EEPROM
so that they can survive power resets.
• Read from the EEPROM on startup and find a mechanism for determining whether the values
where written by you or simply left from before.
– (new for v2) After restarting until a value is received on a particular channel you MUST not
include that channel in the maximum and minimum operations.
• In your documentation indicate how you lay out the use of the EEPROM and which lines of code
and functions you use to store the information.

3.5 RECENT
This shows your ability to manage larger data sets than single values. [15 marks]
• Store the most recent 64 values and display the average of these as well as the most recent value
on the screen. Keep to the format below:
+----------------+
|ˆ1XXX,AVG       |
|v2YYY,AVG       |
+----------------+
Average is to be rounded to the nearest integer value.
• In your documentation indicate the names and locations of the data structures used to store the
recent values.

3.6 NAMES
This shows your ability to store and display other text. [5 marks]
• Display the channel names to the right of the values using the remaining available space.
+----------------+
|ˆ1XXX Main      |
|v2YYY Secondary |
+----------------+
or if you have implemented RECENT
+----------------+
|ˆAXXX,AVG Main  |
|vBYYY,AVG Second| <--- note name is truncated
+----------------+
• In your documentation indicate the data structure you use to store the channel name and how it is
printed to the display.

3.7 SCROLL
This shows your ability to implement a more complicated FSM that can take time into account as well as
listening to the Serial Monitor and button presses. [10 marks]
• If you have implemented NAMES then when the channel description is too big then add code to
scroll it left at 2 characters per second, returning to the start when the full name has been displayed.
• All other functionality should remain implemented as described.
• In your documentation highlight the parts of the state machine required for this particular requirement and the lines of code and functions that carry this implementation.
