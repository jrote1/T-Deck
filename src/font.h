static const void *get_imgfont_path(const lv_font_t *font, uint32_t unicode, uint32_t unicode_next,
                                    int32_t *offset_y, void *user_data)
{
    LV_UNUSED(font);
    LV_UNUSED(unicode_next);
    LV_UNUSED(offset_y);
    LV_UNUSED(user_data);

    switch (unicode)
    {
    default:
        return NULL;
    }
}