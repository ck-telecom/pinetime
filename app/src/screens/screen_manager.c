#include "screen_manager.h"
#include "../service/view.h"

struct view *screen_ids[SCREEN_ID_MAX] = {
    &home,
    &clock_face_view,
    &date_settings
};

void screen_manager_push(enum screen_id id)
{
//screen_ids[id]->init()
}

void screen_manager_pop(enum screen_id id)
{

}