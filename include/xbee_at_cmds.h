/**
 * @file xbee_at_cmds.h
 * @brief XBee AT Commands Header
 * @version 1.1
 * @date 2024-08-08
 *
 * @license MIT
 *
 * This file contains definitions and functions for interacting with XBee modules
 * using AT commands. The AT commands allow for configuration and control of the
 * XBee module, including setting network parameters, reading device information,
 * and managing device operations.
 *
 * Example usage:
 * @code
 * uint8_t param[] = { 0x01 };
 * at_command_send(AT_ID, param, sizeof(param));
 * @endcode
 *
 * @note Ensure the UART is properly initialized before sending any AT commands.
 *
 * @author Felix Galindo, Digi International
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
    AT_,
    AT_DE,   /**< Device EUI (LoRaWAN specific) */
    AT_FQ,   /**< Test Configuration Frequency */
    AT_PW,   /**< Test Configuration Power */
    // ... (other existing AT commands) ...
    AT_ID,   /**< PAN ID */
    AT_CH,   /**< Operating Channel */
    AT_MY,   /**< 16-bit Source Address */
    AT_DH,   /**< Destination Address High */
    AT_DL,   /**< Destination Address Low */
    AT_CN,   /**< Exit Command Mode */
    AT_AP,   /**< API Enable */
    AT_NI,   /**< Node Identifier */
    AT_BD,   /**< Baud Rate */
    AT_WR,   /**< Write to non-volatile memory */
    AT_RE,   /**< Restore factory defaults */
    AT_VR,   /**< Firmware Version */
    AT_CE,   /**< Coordinator Enable */
    AT_RO,   /**< Packetization Timeout */
    AT_ND,   /**< Node Discover */
    AT_AC,   /**< Apply Changes */
    AT_NR    /**< Network Reset */
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

