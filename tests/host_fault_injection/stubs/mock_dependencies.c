#include <string.h>
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_ops.h"
#include "st7701_rgb.h"
#include "lvgl.h"
#include "esp_log.h"
#include "mock_support.h"

#define MAX_TRACKED_PTRS 16

static void *alloc_sequence[MAX_TRACKED_PTRS];
static size_t alloc_sequence_length;
static size_t alloc_sequence_index;
static size_t active_allocations;
static void *freed_ptrs[MAX_TRACKED_PTRS];
static size_t freed_ptrs_count;

static size_t backlight_call_count;
static bool last_backlight_level;

static bool panel_del_invoked;
static bool panel_disp_off_invoked;

static mock_panel_t panel_instance;

int64_t esp_timer_get_time(void)
{
    return 0;
}

void test_reset_mocks(void)
{
    memset(alloc_sequence, 0, sizeof(alloc_sequence));
    alloc_sequence_length = 0;
    alloc_sequence_index = 0;
    active_allocations = 0;
    memset(freed_ptrs, 0, sizeof(freed_ptrs));
    freed_ptrs_count = 0;
    backlight_call_count = 0;
    last_backlight_level = false;
    panel_del_invoked = false;
    panel_disp_off_invoked = false;
}

void test_heap_caps_set_sequence(void *const *sequence, size_t length)
{
    if (length > MAX_TRACKED_PTRS) {
        length = MAX_TRACKED_PTRS;
    }
    for (size_t i = 0; i < length; ++i) {
        alloc_sequence[i] = sequence[i];
    }
    alloc_sequence_length = length;
    alloc_sequence_index = 0;
}

size_t test_heap_caps_active_allocations(void)
{
    return active_allocations;
}

bool test_heap_caps_pointer_freed(const void *ptr)
{
    for (size_t i = 0; i < freed_ptrs_count; ++i) {
        if (freed_ptrs[i] == ptr) {
            return true;
        }
    }
    return false;
}

size_t test_backlight_call_count(void)
{
    return backlight_call_count;
}

bool test_backlight_last_level(void)
{
    return last_backlight_level;
}

bool test_panel_del_called(void)
{
    return panel_del_invoked;
}

bool test_panel_disp_off_called(void)
{
    return panel_disp_off_invoked;
}

void *heap_caps_malloc(size_t size, uint32_t caps)
{
    (void)size;
    (void)caps;
    void *ptr = NULL;
    if (alloc_sequence_index < alloc_sequence_length) {
        ptr = alloc_sequence[alloc_sequence_index++];
    }
    if (ptr) {
        ++active_allocations;
    }
    return ptr;
}

void heap_caps_free(void *ptr)
{
    if (!ptr) {
        return;
    }
    if (freed_ptrs_count < MAX_TRACKED_PTRS) {
        freed_ptrs[freed_ptrs_count++] = ptr;
    }
    if (active_allocations > 0) {
        --active_allocations;
    }
}

esp_err_t ch422g_init(void)
{
    return ESP_OK;
}

esp_err_t ch422g_set_pin(ch422g_pin_t exio, bool level)
{
    (void)exio;
    ++backlight_call_count;
    last_backlight_level = level;
    return ESP_OK;
}

bool ch422g_get_pin(ch422g_pin_t exio)
{
    (void)exio;
    return last_backlight_level;
}

esp_err_t st7701_rgb_new_panel(esp_lcd_panel_handle_t *handle)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }
    *handle = &panel_instance;
    return ESP_OK;
}

esp_err_t esp_lcd_panel_draw_bitmap(esp_lcd_panel_handle_t handle,
                                    int x_start, int y_start,
                                    int x_end, int y_end,
                                    const void *color_data)
{
    (void)handle;
    (void)x_start;
    (void)y_start;
    (void)x_end;
    (void)y_end;
    (void)color_data;
    return ESP_OK;
}

esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t handle, bool on)
{
    (void)handle;
    if (!on) {
        panel_disp_off_invoked = true;
    }
    return ESP_OK;
}

esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t handle)
{
    (void)handle;
    panel_del_invoked = true;
    return ESP_OK;
}

esp_err_t esp_lcd_panel_disp_sleep(esp_lcd_panel_handle_t handle, bool sleep)
{
    (void)handle;
    (void)sleep;
    return ESP_OK;
}

struct lv_display_t {
    int dummy;
};

static lv_display_t display_instance;

lv_display_t *lv_display_create(int32_t hor_res, int32_t ver_res)
{
    (void)hor_res;
    (void)ver_res;
    return &display_instance;
}

void lv_display_delete(lv_display_t *display)
{
    (void)display;
}

void lv_display_set_default(lv_display_t *display)
{
    (void)display;
}

void lv_display_set_flush_cb(lv_display_t *display, lv_display_flush_cb_t cb)
{
    (void)display;
    (void)cb;
}

void lv_display_set_buffers(lv_display_t *display, void *buf1, void *buf2,
                            size_t size_in_bytes, lv_display_render_mode_t mode)
{
    (void)display;
    (void)buf1;
    (void)buf2;
    (void)size_in_bytes;
    (void)mode;
}

void lv_display_flush_ready(lv_display_t *display)
{
    (void)display;
}
