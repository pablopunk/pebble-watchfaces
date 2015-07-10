#include <pebble.h>

static Window* s_main_window;

static Layer* main_layer;

static TextLayer* hours_text_layer;
static int hours;
static TextLayer* minutes_text_layer;
static int minutes;
static int isIn24hrMode = 0;

static void main_window_load(Window *window) {

  window_set_background_color(window, GColorBlack);

  main_layer = layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_get_root_layer(window), main_layer);

  // Create time TextLayer
  hours_text_layer = text_layer_create(GRect(0, 15, 144, 50));
  text_layer_set_background_color(hours_text_layer, GColorClear);
  text_layer_set_text_color(hours_text_layer, GColorWhite);
  // Improve the layout to be more like a watchface
  text_layer_set_font(hours_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(hours_text_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(hours_text_layer));

  // Create time TextLayer
  minutes_text_layer = text_layer_create(GRect(0, 97, 144, 50));
  text_layer_set_background_color(minutes_text_layer, GColorClear);
  text_layer_set_text_color(minutes_text_layer, GColorWhite);
  // Improve the layout to be more like a watchface
  text_layer_set_font(minutes_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(minutes_text_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minutes_text_layer));

}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(hours_text_layer);
  text_layer_destroy(minutes_text_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char h[] = "00";
  static char m[] = "00";

  // Write the current hours_text_layer and minutes into the buffer
  strftime(m, sizeof("00"), "%M", tick_time);
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(h, sizeof("00"), "%H", tick_time);
    isIn24hrMode = 1;
  } else {
    // Use 12 hour format
    strftime(h, sizeof("00"), "%I", tick_time);
    isIn24hrMode = 0;
  }

  hours   = atoi(h); // update global time
  minutes = atoi(m);

  // Display this time on the TextLayer
  text_layer_set_text(hours_text_layer, h);
  text_layer_set_text(minutes_text_layer, m);
}

static void update_bars(Layer *this_layer, GContext *ctx) {
  // hours
  graphics_context_set_fill_color(ctx, GColorMediumSpringGreen);
  graphics_fill_rect(ctx, GRect(0, 0, (144/ (12+(isIn24hrMode*12)))*hours, 84), 0, GCornerNone);
  // minutes
  graphics_context_set_fill_color(ctx, GColorVividCerulean);
  graphics_fill_rect(ctx, GRect(0, 84, (144/60)*minutes, 84), 0, GCornerNone);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  layer_set_update_proc(main_layer, update_bars);
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
