/*************************************************************************
 * FT857D CAT Library, by Pavel Milanes, CO7WT, pavelmc@gmail.com
 *
 * The goal of this lib is to act as a Yaesu FT-857D radio from the
 * CAT point of view, then you can talk with your sketch from the PC like
 * if it was a real radio via CAT commands; to command a DDS for example.
 *
 * This work was a need from my side for the arduino-arcs project
 * see it here https://github.com/pavelmc/arduino-arcs
 *
 * This code has been built with the review of various sources:
 * - James Buck, VE3BUX, FT857D arduino Lib [http://www.ve3bux.com]
 * - Hamlib source code
 * - FLRig source code
 * - Chirp source code
 *
 * You can always found the last version in https://github.com/pavelmc/FT857d/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * **************************************************************************/

/*
 * This is the example for the library, just upload it to a Uno and configure
 * your software with 57600 @ 8N1 and enjoy
 */

#include <ft857d.h>

ft857d radio = ft857d();

// variables
long freq = 7110000;
boolean ptt = false;
boolean splitActive = false;
boolean vfoAActive = true;
byte mode = 0;

// radio modes
#define MODE_LSB 00
#define MODE_USB 01
#define MODE_CW 02

// DEBUG flag, uncomment it if you want to test it by hand
//#define DEBUG true

// function to run when we must put radio on TX/RX
void catGoPtt(boolean pttf) {
    // the var ptt follows the value passed, but you can do a few more thing here
    ptt = pttf;

    #if defined (DEBUG)
    // debug
    Serial.print("PTT Status is: ");
    Serial.println(ptt);
    #endif
}

// function to run when VFOs A/B are toggled
void catGoToggleVFOs() {
    // here we simply toggle the value
    vfoAActive = !vfoAActive;

    #if defined (DEBUG)
    // debug
    Serial.print("VFO A active?: ");
    Serial.println(vfoAActive);
    #endif
}

// function to set a freq from CAT
void catSetFreq(long f) {
    // the var freq follows the value passed, but you can do a few more thing here
    freq = f;

    #if defined (DEBUG)
    // debug
    Serial.print("Active VFO freq is: ");
    Serial.println(freq);
    #endif
}

// function to set the mode from the cat command
void catSetMode(byte m) {
    // the var mode follows the value passed, but you can do a few more thing here
    mode = m;

    #if defined (DEBUG)
    // debug
    Serial.print("Active VFO mode is: ");
    Serial.println(mode);
    #endif
}

// function to pass the freq to the cat library
long catGetFreq() {
    // this must return the freq as an unsigned long in Hz, you must prepare it before

    #if defined (DEBUG)
    // debug
    Serial.println("Asked for freq");
    #endif

    // pass it away
    return freq;
}

// function to pass the mode to the cat library
byte catGetMode() {
    // this must return the mode in the wat the CAT protocol expect it

    #if defined (DEBUG)
    // debug
    Serial.println("Asked for mode");
    #endif

    // pass it away
    return mode;
}

// function to pass the smeter reading in RX mode
byte catGetSMeter() {
    // this must return a byte in with the 4 LSB are the S meter data
    // so this procedure must take care of convert your S meter and scale it
    // up to just 4 bits

    #if defined (DEBUG)
    // debug
    Serial.println("Asked for S meter");
    #endif

    // pass it away (fixed here just for testing)
    return byte(4);
}

// function to pass the TX status
byte catGetTXStatus() {
    /*
     * this must return a byte in wich the different bits means this:
     * 0b abcdefgh
     *  a = 0 = PTT off
     *  a = 1 = PTT on
     *  b = 0 = HI SWR off
     *  b = 1 = HI SWR on
     *  c = 0 = split on
     *  c = 1 = split off
     *  d = dummy data
     *  efgh = PO meter data
     */

    #if defined (DEBUG)
    // debug
    Serial.println("Asked for TX status");
    #endif

    // you have to craft the byte from your data, we will built it from
    // our data
    byte r = 0;
    // we fix the TX power to half scale (8)
    r = ptt<<7 + splitActive<<5 + 8;

    return r;
}


void setup() {
    // preload the vars in the cat library
    radio.addCATPtt(catGoPtt);
    radio.addCATAB(catGoToggleVFOs);
    radio.addCATFSet(catSetFreq);
    radio.addCATMSet(catSetMode);
    radio.addCATGetFreq(catGetFreq);
    radio.addCATGetMode(catGetMode);
    radio.addCATSMeter(catGetSMeter);
    radio.addCATTXStatus(catGetTXStatus);

    // now we activate the library
    radio.begin(57600, SERIAL_8N1);

    #if defined (DEBUG)
    // serial welcome
    Serial.println("CAT Serial Test Ready");
    #endif

}

void loop() {
    radio.check();
}
