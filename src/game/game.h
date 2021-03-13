/**
 * utils/game.h
 *
 * Game value definitions
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef _asc_game_constants
#define _asc_game_constants

// Define score values
#define GOAL0_SCORE 0
#define GOAL1_SCORE 0
#define GOAL2_SCORE 0
#define GOAL3_SCORE 0

// Define an alliance color enum
#ifndef __py_parser // the HeaderParser doesn't need to parse this

typedef enum AllianceColor_t {
    RED, BLUE
} AllianceColor;

#endif

#endif
