#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXIO0 0
#define EXIO1 1
#define EXIO2 2
#define EXIO3 3
#define EXIO4 4
#define EXIO5 5
#define EXIO6 6
#define EXIO7 7

typedef uint8_t ch422g_pin_t;

esp_err_t ch422g_init(void);
void ch422g_deinit(void);
esp_err_t ch422g_set_pin(ch422g_pin_t exio, bool level);
bool ch422g_get_pin(ch422g_pin_t exio);

#ifdef __cplusplus
}
#endif

