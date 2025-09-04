#include "st7262_rgb.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_io_spi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "esp_log.h"

#define TAG "st7262_rgb"

#define LCD_H_RES 800
#define LCD_V_RES 480
#define LCD_PIXEL_CLOCK_HZ (10 * 1000 * 1000)

#define LCD_HSYNC_GPIO 46
#define LCD_VSYNC_GPIO 3
#define LCD_DE_GPIO 5
#define LCD_PCLK_GPIO 7

static const int lcd_data_gpios[16] = {45, 48, 47, 21, 14, 13, 12, 11,
                                       10, 9,  46, 3,  8,  18, 17, 16};

// SPI interface for ST7262 command configuration
#define LCD_CMD_SPI_HOST SPI2_HOST
#define LCD_CMD_SCLK_GPIO  6
#define LCD_CMD_MOSI_GPIO  7
#define LCD_CMD_CS_GPIO    5
#define LCD_CMD_DC_GPIO    4

typedef struct {
  uint8_t cmd;
  uint8_t data[14];
  uint8_t data_bytes;
  uint16_t delay_ms;
} st7262_init_cmd_t;

static const st7262_init_cmd_t st7262_init_cmds[] = {
    {0x11, {0}, 0, 120},      // Sleep out
    {0x3A, {0x55}, 1, 0},     // 16-bit color format
    {0x36, {0x00}, 1, 0},     // Memory access control
    {0x29, {0}, 0, 20},       // Display on
    {0x00, {0}, 0, 0},        // End marker
};

esp_err_t st7262_rgb_new_panel(esp_lcd_panel_handle_t *ret_panel) {
  esp_err_t ret;
  esp_lcd_panel_handle_t panel = NULL;

  // Initialize SPI bus for command interface
  spi_bus_config_t buscfg = {
      .sclk_io_num = LCD_CMD_SCLK_GPIO,
      .mosi_io_num = LCD_CMD_MOSI_GPIO,
      .miso_io_num = -1,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
  };
  ret = spi_bus_initialize(LCD_CMD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "spi bus init failed: %d", ret);
    return ret;
  }

  esp_lcd_panel_io_handle_t io_handle = NULL;
  esp_lcd_panel_io_spi_config_t io_config = {
      .cs_gpio_num = LCD_CMD_CS_GPIO,
      .dc_gpio_num = LCD_CMD_DC_GPIO,
      .spi_mode = 0,
      .pclk_hz = 10 * 1000 * 1000,
      .trans_queue_depth = 10,
      .lcd_cmd_bits = 8,
      .lcd_param_bits = 8,
  };
  ret = esp_lcd_new_panel_io_spi(LCD_CMD_SPI_HOST, &io_config, &io_handle);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "panel io create failed: %d", ret);
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
              .hsync_pulse_width = 10,
              .hsync_back_porch = 40,
              .hsync_front_porch = 40,
              .vsync_pulse_width = 10,
              .vsync_back_porch = 20,
              .vsync_front_porch = 10,
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
      .num_fbs = 0,
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
      .flags =
          {
              .fb_in_psram = true,
          },
  };

  ret = esp_lcd_new_rgb_panel(&rgb_config, &panel);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "rgb panel create failed: %d", ret);
    esp_lcd_panel_io_del(io_handle);
    spi_bus_free(LCD_CMD_SPI_HOST);
    return ret;
  }

  // Send initialization commands over SPI
  for (int i = 0; st7262_init_cmds[i].cmd != 0; i++) {
    ret = esp_lcd_panel_io_tx_param(io_handle, st7262_init_cmds[i].cmd,
                                    st7262_init_cmds[i].data,
                                    st7262_init_cmds[i].data_bytes);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "init cmd %d failed: %d", i, ret);
      esp_lcd_panel_io_del(io_handle);
      spi_bus_free(LCD_CMD_SPI_HOST);
      return ret;
    }
    if (st7262_init_cmds[i].delay_ms) {
      vTaskDelay(pdMS_TO_TICKS(st7262_init_cmds[i].delay_ms));
    }
  }

  esp_lcd_panel_io_del(io_handle);
  spi_bus_free(LCD_CMD_SPI_HOST);

  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));

  *ret_panel = panel;
  return ESP_OK;
}
