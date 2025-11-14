#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t lv_color_t;

static inline lv_color_t lv_color_hex(uint32_t hex)
{
    (void)hex;
    return 0;
}

typedef struct {
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} lv_area_t;

typedef struct lv_display_t lv_display_t;
typedef void (*lv_display_flush_cb_t)(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);

typedef enum {
    LV_DISPLAY_RENDER_MODE_PARTIAL = 0,
} lv_display_render_mode_t;

typedef int32_t lv_coord_t;

typedef struct lv_style_t {
    int dummy;
} lv_style_t;

typedef struct lv_obj_t {
    struct lv_obj_t *parent;
} lv_obj_t;

typedef int32_t lv_style_selector_t;

typedef enum {
    LV_LAYOUT_GRID = 0,
} lv_layout_t;

typedef enum {
    LV_GRID_ALIGN_STRETCH = 0,
} lv_grid_align_t;

#define LV_GRID_TEMPLATE_LAST (INT32_MIN)
#define LV_GRID_FR(x) ((lv_coord_t)(x))

lv_display_t *lv_display_create(int32_t hor_res, int32_t ver_res);
void lv_display_delete(lv_display_t *display);
void lv_display_set_default(lv_display_t *display);
void lv_display_set_flush_cb(lv_display_t *display, lv_display_flush_cb_t cb);
void lv_display_set_buffers(lv_display_t *display, void *buf1, void *buf2,
                            size_t size_in_bytes, lv_display_render_mode_t mode);
void lv_display_flush_ready(lv_display_t *display);

lv_obj_t *lv_obj_create(lv_obj_t *parent);
void lv_obj_del(lv_obj_t *obj);
void lv_obj_remove_style_all(lv_obj_t *obj);
void lv_obj_add_style(lv_obj_t *obj, const lv_style_t *style, lv_style_selector_t selector);
void lv_obj_set_layout(lv_obj_t *obj, lv_layout_t layout);
void lv_obj_set_grid_dsc_array(lv_obj_t *obj, const lv_coord_t *col_dsc, const lv_coord_t *row_dsc);
void lv_obj_set_grid_cell(lv_obj_t *obj,
                          lv_grid_align_t x_align, uint8_t col, uint8_t col_span,
                          lv_grid_align_t y_align, uint8_t row, uint8_t row_span);
void lv_scr_load(lv_obj_t *scr);
lv_obj_t *lv_scr_act(void);
void lv_obj_clean(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

