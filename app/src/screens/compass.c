#include "gui.h"
#include "view.h"
#include "compass.h"

//LV_IMG_CF_INDEXED_2BIT
//LV_IMG_DECLARE(img_cogwheel_argb);

static lv_obj_t *compass_draw(struct compass_widget *w, void *user_data)
{
	lv_obj_t *obj = lv_obj_create(NULL, NULL);
	lv_obj_set_size(obj, 240, 240);

    w->lmeter = lv_linemeter_create(obj, NULL);
	lv_linemeter_set_range(w->lmeter, 0, 360);                   /*Set the range*/
//	lv_linemeter_set_value(w->lmeter, 40);                       /*Set the current value*/
	lv_linemeter_set_scale(w->lmeter, 360, 60);                  /*Set the angle and number of lines*/
	lv_obj_set_size(w->lmeter, 240, 240);
	lv_obj_align(w->lmeter, NULL, LV_ALIGN_CENTER, 0, 0);
/*
	w->img = lv_img_create(obj, NULL);
	lv_obj_set_size(obj, 240, 240);
	lv_img_set_src(img, &img_cogwheel_argb);
	lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_img_set_angle(img, 600); // 60.0 degree
*/
	return obj;
}

int compass_init(struct view *view)
{
//	struct view_compass *vc = from_view();
    struct view_compass *vc;
//    vc->handler.event = EVT_COMPASS_CHANGED;
//    vc->handler.view = view;
//    evt_add_handler(struct evt *ctx, &vc->handler)
}

int compass_launch(struct view *view)
{

}

int compass_draw(struct view *view, lv_obj_t *parent)
{
	struct view_compass *vc;//TODO:
	vc->parent = compass_draw(&vc->widget, NULL);

	return 0;
}

int compass_redraw(struct view *view)
{
	struct view_compass *vc;
	/* arrow update */
	lv_img_set_angle(vc->widget.img, vc->model.angle);
}

lv_obj_t *compass_parent(struct view *view)
{
	struct view_compass *vc;

	return vc->parent ? vc->parent : NULL;
}

int compass_event(struct view *view, uint32_t event)
{
    if (event == EVT_COMPASS_CHANGED) {
        //TODO: 
        view->flags |= WF_DIRTY;
    }
}

int compass_gui_event(struct view *view, uint32_t event)
{
    if (event == EVT_GUI_CHANGED) {
        //TODO: 

    }
}

int compass_close(struct view *view)
{
	struct view_compass *vc;
	lv_obj_del(vc->widget.screen);

	return 0;
}

static const struct view view_compass = {
    .name = "time",
    .init = compass_init,
    .launch = compass_launch,
    .draw = compass_draw,
    .container = compass_parent,
    .redraw = compass_redraw,
    .event = compass_event,
    .gui_event = compass_gui_event,
    .close = compass_close,
};
