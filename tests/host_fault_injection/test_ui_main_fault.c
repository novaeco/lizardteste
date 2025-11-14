#include <assert.h>
#include <stdio.h>
#include "ui_main.h"
#include "mock_support.h"

int main(void)
{
    test_reset_mocks();
    test_ui_set_header_init_result(ESP_FAIL);

    esp_err_t ret = ui_main_init();
    assert(ret == ESP_FAIL);

    assert(test_ui_styles_init_call_count() == 1);
    assert(test_ui_styles_deinit_call_count() == 1);
    assert(test_lvgl_active_object_count() == 0);

    const nova_ui_t *ui = ui_main_get_instance();
    assert(ui->main_screen == NULL);
    assert(ui->header_container == NULL);
    assert(ui->sidebar_container == NULL);
    assert(ui->content_container == NULL);
    assert(ui->footer_container == NULL);

    puts("UI main fault injection test passed");
    return 0;
}
