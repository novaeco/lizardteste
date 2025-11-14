#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t lv_color_t;

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

lv_display_t *lv_display_create(int32_t hor_res, int32_t ver_res);
void lv_display_delete(lv_display_t *display);
void lv_display_set_default(lv_display_t *display);
void lv_display_set_flush_cb(lv_display_t *display, lv_display_flush_cb_t cb);
void lv_display_set_buffers(lv_display_t *display, void *buf1, void *buf2,
                            size_t size_in_bytes, lv_display_render_mode_t mode);
void lv_display_flush_ready(lv_display_t *display);

#ifdef __cplusplus
}
#endif

