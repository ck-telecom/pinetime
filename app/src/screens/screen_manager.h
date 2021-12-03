#ifndef _SCREEN_MANAGER_H
#define _SCREEN_MANAGER_H

enum screen_id {
    SCREEN_ID_HOME,
    SCREEN_ID_MENU,
    SCREEN_ID_CLOCKFACE,
    SCREEN_ID_COMPASS,
    SCREEN_ID_MAX
};
extern struct view home;
extern struct view clock_face_view;
extern struct view date_settings;

void screen_manager_push(enum screen_id id);

void screen_manager_pop(enum screen_id id);

void screen_manager_load(enum screen_id id);

#endif /* _SCREEN_MANAGER_H */