#ifndef WEATHER_H
#define WEATHER_H

static TextLayer *s_temp_layer;
static TextLayer *s_cond_layer;

static GFont s_temp_font;
static GFont s_cond_font;

static const char* week_day[] = {"Вс", "Пн", "Вт", "Ср", "Чт", "Пт", "Сб"};

extern void weather_load();
extern const char* weather_loading();

#endif
