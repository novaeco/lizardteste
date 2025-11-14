#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create and initialize ST7701 RGB panel for Waveshare ESP32-S3 Touch LCD 7B (1024x600)
 *
 * @param[out] ret_panel Returned panel handle
 * @return esp_err_t
 */
esp_err_t st7701_rgb_new_panel(esp_lcd_panel_handle_t *ret_panel);

#ifdef __cplusplus
}
#endif

