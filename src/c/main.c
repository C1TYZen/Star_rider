#include <pebble.h>

static Window *s_main_window;

// Layers
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_temp_layer;
static TextLayer *s_cond_layer;
static TextLayer *s_bluetooth_layer;

static BitmapLayer *s_background_layer;
static BitmapLayer *s_battery_layer;

// Fonts
static GFont s_time_font;
static GFont s_date_font;
static GFont s_weather_font;
static GFont s_temp_font;
static GFont s_cond_font;
static GFont s_bluetooth_font;

// Bitmaps
static GBitmap *s_grid_bitmap;
static GBitmap *s_battery_full_bitmap;
static GBitmap *s_battery_80_bitmap;
static GBitmap *s_battery_60_bitmap;
static GBitmap *s_battery_40_bitmap;
static GBitmap *s_battery_low_bitmap;

// Vars
static int s_battery_level;
static const char* week_day[] = {"Вс", "Пн", "Вт", "Ср", "Чт", "Пт", "Сб"};

static void update_time(struct tm *tt) {
	// Write the current hourc and minutes into a buffer
	static char s_time_buffer[8];
	strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ?
			"%H:%M" : "%I:%M", tt);
	text_layer_set_text(s_time_layer, s_time_buffer);
}

static void update_date(struct tm *tt) {
	static char s_date_buffer[16];

	strftime(s_date_buffer, sizeof(s_date_buffer), "%d.%m", tt);
	strcat(s_date_buffer, week_day[tt->tm_wday]);
	text_layer_set_text(s_date_layer, s_date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time(tick_time);
	update_date(tick_time);

	// Get weather update every 30 minutes
	if(tick_time->tm_min % 30 == 0) {
		// Begin dictionary
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);

		// Add a key-value pair
		dict_write_uint8(iter, 0, 0);

		// Send the message!
		app_message_outbox_send();
	}
}

static void main_window_load(Window *window) {
	// Get information about the Window
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	/* CREATE */

	// Create GBitmap
	s_grid_bitmap =
		gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GRID);
	s_battery_full_bitmap =
		gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_FULL);
	s_battery_80_bitmap =
		gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_80);
	s_battery_60_bitmap =
		gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_60);
	s_battery_40_bitmap =
		gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_40);
	s_battery_low_bitmap =
		gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_LOW);
	
	// Create BITMAP layer to display the GBitmap
	s_background_layer = bitmap_layer_create(bounds);
	s_battery_layer = bitmap_layer_create(GRect(0, 120, 51, 47));
	
	// Create TIME and DATE layers
	s_time_layer = text_layer_create(
		GRect(2, 41, bounds.size.w, 70));
	s_date_layer = text_layer_create(
		GRect(59, 121, (bounds.size.w - 2), 35));
	s_bluetooth_layer = text_layer_create(
		GRect(59, 147, (bounds.size.w - 2), 35));

	// Create WEATHER layers
	s_temp_layer = text_layer_create(
		GRect(0, -3, 50, 30));
	s_cond_layer = text_layer_create(
		GRect(0, 30, 88, 50));

	// Create TIME and DATE font
	s_time_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_70));
	s_date_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_30));
	s_bluetooth_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_20));

	// Create WEATHER font
	s_weather_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_20));
	s_temp_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_25));
	s_cond_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_20));

	/* SETTINGS */

	// Style TIME and DATE text
	window_set_background_color(s_main_window, GColorBlack);
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorWhite);
	text_layer_set_font(s_time_layer, s_time_font);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, GColorWhite);
	text_layer_set_font(s_date_layer, s_date_font);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);

	text_layer_set_background_color(s_bluetooth_layer, GColorClear);
	text_layer_set_text_color(s_bluetooth_layer, GColorBlue);
	text_layer_set_font(s_bluetooth_layer, s_bluetooth_font);
	text_layer_set_text_alignment(s_bluetooth_layer, GTextAlignmentLeft);
	text_layer_set_text(s_bluetooth_layer, "Bluetooth");

	bitmap_layer_set_background_color(s_background_layer, GColorClear);

	// Style WEATHER text
	text_layer_set_background_color(s_temp_layer, GColorClear);
	text_layer_set_text_color(s_temp_layer, GColorWhite);
	text_layer_set_text(s_temp_layer, "");
	text_layer_set_font(s_temp_layer, s_temp_font);
	text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);

	text_layer_set_background_color(s_cond_layer, GColorClear);
	text_layer_set_text_color(s_cond_layer, GColorWhite);
	text_layer_set_text(s_cond_layer, "Загрузка...");
	text_layer_set_font(s_cond_layer, s_cond_font);
	text_layer_set_text_alignment(s_cond_layer, GTextAlignmentCenter);

	/* SET */

	// Set the BITMAP onto the layer and add to the window
	bitmap_layer_set_bitmap(s_background_layer, s_grid_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
	layer_add_child(window_layer, bitmap_layer_get_layer(s_battery_layer));

	// Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_bluetooth_layer));
	layer_add_child(
			window_get_root_layer(window), text_layer_get_layer(s_temp_layer));
	layer_add_child(
			window_get_root_layer(window), text_layer_get_layer(s_cond_layer));
}

static void main_window_unload(Window *window) {
	// Destroy Text layers and fonts
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_bluetooth_layer);
	text_layer_destroy(s_temp_layer);
	text_layer_destroy(s_cond_layer);
	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_date_font);
	fonts_unload_custom_font(s_weather_font);
	fonts_unload_custom_font(s_temp_font);
	fonts_unload_custom_font(s_cond_font);

	// Destroy GBitmap
	gbitmap_destroy(s_grid_bitmap);
	gbitmap_destroy(s_battery_full_bitmap);
	gbitmap_destroy(s_battery_80_bitmap);
	gbitmap_destroy(s_battery_60_bitmap);
	gbitmap_destroy(s_battery_40_bitmap);
	gbitmap_destroy(s_battery_low_bitmap);

	// Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);
}

static void inbox_received_callback(
		DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Inbox message rescieved!");

	// Store incoming information
	static char temperature_buffer[8];
	static char conditions_buffer[32];
	static char temp_layer_buffer[32];
	static char cond_layer_buffer[32];

	// Read tuples for data
	Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
	Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);

	// If all data is available, use it
	if(temp_tuple && conditions_tuple) {
		snprintf(temperature_buffer,
			sizeof(temperature_buffer), "%d°C", (int)temp_tuple->value->int32);
		snprintf(conditions_buffer,
			sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
	}

	// Assemble full string and display
	snprintf(temp_layer_buffer,
		sizeof(temp_layer_buffer), "%s", temperature_buffer);
	text_layer_set_text(s_temp_layer, temp_layer_buffer);

	snprintf(cond_layer_buffer,
		sizeof(cond_layer_buffer), "%s", conditions_buffer);
	text_layer_set_text(s_cond_layer, cond_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(
		DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(
		DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void battery_callback(BatteryChargeState state) {
	// Record the new battery level
	s_battery_level = state.charge_percent;
	APP_LOG(APP_LOG_LEVEL_INFO, "BATTERY: %d", s_battery_level);

	if(s_battery_level > 80)
		bitmap_layer_set_bitmap(s_battery_layer, s_battery_full_bitmap);
	else if(s_battery_level > 60)
		bitmap_layer_set_bitmap(s_battery_layer, s_battery_80_bitmap);
	else if(s_battery_level > 40)
		bitmap_layer_set_bitmap(s_battery_layer, s_battery_60_bitmap);
	else if(s_battery_level > 20)
		bitmap_layer_set_bitmap(s_battery_layer, s_battery_40_bitmap);
	else if(s_battery_level > 0)
		bitmap_layer_set_bitmap(s_battery_layer, s_battery_low_bitmap);
}

static void init() {
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

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
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	update_time(tick_time);
	update_date(tick_time);

	// Register APPLICATION callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);

	// Open AppMessage
	const int inbox_size = 128;
	const int outbox_size = 128;
	app_message_open(inbox_size, outbox_size);

	// Register for battery level updates
	battery_state_service_subscribe(battery_callback);
	// Ensure battery level is displayed from the start
	battery_callback(battery_state_service_peek());

	APP_LOG(APP_LOG_LEVEL_INFO, "INITIALIZED!!!");
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
