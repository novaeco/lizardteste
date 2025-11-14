#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mock_panel {
    bool dummy;
} mock_panel_t;

typedef mock_panel_t* esp_lcd_panel_handle_t;

esp_err_t esp_lcd_panel_draw_bitmap(esp_lcd_panel_handle_t handle,
                                    int x_start, int y_start,
                                    int x_end, int y_end,
                                    const void *color_data);
esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t handle, bool on);
esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t handle);
esp_err_t esp_lcd_panel_disp_sleep(esp_lcd_panel_handle_t handle, bool sleep);

#ifdef __cplusplus
}
#endif

