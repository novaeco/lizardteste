/**
 * @file ch422g.c
 * @brief Minimal I2C driver for CH422G output expander
 */

#include "ch422g.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_check.h"
#include "i2c_bus.h"

#define CH422_I2C_ADDR 0x20
#define CH422_I2C_FREQ 100000

static const char *TAG = "CH422G";

static i2c_master_bus_handle_t s_bus = NULL;
static i2c_master_dev_handle_t s_dev = NULL;
static uint8_t s_output_state = 0x00;

static esp_err_t ch422g_write_state(void)
{
    esp_err_t err = i2c_master_transmit(s_dev, &s_output_state, 1, 1000);
    if (err == ESP_ERR_TIMEOUT) {
        ESP_LOGE(TAG, "i2c_master_transmit timeout");
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2c_master_transmit failed: %s", esp_err_to_name(err));
    }
    return err;
}

esp_err_t ch422g_init(void)
{
    if (s_dev) {
        return ESP_OK;
    }

    s_bus = i2c_bus_get();
    ESP_RETURN_ON_FALSE(s_bus, ESP_ERR_INVALID_STATE, TAG, "bus not init");

    i2c_device_config_t dev_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CH422_I2C_ADDR,
        .scl_speed_hz = CH422_I2C_FREQ,
    };
    ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(s_bus, &dev_conf, &s_dev), TAG, "add dev");

    s_output_state = 0x00;
    return ch422g_write_state();
}

esp_err_t ch422g_set_pin(ch422g_pin_t exio, bool level)
{
    if (!s_dev) {
        return ESP_ERR_INVALID_STATE;
    }
    if (level) {
        s_output_state |= (1u << exio);
    } else {
        s_output_state &= ~(1u << exio);
    }
    return ch422g_write_state();
}

bool ch422g_get_pin(ch422g_pin_t exio)
{
    if (!s_dev) {
        return false;
    }
    return (s_output_state >> exio) & 0x01u;
}

