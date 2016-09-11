/*************************************************************************
 *
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
 * - James Buck, VE3BUX, FT857D Lib [http://www.ve3bux.com]
 * - Hamlib source code
 * - flrig source code
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

#ifndef ft857d_h
#define ft857d_h

#include "Arduino.h"

// CAT Command definition from VE3BUX library.
// first single command ones (toggle)
#define CAT_LOCK_ON             0x00
#define CAT_LOCK_OFF            0x80
#define CAT_PTT_ON              0x08
#define CAT_PTT_OFF             0x88
#define CAT_CLAR_ON             0x05
#define CAT_CLAR_OFF            0x85
#define CAT_SPLIT_ON            0x02
#define CAT_SPLIT_OFF           0x82
#define CAT_VFO_AB              0x81
// now complex ones
#define CAT_FREQ_SET            0x01
#define CAT_MODE_SET            0x07
#define CAT_CLAR_SET            0xF5
#define CAT_RX_DATA_CMD         0xE7
#define CAT_TX_DATA_CMD         0xF7
#define CAT_RX_FREQ_CMD         0x03
#define CAT_RPTR_OFFSET_CMD     0x09
#define CAT_RPTR_FREQ_SET       0xF9
#define CAT_SQL_CMD             0x0A
// modes definition
#define CAT_MODE_LSB            0x00
#define CAT_MODE_USB            0x01
#define CAT_MODE_CW             0x02
#define CAT_MODE_CWR            0x03
#define CAT_MODE_AM             0x04
#define CAT_MODE_FM             0x08
#define CAT_MODE_DIG            0x0A
#define CAT_MODE_PKT            0x0C
#define CAT_MODE_FMN            0x88
// SQL modes
#define CAT_SQL_DCS             0x0A
#define CAT_SQL_DCS_DECD        0x0B
#define CAT_SQL_DCS_ENCD        0x0C
#define CAT_SQL_CTCSS           0x2A
#define CAT_SQL_CTCSS_DECD      0x3A
#define CAT_SQL_CTCSS_ENCD      0x4A
#define CAT_SQL_OFF             0x8A
#define CAT_SQL_CTCSS_SET       0x0B
#define CAT_SQL_DCS_SET         0x0C
// RPT related
#define CAT_RPTR_OFFSET_N       0x09
#define CAT_RPTR_OFFSET_P       0x49
#define CAT_RPTR_OFFSET_S       0x89
// HAMLIB ones
#define CAT_HAMLIB_EEPROM       0xBB


// defining the funtion type by params
typedef void (*FuncPtrVoid)(void);
typedef unsigned long (*FuncPtrVoidLong)(void);
typedef byte (*FuncPtrVoidByte)(void);
typedef void (*FuncPtrToggles)(boolean);
typedef void (*FuncPtrByte)(byte);
typedef void (*FuncPtrULong)(unsigned long);


/*
 * Arduino defines some custom serial modes before hand, if you use it must
 * be one of these:
 *  SERIAL_5N1; SERIAL_6N1; SERIAL_7N1; SERIAL_8N1; SERIAL_5N2; SERIAL_6N2;
 *  SERIAL_7N2; SERIAL_8N2; SERIAL_5E1; SERIAL_6E1; SERIAL_7E1; SERIAL_8E1;
 *  SERIAL_5E2; SERIAL_6E2; SERIAL_7E2; SERIAL_8E2; SERIAL_5O1; SERIAL_6O1;
 *  SERIAL_7O1; SERIAL_8O1; SERIAL_5O2; SERIAL_6O2; SERIAL_7O2; SERIAL_8O2
 */

/*
 * The class...
 */
class ft857d {
 public:
    // we have two kind of constructors here
    void begin(); // default for the radio 9600 @ 8N2
    void begin(unsigned long baudrate, int mode); // custom baudrate and mode
    void check(); // periodic check for serial commands
    // this functions are
    void addCATLock(void (*)(boolean));
    void addCATPtt(void (*)(boolean));
    void addCATSplit(void (*)(boolean));
    void addCATAB(void (*)(void));
    void addCATFSet(void (*)(unsigned long));
    void addCATMSet(void (*)(byte));
    void addCATGetFreq(unsigned long (*)(void));
    void addCATGetMode(byte (*)(void));
    void addCATSMeter(byte (*)(void));
    void addCATTXStatus(byte (*)(void));

 private:
    byte rxBufCount     = 0;
    byte nullPad[5]     = {0,0,0,0,0};
    unsigned long freq  = 0;
    byte ACK            = 0;
    void setFreq(void);
    void from_bcd_be(void);
    void to_bcd_be(unsigned long);
    void sendFreqMode(void);
    void rxStatus(void);
    void sent(void);
    void readEeprom(void);
    void npadClear(void);
    void sendTxStatus(void);
    void sent(byte);
    void fset(void);
    void sendACK(void);
};

#endif
