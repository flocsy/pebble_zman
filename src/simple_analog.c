#include "simple_analog.h"

#include "pebble.h"
#include "hebrewdate.h"

static Window *s_window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer;
static TextLayer *s_zmanlabel_label, *s_zmantime_label, *s_hebday_label, *s_gregday_label;

static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPath *s_minute_arrow, *s_hour_arrow;
static char s_num_buffer[4], s_day_buffer[6], s_hebday_buffer[3];

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

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  //draw zman label
  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  text_layer_set_text(s_zmanlabel_label, "עמש ז''ס");

  //draw zman time
  text_layer_set_text(s_zmantime_label, "55:55");

  //draw gregdate
  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  text_layer_set_text(s_gregday_label, s_num_buffer);

  //draw hebdate
  int julianDay = hdate_gdate_to_jd(t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
  //Convert julian day to hebrew date
  int hDay, hMonth, hYear, hDayTishrey, hNextTishrey;
  hdate_jd_to_hdate(julianDay, &hDay, &hMonth, &hYear, &hDayTishrey, &hNextTishrey);
  text_layer_set_text(s_hebday_label, hebrewNumbers[hDay-1]);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);

  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);

  //create zman title layer
  s_zmanlabel_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(63, 114, 40, 20),
    GRect(46, 110, 80, 20)));
  text_layer_set_text(s_zmanlabel_label, s_day_buffer);
  text_layer_set_background_color(s_zmanlabel_label, GColorBlack);
  text_layer_set_text_color(s_zmanlabel_label, GColorWhite);
  text_layer_set_font(s_zmanlabel_label, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  //add to date layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_zmanlabel_label));

  //create zman time layer
  s_zmantime_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(90, 114, 18, 20),
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
  text_layer_set_font(s_hebday_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
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
}

static void parse_zman_tuple(Tuple *zman_tuple ){
  static char conditions_buffer[5];
  snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", zman_tuple->value->cstring);
  int hourTens = conditions_buffer[0] - '0';
  int hourOnes = conditions_buffer[1] - '0';
  int minuteTens = conditions_buffer[2] - '0';
  int minuteOnes = conditions_buffer[3] - '0';
  APP_LOG(APP_LOG_LEVEL_DEBUG, "first dig %d%d:%d%d", hourTens,hourOnes,minuteTens,minuteOnes);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
   // Read tuples for data
  zmanim[0] = dict_find(iterator, MESSAGE_KEY_ALOS);
  zmanim[1] = dict_find(iterator, MESSAGE_KEY_MISHEYAKIR);
  zmanim[2] = dict_find(iterator, MESSAGE_KEY_NEITZ);
  zmanim[3] = dict_find(iterator, MESSAGE_KEY_SHMA_GRA);
  zmanim[4] = dict_find(iterator, MESSAGE_KEY_TEFILA_GRA);
  zmanim[5] = dict_find(iterator, MESSAGE_KEY_CHATZOS);
  zmanim[6] = dict_find(iterator, MESSAGE_KEY_MINCHA_GEDOLA);
  zmanim[7] = dict_find(iterator, MESSAGE_KEY_SHKIA);
  zmanim[8] = dict_find(iterator, MESSAGE_KEY_TZAIS);
  
  for(int i = 0 ; i<9 ; i++){
    parse_zman_tuple(zmanim[i]);
  }
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
  // Open AppMessage
  const int inbox_size = 62;
  const int outbox_size = 62;
  app_message_open(inbox_size, outbox_size);
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
