
int screen_home_draw(lv_obj_t *parent)
{
    lv_style_t style;
    lv_style_init(&style);


	lv_obj_t *cont = lv_cont_create(parent, NULL);
	lv_obj_set_size(cont, 220, 20);
	lv_obj_align(cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

    /**/
	return 0;
}

