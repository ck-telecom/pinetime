#ifndef _VIEW_H
#define _VIEW_H

#include <lvgl/lvgl.h>

/* widget flags */
#define WF_DIRTY    (1 << 0)
#define WF_VISIBLE  (1 << 1)

struct view {
	const char *name;

	const char *label;

	int (*init)(struct view *view);

	int (*launch)(struct view *view);

	int (*draw)(struct view *view, lv_obj_t *parent);

	int (*redraw)(struct view *view);

	lv_obj_t *(*container)(struct view *view);

    int (*event)(struct view *view, uint32_t event);

    int (*gui_event)(struct view *view, uint32_t gesure);

	int (*close)(struct view *view);

    uint16_t flags;
};


const char *view_get_label(struct view *v);

int view_init(struct view *v);

int view_launch(struct view *v);

int view_draw(struct view *v);

int view_update_draw(struct view *v);

lv_obj_t *view_container(struct view *v);

int view_close(struct view *v);


#endif /* _VIEW_H */
