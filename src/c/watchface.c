#include "watchface.h"

#include "pebble.h"
#include "hebrewdate.h"
#include "zman_calculator.h"
#include "pebble-rtltr/rtltr.h"

int isdigit(int c)
{
  return (c >= '0' && c <= '9' ? 1 : 0);
}

double str_to_double(char *s)
{
  double a = 0.0;
  int e = 0;
  int c;
  while ((c = *s++) != '\0' && isdigit(c)) {
    a = a*10.0 + (c - '0');
  }
  if (c == '.') {
    while ((c = *s++) != '\0' && isdigit(c)) {
      a = a*10.0 + (c - '0');
      e = e-1;
    }
  }
  if (c == 'e' || c == 'E') {
    int sign = 1;
    int i = 0;
    c = *s++;
    if (c == '+')
      c = *s++;
    else if (c == '-') {
      c = *s++;
      sign = -1;
    }
    while (isdigit(c)) {
      i = i*10 + (c - '0');
      c = *s++;
    }
    e += i*sign;
  }
  while (e > 0) {
    a *= 10.0;
    e--;
  }
  while (e < 0) {
    a *= 0.1;
    e++;
  }
  return a;
}

static Window *s_window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer;
static TextLayer *s_zmanlabel_label, *s_zmantime_label, *s_hebday_label, *s_gregday_label;

static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPath *s_minute_arrow, *s_hour_arrow;
static char s_num_buffer[4], s_day_buffer[6], s_hebday_buffer[3], s_zmantime_buffer[6];

static GFont s_zmanlabel_font;

static double longitude;
static double latitude;

struct Zmanim MyZmanim;    
time_t current_zman;
int current_zman_name=0;

static void bg_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWhite);
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    const int x_offset = PBL_IF_ROUND_ELSE(18, 0);
    const int y_offset = PBL_IF_ROUND_ELSE(6, 0);
    gpath_move_to(s_tick_paths[i], GPoint(x_offset, y_offset));
    gpath_draw_filled(ctx, s_tick_paths[i]);
  }
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  const int16_t second_hand_length = PBL_IF_ROUND_ELSE((bounds.size.w / 2) - 19, bounds.size.w / 2);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  GPoint second_hand = {
    .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
  };

  // second hand
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, second_hand, center);

  // minute/hour hand
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);

  //draw a circle
  uint16_t radius = 5;
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, radius);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_circle(ctx, center, radius);
}

static void update_hebrew_date(){
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  if(now>MyZmanim.shkiya){
    t->tm_mday+=1;
  }
  
  //draw hebdate
  int julianDay = hdate_gdate_to_jd(t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
  //Convert julian day to hebrew date
  int hDay, hMonth, hYear, hDayTishrey, hNextTishrey;
  hdate_jd_to_hdate(julianDay, &hDay, &hMonth, &hYear, &hDayTishrey, &hNextTishrey);
  text_layer_set_text(s_hebday_label, hebrewNumbers[hDay-1]);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  //draw gregdate
  strftime(s_num_buffer, sizeof(s_num_buffer), "%e", t);
  text_layer_set_text(s_gregday_label, s_num_buffer);

  update_hebrew_date(); 
}

static void refresh_current_zman(){
  current_zman = MyZmanim.alos;
    time_t now = time(NULL);
  current_zman_name=0;
  
  if(now>MyZmanim.alos){
    current_zman_name=1;
    current_zman = MyZmanim.mishyakir;
  }
  
  if(now>MyZmanim.mishyakir){
    current_zman_name=2;
    current_zman = MyZmanim.haneitz;
  }
  
  if(now>MyZmanim.haneitz){
    current_zman_name=3;
    current_zman=MyZmanim.shma_gra;
  }
  
  if(now>MyZmanim.shma_gra){
    current_zman_name=4;
    current_zman=MyZmanim.tefila_gra;
  }
  
  if(now>MyZmanim.tefila_gra){
    current_zman_name=5;
    current_zman=MyZmanim.chatzos;
  }
  
  if(now>MyZmanim.chatzos){
    current_zman_name=6;
    current_zman=MyZmanim.mincha_gedloa;
  }
  
  if(now>MyZmanim.mincha_gedloa){
    current_zman_name=7;
    current_zman=MyZmanim.shkiya;
  }
  
  if(now>MyZmanim.shkiya){
    current_zman_name=8;
    current_zman=MyZmanim.tzais;
    update_hebrew_date();
  }
  
  if(now>MyZmanim.tzais){
    current_zman_name=9;
    current_zman=MyZmanim.tzais_rt;
  }
  
  if(now>MyZmanim.tzais_rt){
    current_zman_name=10;
    current_zman=MyZmanim.chatzos_halaila;
  }
}

static void update_zman_layer(){
  refresh_current_zman();

  text_layer_set_text(s_zmanlabel_label, zman_names[current_zman_name]);
  struct tm *zmanT = localtime(&current_zman);
  strftime(s_zmantime_buffer, sizeof(s_zmantime_buffer), "%l:%M", zmanT);
  text_layer_set_text(s_zmantime_label, s_zmantime_buffer);
}

static void recalculate_zmanim(){
  time_t now = time(NULL);
  struct tm *time = localtime(&now);
   
  calcZmanim(&MyZmanim, time->tm_year, time->tm_mon+1, time->tm_mday, latitude, longitude);
    
  update_zman_layer();
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));  
  if ((units_changed & MINUTE_UNIT) != 0) {
    //re-draw zmanim layer every minute
    update_zman_layer();
  }
  if ((units_changed & HOUR_UNIT) != 0) {
    //recalculate zmanim every hour
    recalculate_zmanim();
  }
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  s_zmanlabel_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TAAMEI_FRANK_18));

  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);

  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);

  //create zman title layer
  s_zmanlabel_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(20, 110, 110, 20),
    GRect(20, 110, 110, 20)));
  text_layer_set_text(s_zmanlabel_label, s_day_buffer);
  text_layer_set_background_color(s_zmanlabel_label, GColorClear);
  text_layer_set_text_color(s_zmanlabel_label, GColorWhite);
  text_layer_set_font(s_zmanlabel_label, s_zmanlabel_font);
  text_layer_set_text_alignment(s_zmanlabel_label, GTextAlignmentCenter);
  //add to date layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_zmanlabel_label));

  //create zman time layer
  s_zmantime_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(55, 130, 80, 20),
    GRect(55, 130, 80, 20)));
  text_layer_set_text(s_zmantime_label, s_num_buffer);
  text_layer_set_background_color(s_zmantime_label, GColorBlack);
  text_layer_set_text_color(s_zmantime_label, GColorWhite);
  text_layer_set_font(s_zmantime_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  //add to date layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_zmantime_label));

  //create hebday layer
  s_hebday_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(90, 114, 18, 20),
    GRect(125, 60, 20, 20)));
  text_layer_set_text(s_hebday_label, s_hebday_buffer);
  text_layer_set_background_color(s_hebday_label, GColorBlack);
  text_layer_set_text_color(s_hebday_label, GColorWhite);
  text_layer_set_font(s_hebday_label, s_zmanlabel_font);
  //add to date layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_hebday_label));

  //create gregday layer
  s_gregday_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(90, 114, 18, 20),
    GRect(125, 80, 20, 20)));
  text_layer_set_text(s_gregday_label, s_hebday_buffer);
  text_layer_set_background_color(s_gregday_label, GColorBlack);
  text_layer_set_text_color(s_gregday_label, GColorWhite);
  text_layer_set_font(s_gregday_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  //add to date layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_gregday_label));

  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);
}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
  layer_destroy(s_date_layer);

  text_layer_destroy(s_zmanlabel_label);
  text_layer_destroy(s_zmantime_label);
  text_layer_destroy(s_hebday_label);
  text_layer_destroy(s_gregday_label);

  layer_destroy(s_hands_layer);
  fonts_unload_custom_font(s_zmanlabel_font);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read tuples for data
  Tuple* lat_tup = dict_find(iterator, MESSAGE_KEY_LATITUDE);
  Tuple* long_tup = dict_find(iterator, MESSAGE_KEY_LONGITUDE);
  if (lat_tup && long_tup) {
    char *lat_str = lat_tup->value->cstring;
    char *long_str = long_tup->value->cstring;
   
    if (lat_str && long_str) {
      persist_write_string(LAT_KEY, lat_str);
      persist_write_string(LONG_KEY, long_str);
      
      longitude = str_to_double(long_str);
      latitude = str_to_double(lat_str);
      
      recalculate_zmanim();
    }
  }

  rtltr_inbox_received_handler(iterator, context);
}

void update_hebrew_layers() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "update_hebrew_layers");
  update_hebrew_date();
  update_zman_layer();
}

void init_rtltr(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init_rtltr:0");
  // Open AppMessage connection
//  app_message_register_inbox_received(rtltr_inbox_received_handler);
//  app_message_open(128, 128);

  rtltr_ensure_registered_string_arrays_capacity(2);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init_rtltr:1");
  rtltr_register_string_array(zman_names, NUM_ZMANIM);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init_rtltr:2");
  rtltr_register_string_array(hebrewNumbers, 30);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init_rtltr:3");
  rtltr_register_callback_after_reverse_registered_strings(update_hebrew_layers);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init_rtltr:4");
  rtltr_load_settings();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init_rtltr:5");
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  s_day_buffer[0] = '\0';
  s_num_buffer[0] = '\0';

  // init hand paths
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    s_tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
  }

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

  app_message_register_inbox_received(inbox_received_callback);
  
  char lat_str [9];
  char lng_str [9];
  int lat_result = persist_read_string(LAT_KEY, lat_str,  sizeof(lat_str));
  int long_result = persist_read_string(LONG_KEY, lng_str, sizeof(lng_str));
  
  if(lat_result!=E_DOES_NOT_EXIST && long_result!=E_DOES_NOT_EXIST ){
    longitude = str_to_double(lng_str);
    latitude = str_to_double(lat_str);
    recalculate_zmanim();
  }
  
  // Open AppMessage
  const int inbox_size = 372;
  const int outbox_size = 32;
  app_message_open(inbox_size, outbox_size);

  init_rtltr();
}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_destroy(s_tick_paths[i]);
  }

  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
