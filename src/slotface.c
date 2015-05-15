#include <pebble.h>

static Window *window;
static Layer *text_layer;
static InverterLayer *iLayer1;
static InverterLayer *iLayer;
static Layer *faceLayer;
static Layer *tmpLayer;
static Layer *mdLayer;
static void * x;
static GBitmap * map2;
#define CLOCK_RADIUS 65

static void graphics_draw_line2(GContext *ctx, GPoint p0, GPoint p1, int8_t width) {
  // Order points so that lower x is first
  int16_t x0, x1, y0, y1;
  if (p0.x <= p1.x) {
    x0 = p0.x; x1 = p1.x; y0 = p0.y; y1 = p1.y;
  } else {
    x0 = p1.x; x1 = p0.x; y0 = p1.y; y1 = p0.y;
  }
  
  // Init loop variables
  int16_t dx = x1-x0;
  int16_t dy = abs(y1-y0);
  int16_t sy = y0<y1 ? 1 : -1; 
  int16_t err = (dx>dy ? dx : -dy)/2;
  int16_t e2;
  
  // Calculate whether line thickness will be added vertically or horizontally based on line angle
  int8_t xdiff, ydiff;
  
  if (dx > dy) {
    xdiff = 0;
    ydiff = width/2;
  } else {
    xdiff = width/2;
    ydiff = 0;
  }
  
  // Use Bresenham's integer algorithm, with slight modification for line width, to draw line at any angle
  while (true) {
    // Draw line thickness at each point by drawing another line 
    // (horizontally when > +/-45 degrees, vertically when <= +/-45 degrees)
    graphics_draw_line(ctx, GPoint(x0-xdiff, y0-ydiff), GPoint(x0+xdiff, y0+ydiff));
    
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0++; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_read_first(iterator);

  // Process all pairs present
  while(t != NULL) {
    // Process this pair's key
    persist_write_int(t->key, t->value->int32);

    // Get next pair, if any
    t = dict_read_next(iterator);
  }
  layer_mark_dirty(window_get_root_layer(window));
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}



static void my_layer_draw(Layer *layeer, GContext *ctx) {
  graphics_context_set_compositing_mode(ctx,GCompOpSet);


  int invertq = 0;
  int faceC=0;
  int faceD=0;
  if (persist_exists(0)) {
    invertq = persist_read_int(0);
  }
  if (persist_exists(1)) {
    faceC = persist_read_int(1);
  }
  if (persist_exists(2)) {
    faceD = persist_read_int(2);
  }

  if(invertq!=0){
    layer_set_hidden(inverter_layer_get_layer(iLayer),true);
  }else{
    layer_set_hidden(inverter_layer_get_layer(iLayer),false);
  }
  if(faceC != 0){
    graphics_draw_circle(ctx,GPoint(72,84),60);
  }



 GBitmap * tmp = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TEMP);
 void *tmpDat;
 tmpDat=tmp->addr;
 tmp->addr=x;
 graphics_draw_bitmap_in_rect(ctx,tmp,layer_get_bounds(layeer));
 tmp->addr=tmpDat;
 gbitmap_destroy(tmp);
 free(x);
 
 graphics_context_set_fill_color(ctx,GColorBlack);
 graphics_context_set_stroke_color(ctx,GColorBlack);

time_t now = time(NULL);
struct tm *t = localtime(&now);

 int32_t minuteHandLength=70;
 int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
 int y = (-cos_lookup(minute_angle) * minuteHandLength / TRIG_MAX_RATIO) + 84;
 int xc = (sin_lookup(minute_angle) * minuteHandLength / TRIG_MAX_RATIO) + 72;
 graphics_draw_line2(ctx,GPoint(72,84), GPoint(xc, y),3);

 int32_t secondHandLength=60;
 int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
 y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + 84;
 xc = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + 72;
 graphics_draw_line(ctx,GPoint(72,84), GPoint(xc, y));
if(faceD != 0){
 for (int z = 1; z < 13; z++) {
    int32_t mydot_angle = (TRIG_MAX_ANGLE / 12 * z);
    y = (-cos_lookup(mydot_angle) * CLOCK_RADIUS / TRIG_MAX_RATIO) + 84;
    xc = (sin_lookup(mydot_angle) * CLOCK_RADIUS / TRIG_MAX_RATIO) + 72;
    graphics_fill_circle(ctx, GPoint((int)xc, (int)y), 1);
}

}

}



static void bottom_layer_draw(Layer *layeer, GContext *ctx){
  graphics_context_set_fill_color(ctx,GColorBlack);
  graphics_fill_rect(ctx,GRect(0,0,144,168),0,GCornerNone);
  graphics_context_set_fill_color(ctx,GColorWhite);
  graphics_context_set_stroke_color(ctx,GColorWhite);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int32_t hourHandLength=65;
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  //int32_t hour_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  int y = (-cos_lookup(hour_angle) * hourHandLength / TRIG_MAX_RATIO) + 84;
  int xc = (sin_lookup(hour_angle) * hourHandLength / TRIG_MAX_RATIO) + 72;
  graphics_draw_line2(ctx,GPoint(72,84), GPoint(xc, y),15);

  graphics_fill_circle(ctx, GPoint(xc, y),   6 );
  graphics_fill_circle(ctx, GPoint(72, 84),  6 );

 // graphics_fill_rect(ctx,GRect(0,0,50,40),0,GCornerNone);
  map2=graphics_capture_frame_buffer(ctx);

  int rsb = map2->row_size_bytes;
  int hgt = map2->bounds.size.h;
  x = malloc(hgt*rsb);
  memcpy(x,map2->addr,rsb*hgt);
  graphics_release_frame_buffer(ctx,map2);
  layer_mark_dirty(faceLayer);
}
static void blank_layer_draw(Layer *layeer, GContext *ctx){
  graphics_context_set_fill_color(ctx,GColorWhite);
  graphics_fill_rect(ctx,GRect(0,0,144,168),0,GCornerNone);
}

static char * itoa10 (int value, char *result)
{
    char const digit[] = "0123456789";
    char *p = result;
    if (value < 0) {
        *p++ = '-';
        value *= -1;
    }

    /* move number of required chars and null terminate */
    int shift = value;
    do {
        ++p;
        shift /= 10;
    } while (shift);
    *p = '\0';

    /* populate result in reverse order */
    do {
        *--p = digit [value % 10];
        value /= 10;
    } while (value);

    return result;
}

static void text_layer_draw(Layer *layeer, GContext *ctx){
  graphics_context_set_fill_color(ctx,GColorWhite);
  graphics_context_set_stroke_color(ctx,GColorWhite);
  char* tbuf = malloc(3);
 for (int z = 1; z < 13; z++) {
    tbuf=itoa10(z,tbuf);

    int32_t mydot_angle = (TRIG_MAX_ANGLE / 12 * z);

    GSize sz = graphics_text_layout_get_content_size(tbuf, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),GRect(0,0,30,40),GTextOverflowModeFill,GTextAlignmentLeft);
    int xoff=sz.w/2.0;
    int yoff=sz.h/2.0;
    int y = (-cos_lookup(mydot_angle) * CLOCK_RADIUS / TRIG_MAX_RATIO) + 79 - yoff;
    int xc = (sin_lookup(mydot_angle) * CLOCK_RADIUS / TRIG_MAX_RATIO) + 72 - xoff;
    graphics_draw_text(ctx,tbuf, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),GRect(xc,y,30,40),GTextOverflowModeFill,GTextAlignmentLeft,NULL);
}
  free(tbuf);
}



static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  iLayer1=inverter_layer_create(GRect(0, 0, 144, 168));
  iLayer = inverter_layer_create(GRect(0, 0, 144, 168));

  

  faceLayer=layer_create(GRect(0, 0, 144, 168));
  tmpLayer=layer_create(GRect(0, 0, 144, 168));
  mdLayer=layer_create(GRect(0, 0, 144, 168));
  text_layer=layer_create(GRect(0, 0, 144, 168));

  layer_set_update_proc(tmpLayer, bottom_layer_draw);
  layer_set_update_proc(mdLayer, blank_layer_draw);
  layer_set_update_proc(faceLayer, my_layer_draw);
  layer_set_update_proc(text_layer, text_layer_draw);


  layer_add_child(window_layer, tmpLayer);
  layer_add_child(tmpLayer,mdLayer);
  layer_add_child(mdLayer,inverter_layer_get_layer(iLayer1));
  layer_add_child(inverter_layer_get_layer(iLayer1),text_layer);
  layer_add_child(text_layer,faceLayer);
  layer_add_child(faceLayer,inverter_layer_get_layer(iLayer));

}

static void window_unload(Window *window) {
  layer_destroy(faceLayer);
  layer_destroy(text_layer);
  inverter_layer_destroy(iLayer1);
  layer_destroy(mdLayer);
  layer_destroy(tmpLayer);

}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  layer_mark_dirty(tmpLayer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);


app_message_register_inbox_received(inbox_received_callback);
app_message_register_inbox_dropped(inbox_dropped_callback);
app_message_register_outbox_failed(outbox_failed_callback);
app_message_register_outbox_sent(outbox_sent_callback);

app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

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
