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

#include "Arduino.h"
#include "ft857d.h"

// function work vars, must be static & volatile?
static FuncPtrVoid empty[1];
static FuncPtrVoidByte emptyB[3];
static FuncPtrVoidLong emptyL[1];
static FuncPtrToggles toggle[1];
static FuncPtrByte fbyte[1];
static FuncPtrLong longf[1];

/*
 * Contructor, simple constructor, it initiates the serial port in the
 * default mode for the radio: 9600 @ 8N2
 */
void ft857d::begin() {
    Serial.begin(9600, SERIAL_8N2);
    Serial.flush();
}

// Alternative initializer with a custom baudrate and mode
void ft857d::begin(long br, int mode) {
    /*
     * Allowed Arduino modes for the serial:
     *  SERIAL_5N1; SERIAL_6N1; SERIAL_7N1; SERIAL_8N1; SERIAL_5N2; SERIAL_6N2;
     *  SERIAL_7N2; SERIAL_8N2; SERIAL_5E1; SERIAL_6E1; SERIAL_7E1; SERIAL_8E1;
     *  SERIAL_5E2; SERIAL_6E2; SERIAL_7E2; SERIAL_8E2; SERIAL_5O1; SERIAL_6O1;
     *  SERIAL_7O1; SERIAL_8O1; SERIAL_5O2; SERIAL_6O2; SERIAL_7O2; SERIAL_8O2
     */
    Serial.begin(br, mode);
    Serial.flush();
}

/*
 * Linking the toggle functions from the user code, this ones expect a function
 * in which you pass a bool to activate/deactivate the function
 */

// PTT
void ft857d::addCATPtt(void (*userFunc)(boolean)) {
    toggle[0] = userFunc;
}

/*
 * Linking the functions that are called without parameters
 */

// VFO A/B
void ft857d::addCATAB(void (*userFunc)(void)) {
    empty[0] = userFunc;
}

// AUX: Get the freq of operation, the function must return the freq
void ft857d::addCATGetFreq(long (*userFunc)(void)) {
    emptyL[0] = userFunc;
}

// AUX: Get the mode of operation, the function must return the mode
void ft857d::addCATGetMode(byte (*userFunc)(void)) {
    emptyB[0] = userFunc;
}

// S meter
void ft857d::addCATSMeter(byte (*userFunc)(void)) {
    emptyB[1] = userFunc;
}

// TX status
void ft857d::addCATTXStatus(byte (*userFunc)(void)) {
    emptyB[2] = userFunc;
}


/*
 * Linking the function for the freq set, this one must link a function that
 * accept a unisgned long as the freq in hz
 */

// FREQ SET
void ft857d::addCATFSet(void (*userFunc)(long)) {
    longf[0] = userFunc;
}

/*
 * Linking the function for the mode set, this expect a function that accepts a
 * byte that is the mode in the way the CAT is defined
 */

// MODE SET
void ft857d::addCATMSet(void (*userFunc)(byte)) {
    fbyte[0] = userFunc;
}

/*
 * Periodic call function, this must be placed inside the loop()
 * to check for incomming serial commands.
 */

 // check function
void ft857d::check() {
    // do nothing if it was disabled by software
    if (!enabled) return;

    // first check if we have at least 5 bytes waiting on the buffer
    byte i = Serial.available();
    if (i < 5) return;

    // if you got here then there is at least 5 bytes waiting: get it.
    for (i=0; i<5; i++) {
        nullPad[i] = Serial.read();
    }

    // now chek for the command in the last byte
    switch (nullPad[4]) {
        case CAT_PTT_ON:
            if (toggle[0]) {
                toggle[0](true);
                Serial.write(ACK);
            }
            break;
        case CAT_PTT_OFF:
            if (toggle[0]) {
                toggle[0](false);
                Serial.write(ACK);
            }
            break;
        case CAT_VFO_AB:
            if (empty[0]) {
                empty[0]();
                Serial.write(ACK);
            }
            break;
        case CAT_FREQ_SET:
            if (longf[0]) {
                fset();
                Serial.write(ACK);
            }
            break;
        case CAT_MODE_SET:
            if (fbyte[0]) {
                fbyte[0](nullPad[0]);
                Serial.write(ACK);
            }
            break;
        case CAT_RX_FREQ_CMD:
            if (emptyL[0] and emptyB[0]) sendFreqMode(); // without ACK
            break;
        case CAT_HAMLIB_EEPROM:
            readEeprom();
            break;
        case CAT_RX_DATA_CMD:
            if (emptyB[1]) rxStatus(); // without ACK
            break;
        case CAT_TX_DATA_CMD:
            if (emptyB[2]) sendTxStatus(); // without ACK
            break;
        default:
            Serial.write(ACK);
            break;
    }
}

// set a frequency
void ft857d::fset() {
    // reconstruct the freq from the bytes we got
    from_bcd_be();

    // call the function with the freq as parameter
    longf[0](freq);
}

// send the TX status
void ft857d::sendTxStatus() {
    // just one byte with the format the CAT expect, see the exemple in the library

    // get it
    nullPad[0] = emptyB[2]();

    // send it
    sent(1);
}

// send freq and mode
void ft857d::sendFreqMode() {
    // this function must return 5 bytes via the serial port, the first four
    // are the freq in BCD BE and the 5th is the mode

    // clear the nullpad
    npadClear();

    // put the freq in the nullPad 4 first bytes
    to_bcd_be(emptyL[0]());

    // put the mode in the last byte
    nullPad[4] = emptyB[0]();

    // sent it
    sent(5);
}

// READ EEPROM, tis is a trick of Hamlib
void ft857d::readEeprom() {
    // This is to make hamlib happy, PC requested reading two bytes
    // we must answer with two bytes, we forge it as empty ones or...
    // if the second byte in the request is 0x78 we have to send the first
    // with the 5th bit set if the USB or zero if LSB.

    // mem zone to "read"
    byte temp = nullPad[1];

    // clear the nullpad
    npadClear();

    // The user registered the function?
    if (emptyB[0] and temp == 0x78) {
        // get the data in place
        nullPad[0] = emptyB[0]();

        // check, it must be a bit argument
        if (nullPad[0] != 0) nullPad[0] = 1<<5;
    }

    // sent the data
    sent(2);
}

// read the rx Status
void ft857d::rxStatus() {
    /*
     * Data to be returned
     *    D1 = {0xij} i = 0 = squelch off
     *                i = 1 = squelch on
     *                j = 0 = CTCSS/DCS matched
     *                j = 1 = CTCSS/DCS unmatched
     *    D2 = {0xkl} k = 0 = discriminator centered
     *                k = 1 = discriminator offcentered
     *                l = dummy data
     *    D3-D4 = S-meter data
     *
    */

    // clear the nullpad
    npadClear();

    // we only return the s-meter here, just the 4 bits.
    nullPad[0] = emptyB[1]() & 0b00001111;

    // sent it
    sent(1);
}

// procedure to clear the nullpad
void ft857d::npadClear() {
    // this is used to initialize the nullpad
    for (byte i=0; i<5; i++) nullPad[i] = 0;
}

// sent the data to the PC
void ft857d::sent(byte amount) {
    // sent the nullpad content
    for (byte i=0; i<amount; i++) Serial.write(nullPad[i]);
}

/*
 * This two functions that follows was taken and adapted from hamlib code
 */

// put the freq in the nullpad array
void ft857d::to_bcd_be(long f) {
    unsigned char a;

    // the freq is sent the 10th of the hz
    f /= 100;

    // clear the nullpad
    npadClear();

    // do the magic
    nullPad[3] &= 0x0f;
    nullPad[3] |= (f%10)<<4;
    f /= 10;

    for (int i=2; i >= 0; i--) {
        a = f%10;
        f /= 10;
        a |= (f%10)<<4;
        f /= 10;
        nullPad[i] = a;
    }
}

// put the freq in the freq var from the nullpad array
void ft857d::from_bcd_be() {
    // {0x01,0x40,0x07,0x00,0x01} tunes to 14.070MHz
    freq = 0;
    for (byte i=0; i<4; i++) {
        freq *= 10;
        freq += nullPad[i]>>4;
        freq *= 10;
        freq += nullPad[i] & 0x0f;
    }

    freq *= 10;
    freq += nullPad[4]>>4;
}
