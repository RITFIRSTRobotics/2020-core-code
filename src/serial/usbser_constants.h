/**
 * serial/usbser_constants.hpp
 *
 * A bunch of constants used for serial transmission between the FMS and the ASC
 */
#ifndef _serial_constants_hpp
#define _serial_constants_hpp

#define BAUD_RATE 115200 // Baud Rate of the connection

#define NEWLINE '\n' // newline character and end-of-line character

/**************************************************************************
 * Transmitted codes
 *
 * These codes are designed to be as short as possible in the name of speed
 *************************************************************************/

/*
 * Defines the basic data delimiter
 */
#define DELIMITER ':'


/*
 * Message that tells the ASC to blink it's debug LED
 *
 * @param %u mode parameter, currently unused
 */
#define BLINK_MESSAGE "b0:%u"

/*
 * Message that tells the ASC to initialize
 *
 * @param %c the ASC color ('r' for red or 'b' for blue)
 */
#define INIT_MESSAGE "i0:%c"

/*
 * Response from the ASC once initialization has completed successfully
 *
 * @param %u return code, currently unused
 */
#define INIT_RESPONSE "i1:%u"


/*
 * Message that tells the ASC to calibrate a goal
 *
 * @param %1u the goal number
 */
#define CALIBRATE_MESSAGE "c0:%1u"

/*
 * Response from the ASC once the goal has been calibrated
 *
 * @param %u return code, currently unused
 */
#define CALIBRATE_RESPONSE "c1:%u"

/*
 * Message from the ASC/controller with data about the current state of a controller
 *
 * @param %u the controller number (0, 1, or 2)
 * @param %u the joystick0 X value (0 to 255)
 * @param %u the joystick0 Y value (0 to 255)
 * @param %u the joystick1 X value (0 to 255)
 * @param %u the joystick1 Y value (0 to 255)
 * @param %u the button bitmask (bit 1 is button 0, bit 2 is button 1, etc)
 */
#define CONTROLLER_DATA "md:%u:%u:%u:%u:%u:%u"


/*
 * Message from the ASC made when a goal has been scored
 *
 * @param %u the goal number (0, 1, 2, or 3)
 */
#define SCORE_DATA "sd:%u"

/*
 * Message that tells the ASC to change the state of the ball control fan
 *
 * @param %u the fan to control (0, 1, 2, or 3)
 * @param %u the PWM fan value (0 to 255)
 */
#define BALL_RETURN_CONTROL "bd:%u:%u"


/*
 * Message that tells the ASC to set it's LED strip to be a solid color
 *
 * @param %c which side of the strip should be set ('c' for the close side, 'f' for the far side, 'a' for all)
 * @param %u the red LED value (0 to 255)
 * @param %u the green LED value (0 to 255)
 * @param %u the blue LED value (0 to 255)
 */
#define LED_STRIP_SOLID "ls:%c:%u:%u:%u"

/*
 * Message that tells the ASC to set it's LED strip to a color in a wave
 *
 * @param %c which side of the strip should be set ('c' for the close side, 'f' for the far side, 'a' for all)
 * @param %u the red LED value (0 to 255)
 * @param %u the green LED value (0 to 255)
 * @param %u the blue LED value (0 to 255)
 */
#define LED_STRIP_WAVE  "lc:%c:%u:%u:%u"

/*
 * Message that tells the ASC to set part of it's LED strip to a color
 *
 * @param %c which side of the strip should be set ('c' for the close side, 'f' for the far side, 'a' for all)
 * @param %u the number of LEDs to be set (LEDs are set from the ends towards the center)
 * @param %u the red LED value (0 to 255)
 * @param %u the green LED value (0 to 255)
 * @param %u the blue LED value (0 to 255)
 */
#define LED_STRIP_NUM   "ln:%c:%u:%u:%u:%u"

/*
 * Message that tells the ASC to set one LED on the strip to a color
 *
 * @param %u the number of the LED to set (0 to LED_NUM)
 * @param %u the red LED value (0 to 255)
 * @param %u the green LED value (0 to 255)
 * @param %u the blue LED value (0 to 255)
 */
#define LED_STRIP_ONE   "lo:%u:%u:%u:%u" // set a single LED on the strip to a color


/*
 * Message that tells the ASC to start generating the idle pattern, starting at a given position.
 * The ASC should only stop generating the pattern if it reaches the end of it's strip or it is interrupted by a stop command
 *
 * @param %u the initial red LED value (0 to 255)
 * @param %u the initial green LED value (0 to 255)
 * @param %u the initial blue LED value (0 to 255)
 */
#define LED_STRIP_AUTOWAVE_START   "ws:%u:%u:%u"

/*
 * Message that tells the ASC to stop generating the idle pattern
 *
 * @param %u the number of cycles that it should continue for
 */
#define LED_STRIP_AUTOWAVE_STOP    "wp:%u"

/*
 * Response from the ASC that contains information about the value of lsat generated value
 *
 * @param %u the last red LED value (0 to 255)
 * @param %u the last green LED value (0 to 255)
 * @param %u the last blue LED value (0 to 255)
 */
#define LED_STRIP_AUTOWAVE_RESULTS "wr:%u:%u:%u" // the ASC should return this with an RGB value

#endif
