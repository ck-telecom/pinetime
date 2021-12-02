#include <lvgl.h>

#include "../view.h"
#include "../service/display.h"

#if 1
LV_FONT_DECLARE(lv_font_montserrat_16);
LV_FONT_DECLARE(lv_font_montserrat_32);
LV_FONT_DECLARE(lv_font_montserrat_48);

lv_point_t status_bar_line[] = { { 0, 30 }, { 239, 30} };

static lv_style_t style_line;
lv_style_t style_font;
lv_style_t style_time;
lv_style_t style_date;

lv_obj_t *time_label;
lv_obj_t *battery_label;
lv_obj_t *charger_label;

static lv_task_t *task;

static void home_task(lv_task_t *task)
{
    static int counter = 0;
    uint8_t buffer[32] = { 0 };
    counter++;
    sprintf(buffer, "r: %d", counter);
    lv_label_set_text(time_label, buffer);
}

static void roller_callback(lv_obj_t *obj, lv_event_t event)
{

	if(event == LV_EVENT_CLICKED){
		//printf("\r\nclick = %d",event);
		//if(i++ >=6)
		//	i=0;
		//lv_roller_set_selected(obj, i, LV_ANIM_OFF);
	}
    lv_label_set_text(obj, "Clicked");
}

#if 0
static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}
#endif

int screen_home_draw(struct view *v, lv_obj_t *parent)
{
/*    lv_obj_t* bg_clock_img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(bg_clock_img, &bg_clock);
    lv_obj_align(bg_clock_img, NULL, LV_ALIGN_CENTER, 0, 0);
*/
    lv_style_init(&style_line);
    lv_style_init(&style_font);
    lv_style_init(&style_time);
    lv_style_init(&style_date);

    lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 3);
    lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_line_rounded(&style_line, LV_STATE_DEFAULT, true);

    lv_style_set_text_font(&style_time, LV_STATE_DEFAULT, &lv_font_montserrat_48);
    lv_style_set_text_color(&style_time, LV_STATE_DEFAULT, LV_COLOR_BLUE);

    lv_style_set_text_font(&style_date, LV_STATE_DEFAULT, &lv_font_montserrat_32);
    lv_style_set_text_color(&style_date, LV_STATE_DEFAULT, LV_COLOR_GREEN);

    lv_style_set_text_font(&style_font, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_style_set_text_color(&style_font, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    lv_obj_t *line = lv_line_create(lv_scr_act(), NULL);
    lv_obj_add_style(line, LV_LINE_PART_MAIN, &style_line);

    lv_line_set_points(line, status_bar_line, 2);
//    lv_line_set_points(line, p1, 2);
//    lv_line_set_points(line, p2, 2);
//    lv_line_set_points(line, p3, 2);

    battery_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(battery_label, LV_LABEL_PART_MAIN, &style_font);
    lv_label_set_text(battery_label, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(battery_label, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    charger_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(charger_label, LV_LABEL_PART_MAIN, &style_font);
    lv_label_set_text(charger_label, LV_SYMBOL_CHARGE);
    lv_obj_align(charger_label, NULL, LV_ALIGN_IN_TOP_RIGHT, -30, 0);

    time_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(time_label, LV_LABEL_PART_MAIN, &style_time);
    lv_label_set_text(time_label, "00:00");
    lv_obj_align(time_label, NULL, LV_ALIGN_CENTER, 0, -25);

    lv_obj_t *date_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(date_label, LV_LABEL_PART_MAIN, &style_date);
    lv_label_set_text(date_label, "Jan 1, 1970");
    lv_obj_align(date_label, NULL, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_event_cb(date_label, roller_callback);
#if 0
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, parent);
    lv_anim_set_values(&a, lv_obj_get_y(parent), 100);
    lv_anim_set_time(&a, 500);
    lv_anim_set_exec_cb(&a, anim_x_cb);
    lv_anim_set_path(&a, lv_anim_path_overshoot);
    lv_anim_start(&a);
#endif
    task = lv_task_create(home_task, 500, LV_TASK_PRIO_MID, NULL);
    return 0;
}

int home_exit(struct view *v, lv_obj_t *parent)
{
    lv_task_del(task);

    lv_obj_clean(parent);

    return 0;
}

bool home_event(struct view *view, uint32_t event, void *arg)
{
    uint32_t gesture = *(uint32_t *)arg;
    if (gesture == 0x02) {
        view_switch_screen(view, &clock_face_view);
    }
}
#endif

struct view home = {
    .id = HOME_ID,
    .init = screen_home_draw,
    .event = home_event,
    .exit = home_exit,
};