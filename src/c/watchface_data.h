#pragma once

#include <pebble.h>

#define LAT_KEY 35
#define LONG_KEY 36

#define NUM_ZMANIM 11
extern char zman_names[NUM_ZMANIM][20];
extern char hebrewNumbers[30][6];

#define NUM_CLOCK_TICKS 13
extern const struct GPathInfo ANALOG_BG_POINTS[NUM_CLOCK_TICKS];
extern const GPathInfo MINUTE_HAND_POINTS;
extern const GPathInfo HOUR_HAND_POINTS;
