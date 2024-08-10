/**
 * @file xbee_at_cmds.c
 * @brief XBee AT Commands
 * @version 1.0
 * @date 2024-08-08
 * 
 * @license MIT
 * 
 * @author Felix Galindo, Digi International
 * @contact felix.galindo@digi.com
 */

#include "xbee_at_cmds.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>

// AT Command Functions

// Convert AT command enum to string
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
        default: return NULL;
    }
}


