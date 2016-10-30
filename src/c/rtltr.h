/*
 * Copyright (c) 2016 Gavriel Fleischer <flocsy@gmail.com>
 */

#pragma once
#include <pebble.h>

//#define RTLTR_INCLUDE_ARABIC 0

// Persistent storage key for RTLTR enabled/disabled setting
#define RTLTR_KEY 42

//void rtltr_save_settings();
void rtltr_load_settings();
void rtltr_enable(bool enable);
bool rtltr_is_enabled();
void rtltr_inbox_received_handler(DictionaryIterator *iter, void *context);

void rtltr_ensure_registered_string_arrays_capacity(size_t capacity);
void rtltr_register_string_array(char* str_arr[], size_t str_arr_size);
void rtltr_ensure_registered_strings_capacity(size_t capacity);
void rtltr_register_string(char* str);

void rtltr_register_callback_after_reverse_registered_strings(void (*callback)(void));

//void rtltr_reverse_string(const char* str);
void rtltr_reverse_registered_strings();
