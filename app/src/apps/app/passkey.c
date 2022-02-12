#include <zephyr.h>

#include "passkey.h"

static const app_spec_t passkey_spec;

passkey_app_t passkey_app = {
    .app = { .spec = &passkey_spec }
};

static inline passkey_app_t *_from_app(app_t *app)
{
    return CONTAINER_OF(app, passkey_app_t, app);
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        //delete_bonds();
        load_application(Clock/*, AnimNone*/);
    }
}

static lv_obj_t *screen_create(passkey_app_t *ht, lv_obj_t * parent)
{
    lv_obj_t *scr = lv_obj_create(parent);

    lv_obj_clear_flag(scr, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_remove_style_all(scr);                            /*Make it transparent*/
    lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
    lv_obj_set_scroll_snap_y(scr, LV_SCROLL_SNAP_CENTER);    /*Snap the children to the center*/

    lv_obj_t * lv_title = lv_label_create(scr);
    lv_label_set_text_static(lv_title, "Pairing Key\nEnter the code\nin your device.");
    lv_obj_set_style_text_align(lv_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lv_title, lv_color_hex(0x00ffff), 0);
    lv_obj_align(lv_title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * lv_key = lv_label_create(scr);
    lv_label_set_text_fmt(lv_key, "%s", pinetimecos.passkey);
    lv_obj_set_style_text_font(lv_key, &lv_font_clock_42, 0);
    lv_obj_set_style_text_align(lv_key, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lv_key, lv_color_hex(0x00ff00), 0);
    lv_obj_align(lv_key, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(scr);
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0xff0000), 0);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Del Bounds");
    lv_obj_center(label);

    return scr;
}

static int init(app_t *app, lv_obj_t *parent)
{
    passkey_app_t *htapp = _from_app(app);
    htapp->screen = screen_create(htapp, parent);

    return 0;
}

static int passkey_exit(app_t *app)
{
    passkey_app_t *ht = _from_app(app);
    lv_obj_clean(ht->screen);
    lv_obj_del(ht->screen);
    ht->screen = NULL;

    return 0;
}

static const app_spec_t passkey_spec = {
    .name = "passkey",
    .init = passkey_init,
    .exit = passkey_exit,
};
