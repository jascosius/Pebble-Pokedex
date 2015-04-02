#include <pebble.h>
  
#include "Pokedex.h"

Window      *window;

TextLayer   *text_layer;
TextLayer   *text_layer_gen;

GBitmap     *poke_image;
BitmapLayer *poke_image_layer;

GBitmap     *mega_image;
BitmapLayer *mega_image_layer;

GBitmap     *legendary_image;
BitmapLayer *legendary_image_layer;
  
Layer       *blank_rect_layer;

GBitmap     *top_bar_image;
BitmapLayer *top_bar_layer;
GBitmap     *bottom_bar_image;
BitmapLayer *bottom_bar_layer;

GFont       *custom_font;

//Languages configured by Pebble settings
int8_t first_lang = LANG_ENG;
int8_t second_lang = NONE;

int16_t currentID;
int8_t mode;
int8_t next_mode;
int8_t sort;

static char num[4];
static char text[80];
static char mega_name[9];

static long seed;


static void blank_rect_layer_update(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

void update_selection() {
  
  //Remove picture layers
  layer_remove_from_parent(bitmap_layer_get_layer( mega_image_layer ));
  layer_remove_from_parent(bitmap_layer_get_layer( poke_image_layer ));
  layer_remove_from_parent(bitmap_layer_get_layer( legendary_image_layer ));
  
  //Order selection
  if (currentID == 0) {
    snprintf( text, sizeof(text), "%s\n\n%s\n%s",order_names[first_lang],order_names_selections1[first_lang][sort],order_names_selections2[first_lang][sort]);
    text_layer_set_text (text_layer, text);
    text_layer_set_text (text_layer_gen, "");
    return;
  }
  
  //create Pokemon number
  if (currentID  <  10 ){       snprintf( num, sizeof(num), "00%d", currentID);}
  else if (currentID  < 100 ){ snprintf( num, sizeof(num), "0%d", currentID);  }
  else{                         snprintf( num, sizeof(num), "%d", currentID);  }
  
  //destroy old Pokemon picture
  gbitmap_destroy( poke_image );
  bitmap_layer_destroy( poke_image_layer );
  
  //is there a mega evolution for this pokemon
  int8_t mega = NONE;
  for(int8_t i = 0; i<NUM_MEGA; i++) {
    if((0x03ff&mega_free4_game2_number10[i]) == currentID) {
      mega=i;
      layer_insert_above_sibling(bitmap_layer_get_layer( mega_image_layer ), blank_rect_layer);
      break;
    }
  }
  
  //is there a second mega evolution for this pokemon
  int8_t mega2 = NONE;
  if (mega != NONE && mega+1 < NUM_MEGA && (0x03ff&mega_free4_game2_number10[mega+1]) == currentID) {
    mega2 = mega+1;
  }
  
  //toggle modes
  if(next_mode == 1) {
    
    //from first language
    if(mode==first_lang) {
      if(second_lang != NONE) {
        mode = second_lang;
      } else {
        if(mega != NONE) {
          mode = MEGA1;
        }
      }
      
    //from second language
    } else if(mode==second_lang) {
      if(mega == NONE) {
        mode=first_lang;
      } else {
        mode = MEGA1;
      }
      
    //from first mega evolution
    } else if (mode==MEGA1) {
      if(mega2 == NONE) {
        mode=first_lang;
      } else {
        mode=MEGA2;
        mega=mega2;
      }
      
    //from second mega evolution
    } else {
      mode=first_lang;
    }
  
  //leave mega evolution on every other change (next Pokemon)
  } else {
    if(mode == MEGA1 || mode == MEGA2) {
      mode = first_lang;
    }
  }
  next_mode=0;
  
  //create mega evolution layer
  if      ((0x0c00&mega_free4_game2_number10[mega]) == MEGA_ALL) {strncpy(mega_name, "Mega", 5);}
  else if ((0x0c00&mega_free4_game2_number10[mega]) == MEGA_X)   {strncpy(mega_name, "Mega X", 7);}
  else if ((0x0c00&mega_free4_game2_number10[mega]) == MEGA_Y)   {strncpy(mega_name, "Mega Y", 7);}
  else                                                           {strncpy(mega_name, "Mega S/R", 9);}
    
  //search Pokemon name
  uint8_t mode_tmp = mode;
  if(mode_tmp >= NUM_LANG) {
    mode_tmp = first_lang;
  }
  const char *ptr;
  while(true) {
    if(mode_tmp==LANG_ENG) {
      ptr = poke_names_eng[(currentID-1)/POKEMON_EACH_ROW];
    } else {
      ptr = poke_names_ger[(currentID-1)/POKEMON_EACH_ROW];
    }
    uint8_t number = (currentID-1)%POKEMON_EACH_ROW;
    while(number != 0) {
      while(ptr[0] != '\0') {
       ptr++;
      }
      ptr++;
      number--;
    }
    //if name is empty, search English name (names that are the same in English are only saved in English to save space)
    if(ptr[0] != '\0' || mode_tmp == LANG_ENG) {
      break;
    } else {
      mode_tmp = LANG_ENG;
    }
  }
  
  if (mode < NUM_LANG){
    //generate text (Pokemon number, name, types)
    snprintf( text, sizeof(text), 
      " %s      \n\n%s\n\n%s\n%s",
      num,
      ptr,
      type_names[mode][(0x7c00&flag1_type5_alphabet10[TYPE1][currentID])>>10],
      type_names[mode][(0x7c00&flag1_type5_alphabet10[TYPE2][currentID])>>10]);
  } else {
    //generate text for mega evolution
    snprintf( text, sizeof(text), 
      " %s      \n\n%s\n%s\n\n%s\n%s",
      num,
      mega_name,
      ptr,
      type_names[first_lang][mega_type[mega*2]],
      type_names[first_lang][mega_type[mega*2+1]]);
  }
  text_layer_set_text( text_layer, text);
  
  //set generation of current Pokemon
  if      (currentID  <=  151 ){ text_layer_set_text (text_layer_gen, "1.Gen"); }
  else if (currentID  <=  251 ){ text_layer_set_text (text_layer_gen, "2.Gen");  }
  else if (currentID  <=  386 ){ text_layer_set_text (text_layer_gen, "3.Gen");  }
  else if (currentID  <=  493 ){ text_layer_set_text (text_layer_gen, "4.Gen");  }
  else if (currentID  <=  649 ){ text_layer_set_text (text_layer_gen, "5.Gen");  }
  else                         { text_layer_set_text (text_layer_gen, "6.Gen");  }
  
  //set legendary flag of current pokemon (star image)
  if(0x8000&flag1_type5_alphabet10[LEGENDARY][currentID]) {
    layer_insert_above_sibling(bitmap_layer_get_layer( legendary_image_layer ), blank_rect_layer);
  }
  
  //search and place image for current pokemon (several images in one file, because of the 256 images restriction)
  int offset = ((currentID-1)%IMAGES_EACH_ROW)*32;
  poke_image       = gbitmap_create_with_resource(poke_images[(currentID-1)/IMAGES_EACH_ROW]);
  poke_image_layer = bitmap_layer_create( GRect( 95-offset, 32, 32+offset, 30) );
  bitmap_layer_set_alignment( poke_image_layer, GAlignLeft);
  bitmap_layer_set_background_color( poke_image_layer, GColorClear);
  bitmap_layer_set_compositing_mode( poke_image_layer, GCompOpAnd);
  bitmap_layer_set_bitmap( poke_image_layer, poke_image);
  layer_insert_below_sibling(bitmap_layer_get_layer( poke_image_layer ), blank_rect_layer);
}

  //genarate random number
  int random(int max) { // returns 1 ... max
    seed = (((seed * 214013L + 2531011L) >> 16) & 32767);
    return ((seed % max) + 1);
  }

///////////////////////// INPUT /////////////////////

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  
  if(sort == SORT_NUM) {
    //if Pokemon are sorted by number
    if (click_number_of_clicks_counted( recognizer ) >= 3) {
      currentID = currentID - 25;
      if(currentID<=0) {
        currentID = 1;
      }
    } else if (click_number_of_clicks_counted( recognizer ) == 1) {
      currentID = currentID -1;
      if(currentID<0) {
        currentID = NUM_POKEMON;
      }
    }
  } else {
    //if Pokemon are sorted by name
    if (click_number_of_clicks_counted( recognizer ) >= 3) {
      for(int16_t i = NUM_POKEMON + 1; i>=0; i--) {
        if((0x03ff&flag1_type5_alphabet10[sort][i]) == currentID) {
          int16_t target = i-25;
          if(target<1) {
            target = 1;
          }
          currentID = 0x03ff&flag1_type5_alphabet10[sort][target];
          break;
        }
      }
    }
    if (click_number_of_clicks_counted( recognizer ) == 1) {
      for(int16_t i = NUM_POKEMON + 1; i>=0; i--) {
        if((0x03ff&flag1_type5_alphabet10[sort][i]) == currentID) {
          currentID = 0x03ff&flag1_type5_alphabet10[sort][i-1];
          break;
        }
      }
    }

  }
  
  update_selection();
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  
  if(sort == SORT_NUM) {
    //if Pokemon are sorted by number
    if (click_number_of_clicks_counted( recognizer ) >= 3) {
      currentID = currentID + 25;
      if(currentID>NUM_POKEMON) {
        currentID = NUM_POKEMON;
      }
    } else if (click_number_of_clicks_counted( recognizer ) == 1) {
      currentID = currentID +1;
      if(currentID>NUM_POKEMON) {
        currentID = 0;
      }
    }
  } else {
    //if Pokemon are sorted by name
    if (click_number_of_clicks_counted( recognizer ) >= 3) {
      for(int16_t i = 0; i<=NUM_POKEMON + 1; i++) {
        if((0x03ff&flag1_type5_alphabet10[sort][i]) == currentID) {
          int16_t target = i+25;
          if(target>NUM_POKEMON) {
            target = NUM_POKEMON;
          }
          currentID = 0x03ff&flag1_type5_alphabet10[sort][target];
          break;
        }
      }
    }
    if (click_number_of_clicks_counted( recognizer ) == 1) {
      for(int16_t i = 0; i<=NUM_POKEMON + 1; i++) {
        if((0x03ff&flag1_type5_alphabet10[sort][i]) == currentID) {
          currentID = 0x03ff&flag1_type5_alphabet10[sort][i+1];
          break;
        }
      }
    }

  }
  update_selection();
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) 
{  
  if (click_number_of_clicks_counted( recognizer ) <= 2) {
    if(currentID == 0) {
      
      //toggle language
      if(sort == SORT_NUM) {
        sort = first_lang;
        mode = first_lang;
      } else if (sort == first_lang) {
        if (second_lang != NONE) {
          sort = second_lang;
          mode = second_lang;
        } else {
          sort = SORT_NUM;
          mode = first_lang;
        }
      } else {
        sort = SORT_NUM;
        mode = first_lang;
      }
    } else {
      //toggle mode
      next_mode = 1;
    }
  }
  
  //choose random Pokemon
  if (click_number_of_clicks_counted( recognizer ) >= 2 && currentID != 0) {
    currentID = random( NUM_POKEMON );
  }
  update_selection();
}


void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 500, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP,     300,     up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN,   300,   down_click_handler);
}


/////////////////// SETTINGS ///////////////////

void checkAndSaveInt(int8_t *var, int val, int key) {
  if (*var != val) {
    *var = (int8_t)val;
    persist_write_int(key, val);
  } 
}

//save and set new settings
void in_received_handler(DictionaryIterator *received, void *context) {
  
  Tuple *first_lang_tuple = dict_find(received, CONFIG_KEY_FIRST_LANG);
  Tuple *second_lang_tuple = dict_find(received, CONFIG_KEY_SECOND_LANG);

  if (first_lang_tuple && second_lang_tuple) {
    checkAndSaveInt(&first_lang, first_lang_tuple->value->int32, CONFIG_KEY_FIRST_LANG);
    checkAndSaveInt(&second_lang, second_lang_tuple->value->int32, CONFIG_KEY_SECOND_LANG);
  }

  next_mode = 1;
  update_selection();
}

///////////////////////// INIT /////////////////////

//init message box
static void app_message_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_open(8, 0);
}

void handle_init(void) {
  
  app_message_init();
  //read settings from persistence storage
  if (persist_exists(CONFIG_KEY_FIRST_LANG) && persist_exists(CONFIG_KEY_SECOND_LANG)) {
    first_lang = persist_read_int(CONFIG_KEY_FIRST_LANG);
    second_lang = persist_read_int(CONFIG_KEY_SECOND_LANG);
  } else {
    first_lang = LANG_ENG;
    second_lang = NONE;
  }
  
  seed = time( NULL );
  currentID = 1;//random( NUM_POKEMON ); //Random ID between 1 and 721
  
  mode = first_lang;
  sort = SORT_NUM;
  
  window = window_create();
  window_set_background_color( window, GColorWhite);
  Layer *window_layer = window_get_root_layer( window );
  
  window_set_click_config_provider( window, click_config_provider );
  
   /////// IMAGE LEGENDARY ////////////
  legendary_image  = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LEGENDARY);
  legendary_image_layer = bitmap_layer_create( GRect( 7, 43, 12, 12) );
  bitmap_layer_set_alignment( legendary_image_layer, GAlignLeft);
  bitmap_layer_set_background_color( legendary_image_layer, GColorClear);
  bitmap_layer_set_compositing_mode( legendary_image_layer, GCompOpAnd);
  bitmap_layer_set_bitmap( legendary_image_layer, legendary_image);
  
  /////// IMAGE MEGA ////////////
  mega_image       = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MEGA);
  mega_image_layer = bitmap_layer_create( GRect( 67, 44, 12, 12) );
  bitmap_layer_set_alignment( mega_image_layer, GAlignLeft);
  bitmap_layer_set_background_color( mega_image_layer, GColorClear);
  bitmap_layer_set_compositing_mode( mega_image_layer, GCompOpAnd);
  bitmap_layer_set_bitmap( mega_image_layer, mega_image);
  
  //////// BLANK ////////////
  blank_rect_layer = layer_create( GRect( 0, 32, 95, 30) );
  layer_add_child(window_layer, blank_rect_layer);
  layer_set_update_proc(blank_rect_layer, blank_rect_layer_update);
  
  //////// TEXT LAYER ////////////
  text_layer = text_layer_create( GRect(4, 41, 140 /* width */, 101 /* height */));
  custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PKMN_14));
  text_layer_set_font( text_layer, custom_font);
  text_layer_set_text_color( text_layer, GColorBlack);
  text_layer_set_overflow_mode( text_layer, GTextOverflowModeWordWrap);
  text_layer_set_background_color( text_layer, GColorClear);
  text_layer_set_text_alignment( text_layer, GTextAlignmentCenter);
  layer_add_child( window_layer, text_layer_get_layer( text_layer ));
  
  /////// UI TOP ////////////
  top_bar_image = gbitmap_create_with_resource( RESOURCE_ID_UI_TOP );
  top_bar_layer = bitmap_layer_create(GRect( 0, 0, 144, 11));
  bitmap_layer_set_bitmap( top_bar_layer, top_bar_image);
  layer_add_child( window_layer, bitmap_layer_get_layer( top_bar_layer ));
    
  /////// UI BOTTOM ////////////
  bottom_bar_image = gbitmap_create_with_resource( RESOURCE_ID_UI_BOTTOM );
  bottom_bar_layer = bitmap_layer_create( GRect( 0, 168-11-16, 144, 11) );
  bitmap_layer_set_bitmap( bottom_bar_layer, bottom_bar_image);
  layer_add_child( window_layer, bitmap_layer_get_layer( bottom_bar_layer ));

  //////// TEXT LAYER POKEMON GENERATION ////////////
  text_layer_gen = text_layer_create( GRect(20, 5, 104 /* width */, 20 /* height */));
  custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PKMN_14));
  text_layer_set_font( text_layer_gen, custom_font);
  text_layer_set_text_color( text_layer_gen, GColorBlack);
  text_layer_set_overflow_mode( text_layer_gen, GTextOverflowModeWordWrap);
  text_layer_set_background_color( text_layer_gen, GColorClear);
  text_layer_set_text_alignment( text_layer_gen, GTextAlignmentCenter);
  layer_add_child( window_layer, text_layer_get_layer( text_layer_gen ));
  
  update_selection();
  
  window_stack_push( window, true /* Animated */);
}


///////////////////////// DE INIT /////////////////////

void handle_deinit(void) {
  bitmap_layer_destroy( poke_image_layer );
  gbitmap_destroy( poke_image );
  
  bitmap_layer_destroy( top_bar_layer );
  bitmap_layer_destroy( bottom_bar_layer );
  layer_destroy (blank_rect_layer);
  
  bitmap_layer_destroy( mega_image_layer );
  gbitmap_destroy( mega_image );
  
  bitmap_layer_destroy( legendary_image_layer );
  gbitmap_destroy( legendary_image );
  
  gbitmap_destroy( top_bar_image );
  gbitmap_destroy( bottom_bar_image );
  
  text_layer_destroy( text_layer );
  text_layer_destroy( text_layer_gen );

  window_destroy( window );
  
  fonts_unload_custom_font( custom_font );
}

///////////////////////// MAIN /////////////////////
int main(void) {
    handle_init();
    app_event_loop();
    handle_deinit();
}
