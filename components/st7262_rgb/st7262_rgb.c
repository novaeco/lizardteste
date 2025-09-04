#include "st7262_rgb.h"
#include "esp_lcd_panel_io.h"
#include "esp_log.h"

#define TAG "st7262_rgb"

#define LCD_H_RES 800
#define LCD_V_RES 480
#define LCD_PIXEL_CLOCK_HZ (10 * 1000 * 1000)

#define LCD_HSYNC_GPIO 39
#define LCD_VSYNC_GPIO 41
#define LCD_DE_GPIO    40
#define LCD_PCLK_GPIO  42

static const int lcd_data_gpios[16] = {
    45, 48, 47, 21, 14, 13, 12, 11,
    10, 9, 46, 3, 8, 18, 17, 16
};

esp_err_t st7262_rgb_new_panel(esp_lcd_panel_handle_t *ret_panel)
{
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel = NULL;

    esp_lcd_panel_io_rgb_config_t io_config = {
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .de_gpio_num = LCD_DE_GPIO,
        .pclk_gpio_num = LCD_PCLK_GPIO,
        .vsync_gpio_num = LCD_VSYNC_GPIO,
        .hsync_gpio_num = LCD_HSYNC_GPIO,
        .data_gpio_nums = {
            [0] = lcd_data_gpios[0],
            [1] = lcd_data_gpios[1],
            [2] = lcd_data_gpios[2],
            [3] = lcd_data_gpios[3],
            [4] = lcd_data_gpios[4],
            [5] = lcd_data_gpios[5],
            [6] = lcd_data_gpios[6],
            [7] = lcd_data_gpios[7],
            [8] = lcd_data_gpios[8],
            [9] = lcd_data_gpios[9],
            [10] = lcd_data_gpios[10],
            [11] = lcd_data_gpios[11],
            [12] = lcd_data_gpios[12],
            [13] = lcd_data_gpios[13],
            [14] = lcd_data_gpios[14],
            [15] = lcd_data_gpios[15],
        },
        .data_width = 16,
        .timings = {
            .pclk_hz = LCD_PIXEL_CLOCK_HZ,
            .h_res = LCD_H_RES,
            .v_res = LCD_V_RES,
            .hsync_pulse_width = 10,
            .hsync_back_porch = 40,
            .hsync_front_porch = 40,
            .vsync_pulse_width = 10,
            .vsync_back_porch = 20,
            .vsync_front_porch = 10,
            .flags = {
                .hsync_idle_low = true,
                .vsync_idle_low = true,
                .de_idle_high = false,
                .pclk_active_neg = true,
            },
        },
    };

    esp_err_t ret = esp_lcd_new_panel_io_rgb(&io_config, &io_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "io init failed: %d", ret);
        return ret;
    }

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };

    ret = esp_lcd_new_panel_st7262(io_handle, &panel_config, &panel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "panel create failed: %d", ret);
        return ret;
    }

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));

    *ret_panel = panel;
    return ESP_OK;
}

