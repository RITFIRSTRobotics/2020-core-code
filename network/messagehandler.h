//
// Created by Alex Kneipp on 2/15/20.
//

#ifndef INC_2020_CORE_CODE_MESSAGEHANDLER_H
#define INC_2020_CORE_CODE_MESSAGEHANDLER_H

#include "packet.h"
#include "lowlevel.h"

#define MH_SUCCESS 0
#define MH_ERR_ALREADY_INITIALIZED 1
#define MH_ERR_INITIALIZATION_FAILED 2
#define MH_WRN_INITIALIZATION_INCOMPLETE 3

/**
 * Initializes internal memory for the message handler.
 *
 * @return
 *  An error code
 *  ERROR CODE | Reason
 *  -------------------
 *  MH_SUCCESS | No error
 *  MH_ERR_ALREADY_INITIALIZED | The MessageHandler has already been initialized and shouldn't be done again
 *  MH_ERR_INITIALIZATION_FAILED | The MessageHandler initialization failed in a non-recoverable
 *  MH_WRN_INITIALIZATION_INCOMPLETE | The MessageHandler was not completely initialized, but will attempt to repair functionality as it is used
 *
 *
 * If MH_WRN_INITIALIZATION_INCOMPLETE is returned, later function calls may return errors; ensure they are properly handled.
 */
int mh_init();

#endif //INC_2020_CORE_CODE_MESSAGEHANDLER_H
