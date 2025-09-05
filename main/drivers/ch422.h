#pragma once

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EXIO0 = 0,
    EXIO1,
    EXIO2,
    EXIO3,
    EXIO4,
    EXIO5,
    EXIO6,
    EXIO7,
} ch422_pin_t;

esp_err_t ch422_init(void);
esp_err_t ch422_set_pin(ch422_pin_t pin, bool level);

#ifdef __cplusplus
}
#endif
