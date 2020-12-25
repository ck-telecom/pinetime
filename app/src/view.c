#include "view.h"

const char *view_get_label(struct view *v)
{
	if (v->label)
		return v->label;

	return NULL;
}

int view_init(struct view *v)
{
	if (v->init)
		return v->init(v);

	return 0;
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

int view_close(struct view *v)
{
	if (v->close)
		return v->close(v);

	return 0;
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
