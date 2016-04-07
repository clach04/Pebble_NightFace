#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  
  // Display this time on the Textlayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void update_timer_callback(void *data) {
  // Make Time Invisible
  text_layer_set_text_color(s_time_layer, GColorBlack);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  // A tap event occured
  update_time();
  // Make Time Visible
  text_layer_set_text_color(s_time_layer, PBL_IF_COLOR_ELSE(GColorDarkGray, GColorWhite));
  // Register an timer for 5 seconds to make the time invisible
  AppTimer *updateTimer = app_timer_register(5000, (AppTimerCallback) update_timer_callback, NULL);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Subscribe to tap events
  accel_tap_service_subscribe(accel_tap_handler);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Set Window background color
  window_set_background_color(s_main_window, GColorBlack);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}