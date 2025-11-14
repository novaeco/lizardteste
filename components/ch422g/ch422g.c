/**
 * @file ch422g.c
 * @brief Minimal I2C driver for CH422G output expander
 */

#include "ch422g.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "i2c_bus.h"

#define CH422_I2C_FREQ_HZ         (100000)
#define CH422_I2C_TIMEOUT_TICKS   (pdMS_TO_TICKS(10))

/* Each CH422G register is exposed on a dedicated I²C address */
#define CH422_ADDR_WR_SET         (0x48 >> 1)  /* Output enable / mode control */
#define CH422_ADDR_WR_IO          (0x70 >> 1)  /* Push-pull outputs EXIO0..7  */
#define CH422_ADDR_RD_IO          (0x4D >> 1)  /* Readback for EXIO0..7       */

#define CH422_WR_SET_DEFAULT      (0x01)       /* Enable push-pull outputs */

static const char *TAG = "CH422G";

typedef struct {
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t dev_wr_set;
    i2c_master_dev_handle_t dev_wr_io;
    i2c_master_dev_handle_t dev_rd_io;
    uint8_t wr_set_shadow;
    uint8_t wr_io_shadow;
    bool initialized;
} ch422g_ctx_t;

static ch422g_ctx_t s_ctx = {0};

static void ch422g_cleanup_devices(void)
{
    if (s_ctx.dev_wr_set) {
        esp_err_t err = i2c_master_bus_rm_device(s_ctx.dev_wr_set);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Failed to remove WR_SET device: %s", esp_err_to_name(err));
        }
        s_ctx.dev_wr_set = NULL;
    }
    if (s_ctx.dev_wr_io) {
        esp_err_t err = i2c_master_bus_rm_device(s_ctx.dev_wr_io);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Failed to remove WR_IO device: %s", esp_err_to_name(err));
        }
        s_ctx.dev_wr_io = NULL;
    }
    if (s_ctx.dev_rd_io) {
        esp_err_t err = i2c_master_bus_rm_device(s_ctx.dev_rd_io);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Failed to remove RD_IO device: %s", esp_err_to_name(err));
        }
        s_ctx.dev_rd_io = NULL;
    }
}

static esp_err_t ch422g_write_state(void)
{
    if (!s_ctx.initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t data = s_ctx.wr_io_shadow;
    esp_err_t err = i2c_master_transmit(s_ctx.dev_wr_io, &data, sizeof(data), CH422_I2C_TIMEOUT_TICKS);
    if (err == ESP_ERR_TIMEOUT) {
        ESP_LOGE(TAG, "WR_IO transmit timeout");
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "WR_IO transmit failed: %s", esp_err_to_name(err));
    }
    return err;
}

static esp_err_t ch422g_configure_defaults(void)
{
    s_ctx.wr_set_shadow = CH422_WR_SET_DEFAULT;
    esp_err_t err = i2c_master_transmit(s_ctx.dev_wr_set, &s_ctx.wr_set_shadow, sizeof(uint8_t), CH422_I2C_TIMEOUT_TICKS);
    if (err != ESP_OK) {
        return err;
    }

    /* Drive all outputs low by default (keeps peripherals in reset until released). */
    s_ctx.wr_io_shadow = 0x00;
    return ch422g_write_state();
}

esp_err_t ch422g_init(void)
{
    if (s_ctx.initialized) {
        return ESP_OK;
    }

    s_ctx.bus = i2c_bus_get();
    ESP_RETURN_ON_FALSE(s_ctx.bus, ESP_ERR_INVALID_STATE, TAG, "I2C bus not initialised");

    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .scl_speed_hz = CH422_I2C_FREQ_HZ,
    };

    cfg.device_address = CH422_ADDR_WR_SET;
    ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(s_ctx.bus, &cfg, &s_ctx.dev_wr_set), TAG, "add WR_SET");

    cfg.device_address = CH422_ADDR_WR_IO;
    esp_err_t err = i2c_master_bus_add_device(s_ctx.bus, &cfg, &s_ctx.dev_wr_io);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add WR_IO device: %s", esp_err_to_name(err));
        ch422g_deinit();
        return err;
    }

    cfg.device_address = CH422_ADDR_RD_IO;
    err = i2c_master_bus_add_device(s_ctx.bus, &cfg, &s_ctx.dev_rd_io);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to add RD_IO device: %s", esp_err_to_name(err));
        s_ctx.dev_rd_io = NULL;
    }

    err = ch422g_configure_defaults();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure defaults: %s", esp_err_to_name(err));
        ch422g_deinit();
        return err;
    }

    s_ctx.initialized = true;
    return ESP_OK;
}

esp_err_t ch422g_set_pin(ch422g_pin_t exio, bool level)
{
    ESP_RETURN_ON_FALSE(s_ctx.initialized, ESP_ERR_INVALID_STATE, TAG, "driver not initialised");
    ESP_RETURN_ON_FALSE(exio < 8, ESP_ERR_INVALID_ARG, TAG, "invalid EXIO index");

    uint8_t mask = (uint8_t)(1u << exio);
    uint8_t new_state = s_ctx.wr_io_shadow;
    if (level) {
        new_state |= mask;
    } else {
        new_state &= (uint8_t)~mask;
    }

    if (new_state == s_ctx.wr_io_shadow) {
        return ESP_OK;
    }

    s_ctx.wr_io_shadow = new_state;
    return ch422g_write_state();
}

bool ch422g_get_pin(ch422g_pin_t exio)
{
    if (!s_ctx.initialized || exio >= 8) {
        return false;
    }

    if (s_ctx.dev_rd_io) {
        uint8_t data = 0;
        esp_err_t err = i2c_master_receive(s_ctx.dev_rd_io, &data, sizeof(data), CH422_I2C_TIMEOUT_TICKS);
        if (err == ESP_OK) {
            return ((data >> exio) & 0x01u) != 0;
        }
        ESP_LOGW(TAG, "RD_IO read failed (%s), falling back to shadow", esp_err_to_name(err));
    }

    return ((s_ctx.wr_io_shadow >> exio) & 0x01u) != 0;
}

void ch422g_deinit(void)
{
    ch422g_cleanup_devices();
    s_ctx = (ch422g_ctx_t){0};
}

