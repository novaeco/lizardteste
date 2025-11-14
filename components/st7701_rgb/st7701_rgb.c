#include "st7701_rgb.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "st7701_rgb"

#define LCD_H_RES 1024
#define LCD_V_RES 600
#define LCD_PIXEL_CLOCK_HZ (30 * 1000 * 1000)

#define LCD_HSYNC_GPIO 46
#define LCD_VSYNC_GPIO 3
#define LCD_DE_GPIO 5
#define LCD_PCLK_GPIO 7

/*
 * Data line mapping for 16-bit RGB565 (Waveshare ESP32-S3 Touch LCD 7B):
 * D0→B3, D1→B4, D2→B5, D3→B6, D4→B7,
 * D5→G2, D6→G3, D7→G4, D8→G5, D9→G6, D10→G7,
 * D11→R3, D12→R4, D13→R5, D14→R6, D15→R7
 */
static const int lcd_data_gpios[16] = {
    14, 38, 18, 17, 10, 39, 0, 45,
    48, 47, 21, 1, 2, 42, 41, 40};

#define LCD_CMD_SPI_HOST SPI3_HOST
#define LCD_CMD_SCLK_GPIO 6
#define LCD_CMD_MOSI_GPIO 11
#define LCD_CMD_CS_GPIO 12
#define LCD_CMD_DC_GPIO 4

typedef struct {
    uint8_t cmd;
    uint8_t data[32];
    uint8_t data_bytes;
    uint16_t delay_ms;
} st7701_init_cmd_t;

static const st7701_init_cmd_t st7701_init_cmds[] = {
    {0xFF, {0x77, 0x01, 0x00, 0x00, 0x10}, 5, 0},
    {0xC0, {0x3B, 0x00}, 2, 0},
    {0xC1, {0x0D, 0x02}, 2, 0},
    {0xC2, {0x31, 0x04}, 2, 0},
    {0xCD, {0x00}, 1, 0},
    {0xB0, {0x00, 0x10, 0x16, 0x0C, 0x12, 0x05, 0x02, 0x07, 0x08, 0x21, 0x06, 0x11, 0x11, 0xA6, 0x31, 0x18}, 16, 0},
    {0xB1, {0x00, 0x0F, 0x15, 0x0B, 0x11, 0x05, 0x02, 0x07, 0x08, 0x20, 0x04, 0x12, 0x11, 0xA4, 0x31, 0x18}, 16, 0},
    {0xFF, {0x77, 0x01, 0x00, 0x00, 0x11}, 5, 0},
    {0xB0, {0x6D}, 1, 0},
    {0xB1, {0x37}, 1, 0},
    {0xB2, {0x81}, 1, 0},
    {0xB3, {0x80}, 1, 0},
    {0xB5, {0x43}, 1, 0},
    {0xB7, {0x85}, 1, 0},
    {0xB8, {0x20}, 1, 0},
    {0xB9, {0x10}, 1, 0},
    {0xBA, {0x24}, 1, 0},
    {0xC1, {0x78}, 1, 0},
    {0xC2, {0x78}, 1, 0},
    {0xC3, {0x8C}, 1, 0},
    {0xD0, {0x88}, 1, 0},
    {0xE0, {0x00, 0x00, 0x02}, 3, 0},
    {0xE1, {0x04, 0xA0, 0x00, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x20, 0x20}, 11, 0},
    {0xE2, {0x30, 0x30, 0x04, 0x30, 0x30, 0x04}, 6, 0},
    {0xE3, {0x00, 0x00, 0x33, 0x33}, 4, 0},
    {0xE4, {0x44, 0x44}, 2, 0},
    {0xE5, {0x0C, 0x30, 0xA0, 0xA0, 0x0F, 0x30, 0xA0, 0xA0, 0x08, 0x30, 0xA0, 0xA0, 0x0A, 0x30, 0xA0, 0xA0}, 16, 0},
    {0xE6, {0x00, 0x00, 0x33, 0x33}, 4, 0},
    {0xE7, {0x44, 0x44}, 2, 0},
    {0xE8, {0x0D, 0x30, 0xA0, 0xA0, 0x0C, 0x30, 0xA0, 0xA0, 0x0F, 0x30, 0xA0, 0xA0, 0x08, 0x30, 0xA0, 0xA0}, 16, 0},
    {0xEB, {0x00, 0x01, 0xE4, 0xE4, 0x44, 0x88, 0x40}, 7, 0},
    {0xED, {0xFF, 0x02, 0x01, 0x00, 0x00, 0x10, 0x00}, 7, 0},
    {0xEF, {0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F}, 6, 0},
    {0xFF, {0x77, 0x01, 0x00, 0x00, 0x13}, 5, 0},
    {0xEF, {0x08}, 1, 0},
    {0xFF, {0x77, 0x01, 0x00, 0x00, 0x00}, 5, 0},
    {0x3A, {0x55}, 1, 0},
    {0x36, {0x00}, 1, 0},
    {0x11, {0x00}, 0, 120},
    {0x29, {0x00}, 0, 20},
    {0x00, {0x00}, 0, 0},
};

static esp_err_t st7701_send_cmds(spi_device_handle_t dev)
{
    esp_err_t ret = ESP_OK;
    for (int i = 0; st7701_init_cmds[i].cmd != 0; ++i) {
        spi_transaction_t t = {
            .flags = 0,
            .length = 8,
            .tx_buffer = &st7701_init_cmds[i].cmd,
        };
        gpio_set_level(LCD_CMD_DC_GPIO, 0);
        ret = spi_device_polling_transmit(dev, &t);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "command 0x%02X failed (%d)", st7701_init_cmds[i].cmd, ret);
            return ret;
        }
        if (st7701_init_cmds[i].data_bytes) {
            t.length = st7701_init_cmds[i].data_bytes * 8;
            t.tx_buffer = st7701_init_cmds[i].data;
            gpio_set_level(LCD_CMD_DC_GPIO, 1);
            ret = spi_device_polling_transmit(dev, &t);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "data for cmd 0x%02X failed (%d)", st7701_init_cmds[i].cmd, ret);
                return ret;
            }
        }
        if (st7701_init_cmds[i].delay_ms) {
            vTaskDelay(pdMS_TO_TICKS(st7701_init_cmds[i].delay_ms));
        }
    }
    return ESP_OK;
}

esp_err_t st7701_rgb_new_panel(esp_lcd_panel_handle_t *ret_panel)
{
    if (!ret_panel) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .sclk_io_num = LCD_CMD_SCLK_GPIO,
        .mosi_io_num = LCD_CMD_MOSI_GPIO,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ret = spi_bus_initialize(LCD_CMD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_initialize failed (%d)", ret);
        return ret;
    }

    gpio_config_t dc_conf = {
        .pin_bit_mask = 1ULL << LCD_CMD_DC_GPIO,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&dc_conf);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = LCD_CMD_CS_GPIO,
        .queue_size = 8,
    };
    spi_device_handle_t cmd_dev;
    ret = spi_bus_add_device(LCD_CMD_SPI_HOST, &devcfg, &cmd_dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "spi_bus_add_device failed (%d)", ret);
        spi_bus_free(LCD_CMD_SPI_HOST);
        return ret;
    }

    esp_lcd_rgb_panel_config_t rgb_config = {
        .clk_src = LCD_CLK_SRC_PLL160M,
        .timings =
            {
                .pclk_hz = LCD_PIXEL_CLOCK_HZ,
                .h_res = LCD_H_RES,
                .v_res = LCD_V_RES,
                .hsync_pulse_width = 20,
                .hsync_back_porch = 140,
                .hsync_front_porch = 160,
                .vsync_pulse_width = 3,
                .vsync_back_porch = 20,
                .vsync_front_porch = 12,
                .flags =
                    {
                        .hsync_idle_low = true,
                        .vsync_idle_low = true,
                        .de_idle_high = false,
                        .pclk_active_neg = true,
                    },
            },
        .data_width = 16,
        .bits_per_pixel = 16,
        .num_fbs = 1,
        .hsync_gpio_num = LCD_HSYNC_GPIO,
        .vsync_gpio_num = LCD_VSYNC_GPIO,
        .de_gpio_num = LCD_DE_GPIO,
        .pclk_gpio_num = LCD_PCLK_GPIO,
        .disp_gpio_num = -1,
        .data_gpio_nums =
            {
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
#if CONFIG_SPIRAM
        .flags =
            {
                .fb_in_psram = true,
            },
#endif
    };

    esp_lcd_panel_handle_t panel = NULL;
    ret = esp_lcd_new_rgb_panel(&rgb_config, &panel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_lcd_new_rgb_panel failed (%d)", ret);
        spi_bus_remove_device(cmd_dev);
        spi_bus_free(LCD_CMD_SPI_HOST);
        return ret;
    }

    ret = st7701_send_cmds(cmd_dev);
    if (ret != ESP_OK) {
        spi_bus_remove_device(cmd_dev);
        spi_bus_free(LCD_CMD_SPI_HOST);
        esp_lcd_panel_del(panel);
        return ret;
    }

    spi_bus_remove_device(cmd_dev);
    spi_bus_free(LCD_CMD_SPI_HOST);

    ret = esp_lcd_panel_reset(panel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "panel reset failed (%d)", ret);
        esp_lcd_panel_del(panel);
        return ret;
    }

    ret = esp_lcd_panel_init(panel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "panel init failed (%d)", ret);
        esp_lcd_panel_del(panel);
        return ret;
    }

    ret = esp_lcd_panel_disp_on_off(panel, true);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "panel on failed (%d)", ret);
        esp_lcd_panel_del(panel);
        return ret;
    }

    *ret_panel = panel;
    ESP_LOGI(TAG, "ST7701 RGB panel configured (%dx%d @ %.2f MHz)", LCD_H_RES, LCD_V_RES, LCD_PIXEL_CLOCK_HZ / 1e6);
    return ESP_OK;
}
