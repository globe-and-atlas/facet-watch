#include <pebble.h>
#include "config.h"
#include "policy.h"
enum{P_PALETTE=1,P_BG,P_FG,P_ACCENT,P_TIME,P_DATE,P_SECONDS,P_DIGITAL,P_BATTERY,P_DIAL,P_UNITS,P_WTEMP,P_WCODE,P_WTIME};
typedef struct{int palette,bg,fg,accent,time,date,seconds,digital,battery,dial,units;}Settings;
static Settings s={0,0,0xFFFFFF,0xAAAAAA,0,1,1,0,1,0,0};
static const uint32_t PALETTES[6][3]={{0,0xFFFFFF,0xAAAAAA},{0xAAAAAA,0,0},{0x005500,0xFFFFAA,0xAAFFAA},{0xFFFFAA,0,0xAA0000},{0x000055,0xFFFFFF,0x55FFFF},{0xAA5500,0xFFFFAA,0xFFFF00}};
static Window *window;static Layer *base,*seconds_layer,*hands;static struct tm current;static int charge,steps=-1,wtemp=0,wcode=0,wtime=0;static time_t next_weather=0;static bool messaging_ready=false;
#if FACET_FIXTURE
static int fsteps=5832,fweather=1,ftextonly=0;
static int fh=FACET_HOUR,fm=FACET_MINUTE,fs=FACET_SECOND,fb=FACET_BATTERY;static Calendar fd={FACET_YEAR,FACET_MONTH,FACET_DAY};
#endif
static uint32_t color(int i){return s.palette<6?PALETTES[s.palette][i]:(uint32_t)(i==0?s.bg:i==1?s.fg:s.accent);}
static uint32_t dim(void){return mix_color(color(0),color(1),1);}
static uint32_t edge(void){return mix_color(color(0),color(1),2);}
static GPoint radial(int x,int y,int angle,int radius){int32_t a=TRIG_MAX_ANGLE*angle/3600;return GPoint(x+(int32_t)sin_lookup(a)*radius/TRIG_MAX_RATIO,y-(int32_t)cos_lookup(a)*radius/TRIG_MAX_RATIO);}
static void polygon(GContext *ctx,GPoint *points,int count,uint32_t fill){GPathInfo info={count,points};GPath *path=gpath_create(&info);graphics_context_set_fill_color(ctx,GColorFromHEX(fill));gpath_draw_filled(ctx,path);gpath_destroy(path);}
static void rotated(GContext *ctx,const GPoint *local,int count,int x,int y,int angle,uint32_t fill){GPathInfo info={count,(GPoint*)local};GPath *path=gpath_create(&info);gpath_rotate_to(path,TRIG_MAX_ANGLE*angle/3600);gpath_move_to(path,GPoint(x,y));graphics_context_set_fill_color(ctx,GColorFromHEX(fill));gpath_draw_filled(ctx,path);gpath_destroy(path);}
static void label(GContext *ctx,const char *value,GRect box,const char *font,GTextAlignment alignment,uint32_t ink){graphics_context_set_text_color(ctx,GColorFromHEX(ink));graphics_draw_text(ctx,value,fonts_get_system_font(font),box,GTextOverflowModeFill,alignment,NULL);}
/* Soft octagon: straight cardinal flats, short diagonals, rounded junctions. */
static void dial_frame(GContext *ctx){
 GPoint outer[]={{4,80},{4,77},{5,73},{5,70},{7,67},{8,64},{10,61},{12,59},{14,56},{44,28},{46,25},{49,23},{52,22},{55,20},{58,19},{61,19},{65,18},{68,18},{132,18},{135,18},{139,19},{142,19},{145,20},{148,22},{151,23},{154,25},{156,28},{186,56},{188,59},{190,61},{192,64},{193,67},{195,70},{195,73},{196,77},{196,80},{196,148},{196,151},{195,155},{195,158},{193,161},{192,164},{190,167},{188,169},{186,172},{156,200},{154,203},{151,205},{148,206},{145,208},{142,209},{139,209},{135,210},{132,210},{68,210},{65,210},{61,209},{58,209},{55,208},{52,206},{49,205},{46,203},{44,200},{14,172},{12,169},{10,167},{8,164},{7,161},{5,158},{5,155},{4,151},{4,148}};
 GPoint inner[]={{9,82},{9,79},{10,75},{10,72},{12,70},{13,67},{15,64},{17,62},{19,59},{47,33},{49,30},{52,28},{55,27},{57,25},{60,24},{63,24},{67,23},{70,23},{130,23},{133,23},{137,24},{140,24},{143,25},{145,27},{148,28},{151,30},{153,33},{181,59},{183,62},{185,64},{187,67},{188,70},{190,72},{190,75},{191,79},{191,82},{191,146},{191,149},{190,153},{190,156},{188,158},{187,161},{185,164},{183,166},{181,169},{153,195},{151,198},{148,200},{145,201},{143,203},{140,204},{137,204},{133,205},{130,205},{70,205},{67,205},{63,204},{60,204},{57,203},{55,201},{52,200},{49,198},{47,195},{19,169},{17,166},{15,164},{13,161},{12,158},{10,156},{10,153},{9,149},{9,146}};
 polygon(ctx,outer,72,dim());polygon(ctx,inner,72,color(0));
}
static void draw_fan(GContext *ctx){
 Calendar today={current.tm_year+1900,current.tm_mon+1,current.tm_mday};Calendar prior=previous_day(today),after=next_day(today);
 GPoint fan[]={{149,94},{180,86},{187,103},{187,125},{180,142},{149,134}};polygon(ctx,fan,6,dim());
 GPoint inside[]={{153,97},{178,91},{182,105},{182,123},{178,137},{153,131}};polygon(ctx,inside,6,color(0));
 char value[16];int days[]={after.day,today.day,prior.day};
 for(int i=0;i<3;i++){snprintf(value,sizeof(value),"%d",days[i]);label(ctx,value,GRect(158,91+i*15,25,20),FONT_KEY_GOTHIC_18_BOLD,GTextAlignmentCenter,i==1?color(1):dim());}
 graphics_context_set_fill_color(ctx,GColorFromHEX(color(1)));graphics_fill_rect(ctx,GRect(149,111,6,5),1,GCornersAll);
}
#if defined(PBL_ROUND)
#include "round_analog.h"
static RoundData round_state(void){RoundData d={0};d.now=current;d.bg=color(0);d.fg=color(1);d.accent=color(2);d.battery=charge;d.time_mode=s.time;d.date_mode=s.date;d.seconds=s.seconds;d.digital=s.digital;d.show_battery=s.battery;d.dial=s.dial;d.units=s.units;d.steps=steps;d.temp=wtemp;d.code=wcode;d.stamp=wtime;
#if FACET_FIXTURE
 d.phase=ftextonly;d.stamp=fweather==1?(int)time(NULL):fweather==2?1:0;
#endif
return d;}
#endif
static void draw_base(Layer *own,GContext *ctx){
#if defined(PBL_ROUND)
 {(void)own;RoundData d=round_state();round_analog_base(ctx,&d,3,rseven);rproof_marker(ctx);return;}
#endif

 graphics_context_set_antialiased(ctx,false);graphics_context_set_fill_color(ctx,GColorFromHEX(color(0)));graphics_fill_rect(ctx,layer_get_bounds(own),0,GCornerNone);
#if FACET_FIXTURE
 if(ftextonly)return;
#endif
 dial_frame(ctx);
 graphics_context_set_stroke_color(ctx,GColorFromHEX(dim()));graphics_context_set_stroke_width(ctx,1);
 for(int i=0;i<60;i++)if(i%5){graphics_draw_line(ctx,radial(100,114,i*60,85),radial(100,114,i*60,88));}
 const GPoint marker[]={{-4,-83},{4,-83},{4,-65},{-4,-65}};const GPoint facet[]={{2,-82},{4,-82},{4,-66},{2,-66}};
 for(int h=0;h<12;h++){
  if(h==3 && s.date==1)continue;
  if(h==9 && s.seconds){graphics_context_set_fill_color(ctx,GColorFromHEX(edge()));graphics_fill_rect(ctx,GRect(12,111,7,6),0,GCornerNone);continue;}
  if(h==0){const GPoint left[]={{-7,-83},{-2,-83},{-2,-65},{-7,-65}};const GPoint right[]={{2,-83},{7,-83},{7,-65},{2,-65}};rotated(ctx,left,4,100,114,0,edge());rotated(ctx,right,4,100,114,0,edge());}
  else{rotated(ctx,marker,4,100,114,h*300,edge());rotated(ctx,facet,4,100,114,h*300,dim());}
 }
 label(ctx,"P-SHOCK",GRect(66,65,68,19),FONT_KEY_GOTHIC_14_BOLD,GTextAlignmentCenter,edge());
 if(s.date==1)draw_fan(ctx);
 if(s.battery){char value[16];snprintf(value,sizeof(value),"%d%%",charge);label(ctx,"BATTERY",GRect(65,148,70,16),FONT_KEY_GOTHIC_14,GTextAlignmentCenter,dim());label(ctx,value,GRect(65,159,70,20),FONT_KEY_GOTHIC_18_BOLD,GTextAlignmentCenter,color(1));}
 char date[64]="",digital[32]="",footer[128]="";
 if(s.date==2)snprintf(date,sizeof(date),"%04d-%02d-%02d",current.tm_year+1900,current.tm_mon+1,current.tm_mday);
 if(s.date==3)snprintf(date,sizeof(date),"%02d/%02d/%04d",current.tm_mon+1,current.tm_mday,current.tm_year+1900);
 if(s.date==4)snprintf(date,sizeof(date),"%02d/%02d/%04d",current.tm_mday,current.tm_mon+1,current.tm_year+1900);
 if(s.digital){int h=display_hour(current.tm_hour,s.time,clock_is_24h_style());snprintf(digital,sizeof(digital),uses24(s.time,clock_is_24h_style())?"%02d:%02d":"%02d:%02d%s",h,current.tm_min,current.tm_hour>=12?"PM":"AM");}
 snprintf(footer,sizeof(footer),"%s%s%s",digital,digital[0]&&date[0]?"  ":"",date);
 label(ctx,footer,GRect(4,210,192,18),FONT_KEY_GOTHIC_14_BOLD,GTextAlignmentCenter,color(1));
}
static const char *condition(int code){
 if(code==0)return "CLEAR";
 if(code<=3)return "CLOUD";
 if(code==45 || code==48)return "FOG";
 if(code>=95)return "STORM";
 if((code>=71 && code<=77) || code==85 || code==86)return "SNOW";
 if((code>=51 && code<=67) || (code>=80 && code<=82))return "RAIN";
 return "--";
}
static void draw_complication(GContext *ctx){
 #if FACET_FIXTURE
 if(!ftextonly)
#endif
 {graphics_context_set_stroke_color(ctx,GColorFromHEX(dim()));graphics_context_set_stroke_width(ctx,2);graphics_draw_circle(ctx,GPoint(36,36),33);}
 char value[24];const char *title=s.dial==1?"WEATHER":s.dial==2?"BATTERY":"STEPS";
 label(ctx,title,GRect(13,13,46,18),FONT_KEY_GOTHIC_14_BOLD,GTextAlignmentCenter,edge());
 if(s.dial==2){
  snprintf(value,sizeof(value),"%d%%",charge);
  label(ctx,value,GRect(7,23,58,27),FONT_KEY_GOTHIC_24_BOLD,GTextAlignmentCenter,color(1));
  graphics_context_set_fill_color(ctx,GColorFromHEX(dim()));graphics_fill_rect(ctx,GRect(19,56,34,3),1,GCornersAll);
  graphics_context_set_fill_color(ctx,GColorFromHEX(color(2)));if(charge)graphics_fill_rect(ctx,GRect(19,56,34*charge/100,3),1,GCornersAll);
 }else if(s.dial==3){
  if(steps<0)snprintf(value,sizeof(value),"--");
  else if(steps<10000)snprintf(value,sizeof(value),"%d",steps);
  else if(steps<100000)snprintf(value,sizeof(value),"%d.%dk",steps/1000,(steps%1000)/100);
  else snprintf(value,sizeof(value),"%dk",steps/1000);
  label(ctx,value,GRect(7,23,58,27),FONT_KEY_GOTHIC_24_BOLD,GTextAlignmentCenter,color(1));
  label(ctx,steps<0?"NO DATA":"TODAY",GRect(13,43,46,16),FONT_KEY_GOTHIC_14,GTextAlignmentCenter,edge());
 }else{
  bool fresh=weather_fresh((int)time(NULL),wtime);
#if FACET_FIXTURE
  fresh=fweather==1;
#endif
  if(fresh)snprintf(value,sizeof(value),"%d%s",temperature_degrees(wtemp,s.units),s.units?"C":"F");else snprintf(value,sizeof(value),"--");
  label(ctx,value,GRect(7,23,58,27),FONT_KEY_GOTHIC_24_BOLD,GTextAlignmentCenter,color(1));
  label(ctx,fresh?condition(wcode):(wtime?"STALE":"NO DATA"),GRect(13,43,46,16),FONT_KEY_GOTHIC_14,GTextAlignmentCenter,edge());
 }
}
static void read_steps(void){
 steps=-1;
#if FACET_FIXTURE
 steps=fsteps;
#else
 if(s.dial==3 && s.seconds){time_t now=time(NULL);HealthServiceAccessibilityMask access=health_service_metric_accessible(HealthMetricStepCount,time_start_of_today(),now);if(access & HealthServiceAccessibilityMaskAvailable)steps=(int)health_service_sum_today(HealthMetricStepCount);}
#endif
}
static void health_changed(HealthEventType event,void *context){(void)event;(void)context;if(s.dial==3 && s.seconds){read_steps();layer_mark_dirty(seconds_layer);}}
static void request_weather(void){
#if !FACET_FIXTURE
 time_t now=time(NULL);if(!messaging_ready || s.dial!=1 || !s.seconds || now<next_weather)return;
 next_weather=now+300;DictionaryIterator *out;if(app_message_outbox_begin(&out)==APP_MSG_OK){dict_write_int32(out,MESSAGE_KEY_WeatherRequest,1);app_message_outbox_send();}
#endif
}
static void seconds_labels(GContext *ctx){
 label(ctx,"60",GRect(26,8,20,16),FONT_KEY_GOTHIC_14_BOLD,GTextAlignmentCenter,edge());
 label(ctx,"40",GRect(9,35,18,16),FONT_KEY_GOTHIC_14,GTextAlignmentCenter,edge());label(ctx,"20",GRect(45,35,18,16),FONT_KEY_GOTHIC_14,GTextAlignmentCenter,edge());
}
static void draw_seconds(Layer *own,GContext *ctx){
#if defined(PBL_ROUND)
 {(void)own;RoundData d=round_state();round_pbl_dial(ctx,&d);return;}
#endif

#if FACET_FIXTURE
 if(ftextonly){if(s.dial==0)seconds_labels(ctx);else draw_complication(ctx);return;}
#endif
 (void)own;graphics_context_set_antialiased(ctx,false);graphics_context_set_fill_color(ctx,GColorFromHEX(color(0)));graphics_fill_circle(ctx,GPoint(36,36),34);
 if(s.dial!=0){draw_complication(ctx);return;}
 graphics_context_set_stroke_width(ctx,1);graphics_context_set_stroke_color(ctx,GColorFromHEX(dim()));graphics_context_set_stroke_width(ctx,2);graphics_draw_circle(ctx,GPoint(36,36),33);graphics_context_set_stroke_width(ctx,1);
 for(int i=0;i<30;i++){graphics_context_set_stroke_color(ctx,GColorFromHEX(i%5?dim():edge()));graphics_draw_line(ctx,radial(36,36,i*120,i%5?29:27),radial(36,36,i*120,32));}
 seconds_labels(ctx);
 graphics_context_set_stroke_color(ctx,GColorFromHEX(color(2)));graphics_context_set_stroke_width(ctx,2);graphics_draw_line(ctx,radial(36,36,second_angle(current.tm_sec),-6),radial(36,36,second_angle(current.tm_sec),26));
 const GPoint counter[]={{-3,4},{3,4},{4,9},{0,12},{-4,9}};rotated(ctx,counter,5,36,36,second_angle(current.tm_sec),edge());
 graphics_context_set_fill_color(ctx,GColorFromHEX(color(2)));graphics_fill_circle(ctx,GPoint(36,36),3);
}
static void draw_hand(GContext *ctx,int angle,int length,int width){
 GPoint outer[]={{0,-length},{width,-length+5},{width,-9},{width+2,-5},{width,10},{-width,10},{-width-2,-5},{-width,-9},{-width,-length+5}};
 rotated(ctx,outer,9,100,114,angle,edge());GPoint lume[]={{-2,-length+10},{2,-length+10},{2,-13},{-2,-13}};rotated(ctx,lume,4,100,114,angle,color(1));
}
static void draw_hands(Layer *own,GContext *ctx){
#if defined(PBL_ROUND)
 {(void)own;RoundData d=round_state();round_analog_hands(ctx,&d,3);rproof_marker(ctx);return;}
#endif

#if FACET_FIXTURE
 if(ftextonly)return;
#endif
 (void)own;graphics_context_set_antialiased(ctx,false);draw_hand(ctx,hour_angle(current.tm_hour,current.tm_min),52,6);draw_hand(ctx,minute_angle(current.tm_min),73,4);graphics_context_set_fill_color(ctx,GColorFromHEX(dim()));graphics_fill_circle(ctx,GPoint(100,114),7);graphics_context_set_fill_color(ctx,GColorFromHEX(edge()));graphics_fill_circle(ctx,GPoint(100,114),5);graphics_context_set_fill_color(ctx,GColorFromHEX(color(1)));graphics_fill_circle(ctx,GPoint(100,114),2);}
static void tick(struct tm *now,TimeUnits changed){current=*now;
#if FACET_FIXTURE
 current.tm_hour=fh;current.tm_min=fm;current.tm_sec=fs;current.tm_year=fd.year-1900;current.tm_mon=fd.month-1;current.tm_mday=fd.day;
#endif
 if(seconds_active(s.seconds,s.dial))layer_mark_dirty(seconds_layer);
 if(changed&MINUTE_UNIT){read_steps();request_weather();if(s.seconds)layer_mark_dirty(seconds_layer);layer_mark_dirty(base);layer_mark_dirty(hands);}
}
static void battery_changed(BatteryChargeState state){charge=state.charge_percent;
#if FACET_FIXTURE
 charge=fb;
#endif
 layer_mark_dirty(base);if(s.dial==2)layer_mark_dirty(seconds_layer);
}
static void subscription(void){tick_timer_service_unsubscribe();tick_timer_service_subscribe(seconds_active(s.seconds,s.dial)?SECOND_UNIT:MINUTE_UNIT,tick);}
static void setting(DictionaryIterator *iter,uint32_t key,int persistent,int maximum,int *target){Tuple *t=dict_find(iter,key);if(t && (t->type==TUPLE_INT || t->type==TUPLE_UINT)){int v=t->value->int32;if(v>=0 && v<=maximum){*target=v;persist_write_int(persistent,v);}}}
#if FACET_FIXTURE
static void fixture(DictionaryIterator *iter,int key,int maximum,int *target){Tuple *t=dict_find(iter,key);if(t && t->type==TUPLE_INT && t->value->int32>=0 && t->value->int32<=maximum)*target=t->value->int32;}
#endif
static void inbox(DictionaryIterator *iter,void *context){
#if defined(PBL_ROUND) && (FACET_FIXTURE)
 Tuple *round_proof=dict_find(iter,11022);if(round_proof && round_proof->type==TUPLE_INT){r_proof=round_proof->value->int32==1;layer_mark_dirty(base);}
#endif
(void)context;int old=seconds_active(s.seconds,s.dial);int olddial=s.dial;
 setting(iter,MESSAGE_KEY_Palette,P_PALETTE,6,&s.palette);setting(iter,MESSAGE_KEY_Background,P_BG,0xFFFFFF,&s.bg);setting(iter,MESSAGE_KEY_Foreground,P_FG,0xFFFFFF,&s.fg);setting(iter,MESSAGE_KEY_Accent,P_ACCENT,0xFFFFFF,&s.accent);setting(iter,MESSAGE_KEY_TimeFormat,P_TIME,2,&s.time);setting(iter,MESSAGE_KEY_DateFormat,P_DATE,4,&s.date);setting(iter,MESSAGE_KEY_ShowSeconds,P_SECONDS,1,&s.seconds);setting(iter,MESSAGE_KEY_DigitalTime,P_DIGITAL,1,&s.digital);setting(iter,MESSAGE_KEY_ShowBattery,P_BATTERY,1,&s.battery);setting(iter,MESSAGE_KEY_LeftDial,P_DIAL,3,&s.dial);setting(iter,MESSAGE_KEY_WeatherUnits,P_UNITS,1,&s.units);
 Tuple *wt=dict_find(iter,MESSAGE_KEY_WeatherTime),*temp=dict_find(iter,MESSAGE_KEY_WeatherTemp),*code=dict_find(iter,MESSAGE_KEY_WeatherCode);
 if(wt && (wt->type==TUPLE_INT || wt->type==TUPLE_UINT)){int stamp=wt->value->int32;if(stamp==0){wtime=0;persist_write_int(P_WTIME,0);}else if(temp && code && (temp->type==TUPLE_INT || temp->type==TUPLE_UINT) && (code->type==TUPLE_INT || code->type==TUPLE_UINT) && weather_fresh((int)time(NULL),stamp) && temp->value->int32>=-900 && temp->value->int32<=650 && code->value->int32>=0 && code->value->int32<=99){wtemp=temp->value->int32;wcode=code->value->int32;wtime=stamp;persist_write_int(P_WTEMP,wtemp);persist_write_int(P_WCODE,wcode);persist_write_int(P_WTIME,wtime);}}
#if FACET_FIXTURE
 fixture(iter,11000,23,&fh);fixture(iter,11001,59,&fm);fixture(iter,11002,59,&fs);fixture(iter,11004,100,&fb);Calendar candidate=fd;fixture(iter,11005,2099,&candidate.year);fixture(iter,11006,12,&candidate.month);fixture(iter,11007,31,&candidate.day);if(valid_date(candidate))fd=candidate;charge=fb;
 Tuple *st=dict_find(iter,11009);if(st && st->type==TUPLE_INT && st->value->int32>=-1 && st->value->int32<=999999)fsteps=st->value->int32;
 fixture(iter,11013,1,&ftextonly);fixture(iter,11010,2,&fweather);Tuple *ft=dict_find(iter,11011);if(ft && ft->type==TUPLE_INT && ft->value->int32>=-900 && ft->value->int32<=650)wtemp=ft->value->int32;fixture(iter,11012,99,&wcode);if(fweather==0)wtime=0;else if(fweather==2)wtime=1;
#endif
 if(old!=seconds_active(s.seconds,s.dial))subscription();
 if(olddial!=s.dial)next_weather=0;
 layer_set_hidden(seconds_layer,!s.seconds);time_t now=time(NULL);tick(localtime(&now),MINUTE_UNIT);layer_mark_dirty(hands);layer_mark_dirty(seconds_layer);
}
static void restore(int key,int maximum,int *target){if(persist_exists(key)){int value=persist_read_int(key);if(value>=0 && value<=maximum)*target=value;}}
static void init(void){restore(P_PALETTE,6,&s.palette);restore(P_BG,0xFFFFFF,&s.bg);restore(P_FG,0xFFFFFF,&s.fg);restore(P_ACCENT,0xFFFFFF,&s.accent);restore(P_TIME,2,&s.time);restore(P_DATE,4,&s.date);restore(P_SECONDS,1,&s.seconds);restore(P_DIGITAL,1,&s.digital);restore(P_BATTERY,1,&s.battery);restore(P_DIAL,3,&s.dial);restore(P_UNITS,1,&s.units);if(persist_exists(P_WTIME)){wtime=persist_read_int(P_WTIME);wtemp=persist_read_int(P_WTEMP);wcode=persist_read_int(P_WCODE);}
 window=window_create();Layer *root=window_get_root_layer(window);base=layer_create(PBL_IF_ROUND_ELSE(GRect(0,0,PBL_DISPLAY_WIDTH,PBL_DISPLAY_HEIGHT),GRect(0,0,200,228)));layer_set_update_proc(base,draw_base);layer_add_child(root,base);seconds_layer=layer_create(PBL_IF_ROUND_ELSE(rb(20,60,60,60),GRect(16,78,72,72)));layer_set_update_proc(seconds_layer,draw_seconds);layer_add_child(root,seconds_layer);layer_set_hidden(seconds_layer,!s.seconds);hands=layer_create(PBL_IF_ROUND_ELSE(GRect(0,0,PBL_DISPLAY_WIDTH,PBL_DISPLAY_HEIGHT),GRect(0,0,200,228)));layer_set_update_proc(hands,draw_hands);layer_add_child(root,hands);window_stack_push(window,true);time_t now=time(NULL);tick(localtime(&now),MINUTE_UNIT);battery_changed(battery_state_service_peek());subscription();battery_state_service_subscribe(battery_changed);health_service_events_subscribe(health_changed,NULL);app_message_register_inbox_received(inbox);app_message_open(512,64);messaging_ready=true;request_weather();
}
static void deinit(void){tick_timer_service_unsubscribe();battery_state_service_unsubscribe();health_service_events_unsubscribe();app_message_deregister_callbacks();layer_destroy(hands);layer_destroy(seconds_layer);layer_destroy(base);window_destroy(window);}
int main(void){init();app_event_loop();deinit();}
