#include <pebble.h>

static Window *s_main_window;

// Layers
static TextLayer *s_time_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_temp_layer;
static TextLayer *s_cond_layer;
static TextLayer *s_info_layer;
static BitmapLayer *s_background_layer;

// Fonts
static GFont s_time_font;
static GFont s_weather_font;
static GFont s_temp_font;
static GFont s_cond_font;
static GFont s_info_font;

// Bitmaps
static GBitmap *s_background_bitmap;

static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	// Write the current hourc and minutes into a buffer
	static char s_buffer[8];
	strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
			"%H:%M" : "%I:%M", tick_time);

	// Display this time on the TextLayer
	text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();

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
	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_GRID);
	
	// Create BITMAP layer to display the GBitmap
	s_background_layer = bitmap_layer_create(bounds);
	
	// Create the TIME layer with specific bounds
	s_time_layer = text_layer_create(
		GRect(2, 41, (bounds.size.w - 2), 70));

	// Create WEATHER layers
	s_weather_layer = text_layer_create(
		GRect(92, 2, (bounds.size.w - 92), 50));
	s_temp_layer = text_layer_create(
		GRect(0, -5, 50, 50));
	s_cond_layer = text_layer_create(
		GRect(2, 30, 88, 50));

	// Create INFO layer
	s_info_layer = text_layer_create(
		GRect(2, 28, bounds.size.w, 25));

	// Create TIME font
	s_time_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_70));

	// Create WEATHER font
	s_weather_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_20));
	s_temp_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_25));
	s_cond_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_20));

	// Create INFO font
	s_info_font = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_20));

	/* SETTINGS */

	// Style TIME text
	window_set_background_color(s_main_window, GColorBlack);
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorWhite);
	// text_layer_set_text(s_time_layer, "00:00");
	// text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_font(s_time_layer, s_time_font);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	// Style WEATHER text
	text_layer_set_background_color(s_weather_layer, GColorClear);
	text_layer_set_text_color(s_weather_layer, GColorWhite);
	text_layer_set_text(s_weather_layer, "");
	text_layer_set_font(s_weather_layer, s_weather_font);
	text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);

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

	// Style INFO text
	text_layer_set_background_color(s_info_layer, GColorClear);
	text_layer_set_text_color(s_info_layer, GColorWhite);
	text_layer_set_text(s_info_layer, "");
	text_layer_set_font(s_info_layer, s_info_font);
	text_layer_set_text_alignment(s_info_layer, GTextAlignmentLeft);

	/* SET */

	// Set the BITMAP onto the layer and add to the window
	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

	// Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	layer_add_child(
			window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
	layer_add_child(
			window_get_root_layer(window), text_layer_get_layer(s_temp_layer));
	layer_add_child(
			window_get_root_layer(window), text_layer_get_layer(s_cond_layer));
	layer_add_child(
			window_get_root_layer(window), text_layer_get_layer(s_info_layer));
}

static void main_window_unload(Window *window) {
	// Destroy TextLayer
	text_layer_destroy(s_time_layer);

	// Unload GFont
	fonts_unload_custom_font(s_time_font);

	// Destroy GBitmap
	gbitmap_destroy(s_background_bitmap);

	// Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);

	// Destroy WEATHER elements
	text_layer_destroy(s_weather_layer);
	text_layer_destroy(s_temp_layer);
	text_layer_destroy(s_cond_layer);
	fonts_unload_custom_font(s_weather_font);
	fonts_unload_custom_font(s_temp_font);
	fonts_unload_custom_font(s_cond_font);

	// Destroy INFO elements
	text_layer_destroy(s_info_layer);
	fonts_unload_custom_font(s_info_font);
}

static void inbox_received_callback(
		DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Inbox message rescieved!");

	// Store incoming information
	static char temperature_buffer[8];
	static char conditions_buffer[32];
	static char weather_layer_buffer[32];
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

	APP_LOG(APP_LOG_LEVEL_INFO, "%s", weather_layer_buffer);
}

static void inbox_dropped_callback(
		AppMessageResult reason, void *context) {
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
	update_time();

	// Register APPLICATION callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);

	// Open AppMessage
	const int inbox_size = 128;
	const int outbox_size = 128;
	app_message_open(inbox_size, outbox_size);

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
