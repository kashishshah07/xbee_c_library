/**
 * @file xbee_at_cmds.c
 * @brief Implementation of XBee AT commands.
 * 
 * This file contains the implementation of functions used to handle and send AT commands
 * to the XBee module. 
 * 
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @author Felix Galindo
 * @contact felix.galindo@digi.com
 */

#include "xbee_at_cmds.h"
#include "port.h"
#include <stdio.h>
#include <string.h>

// AT Command Functions

/**
 * @brief Converts an AT command enum to its string representation.
 * 
 * This function takes an AT command enum value and returns the corresponding
 * string representation of the command.
 * 
 * @param[in] command The AT command enum value.
 * 
 * @return The string representation of the AT command.
 */
const char* at_command_to_string(at_command_t command) {
    switch (command) {
        case AT_: return "AT";
        case AT_DE: return "DE";
        case AT_FQ: return "FQ";
        case AT_BD: return "BD";
        case AT_AK: return "AK";
        case AT_AE: return "AE";
        case AT_NK: return "NK";
        case AT_JS: return "JS";
        case AT_WR: return "WR";
        case AT_AC: return "AC";
        default: return NULL;
    }
}


