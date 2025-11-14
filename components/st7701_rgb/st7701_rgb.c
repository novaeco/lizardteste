#include "st7701_rgb.h"

#include <stdlib.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_lcd_panel_commands.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LCD_CMD_SPI_HOST   SPI3_HOST
#define LCD_CMD_MOSI_GPIO  11
#define LCD_CMD_SCLK_GPIO   6
#define LCD_CMD_CS_GPIO    12
#define LCD_CMD_DC_GPIO     4

#define LCD_CMD_BITS        8
#define LCD_PARAM_BITS      8
#define LCD_CMD_SPI_CLOCK_HZ (10 * 1000 * 1000)

#define LCD_PIXEL_CLOCK_HZ  (30 * 1000 * 1000)

#define LCD_DE_GPIO         5
#define LCD_PCLK_GPIO       7
#define LCD_VSYNC_GPIO      3
#define LCD_HSYNC_GPIO     46

#define LCD_DATA_WIDTH     16
#define LCD_BITS_PER_PIXEL 16

#define LCD_H_RES         1024
#define LCD_V_RES          600

static const char *TAG = "st7701_rgb";

typedef struct {
    uint8_t cmd;
    const uint8_t *data;
    uint8_t data_bytes;
    uint16_t delay_ms;
} st7701_init_cmd_t;

typedef struct {
    void *base_user_data;
    esp_lcd_panel_io_handle_t io;
    spi_host_device_t host_id;
    bool free_bus_on_del;
    esp_err_t (*orig_del)(esp_lcd_panel_t *panel);
    esp_err_t (*orig_disp_on_off)(esp_lcd_panel_t *panel, bool on_off);
    esp_err_t (*orig_disp_sleep)(esp_lcd_panel_t *panel, bool enter_sleep);
} st7701_rgb_panel_ctx_t;

static const int lcd_data_gpios[LCD_DATA_WIDTH] = {
    14, // B3 (LSB)
    38, // B4
    18, // B5
    17, // B6
    10, // B7 (MSB for blue)
    39, // G2 (LSB for green)
    0,  // G3
    45, // G4
    48, // G5
    47, // G6
    21, // G7 (MSB for green)
    1,  // R3 (LSB for red)
    2,  // R4
    42, // R5
    41, // R6
    40, // R7 (MSB for red)
};

static const esp_lcd_rgb_timing_t st7701_rgb_timing = {
    .pclk_hz = LCD_PIXEL_CLOCK_HZ,
    .h_res = LCD_H_RES,
    .v_res = LCD_V_RES,
    .hsync_pulse_width = 10,
    .hsync_back_porch = 160,
    .hsync_front_porch = 160,
    .vsync_pulse_width = 1,
    .vsync_back_porch = 23,
    .vsync_front_porch = 12,
    .flags = {
        .hsync_idle_low = true,
        .vsync_idle_low = true,
        .de_idle_high = true,
        .pclk_active_neg = true,
        .pclk_idle_high = false,
    },
};

static const st7701_init_cmd_t st7701_init_cmds[] = {
    {LCD_CMD_SWRESET, NULL, 0, 120},
    {0xFF, (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x13}, 5, 0},
    {0xEF, (uint8_t[]){0x08}, 1, 0},
    {0xFF, (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x10}, 5, 0},
    {0xC0, (uint8_t[]){0x3B, 0x00}, 2, 0},
    {0xC1, (uint8_t[]){0x10, 0x02}, 2, 0},
    {0xC2, (uint8_t[]){0x20, 0x06}, 2, 0},
    {0xCC, (uint8_t[]){0x10}, 1, 0},
    {0xB0, (uint8_t[]){0x00, 0x13, 0x5A, 0x0F, 0x12, 0x07, 0x09, 0x08, 0x08, 0x24, 0x07, 0x13, 0x12, 0x6B, 0x73, 0xFF}, 16, 0},
    {0xB1, (uint8_t[]){0x00, 0x13, 0x5A, 0x0F, 0x12, 0x07, 0x09, 0x08, 0x08, 0x24, 0x07, 0x13, 0x12, 0x6B, 0x73, 0xFF}, 16, 0},
    {0xFF, (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x11}, 5, 0},
    {0xB0, (uint8_t[]){0x8D}, 1, 0},
    {0xB1, (uint8_t[]){0x48}, 1, 0},
    {0xB2, (uint8_t[]){0x89}, 1, 0},
    {0xB3, (uint8_t[]){0x80}, 1, 0},
    {0xB5, (uint8_t[]){0x49}, 1, 0},
    {0xB7, (uint8_t[]){0x85}, 1, 0},
    {0xB8, (uint8_t[]){0x32}, 1, 0},
    {0xC1, (uint8_t[]){0x78}, 1, 0},
    {0xC2, (uint8_t[]){0x78}, 1, 0},
    {0xD0, (uint8_t[]){0x88}, 1, 100},
    {0xE0, (uint8_t[]){0x00, 0x00, 0x02}, 3, 0},
    {0xE1, (uint8_t[]){0x05, 0xC0, 0x07, 0xC0, 0x04, 0xC0, 0x06, 0xC0, 0x00, 0x44, 0x44}, 11, 0},
    {0xE2, (uint8_t[]){0x00, 0x00, 0x33, 0x33, 0x01, 0xC0, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x00}, 13, 0},
    {0xE3, (uint8_t[]){0x00, 0x00, 0x11, 0x11}, 4, 0},
    {0xE4, (uint8_t[]){0x44, 0x44}, 2, 0},
    {0xE5, (uint8_t[]){0x0D, 0xF1, 0x10, 0x98, 0x0F, 0xF3, 0x10, 0x98, 0x09, 0xED, 0x10, 0x98, 0x0B, 0xEF, 0x10, 0x98}, 16, 0},
    {0xE6, (uint8_t[]){0x00, 0x00, 0x11, 0x11}, 4, 0},
    {0xE7, (uint8_t[]){0x44, 0x44}, 2, 0},
    {0xE8, (uint8_t[]){0x0C, 0xF0, 0x10, 0x98, 0x0E, 0xF2, 0x10, 0x98, 0x08, 0xEC, 0x10, 0x98, 0x0A, 0xEE, 0x10, 0x98}, 16, 0},
    {0xEB, (uint8_t[]){0x00, 0x01, 0xE4, 0xE4, 0x44, 0x88, 0x00}, 7, 0},
    {0xED, (uint8_t[]){0xFF, 0x04, 0x56, 0x7F, 0xBA, 0x2F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF2, 0xAB, 0xF7, 0x65, 0x40, 0xFF}, 16, 0},
    {0xEF, (uint8_t[]){0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F}, 6, 0},
    {0x36, (uint8_t[]){0x00}, 1, 0},
    {0x3A, (uint8_t[]){0x55}, 1, 0},
    {0xFF, (uint8_t[]){0x77, 0x01, 0x00, 0x00, 0x00}, 5, 0},
    {LCD_CMD_SLPOUT, NULL, 0, 120},
    {LCD_CMD_DISPON, NULL, 0, 20},
};

static inline st7701_rgb_panel_ctx_t *get_ctx(esp_lcd_panel_t *panel)
{
    return (st7701_rgb_panel_ctx_t *)panel->user_data;
}

static void restore_user_data(esp_lcd_panel_t *panel, st7701_rgb_panel_ctx_t *ctx)
{
    panel->user_data = ctx->base_user_data;
}

static void adopt_user_data(esp_lcd_panel_t *panel, st7701_rgb_panel_ctx_t *ctx)
{
    panel->user_data = ctx;
}

static esp_err_t st7701_panel_send_init_sequence(esp_lcd_panel_io_handle_t io)
{
    ESP_RETURN_ON_FALSE(io, ESP_ERR_INVALID_ARG, TAG, "invalid io handle");

    for (size_t i = 0; i < sizeof(st7701_init_cmds) / sizeof(st7701_init_cmd_t); ++i) {
        const st7701_init_cmd_t *cmd = &st7701_init_cmds[i];
        const uint8_t *data = cmd->data;
        esp_err_t ret = esp_lcd_panel_io_tx_param(io, cmd->cmd, data, cmd->data_bytes);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to send cmd 0x%02X: %s", cmd->cmd, esp_err_to_name(ret));
            return ret;
        }
        if (cmd->delay_ms) {
            vTaskDelay(pdMS_TO_TICKS(cmd->delay_ms));
        }
    }
    return ESP_OK;
}

static esp_err_t st7701_panel_check_id(esp_lcd_panel_io_handle_t io)
{
    uint8_t id[3] = {0};
    esp_err_t ret = esp_lcd_panel_io_rx_param(io, 0x04, id, sizeof(id));
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Unable to read ST7701 ID: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "ST7701 ID: %02X %02X %02X", id[0], id[1], id[2]);
    if (id[0] == 0 && id[1] == 0 && id[2] == 0) {
        ESP_LOGW(TAG, "Unexpected ST7701 ID (all zeros)");
        return ESP_ERR_INVALID_RESPONSE;
    }
    ESP_LOGI(TAG, "ST7701 panel self-test passed");
    return ESP_OK;
}

static esp_err_t st7701_panel_disp_on_off(esp_lcd_panel_t *panel, bool on)
{
    st7701_rgb_panel_ctx_t *ctx = get_ctx(panel);
    esp_err_t ret = ESP_OK;

    if (ctx->io) {
        uint8_t cmd = on ? LCD_CMD_DISPON : LCD_CMD_DISPOFF;
        ret = esp_lcd_panel_io_tx_param(ctx->io, cmd, NULL, 0);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to switch display %s: %s", on ? "ON" : "OFF", esp_err_to_name(ret));
            return ret;
        }
        if (!on) {
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }

    if (ctx->orig_disp_on_off) {
        restore_user_data(panel, ctx);
        esp_err_t base_ret = ctx->orig_disp_on_off(panel, on);
        adopt_user_data(panel, ctx);
        if (base_ret != ESP_OK) {
            ret = base_ret;
        }
    }

    return ret;
}

static esp_err_t st7701_panel_disp_sleep(esp_lcd_panel_t *panel, bool enter_sleep)
{
    st7701_rgb_panel_ctx_t *ctx = get_ctx(panel);
    esp_err_t ret = ESP_OK;

    if (ctx->io) {
        uint8_t cmd = enter_sleep ? LCD_CMD_SLPIN : LCD_CMD_SLPOUT;
        ret = esp_lcd_panel_io_tx_param(ctx->io, cmd, NULL, 0);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to %s sleep: %s", enter_sleep ? "enter" : "exit", esp_err_to_name(ret));
            return ret;
        }
        vTaskDelay(pdMS_TO_TICKS(enter_sleep ? 20 : 120));
    }

    if (ctx->orig_disp_sleep) {
        restore_user_data(panel, ctx);
        esp_err_t base_ret = ctx->orig_disp_sleep(panel, enter_sleep);
        adopt_user_data(panel, ctx);
        if (base_ret != ESP_OK) {
            ret = base_ret;
        }
    }

    return ret;
}

static esp_err_t st7701_panel_del(esp_lcd_panel_t *panel)
{
    st7701_rgb_panel_ctx_t *ctx = get_ctx(panel);
    esp_err_t ret = ESP_OK;

    if (ctx->io) {
        esp_lcd_panel_io_del(ctx->io);
        ctx->io = NULL;
    }

    if (ctx->free_bus_on_del) {
        spi_bus_free(ctx->host_id);
        ctx->free_bus_on_del = false;
    }

    if (ctx->orig_del) {
        restore_user_data(panel, ctx);
        ret = ctx->orig_del(panel);
        // No need to re-adopt user data, panel is invalid afterwards
    }

    free(ctx);
    return ret;
}

esp_err_t st7701_rgb_new_panel(esp_lcd_panel_handle_t *ret_panel)
{
    ESP_RETURN_ON_FALSE(ret_panel, ESP_ERR_INVALID_ARG, TAG, "ret_panel is null");

    esp_err_t ret = ESP_OK;
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    st7701_rgb_panel_ctx_t *ctx = NULL;
    bool bus_acquired = false;

    spi_bus_config_t bus_config = {
        .sclk_io_num = LCD_CMD_SCLK_GPIO,
        .mosi_io_num = LCD_CMD_MOSI_GPIO,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
    };

    ret = spi_bus_initialize(LCD_CMD_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO);
    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "SPI bus already initialized, reusing it");
        ret = ESP_OK;
    } else if (ret == ESP_OK) {
        bus_acquired = true;
    }
    ESP_RETURN_ON_ERROR(ret, TAG, "spi_bus_initialize failed");

    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = LCD_CMD_DC_GPIO,
        .cs_gpio_num = LCD_CMD_CS_GPIO,
        .pclk_hz = LCD_CMD_SPI_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 4,
    };

    ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_CMD_SPI_HOST, &io_config, &io_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create panel IO: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ret = st7701_panel_send_init_sequence(io_handle);
    if (ret != ESP_OK) {
        goto cleanup;
    }

    ret = st7701_panel_check_id(io_handle);
    if (ret != ESP_OK) {
        goto cleanup;
    }

    esp_lcd_rgb_panel_config_t rgb_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .timings = st7701_rgb_timing,
        .data_width = LCD_DATA_WIDTH,
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .out_color_format = LCD_COLOR_FMT_RGB565,
        .num_fbs = 1,
        .bounce_buffer_size_px = 0,
        .dma_burst_size = 64,
        .hsync_gpio_num = LCD_HSYNC_GPIO,
        .vsync_gpio_num = LCD_VSYNC_GPIO,
        .de_gpio_num = LCD_DE_GPIO,
        .pclk_gpio_num = LCD_PCLK_GPIO,
        .disp_gpio_num = -1,
        .data_gpio_nums = {
            lcd_data_gpios[0], lcd_data_gpios[1], lcd_data_gpios[2], lcd_data_gpios[3],
            lcd_data_gpios[4], lcd_data_gpios[5], lcd_data_gpios[6], lcd_data_gpios[7],
            lcd_data_gpios[8], lcd_data_gpios[9], lcd_data_gpios[10], lcd_data_gpios[11],
            lcd_data_gpios[12], lcd_data_gpios[13], lcd_data_gpios[14], lcd_data_gpios[15],
        },
        .flags = {
            .fb_in_psram = 1,
        },
    };

    ret = esp_lcd_new_rgb_panel(&rgb_config, &panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create RGB panel: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ret = esp_lcd_panel_reset(panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_lcd_panel_reset failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ret = esp_lcd_panel_init(panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_lcd_panel_init failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ctx = calloc(1, sizeof(*ctx));
    if (!ctx) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    ctx->base_user_data = panel_handle->user_data;
    ctx->io = io_handle;
    ctx->host_id = LCD_CMD_SPI_HOST;
    ctx->free_bus_on_del = bus_acquired;
    ctx->orig_del = panel_handle->del;
    ctx->orig_disp_on_off = panel_handle->disp_on_off;
    ctx->orig_disp_sleep = panel_handle->disp_sleep;

    panel_handle->del = st7701_panel_del;
    panel_handle->disp_on_off = st7701_panel_disp_on_off;
    panel_handle->disp_sleep = st7701_panel_disp_sleep;
    adopt_user_data(panel_handle, ctx);

    ESP_LOGI(TAG, "ST7701 RGB panel initialized (%dx%d @ %.1f MHz)",
             LCD_H_RES, LCD_V_RES, LCD_PIXEL_CLOCK_HZ / 1000000.0f);

    *ret_panel = panel_handle;
    return ESP_OK;

cleanup:
    if (ctx) {
        free(ctx);
    }
    if (panel_handle) {
        esp_lcd_panel_del(panel_handle);
        panel_handle = NULL;
    }
    if (io_handle) {
        esp_lcd_panel_io_del(io_handle);
        io_handle = NULL;
    }
    if (bus_acquired) {
        spi_bus_free(LCD_CMD_SPI_HOST);
    }
    return ret;
}

