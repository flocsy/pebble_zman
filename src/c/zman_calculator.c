/*
 * based on
 * - http://williams.best.vwh.net/sunrise_sunset_algorithm.htm
 */
#include "zman_calculator.h"
#include "my_math.h"

float calcSun(int year, int month, int day, float latitude, float longitude, int sunset, float zenith)
{
  int N1 = my_floor(275 * month / 9);
  int N2 = my_floor((month + 9) / 12);
  int N3 = (1 + my_floor((year - 4 * my_floor(year / 4) + 2) / 3));
  int N = N1 - (N2 * N3) + day - 30;

  float lngHour = longitude / 15;
  
  float t;
  if (!sunset)
  {
    //if rising time is desired:
    t = N + ((6 - lngHour) / 24);
  }
  else
  {
    //if setting time is desired:
    t = N + ((18 - lngHour) / 24);
  }

  float M = (0.9856 * t) - 3.289;

  //calculate the Sun's true longitude
  //L = M + (1.916 * sin(M)) + (0.020 * sin(2 * M)) + 282.634
  float L = M + (1.916 * my_sin((M_PI/180.0f) * M)) + (0.020 * my_sin((M_PI/180.0f) * 2 * M)) + 282.634;
  if (L<0) L+=360.0f;
  if (L>360) L-=360.0f;

  //5a. calculate the Sun's right ascension
  //RA = atan(0.91764 * tan(L))
  float RA = (180.0f/M_PI) * my_atan(0.91764 * my_tan((M_PI/180.0f) * L));
  if (RA<0) RA+=360;
  if (RA>360) RA-=360;

  //5b. right ascension value needs to be in the same quadrant as L
  float Lquadrant  = (my_floor( L/90)) * 90;
  float RAquadrant = (my_floor(RA/90)) * 90;
  RA = RA + (Lquadrant - RAquadrant);

  //5c. right ascension value needs to be converted into hours
  RA = RA / 15;

  //6. calculate the Sun's declination
  float sinDec = 0.39782 * my_sin((M_PI/180.0f) * L);
  float cosDec = my_cos(my_asin(sinDec));

  //7a. calculate the Sun's local hour angle
  //cosH = (cos(zenith) - (sinDec * sin(latitude))) / (cosDec * cos(latitude))
  float cosH = (my_cos((M_PI/180.0f) * zenith) - (sinDec * my_sin((M_PI/180.0f) * latitude))) / (cosDec * my_cos((M_PI/180.0f) * latitude));
  
  if (cosH >  1) {
    return 0;
  }
  else if (cosH < -1)
  {
    return 0;
  }
    
  //7b. finish calculating H and convert into hours
  
  float H;
  if (!sunset)
  {
    //if rising time is desired:
    H = 360 - (180.0f/M_PI) * my_acos(cosH);
  }
  else
  {
    //if setting time is desired:
    H = (180.0f/M_PI) * my_acos(cosH);
  }
  
  H = H / 15;

  //8. calculate local mean time of rising/setting
  float T = H + RA - (0.06571 * t) - 6.622;

  //9. adjust back to UTC
  float UT = T - lngHour;
  if (UT<0) {UT+=24;}
  if (UT>24) {UT-=24;}
  
  time_t now = time(NULL);
struct tm *tick_time = localtime(&now);
struct tm *gm_time = gmtime(&now);
  
int timezoneoffset = 60 * (60 * (24 * (tick_time->tm_wday - gm_time->tm_wday) + tick_time->tm_hour - gm_time->tm_hour) + tick_time->tm_min - gm_time->tm_min);
// Correct for transitions at the end of the week.
  int SECONDS_IN_WEEK=604800;
if (timezoneoffset > SECONDS_IN_WEEK/2) timezoneoffset -= SECONDS_IN_WEEK;
if (timezoneoffset < -SECONDS_IN_WEEK/2) timezoneoffset += SECONDS_IN_WEEK;

  timezoneoffset /= 3600;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "timezone offset %d", timezoneoffset);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "other timezone offset %d", (tick_time-gm_time));
// return UT+(tick_time-gm_time);
  return UT;
}

void calcZmanim(struct Zmanim* out_zmanim, int year, int month, int day, float latitude, float longitude)
{
  float sunrise = calcSunRise( year,  month,  day,  latitude,  longitude,  90.5f);
  float sunset =  calcSunSet( year,  month,  day,  latitude,  longitude,  90.5f);
  float alos = calcSunRise( year,  month,  day,  latitude,  longitude,  106.6f);
  float mishyakir = calcSunRise( year,  month,  day,  latitude,  longitude,  102.0f);
  float tzais= calcSunSet( year,  month,  day,  latitude,  longitude,  99.0f);
  
  time_t day_start=time_start_of_today();
  float haneitz=sunrise*60*60+day_start;
  float shkiya=sunset*60*60+day_start;
  float sha_gra = (shkiya-haneitz)/12.0f;

  float shma_gra=haneitz+sha_gra*3;
  float tefila_gra=haneitz+sha_gra*4;
  float chatzos=haneitz+sha_gra*6;
  float mincha_gedloa=haneitz+sha_gra*6.5;
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "start of today %d", (int) day_start);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "sha gra %d", (int) sha_gra);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "tefila gra %d", (int) tefila_gra);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "sunrise %d", (int) sunrise*1000);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "sunset %d", (int) sunset*1000);
   APP_LOG(APP_LOG_LEVEL_DEBUG, "haneitz %d", (int) haneitz); 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "shkiya %d", (int) shkiya);
  
  out_zmanim->alos=alos*60*60+day_start;
  out_zmanim->mishyakir=mishyakir*60*60+day_start;
  out_zmanim->haneitz=haneitz;
   out_zmanim->shma_gra=shma_gra;
   out_zmanim->tefila_gra=tefila_gra;
   out_zmanim->chatzos=chatzos;
   out_zmanim->mincha_gedloa=mincha_gedloa;
   out_zmanim->shkiya=shkiya;
  out_zmanim->tzais=tzais*60*60+day_start;
  out_zmanim->tzais_rt=(sunset*60*60+day_start)+72*60;
  out_zmanim->chatzos_halaila=chatzos+(12*60*60);
  
   APP_LOG(APP_LOG_LEVEL_DEBUG, "chatzos  %d", (int) out_zmanim->chatzos);
   APP_LOG(APP_LOG_LEVEL_DEBUG, "chatzos haliala %d", (int) out_zmanim->chatzos_halaila);
}



void floatToTm(struct tm* time_sructure, float outFloat){ 
   time_sructure->tm_min = (int)(60*(outFloat-((int)(outFloat))));
   time_sructure->tm_hour = (int)outFloat - 12;
}

float calcSunRise(int year, int month, int day, float latitude, float longitude, float zenith)
{
  return calcSun(year, month, day, latitude, longitude, 0, zenith);
}

float calcSunSet(int year, int month, int day, float latitude, float longitude, float zenith)
{
  return calcSun(year, month, day, latitude, longitude, 1, zenith);
}