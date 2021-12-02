#ifndef _VIEW_H
#define _VIEW_H

#include <lvgl.h>
#include <stdbool.h>

struct msg;
/* widget flags */
#define WF_DIRTY    (1 << 0)
#define WF_VISIBLE  (1 << 1)

struct view {
    uint32_t id;

	const char *name;

	const char *label;

	int (*init)(struct view *view, lv_obj_t *parent);

	int (*launch)(struct view *view);

	int (*draw)(struct view *view, lv_obj_t *parent);

	int (*redraw)(struct view *view);

	lv_obj_t *(*container)(struct view *view);

    bool (*event)(struct view *view, uint32_t event, void *arg);

    int (*gui_event)(struct view *view, uint32_t gesure);

	int (*exit)(struct view *view, lv_obj_t *parent);

    uint16_t flags;
};


const char *view_get_label(struct view *v);

int view_init(struct view *v, lv_obj_t *parent);

int view_launch(struct view *v);

int view_draw(struct view *v);

int view_update_draw(struct view *v);

lv_obj_t *view_container(struct view *v);

int view_exit(struct view *v, lv_obj_t *parent);

void view_switch_screen(struct view *current_screen, struct view *new_screen);


#define HOME_ID         0
#define CLOCK_FACE_ID   1

#endif /* _VIEW_H */
