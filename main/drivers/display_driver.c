/**
 * @file display_driver.c
 * @brief Driver d'affichage ST7262 via interface RGB
 */

#include "display_driver.h"
#include "st7262_rgb.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "driver/ledc.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"

static const char *TAG = "Display_Driver";

#define PIN_BCKL 2

static esp_lcd_panel_handle_t panel_handle;
static lv_display_t *display;
static lv_color_t *buf1 = NULL;
static lv_color_t *buf2 = NULL;

static void display_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;
    int64_t start = esp_timer_get_time();
    ESP_LOGD(TAG, "flush start (%d,%d)->(%d,%d) size %dx%d",
             area->x1, area->y1, area->x2, area->y2, w, h);
    if (esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1,
                                  area->x2 + 1, area->y2 + 1, px_map) == ESP_OK) {
        lv_display_flush_ready(disp);
    } else {
        ESP_LOGE(TAG, "esp_lcd_panel_draw_bitmap failed");
    }
    int64_t end = esp_timer_get_time();
    ESP_LOGD(TAG, "flush end (%d,%d)->(%d,%d) size %dx%d, %lld us",
             area->x1, area->y1, area->x2, area->y2, w, h,
             (long long)(end - start));
}

static esp_err_t init_backlight(void)
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 5000,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    esp_err_t ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK) {
        return ret;
    }
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_0,
        .duty = 200,
        .gpio_num = PIN_BCKL,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0,
    };
    ret = ledc_channel_config(&ledc_channel);
    if (ret != ESP_OK) {
        return ret;
    }
    ESP_LOGI(TAG, "Backlight initialized");
    return ESP_OK;
}

esp_err_t display_driver_init(void)
{
    esp_err_t ret = st7262_rgb_new_panel(&panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init panel: %d", ret);
        return ret;
    }
    ret = init_backlight();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Backlight init failed: %d", ret);
        return ret;
    }
    buf1 = heap_caps_malloc(DISPLAY_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    buf2 = heap_caps_malloc(DISPLAY_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (!buf1 || !buf2) {
        if (buf1) { heap_caps_free(buf1); buf1 = NULL; }
        if (buf2) { heap_caps_free(buf2); buf2 = NULL; }
        ESP_LOGE(TAG, "Buffer alloc failed");
        return ESP_ERR_NO_MEM;
    }
    display = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    lv_display_set_default(display);
    lv_display_set_flush_cb(display, display_flush_cb);
    lv_display_set_buffers(display, buf1, buf2,
                           DISPLAY_BUF_SIZE * sizeof(lv_color_t),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    ESP_LOGI(TAG, "Display driver initialized");
    return ESP_OK;
}

void display_driver_deinit(void)
{
    if (display) {
        lv_display_delete(display);
        display = NULL;
    }
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_timer_rst(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0);
    if (buf1) {
        heap_caps_free(buf1);
        buf1 = NULL;
    }
    if (buf2) {
        heap_caps_free(buf2);
        buf2 = NULL;
    }
    if (panel_handle) {
        esp_lcd_panel_disp_on_off(panel_handle, false);
        esp_lcd_panel_del(panel_handle);
        panel_handle = NULL;
    }
    ESP_LOGI(TAG, "Display driver deinit");
}

void display_set_brightness(uint8_t brightness)
{
    if (brightness > 100) brightness = 100;
    uint32_t duty = (brightness * 255) / 100;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void display_set_sleep(bool sleep)
{
    esp_lcd_panel_disp_sleep(panel_handle, sleep);
    if (sleep) {
        display_set_brightness(0);
    } else {
        display_set_brightness(80);
    }
}

