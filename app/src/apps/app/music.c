#include "music.h"

static const app_spec_t music_spec;

app_music_t app_music = {
	.app = { .spec = &music_spec }
};

static inline app_music_t *_from_app(app_t *app)
{
	return CONTAINER_OF(app, app_music_t, app);
}

static int music_init(app_t *app, lv_obj_t *parent)
{
	app_music_t *ht = _from_app(app);	
}

static int music_event(app_t *app, uint32_t event, unsigned long data)
{
	app_music_t *ht = _from_app(app);	
}

static int music_exit(app_t *app)
{
	app_music_t *ht = _from_app(app);

	lv_obj_clean(ht->screen);
	lv_obj_del(ht->screen);
	ht->screen = NULL;	
}

static const app_spec_t music_spec = {
	.name = "music",
	.init = music_init,
	.event = music_event,
	.exit = music_exit,
};
