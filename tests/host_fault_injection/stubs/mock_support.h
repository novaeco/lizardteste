#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"
#include "ch422g.h"

#ifdef __cplusplus
extern "C" {
#endif

void test_reset_mocks(void);

void test_heap_caps_set_sequence(void *const *sequence, size_t length);
size_t test_heap_caps_active_allocations(void);
bool test_heap_caps_pointer_freed(const void *ptr);

size_t test_backlight_call_count(void);
bool test_backlight_last_level(void);
size_t test_ch422g_deinit_call_count(void);

bool test_panel_del_called(void);
bool test_panel_disp_off_called(void);

void test_lvgl_reset_objects(void);
size_t test_lvgl_active_object_count(void);

void test_ui_set_header_init_result(esp_err_t result);
size_t test_ui_styles_init_call_count(void);
size_t test_ui_styles_deinit_call_count(void);

#ifdef __cplusplus
}
#endif

