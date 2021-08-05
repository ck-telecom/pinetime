#ifndef _COMPASS_H
#define _COMPASS_H


struct compass_model {
	int16_t angle;
};

struct compass_widget {
//	lv_obj_t *img;
    lv_obj_t *lmeter;
};

struct view_compass {
    struct view view;
    lv_obj_t *parent;
    //struct controllor controllor;
    struct compass_widget widget;
    struct compass_model model;
    struct evt_node handler;
};

#endif /* _COMPASS_H */
