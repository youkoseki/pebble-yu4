#include "simple_analog.h"

#include "pebble.h"

static Window *window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer;
static TextLayer *m_num_label,*h_num_label,*d_num_label;

static GPath *s_minute_arrow, *s_hour_arrow;
static char m_num_buffer[4], h_num_buffer[4],d_num_buffer[4];
static GPoint s_center;
static GPoint mHandCircle;
static GPoint hHandCircle;

static void bg_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWhite);
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  int16_t second_hand_length = bounds.size.w / 2;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // minute/hour hand
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_move_to(s_minute_arrow, GPoint(s_center.x,s_center.y+50));
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorWhite);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));

  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  // dot in the middle
  mHandCircle.x = 60*sin_lookup(TRIG_MAX_ANGLE * t->tm_min / 60)/TRIG_MAX_RATIO + s_center.x;
  mHandCircle.y = -60*cos_lookup(TRIG_MAX_ANGLE * t->tm_min / 60)/TRIG_MAX_RATIO + s_center.y;
  hHandCircle.x = 30*sin_lookup(TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10)) / (12 * 6))/TRIG_MAX_RATIO + s_center.x;
  hHandCircle.y = -30*cos_lookup(TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10)) / (12 * 6))/TRIG_MAX_RATIO + s_center.y;

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(s_center.x,s_center.y-50), 22);
  graphics_context_set_fill_color(ctx, GColorRed);
  graphics_fill_circle(ctx, GPoint(s_center.x,s_center.y-50), 20);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(s_center.x,s_center.y), 22);
  graphics_context_set_fill_color(ctx, GColorYellow);
  graphics_fill_circle(ctx, GPoint(s_center.x,s_center.y), 20);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(s_center.x,s_center.y+50), 22);
  graphics_context_set_fill_color(ctx, GColorGreen);
  graphics_fill_circle(ctx, GPoint(s_center.x,s_center.y+50), 20);
  
  text_layer_destroy(m_num_label);
  text_layer_destroy(h_num_label);
  text_layer_destroy(d_num_label);

  m_num_label = text_layer_create(GRect(s_center.x-19, s_center.y+50-17, 40, 40));
  text_layer_set_background_color(m_num_label, GColorClear);
  text_layer_set_text_color(m_num_label, GColorBlack);
  text_layer_set_font(m_num_label, fonts_get_system_font(FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM));
  text_layer_set_text_alignment(m_num_label,GTextAlignmentCenter);
  strftime(m_num_buffer, sizeof(m_num_buffer), "%M", t);
  text_layer_set_text(m_num_label, m_num_buffer);
  layer_add_child(layer, text_layer_get_layer(m_num_label));


  h_num_label = text_layer_create(GRect(s_center.x-19, s_center.y-17, 40, 40));
  text_layer_set_background_color(h_num_label, GColorClear);
  text_layer_set_text_color(h_num_label, GColorBlack);
  text_layer_set_font(h_num_label, fonts_get_system_font(FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM));
  text_layer_set_text_alignment(h_num_label,GTextAlignmentCenter);
  strftime(h_num_buffer, sizeof(h_num_buffer), "%H", t);
  text_layer_set_text(h_num_label, h_num_buffer);
  layer_add_child(layer, text_layer_get_layer(h_num_label));

  
  d_num_label = text_layer_create(GRect(s_center.x-19, s_center.y-50-17, 40, 40));
  text_layer_set_background_color(d_num_label, GColorClear);
  text_layer_set_text_color(d_num_label, GColorBlack);
  text_layer_set_font(d_num_label, fonts_get_system_font(FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM));
  text_layer_set_text_alignment(d_num_label,GTextAlignmentCenter);
  strftime(d_num_buffer, sizeof(d_num_buffer), "%d", t);
  text_layer_set_text(d_num_label, d_num_buffer);
  layer_add_child(layer, text_layer_get_layer(d_num_label));

  

  
  /*  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(s_center.x,s_center.y), 20);*/
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

//  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
//  text_layer_set_text(s_day_label, s_day_buffer);

//  text_layer_set_text(s_num_label,"88");
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  GRect bounds = layer_get_bounds(window_layer);
  s_center = grect_center_point(&bounds);
  
  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);

  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);


}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
//  layer_destroy(s_date_layer);

  layer_destroy(s_hands_layer);
}

static void init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  m_num_buffer[0] = '\0';
  h_num_buffer[0] = '\0';
  d_num_buffer[0] = '\0';

  // init hand paths
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);


  tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);
}

static void deinit() {
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
