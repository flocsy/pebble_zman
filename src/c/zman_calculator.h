#define ZENITH_OFFICIAL 90.83
#define ZENITH_CIVIL    96.0
#define ZENITH_NAUTICAL 102.0
#define ZENITH_ASTRONOMICAL 108.0

#include "pebble.h"

struct Zmanim {
    time_t  alos;
     time_t  mishyakir;
    time_t haneitz;
    time_t shma_gra;
    time_t tefila_gra;
    time_t chatzos;
     time_t  mincha_gedloa;
    time_t shkiya;
    time_t  tzais;
     time_t  tzais_rt;
    time_t  chatzos_halaila;
};

float calcSun(int year, int month, int day, float latitude, float longitude, int sunset, float zenith);
void calcZmanim(struct Zmanim* out_zmanim, int year, int month, int day, float latitude, float longitude);
void floatToTm(struct tm* time, float outFloat);
float calcSunRise(int year, int month, int day, float latitude, float longitude, float zenith);
float calcSunSet(int year, int month, int day, float latitude, float longitude, float zenith);
