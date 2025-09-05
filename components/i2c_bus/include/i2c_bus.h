#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"

/**
 * @brief Initialize the shared I2C master bus.
 *
 * The bus uses port I2C_NUM_0 with SDA on GPIO 8 and SCL on GPIO 9. This
 * function creates the bus only once; subsequent calls will return ESP_OK
 * without reinitializing it.
 *
 * @return ESP_OK on success or an error code from i2c_new_master_bus.
 */
esp_err_t i2c_bus_init(void);

/**
 * @brief Get the handle of the shared I2C master bus.
 *
 * @return The handle created by i2c_bus_init(), or NULL if the bus has not
 *         been initialized.
 */
i2c_master_bus_handle_t i2c_bus_get(void);

/**
 * @brief Deinitialize the shared I2C master bus.
 *
 * Deletes the bus created by i2c_bus_init() and resets the internal handle. If
 * the bus was not initialized, this function does nothing and returns ESP_OK.
 *
 * @return ESP_OK on success or an error code from i2c_del_master_bus.
 */
esp_err_t i2c_bus_deinit(void);

