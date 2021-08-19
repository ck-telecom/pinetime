#include <lvgl.h>

#include "view.h"

#if 0
const char *view_get_label(struct view *v)
{
	if (v->label)
		return v->label;

	return NULL;
}



int view_launch(struct view *v)
{
	if (v->launch)
		return v->launch(v);

	return 0;
}

int view_draw(struct view *v)
{
	if (v->draw)
		return v->draw(v, NULL);

	return 0;
}

int view_update_draw(struct view *v)
{
	if (v->update_draw)
		return v->update_draw(v);

	return 0;
}

lv_obj_t *view_container(struct view *v)
{
	if (v->container)
		v->container(v);

	return NULL;
}



int widget_face_gui_event(widget_t *widget, int event)
{
    switch (event) {
    case GUI_EVENT_GESTURE_UP:
        controller_submit_event();
        break;

    case GUI_EVENT_GESTURE_DOWN:
        break;

    case GUI_EVENT_GESTURE_LEFT:
        break;

    case GUI_EVENT_GESTURE_RIGHT:
        break;

    default:
        break;
    }
}
#endif
int view_init(struct view *v, lv_obj_t *parent)
{
	if (v->init)
		return v->init(v, parent);

	return 0;
}

int view_exit(struct view *v, lv_obj_t *parent)
{
	if (v->exit)
		return v->exit(v, parent);

	return 0;
}

void view_switch_screen(struct view *current_screen, struct view *new_screen)
{
    view_exit(current_screen, lv_scr_act());
    current_screen = new_screen;
    view_init(current_screen, lv_scr_act());
}