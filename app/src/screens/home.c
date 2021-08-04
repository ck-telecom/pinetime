#include <lvgl.h>

LV_FONT_DECLARE(lv_font_montserrat_32);
LV_FONT_DECLARE(lv_font_montserrat_48);

lv_point_t status_bar_line[] = { { 0, 30 }, { 239, 30} };
lv_point_t p1[] = { { 119, 234}, { 119, 239} };
lv_point_t p2[] = { { 0, 119 }, { 10, 119} };
lv_point_t p3[] = { { 114, 119 }, { 119, 119} };

lv_style_t style_line;
lv_style_t style_font;
lv_style_t style_time;
lv_style_t style_date;

lv_obj_t *time_label;
lv_obj_t *battery_label;

static void home_task(lv_task_t *task)
{
    static int counter = 0;
    uint8_t buffer[32] = { 0 };
    counter++;
    sprintf(buffer, "r: %d", counter);
    lv_label_set_text(time_label, buffer);
}

lv_obj_t *screen_home_draw()
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

    lv_obj_t *line = lv_line_create(lv_scr_act(), NULL);
    lv_obj_add_style(line, LV_LINE_PART_MAIN, &style_line);

    lv_line_set_points(line, status_bar_line, 2);
//    lv_line_set_points(line, p1, 2);
//    lv_line_set_points(line, p2, 2);
//    lv_line_set_points(line, p3, 2);

    battery_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(battery_label, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(battery_label, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    time_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(time_label, LV_LABEL_PART_MAIN, &style_time);
    lv_label_set_text(time_label, "00:00");
    lv_obj_align(time_label, NULL, LV_ALIGN_CENTER, 0, -25);

    lv_obj_t *date_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(date_label, LV_LABEL_PART_MAIN, &style_date);
    lv_label_set_text(date_label, "Jan 1, 1970");
    lv_obj_align(date_label, NULL, LV_ALIGN_CENTER, 0, 30);

    lv_task_t *task = lv_task_create(home_task, 500, LV_TASK_PRIO_MID, NULL);
    return NULL;
}
