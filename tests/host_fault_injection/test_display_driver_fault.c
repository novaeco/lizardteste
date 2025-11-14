#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "display_driver.h"
#include "mock_support.h"

int main(void)
{
    test_reset_mocks();

    static lv_color_t dummy_buf1[4];
    static lv_color_t dummy_buf2[4];

    void *sequence[] = {
        dummy_buf1,
        NULL,
        dummy_buf2,
        NULL,
    };
    size_t sequence_len = sizeof(sequence) / sizeof(sequence[0]);
    for (int attempt = 0; attempt < 2; ++attempt) {
        test_heap_caps_set_sequence(sequence, sequence_len);

        esp_err_t ret = display_driver_init();
        assert(ret == ESP_ERR_NO_MEM);

        /* Each failure must trigger a full CH422G teardown. */
        assert(test_ch422g_deinit_call_count() == (size_t)(attempt + 1));
    }

    /* Backlight must be turned off when cleanup path is triggered. */
    assert(test_backlight_call_count() >= 2);
    assert(test_backlight_last_level() == false);

    /* All partially allocated buffers must be released. */
    assert(test_heap_caps_pointer_freed(dummy_buf1));
    assert(test_heap_caps_pointer_freed(dummy_buf2));
    assert(test_heap_caps_active_allocations() == 0);

    /* Panel resources must be disposed. */
    assert(test_panel_disp_off_called());
    assert(test_panel_del_called());

    puts("Fault injection test passed");
    return 0;
}
