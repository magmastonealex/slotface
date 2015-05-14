#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static InverterLayer *iLayer1;
static Layer *faceLayer;
static GBitmap * map;
static BitmapLayer * bLayer;

static void my_layer_draw(Layer *layeer, GContext *ctx) {
 graphics_context_set_compositing_mode(ctx,GCompOpClear);
 graphics_context_set_fill_color(ctx,GColorWhite);
 graphics_fill_rect(ctx,GRect(0,0,144,168),0,GCornerNone);
 graphics_context_set_fill_color(ctx,GColorBlack);
 graphics_context_set_stroke_color(ctx,GColorBlack);
 graphics_fill_circle(ctx,GPoint(40,30),40);

// map=graphics_capture_frame_buffer(ctx);
 //bitmap_layer_set_bitmap(bLayer,map);
// graphics_release_frame_buffer(ctx,map);

 graphics_context_set_fill_color(ctx,GColorWhite);
 graphics_fill_rect(ctx,GRect(0,0,144,168),0,GCornerNone);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  iLayer1=inverter_layer_create(GRect(0, 0, 144, 168));
  text_layer=text_layer_create(GRect(40, 30, 90, 100));
  text_layer_set_text(text_layer, "10");
  text_layer_set_background_color(text_layer,GColorClear);
  text_layer_set_text_color(text_layer,GColorWhite);
  

  faceLayer=layer_create(GRect(0, 0, 144, 168));

  layer_set_update_proc(faceLayer, my_layer_draw);
       
//  map= __gbitmap_create_blank(GSize(144,168));
  map=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TEMP);
  bLayer = bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_compositing_mode(bLayer,GCompOpSet);
  bitmap_layer_set_bitmap(bLayer,map);
  layer_add_child(window_layer, faceLayer);

  layer_add_child(faceLayer,inverter_layer_get_layer(iLayer1));

  layer_add_child(inverter_layer_get_layer(iLayer1),text_layer_get_layer(text_layer));
  layer_add_child(inverter_layer_get_layer(iLayer1),bitmap_layer_get_layer(bLayer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
