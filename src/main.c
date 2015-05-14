#include <pebble.h>

static Window *s_main_window;
static TextLayer * s_time_layer;
static TextLayer * s_date_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_bt_layer;
static TextLayer *s_day_layer;

static void bt_handler(bool connected) {
  if (connected) {
    text_layer_set_text(s_bt_layer, "BT:On");
  } else {
    text_layer_set_text(s_bt_layer, "BT:Off");
  }
}

static void battery_handler(BatteryChargeState new_state) {
  static char s_battery_buffer[32];
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d %%",new_state.charge_percent);
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}

static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  static char s_timeBuffer[10];
  static char s_dateBuffer[12];
  static char s_dayBuffer[12];
  
  if(clock_is_24h_style() == true) {
    strftime(s_timeBuffer, sizeof(s_timeBuffer), "%H:%M", tick_time);
  } else {
    strftime(s_timeBuffer, sizeof(s_timeBuffer), "%I:%M", tick_time);
  }
  
  text_layer_set_text(s_time_layer, s_timeBuffer);
  
  strftime(s_dateBuffer, sizeof(s_dateBuffer), "%e %b %Y", tick_time); 
  text_layer_set_text(s_date_layer, s_dateBuffer);
  
  strftime(s_dayBuffer, sizeof(s_dayBuffer), "%A", tick_time); 
  text_layer_set_text(s_day_layer, s_dayBuffer);
}

static void main_window_load(Window *window) {
  s_time_layer = text_layer_create(GRect(0, 50, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  s_date_layer = text_layer_create(GRect(0, 100, 144, 50));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
  s_battery_layer = text_layer_create(GRect(0, 0, 144, 25));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorBlack);
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
  battery_handler(battery_state_service_peek());
  
  s_bt_layer = text_layer_create(GRect(0, 0, 144, 25));
  text_layer_set_background_color(s_bt_layer, GColorClear);
  text_layer_set_text_color(s_bt_layer, GColorBlack);
  text_layer_set_font(s_bt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_bt_layer, GTextAlignmentLeft);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bt_layer));
  bt_handler(bluetooth_connection_service_peek());
  
  s_day_layer = text_layer_create(GRect(0, 150, 144, 50));
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, GColorBlack);
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_bt_layer);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_battery_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);
  update_time();
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  bluetooth_connection_service_subscribe(bt_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}
