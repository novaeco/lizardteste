#include <string.h>
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_ops.h"
#include "st7701_rgb.h"
#include "lvgl.h"
#include "esp_log.h"
#include "mock_support.h"
#include "ui_main.h"
#include "ui_header.h"
#include "ui_sidebar.h"
#include "ui_content.h"
#include "ui_footer.h"
#include "ui_data.h"
#include "ui_styles.h"

#define MAX_TRACKED_PTRS 16
#define MAX_LV_OBJECTS 32

typedef struct {
    bool used;
    lv_obj_t obj;
} lv_obj_slot_t;

enum {
    STYLE_MAIN_BG = 0,
    STYLE_HEADER_BG,
    STYLE_SIDEBAR_BG,
    STYLE_CONTENT_BG,
    STYLE_FOOTER_BG,
    STYLE_BUTTON_PRIMARY,
    STYLE_BUTTON_SECONDARY,
    STYLE_BUTTON_DANGER,
    STYLE_CARD,
    STYLE_TEXT_TITLE,
    STYLE_TEXT_SUBTITLE,
    STYLE_TEXT_BODY,
    STYLE_TEXT_SMALL,
    STYLE_NAV_ITEM_NORMAL,
    STYLE_NAV_ITEM_ACTIVE,
    STYLE_NAV_ITEM_HOVER,
    STYLE_NAV_INDICATOR,
    STYLE_STATUS_OK,
    STYLE_ALERT_CRITICAL,
    STYLE_ALERT_WARNING,
    STYLE_ALERT_INFO,
    STYLE_COUNT
};

static void *alloc_sequence[MAX_TRACKED_PTRS];
static size_t alloc_sequence_length;
static size_t alloc_sequence_index;
static size_t active_allocations;
static void *freed_ptrs[MAX_TRACKED_PTRS];
static size_t freed_ptrs_count;

static lv_obj_slot_t lv_obj_pool[MAX_LV_OBJECTS];
static size_t lv_obj_active_count;
static lv_obj_t *lv_active_screen;

static lv_style_t style_pool[STYLE_COUNT];
static esp_err_t header_init_result = ESP_OK;
static size_t styles_init_calls;
static size_t styles_deinit_calls;
static lv_obj_t *content_container_ref;

static size_t backlight_call_count;
static bool last_backlight_level;

static bool panel_del_invoked;
static bool panel_disp_off_invoked;

static mock_panel_t panel_instance;

ui_menu_item_t g_ui_menu_items[1];
size_t g_ui_menu_items_count;
const char *g_ui_reptiles[1];
size_t g_ui_reptiles_count;
ui_alert_item_t g_ui_alerts[1];
size_t g_ui_alerts_count;
const char *g_ui_settings_sections[1];
size_t g_ui_settings_sections_count;

static void reset_lvgl_objects_state(void);
static void lv_delete_children(lv_obj_t *parent);
static void lv_obj_del_internal(lv_obj_t *obj);

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
    reset_lvgl_objects_state();
    styles_init_calls = 0;
    styles_deinit_calls = 0;
    header_init_result = ESP_OK;
    memset(style_pool, 0, sizeof(style_pool));
    content_container_ref = NULL;
    g_ui_menu_items_count = 0;
    g_ui_reptiles_count = 0;
    g_ui_alerts_count = 0;
    g_ui_settings_sections_count = 0;
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

static void reset_lvgl_objects_state(void)
{
    memset(lv_obj_pool, 0, sizeof(lv_obj_pool));
    lv_obj_active_count = 0;
    lv_active_screen = NULL;
}

void test_lvgl_reset_objects(void)
{
    reset_lvgl_objects_state();
}

size_t test_lvgl_active_object_count(void)
{
    return lv_obj_active_count;
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

lv_obj_t *lv_obj_create(lv_obj_t *parent)
{
    for (size_t i = 0; i < MAX_LV_OBJECTS; ++i) {
        if (!lv_obj_pool[i].used) {
            lv_obj_pool[i].used = true;
            lv_obj_pool[i].obj.parent = parent;
            ++lv_obj_active_count;
            return &lv_obj_pool[i].obj;
        }
    }
    return NULL;
}

static void lv_delete_children(lv_obj_t *parent)
{
    if (!parent) {
        return;
    }

    for (size_t i = 0; i < MAX_LV_OBJECTS; ++i) {
        if (lv_obj_pool[i].used && lv_obj_pool[i].obj.parent == parent) {
            lv_obj_del_internal(&lv_obj_pool[i].obj);
        }
    }
}

static void lv_obj_del_internal(lv_obj_t *obj)
{
    if (!obj) {
        return;
    }

    lv_delete_children(obj);

    for (size_t i = 0; i < MAX_LV_OBJECTS; ++i) {
        if (lv_obj_pool[i].used && &lv_obj_pool[i].obj == obj) {
            lv_obj_pool[i].used = false;
            if (lv_obj_active_count > 0) {
                --lv_obj_active_count;
            }
            if (content_container_ref == obj) {
                content_container_ref = NULL;
            }
            break;
        }
    }

    if (lv_active_screen == obj) {
        lv_active_screen = NULL;
    }
}

void lv_obj_del(lv_obj_t *obj)
{
    lv_obj_del_internal(obj);
}

void lv_obj_remove_style_all(lv_obj_t *obj)
{
    (void)obj;
}

void lv_obj_add_style(lv_obj_t *obj, const lv_style_t *style, lv_style_selector_t selector)
{
    (void)obj;
    (void)style;
    (void)selector;
}

void lv_obj_set_layout(lv_obj_t *obj, lv_layout_t layout)
{
    (void)obj;
    (void)layout;
}

void lv_obj_set_grid_dsc_array(lv_obj_t *obj, const lv_coord_t *col_dsc, const lv_coord_t *row_dsc)
{
    (void)obj;
    (void)col_dsc;
    (void)row_dsc;
}

void lv_obj_set_grid_cell(lv_obj_t *obj,
                          lv_grid_align_t x_align, uint8_t col, uint8_t col_span,
                          lv_grid_align_t y_align, uint8_t row, uint8_t row_span)
{
    (void)obj;
    (void)x_align;
    (void)col;
    (void)col_span;
    (void)y_align;
    (void)row;
    (void)row_span;
}

void lv_scr_load(lv_obj_t *scr)
{
    lv_active_screen = scr;
}

lv_obj_t *lv_scr_act(void)
{
    return lv_active_screen;
}

void lv_obj_clean(lv_obj_t *obj)
{
    if (!obj) {
        return;
    }
    lv_delete_children(obj);
}

size_t test_ui_styles_init_call_count(void)
{
    return styles_init_calls;
}

size_t test_ui_styles_deinit_call_count(void)
{
    return styles_deinit_calls;
}

void test_ui_set_header_init_result(esp_err_t result)
{
    header_init_result = result;
}

esp_err_t ui_styles_init(void)
{
    ++styles_init_calls;
    return ESP_OK;
}

void ui_styles_deinit(void)
{
    ++styles_deinit_calls;
}

lv_style_t* ui_styles_get_main_bg(void)
{
    return &style_pool[STYLE_MAIN_BG];
}

lv_style_t* ui_styles_get_header_bg(void)
{
    return &style_pool[STYLE_HEADER_BG];
}

lv_style_t* ui_styles_get_sidebar_bg(void)
{
    return &style_pool[STYLE_SIDEBAR_BG];
}

lv_style_t* ui_styles_get_content_bg(void)
{
    return &style_pool[STYLE_CONTENT_BG];
}

lv_style_t* ui_styles_get_footer_bg(void)
{
    return &style_pool[STYLE_FOOTER_BG];
}

lv_style_t* ui_styles_get_button_primary(void)
{
    return &style_pool[STYLE_BUTTON_PRIMARY];
}

lv_style_t* ui_styles_get_button_secondary(void)
{
    return &style_pool[STYLE_BUTTON_SECONDARY];
}

lv_style_t* ui_styles_get_button_danger(void)
{
    return &style_pool[STYLE_BUTTON_DANGER];
}

lv_style_t* ui_styles_get_card_style(void)
{
    return &style_pool[STYLE_CARD];
}

lv_style_t* ui_styles_get_text_title(void)
{
    return &style_pool[STYLE_TEXT_TITLE];
}

lv_style_t* ui_styles_get_text_subtitle(void)
{
    return &style_pool[STYLE_TEXT_SUBTITLE];
}

lv_style_t* ui_styles_get_text_body(void)
{
    return &style_pool[STYLE_TEXT_BODY];
}

lv_style_t* ui_styles_get_text_small(void)
{
    return &style_pool[STYLE_TEXT_SMALL];
}

lv_style_t* ui_styles_get_nav_item_normal(void)
{
    return &style_pool[STYLE_NAV_ITEM_NORMAL];
}

lv_style_t* ui_styles_get_nav_item_active(void)
{
    return &style_pool[STYLE_NAV_ITEM_ACTIVE];
}

lv_style_t* ui_styles_get_nav_item_hover(void)
{
    return &style_pool[STYLE_NAV_ITEM_HOVER];
}

lv_style_t* ui_styles_get_nav_indicator(void)
{
    return &style_pool[STYLE_NAV_INDICATOR];
}

lv_style_t* ui_styles_get_status_ok(void)
{
    return &style_pool[STYLE_STATUS_OK];
}

lv_style_t* ui_styles_get_alert_level_critical(void)
{
    return &style_pool[STYLE_ALERT_CRITICAL];
}

lv_style_t* ui_styles_get_alert_level_warning(void)
{
    return &style_pool[STYLE_ALERT_WARNING];
}

lv_style_t* ui_styles_get_alert_level_info(void)
{
    return &style_pool[STYLE_ALERT_INFO];
}

void ui_data_load_defaults(void)
{
    g_ui_menu_items_count = 0;
    g_ui_reptiles_count = 0;
    g_ui_alerts_count = 0;
    g_ui_settings_sections_count = 0;
}

void ui_data_reload(void)
{
}

esp_err_t ui_header_init(lv_obj_t *parent)
{
    (void)parent;
    return header_init_result;
}

void ui_header_deinit(void)
{
}

void ui_header_set_title(const char *title)
{
    (void)title;
}

void ui_header_set_connection_status(bool connected)
{
    (void)connected;
}

void ui_header_set_time(const char *time_str)
{
    (void)time_str;
}

esp_err_t ui_header_open_profile_menu(void)
{
    return ESP_OK;
}

esp_err_t ui_header_open_quick_settings(void)
{
    return ESP_OK;
}

esp_err_t ui_sidebar_init(lv_obj_t *parent)
{
    (void)parent;
    return ESP_OK;
}

void ui_sidebar_set_active_item(nova_screen_t screen_type)
{
    (void)screen_type;
}

void ui_sidebar_update_indicators(void)
{
}

esp_err_t ui_content_init(lv_obj_t *parent)
{
    content_container_ref = parent;
    return ESP_OK;
}

esp_err_t ui_content_load_screen(nova_screen_t screen_type)
{
    (void)screen_type;
    return ESP_OK;
}

void ui_content_update_realtime_data(void)
{
}

lv_obj_t* ui_content_get_container(void)
{
    return content_container_ref;
}

esp_err_t ui_footer_init(lv_obj_t *parent)
{
    (void)parent;
    return ESP_OK;
}

void ui_footer_update_status(void)
{
}

void ui_footer_set_wifi_status(bool connected, int signal_strength)
{
    (void)connected;
    (void)signal_strength;
}

void ui_footer_set_notification_count(int count)
{
    (void)count;
}

void ui_footer_set_datetime(const char *time_str)
{
    (void)time_str;
}
