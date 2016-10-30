#pragma once

#include "pebble.h"

#define NUM_CLOCK_TICKS 11
#define NUM_ZMANIM 11

const int LAT_KEY=35;
const int LONG_KEY=36;

char *zman_names[NUM_ZMANIM] = {"רחשה תולע",
                             "ריכישמ",
                             "המחה ץנה",
                             "א''רג עמש", 
                             "א''רג הליפת",
                             "תוצח",
                             "הלודג החנמ", 
                             "המחה תעיקש", 
                             "םיבככה תאצ",
                            "ת''ר תאצ",
                            "הלילה תוצח"};

static const struct GPathInfo ANALOG_BG_POINTS[] = {
  { 4,
    (GPoint []) {
      {68, 0},
      {71, 0},
      {71, 12},
      {68, 12}
    }
  },
  { 4, (GPoint []){
      {72, 0},
      {75, 0},
      {75, 12},
      {72, 12}
    }
  },
  { 4, (GPoint []){
      {112, 10},
      {114, 12},
      {108, 23},
      {106, 21}
    }
  },
  { 4, (GPoint []){
      {132, 47},
      {144, 40},
      {144, 44},
      {135, 49}
    }
  },
  { 4, (GPoint []){
      {135, 118},
      {144, 123},
      {144, 126},
      {132, 120}
    }
  },
  { 4, (GPoint []){
      {108, 144},
      {114, 154},
      {112, 157},
      {106, 147}
    }
  },
  { 4, (GPoint []){
      {70, 155},
      {73, 155},
      {73, 167},
      {70, 167}
    }
  },
  { 4, (GPoint []){
      {32, 10},
      {30, 12},
      {36, 23},
      {38, 21}
    }
  },
  { 4, (GPoint []){
      {12, 47},
      {-1, 40},
      {-1, 44},
      {9, 49}
    }
  },
  { 4, (GPoint []){
      {9, 118},
      {-1, 123},
      {-1, 126},
      {12, 120}
    }
  },
  { 4, (GPoint []){
      {36, 144},
      {30, 154},
      {32, 157},
      {38, 147}
    }
  },

};

static const GPathInfo MINUTE_HAND_POINTS = {
14,
(GPoint []) {
{-2, -4 },
{ 2, -4 },
{ 2, -10 },
{ 3, -11 },
{ 4, -12 },
{ 4, -68 },
{ 3, -69 },
{ 2, -70 },
{-2, -70 },
{-3, -69 },
{-4, -68 },
{-4, -12 },
{-3, -11 },
{-2, -10 }

}
};
static const GPathInfo HOUR_HAND_POINTS = {
12,
(GPoint []){
{-2, -4 },
{ 2, -4 },
{ 2, -10 },
{ 4, -10 },
{ 4, -38 },
{ 3, -39 },
{ 2, -40 },
{-2, -40 },
{-3, -39 },
{-4, -38 },
{-4, -10 },
{-2, -10 }

}
};

static char *hebrewNumbers[] = {"א","ב","ג","ד","ה","ו","ז","ח","ט","י","אי","בי","גי","די","וט","זט","זי","חי","טי","כ","אכ","בכ","גכ","דכ","הכ","וכ","זכ","חכ","טכ","ל"};

