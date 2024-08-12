/**
 * @file xbee_at_cmds.h
 * @brief Header file for XBee AT commands.
 * 
 * This file defines the interface for working with XBee AT commands. AT commands
 * are used for configuring and controlling the XBee module. 
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


#ifndef XBEE_AT_H
#define XBEE_AT_H

#include <stdint.h>

/**
 * @enum at_command_t
 * @brief Enumeration of common XBee AT commands.
 *
 * This enum defines the AT commands supported by the XBee module. Each command
 * corresponds to a specific configuration or control operation within the module.
 */
typedef enum {
    /**< XBee Common AT Commands */
    AT_,
    AT_CN,   /**< Exit Command Mode */
    AT_AP,   /**< API Enable */
    AT_BD,   /**< Baud Rate */
    AT_WR,   /**< Write to non-volatile memory */
    AT_RE,   /**< Restore factory defaults */
    AT_VR,   /**< Firmware Version */
    AT_AC,   /**< Apply Changes */
    AT_NR,   /**< Network Reset */

    /**< XBee LR Specific AT Commands */
    AT_DE,   /**< LoRaWAN Device EUI */
    AT_AK,   /**< LoRaWAN Application Key */
    AT_AE,   /**< LoRaWAN Application EUI */
    AT_NK,   /**< LoRaWAN Network Key */
    AT_JS,   /**< LoRaWAN Join Status */
    AT_FQ,   /**< Test Configuration Frequency */
    AT_PW,   /**< Test Configuration Power */

    // ... (other existing AT commands) ...


} at_command_t;

/**
 * @brief Converts an AT command enum to its corresponding string representation.
 *
 * This function takes an AT command enum and returns the corresponding string representation
 * that can be sent to the XBee module.
 *
 * @param command The AT command enum to convert.
 * @return const char* The string representation of the AT command.
 */
const char* at_command_to_string(at_command_t command);

#endif // XBEE_AT_H

