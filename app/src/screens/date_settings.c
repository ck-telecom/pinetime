#include <lvgl.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "../view.h"
#include "../service/display_api.h"
#include "../event/event_service.h"

LOG_MODULE_REGISTER(date_settings, LOG_LEVEL_INF);

lv_obj_t* roller1;

void accel_raw_data_handler(EventServiceCommand command, void *data, void *context)
{
    struct sensor_value *value = (struct sensor_value *)data;

    double x = sensor_value_to_double(&value[0]);
    double y = sensor_value_to_double(&value[1]);
    double z = sensor_value_to_double(&value[2]);

    LOG_INF("sensor_channel_get accel.X %d.%d", value[0].val1, value[0].val2);
    LOG_INF("sensor_channel_get accel.Y %d.%d", value[1].val1, value[1].val2);
    LOG_INF("sensor_channel_get accel.Z %d.%d", value[2].val1, value[2].val2);

}

static void roller_event_test_handler(lv_obj_t* obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        //printf("Selected month: ID:%d Text:%s\n", lv_roller_get_selected(obj), buf);
    }
}

static int date_settings_draw(struct view *v, lv_obj_t *parent)
{

}

static int date_settings_init(struct view *v, lv_obj_t *parent)
{
    roller1 = lv_roller_create(lv_scr_act(), NULL);
    lv_roller_set_options(roller1,
        "January\n"
        "February\n"
        "March\n"
        "April\n"
        "May\n"
        "June\n"
        "July\n"
        "August\n"
        "September\n"
        "October\n"
        "November\n"
        "December",
        LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller1, 4);
    lv_obj_align(roller1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(roller1, roller_event_test_handler);

    event_service_subscribe(EventServiceCommandGeneric, accel_raw_data_handler);
}

static int date_settings_exit(struct view *v, lv_obj_t *parent)
{
    return 0;
}

static bool date_settings_event(struct view *view, uint32_t event, void *arg)
{

}

struct view date_settings = {
    .id = DATE_SETTINGS_ID,
    .init = date_settings_init,
    .event = date_settings_event,
    .exit = date_settings_exit,
};