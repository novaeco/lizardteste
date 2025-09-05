#include "i2c_bus.h"
#include "esp_log.h"

static const char *TAG = "i2c_bus";
static i2c_master_bus_handle_t s_bus = NULL;

esp_err_t i2c_bus_init(void) {
    if (s_bus) {
        return ESP_OK;
    }
    i2c_master_bus_config_t bus_conf = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = 8,
        .scl_io_num = 9,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
            .enable_internal_pullup = true,
        },
    };
    esp_err_t ret = i2c_new_master_bus(&bus_conf, &s_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c_new_master_bus failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

i2c_master_bus_handle_t i2c_bus_get(void) {
    return s_bus;
}

esp_err_t i2c_bus_deinit(void) {
    if (!s_bus) {
        return ESP_OK;
    }
    esp_err_t ret = i2c_del_master_bus(s_bus);
    if (ret == ESP_OK) {
        s_bus = NULL;
    } else {
        ESP_LOGE(TAG, "i2c_del_master_bus failed: %s", esp_err_to_name(ret));
    }
    return ret;
}
