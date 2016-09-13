FT-857D CAT control for Arduino (ft857d)
=======================================

This library give support to any sketch to act as a Yaesu FT-857D radio once you configure it correctly.

For now it's just basic support for the most common uses in HF rig control, that's my goal. This lib can be improved a lot and have a full support for the radio functions if users request it (or support for it), but for now it meets my goals.

Only the following functions are implemented by now:
* PTT (ON/OFF)
* VFO A/B Selection
* Frequency GET/SET
* Mode GET/SET
* S-meter level GET
* TX Level GET

Some other infos for the developers/courious ones:
* The lib will answer with a 0x00 byte to any non programed command as and ACK, this is not documented in the CAT section in the manual but copied from the hamlib code as they follow the natural rig behaviour.
* The lib is hamlib friendly, it will answer to the eeprom reading from hamlib and will follow the SSB selection on the radio.
* The lib will not ACK of accept any eeprom write command.

Be aware that we link the user functions to the library ones (using pointers) and for that we use it like static and volatile variables, that can be RAM agressive; be careful if you have a RAM shortage before including this library in your code.

Inspiration
===========

This library has been possible from the ideas and code code from the following places:
* James Buck (VE3BUX), FT857D arduino Lib [http://www.ve3bux.com]
* Hamlib source code
* FLRig source code
* Chirp source code

How it works?
=============

You have to define and register a few callback functions with specific parameters that will be called up on CAT commands reception. It's easy, just see the example sketch.

Author & contributors
=====================

The only author is Pavel Milanes, CO7WT, reachable at pavelmc@gmail.com, Until now I have no contributors or sponsors.


Where to download the latest version?
======================================

Always download the latest version from the [github repository](https://github.com/pavelmc/ft857d/)


If you need to give thanks...
=============================

Please take a look at the Donations and Funding file to keep me creating/updating/sharing my work with you.
