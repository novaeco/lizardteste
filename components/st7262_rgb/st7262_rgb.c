#include "st7262_rgb.h"

esp_err_t st7262_rgb_new_panel(esp_lcd_panel_handle_t *panel_handle)
{
    return st7701_rgb_new_panel(panel_handle);
}
