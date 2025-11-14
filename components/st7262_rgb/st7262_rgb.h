#pragma once

#include "st7701_rgb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compatibilité : renvoie le driver ST7701 calibré pour le
 *        panneau Waveshare 7B (1024x600).
 */
esp_err_t st7262_rgb_new_panel(esp_lcd_panel_handle_t *panel_handle);

#ifdef __cplusplus
}
#endif
