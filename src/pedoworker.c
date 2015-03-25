/*
 * worker_src/worker.c
 * Launched and killed from the foreground app. Counts seconds passing 
 * and reports them using AppWorkerMessages.
 */

#include <pebble_worker.h>

#define WORKER_TICKS 0
extern SimpleMenuLayer *pedometer_settings;
extern Window *pedometer;
static int32_t s_ticks = 0;

// Dheera: Start
TextLayer *steps;
TextLayer *pedCount;
TextLayer *calories;
GBitmap *pedometerBack;
GBitmap *flame;
BitmapLayer *pedometerBack_layer;
BitmapLayer *flame_layer;


static void tick_handler(struct tm *tick_timer, TimeUnits units_changed) {
  // Update value
  s_ticks++;

  // Construct a data packet
  AppWorkerMessage msg_data = {
    .data0 = s_ticks
  };

  // Send the data to the foreground app
  app_worker_send_message(WORKER_TICKS, &msg_data);
}

//Dheera: Start
void ped_load(Window *window) {
  steps = text_layer_create(GRect(0, 120, 150, 170));
  pedCount = text_layer_create(GRect(0, 75, 150, 170));
  calories = text_layer_create(GRect(0, 50, 150, 170));

  if (isDark) {
    window_set_background_color(pedometer, GColorBlack);
    pedometerBack = gbitmap_create_with_resource(RESOURCE_ID_PED_WHITE);
    flame = gbitmap_create_with_resource(RESOURCE_ID_FLAME_WHITE);
    text_layer_set_background_color(steps, GColorClear);
    text_layer_set_text_color(steps, GColorBlack);
    text_layer_set_background_color(pedCount, GColorClear);
    text_layer_set_text_color(pedCount, GColorBlack);
    text_layer_set_background_color(calories, GColorClear);
    text_layer_set_text_color(calories, GColorWhite);
  } else {
    window_set_background_color(pedometer, GColorWhite);
    pedometerBack = gbitmap_create_with_resource(RESOURCE_ID_PED_BLK);
    flame = gbitmap_create_with_resource(RESOURCE_ID_FLAME_BLK);
    text_layer_set_background_color(steps, GColorClear);
    text_layer_set_text_color(steps, GColorWhite);
    text_layer_set_background_color(pedCount, GColorClear);
    text_layer_set_text_color(pedCount, GColorWhite);
    text_layer_set_background_color(calories, GColorClear);
    text_layer_set_text_color(calories, GColorBlack);
  }

  pedometerBack_layer = bitmap_layer_create(GRect(0, 0, 145, 215));
  flame_layer = bitmap_layer_create(GRect(50, 0, 50, 50));

  bitmap_layer_set_bitmap(pedometerBack_layer, pedometerBack);
  bitmap_layer_set_bitmap(flame_layer, flame);
  
  layer_add_child(window_get_root_layer(pedometer),
      bitmap_layer_get_layer(pedometerBack_layer));
  
  layer_add_child(window_get_root_layer(pedometer),
      bitmap_layer_get_layer(flame_layer));

  layer_add_child(window_get_root_layer(pedometer), (Layer*) steps);
  layer_add_child(window_get_root_layer(pedometer), (Layer*) pedCount);
  layer_add_child(window_get_root_layer(pedometer), (Layer*) calories);

  text_layer_set_font(pedCount, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BEBAS_40)));
  text_layer_set_font(calories, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BEBAS_15)));
  text_layer_set_font(steps, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BEBAS_15)));
  
  text_layer_set_text_alignment(steps, GTextAlignmentCenter);
  text_layer_set_text_alignment(pedCount, GTextAlignmentCenter);
  text_layer_set_text_alignment(calories, GTextAlignmentCenter);

  text_layer_set_text(steps, "s t e p s");

  static char buf[] = "1234567890";
  snprintf(buf, sizeof(buf), "%ld", pedometerCount);
  text_layer_set_text(pedCount, buf);

  static char buf2[] = "1234567890abcdefghijkl";
  snprintf(buf2, sizeof(buf2), "%ld Calories", caloriesBurned);
  text_layer_set_text(calories, buf2);
}

//Dheera: End

static void worker_init() {

  //Dheera: START

  accel_data_service_subscribe(0, NULL);

  menu_items[0].title = "Continue Run";
  menu_items[0].subtitle = "Ready for more?";
  layer_mark_dirty(simple_menu_layer_get_layer(pedometer_settings));

  pedometer = window_create();

  window_set_window_handlers(pedometer, (WindowHandlers ) { .load = ped_load,
          .unload = ped_unload, });

  window_stack_push(pedometer, true);
  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);

  //Dheera: END

/*
  // Use the TickTimer Service as a data source
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  */
}

static void worker_deinit() {
  // Stop using the TickTimerService
  tick_timer_service_unsubscribe();
}

int main(void) {
  worker_init();
  worker_event_loop();
  worker_deinit();
}