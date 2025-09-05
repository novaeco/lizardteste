/**
 * @file ch422.c
 * @brief Minimal I2C driver for CH422G output expander
 */

#include "ch422.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_check.h"

#define CH422_I2C_PORT I2C_NUM_1
#define CH422_PIN_SDA 4
#define CH422_PIN_SCL 5
#define CH422_I2C_ADDR 0x20
#define CH422_I2C_FREQ 100000

static const char *TAG = "CH422";

static i2c_master_bus_handle_t s_bus = NULL;
static i2c_master_dev_handle_t s_dev = NULL;
static uint8_t s_output_state = 0x00;

static esp_err_t ch422_write_state(void)
{
    return i2c_master_transmit(s_dev, &s_output_state, 1, 1000);
}

esp_err_t ch422_init(void)
{
    if (s_dev) {
        return ESP_OK;
    }

    i2c_master_bus_config_t bus_conf = {
        .i2c_port = CH422_I2C_PORT,
        .sda_io_num = CH422_PIN_SDA,
        .scl_io_num = CH422_PIN_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
            .enable_internal_pullup = true,
        },
    };

    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_conf, &s_bus), TAG, "bus init");

    i2c_device_config_t dev_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CH422_I2C_ADDR,
        .scl_speed_hz = CH422_I2C_FREQ,
    };
    ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(s_bus, &dev_conf, &s_dev), TAG, "add dev");

    s_output_state = 0x00;
    return ch422_write_state();
}

esp_err_t ch422_set_pin(ch422_pin_t pin, bool level)
{
    if (!s_dev) {
        return ESP_ERR_INVALID_STATE;
    }
    if (level) {
        s_output_state |= (1u << pin);
    } else {
        s_output_state &= ~(1u << pin);
    }
    return ch422_write_state();
}
