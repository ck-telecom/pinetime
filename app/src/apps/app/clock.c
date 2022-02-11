#include "clock.h"

static const app_spec_t clock_spec;

clock_app_t clock_app = {
	.app = {.spec = &clock_spec }
};

static inline clock_app_t *_from_app(app_t *app)
{
	return CONTAINER_OF(app, clock_app_t, app);
}
#if 0
static void anim_y_cb(void * var, int32_t v)
{
	lv_obj_set_y(var, v);
}

static void anim_label(lv_obj_t *obj, bool show)
{
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, obj);

	if (show) {
		lv_anim_set_values(&a, 20, -25);
		lv_anim_set_time(&a, 500);
		lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
	} else {
		lv_anim_set_values(&a, -25, 20);
		lv_anim_set_time(&a, 500);
		lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
	}
	lv_anim_set_exec_cb(&a, anim_y_cb);
	lv_anim_set_ready_cb(&a, NULL);
	lv_anim_start(&a);
}

static lv_obj_t *screen_clock_create(clock_app_t *ht, lv_obj_t *parent)
{
    //UTCTimeStruct time_tmp;

    lv_obj_t *scr = lv_obj_create(parent);

    lv_obj_clear_flag(scr, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_remove_style_all(scr);                            /*Make it transparent*/
    lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
    //lv_obj_set_scroll_snap_y(scr, LV_SCROLL_SNAP_CENTER);    /*Snap the children to the center*/

    //lv_obj_t *scr = lv_scr_act();

    //lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);

    //get_UTC_time(&time_tmp);


    lv_obj_t * lv_timeh = lv_label_create(scr);
    lv_obj_set_style_text_font(lv_timeh, &lv_font_clock_90, 0);
    lv_label_set_text_fmt(lv_timeh, "%02i", time_tmp.hour);
    lv_obj_set_style_text_align(lv_timeh, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lv_timeh, lv_color_make(0xff, 0xff, 0xff), 0);
    lv_obj_align(lv_timeh, LV_ALIGN_CENTER, -55, -35);

    ht->lv_timeh = lv_timeh;

    lv_obj_t * lv_timem = lv_label_create(scr);
    lv_obj_set_style_text_font(lv_timem, &lv_font_clock_90, 0);
    lv_label_set_text_fmt(lv_timem, "%02i", time_tmp.minutes);
    lv_obj_set_style_text_align(lv_timem, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lv_timem, lv_color_make(0x00, 0xff, 0x00), 0);
    lv_obj_align(lv_timem, LV_ALIGN_CENTER, 55, -35);

    ht->lv_timem = lv_timem;

    lv_obj_t * lv_times = lv_label_create(scr);
    lv_obj_set_style_text_font(lv_times, &lv_font_clock_90, 0);
    lv_label_set_text_static(lv_times, ":");
    lv_obj_set_style_text_align(lv_times, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lv_times, lv_color_make(0xff, 0xff, 0x00), 0);
    lv_obj_align(lv_times, LV_ALIGN_CENTER, 0, -35);

    ht->lv_times = lv_times;

    lv_obj_t * lv_date = lv_label_create(scr);
    //lv_obj_set_style_text_font(lv_date, &lv_font_clock_42, 0);
    lv_label_set_recolor(lv_date, true);
    lv_label_set_text_fmt(lv_date, "#00ff00 %s# %02i %s", get_days(time_tmp.week), time_tmp.day, get_months(time_tmp.month));
    lv_obj_set_style_text_align(lv_date, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lv_date, lv_color_make(0xff, 0xff, 0xff), 0);
    lv_obj_align(lv_date, LV_ALIGN_CENTER, 0, 25);

    ht->lv_date = lv_date;

    /*lv_obj_t * lv_ble = lv_label_create(scr);
    lv_obj_align(lv_ble, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_label_set_text(lv_ble, "\xEE\xA4\x83");
    if ( pinetimecos.bluetoothState == StatusOFF ) {
        lv_obj_set_style_text_color(lv_ble, lv_color_hex(0x909090), 0);
    } else {
        lv_obj_set_style_text_color(lv_ble, lv_color_hex(0x0000ff), 0);
    }

    ht->lv_ble = lv_ble;

    lv_obj_t * lv_power = lv_label_create(scr);
    lv_obj_set_style_text_color(lv_power, lv_color_hex(0xffffff), 0);
    lv_obj_align(lv_power, LV_ALIGN_TOP_RIGHT, -35, 5);
    if ( pinetimecos.chargingState == StatusOFF ) {
        lv_label_set_text_fmt(lv_power, "%s", battery_get_icon());
    } else {
        lv_label_set_text(lv_power, "\xEE\xA4\x85 \xEE\xA4\xA0");
    }

    ht->lv_power = lv_power;*/


    ht->time_old = time_tmp;


    lv_obj_t * lv_debug = lv_label_create(scr);
    ht->lv_demo = lv_debug;
    lv_obj_set_style_text_color(ht->lv_demo, lv_color_make(0xff, 0xf, 0xff), 0);
    lv_obj_set_width(ht->lv_demo, 240);
    lv_label_set_long_mode(ht->lv_demo, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_recolor(ht->lv_demo, true);
    if ( pinetimecosBLE.weather.hasData ) {
        lv_obj_align(ht->lv_demo, LV_ALIGN_BOTTOM_LEFT, 0, -25);
        lv_label_set_text_fmt(ht->lv_demo, "%s - %s, %i��C - For today #ff0000 %i��C# #0000ff %i��C#",
            pinetimecosBLE.weather.location,
            pinetimecosBLE.weather.currentCondition,
            pinetimecosBLE.weather.currentTemp,
            pinetimecosBLE.weather.todayMaxTemp,
            pinetimecosBLE.weather.todayMinTemp
            );
    } else {
        lv_obj_align(ht->lv_demo, LV_ALIGN_BOTTOM_LEFT, 0, -25);
        lv_label_set_text_static(ht->lv_demo, "");
    }

    return scr;
}
#endif
static void clock_update_task(lv_task_t *task)
{
	app_t *app = task->user_data;
	clock_app_t *htapp = _from_app(app);

	//update(app);
}

static int clock_init(app_t *app, lv_obj_t *parent)
{
	clock_app_t *htapp = _from_app(app);
	//htapp->screen = screen_clock_create(htapp, parent);

	htapp->lv_task_clock = lv_task_create(clock_update_task, 500, LV_TASK_PRIO_MID, app);

	return 0;
}
#if 0
static int update(app_t *app)
{
    UTCTimeStruct time_tmp;

    clock_app_t *ht = _from_app(app);
    get_UTC_time(&time_tmp);

    if (time_tmp.hour != ht->time_old.hour)
        lv_label_set_text_fmt(ht->lv_timeh, "%02i", time_tmp.hour);

    if (time_tmp.minutes != ht->time_old.minutes)
        lv_label_set_text_fmt(ht->lv_timem, "%02i", time_tmp.minutes);

    if ( time_tmp.seconds % 2 == 0 ) {
      lv_label_set_text_static(ht->lv_times,  ":");
    } else {
      lv_label_set_text_static(ht->lv_times,  " ");
    }

    //lv_label_set_text_fmt(ht->lv_time_sec, "%02i", time_tmp.seconds);

    if (time_tmp.day != ht->time_old.day)
        lv_label_set_text_fmt(ht->lv_date, "#00ff00 %s# %02i %s", get_days(time_tmp.week), time_tmp.day, get_months(time_tmp.month));

    /*if ( pinetimecos.bluetoothState == StatusOFF ) {
        lv_obj_set_style_text_color(ht->lv_ble, lv_color_hex(0x404040), 0);
    } else {
        lv_obj_set_style_text_color(ht->lv_ble, lv_color_hex(0x0000ff), 0);
    }

    if ( pinetimecos.chargingState == StatusOFF ) {
        lv_label_set_text_fmt(ht->lv_power, "%s", battery_get_icon());
    } else {
        lv_label_set_text(ht->lv_power, "\xEE\xA4\x85 \xEE\xA4\xA0");
    }*/

    //lv_label_set_text_fmt(ht->lv_demo, "%i", pinetimecos.debug);
    if ( pinetimecosBLE.weather.newData ) {
        anim_label(ht->lv_demo, false);
        lv_label_set_text_fmt(ht->lv_demo, "%s - %s, %i��C - For today #ff0000 %i��C# #0000ff %i��C#",
            pinetimecosBLE.weather.location,
            pinetimecosBLE.weather.currentCondition,
            pinetimecosBLE.weather.currentTemp,
            pinetimecosBLE.weather.todayMaxTemp,
            pinetimecosBLE.weather.todayMinTemp
            );
        pinetimecosBLE.weather.newData = false;
        anim_label(ht->lv_demo, true);
    }

    ht->time_old = time_tmp;

    return 0;
}

static int gesture(app_t *app, enum appGestures gesture)
{

    switch (gesture) {
        case DirBottom:
            load_application(Menu, AnimDown);
            break;
        case DirTop:
            load_application(Notification, AnimUp);
            break;
        case DirRight:
            // Harterate app
            break;
        case DirLeft:
            // Step Counter app
            break;
        default:
            break;
    }
    return 0;
}
#endif
static int clock_exit(app_t *app)
{
	clock_app_t *ht = _from_app(app);

	lv_obj_clean(ht->screen);
	lv_obj_del(ht->screen);
	ht->screen = NULL;
	lv_task_del(ht->lv_task_clock);

	return 0;
}

static const app_spec_t clock_spec = {
	.name = "clock",
	.init = clock_init,
	//.event = app_event_handler,
	.exit = clock_exit,
};