# FT-857D CAT control for Arduino (FT857d) #

This library give support to any sketch to act as a Yaesu FT-857D radio once you configure it correctly.

For now it's just basic support for the most common uses in HF rig control, that's my goal. This lib can be improved a lot and have a full support for the radio functions if users request it (or support for it), but for now, it meets my goals.

Only the following functions are implemented by now:
* PTT (ON/OFF)
* VFO A/B Selection
* Frequency GET/SET
* Mode GET/SET
* S-meter level GET
* TX Level GET

## Supported softwares ##

So far we have tested support for the following CAT capable softwares:

* Hamlib
* Fldigi (using Hamlib)
* Flrig
* MixW

If your preferred software has troubles getting this lib to work, please tell me about it, I will be happy to work with you in solving the issue, please use the "Issues" tab in the Github repository page to report this kind of request.

## Tips for the developers/curious ones: ##

* The lib will answer with a 0x00 byte to any non programed command as and ACK, this is not documented in the CAT section in the manual but copied from the hamlib code as they follow the natural rig behaviour.
* The lib is hamlib friendly, it will answer to the eeprom reading from hamlib and will follow the SSB selection on the radio.
* The lib will not ACK of accept any eeprom write command.
* Be aware that we link the user functions to the library ones (using pointers) and for that we use it like static variables, that can be RAM aggressive; be careful if you have a RAM shortage before including this library in your code.

## Inspiration ##

This library has been possible from the ideas and code code from the following places:

* James Buck (VE3BUX), [FT857D arduino Lib](http://www.ve3bux.com)
* Hamlib source code
* FLRig source code
* Chirp source code

## Install & How it works? ##

See the INSTALL.txt file in the library for instructions.

About how it works? it's simple, you have to define and register a few custom callback functions and procedures with specific parameters. They will be called upon CAT commands reception to process the requested/sent data.

It's more easy than it sounds, just see the example sketch.

_**Note:** Actually there is a bug that makes the example project not show in the "Examples" menu, I'm working on it._

## Author & contributors ##

The only author is Pavel Milanes, CO7WT, reachable at pavelmc@gmail.com, Until now I have no contributors or sponsors.

## Where to download the latest version? ##

Always download the latest version from the [github repository](https://github.com/pavelmc/FT857d/)


## If you like to give thanks... ##

Please take a look at the Donations and Funding file to keep me creating/updating/sharing my work with you.
