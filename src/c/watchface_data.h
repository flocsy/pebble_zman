#pragma once

#include <pebble.h>

#define NUM_ZMANIM 11
#define LEN_ZMANIM 20
extern char zman_names[NUM_ZMANIM][LEN_ZMANIM];
#define NUM_NUMBERS 30
#define LEN_NUMBERS 5
extern char hebrewNumbers[NUM_NUMBERS][LEN_NUMBERS];

#define NUM_CLOCK_TICKS 13
extern const struct GPathInfo ANALOG_BG_POINTS[NUM_CLOCK_TICKS];
extern const GPathInfo MINUTE_HAND_POINTS;
extern const GPathInfo HOUR_HAND_POINTS;
