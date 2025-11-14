#pragma once

#include <stdio.h>

#define ESP_LOGE(tag, fmt, ...) ((void)fprintf(stderr, "E (%s): " fmt "\n", tag, ##__VA_ARGS__))
#define ESP_LOGW(tag, fmt, ...) ((void)fprintf(stderr, "W (%s): " fmt "\n", tag, ##__VA_ARGS__))
#define ESP_LOGI(tag, fmt, ...) ((void)fprintf(stdout, "I (%s): " fmt "\n", tag, ##__VA_ARGS__))
#define ESP_LOGD(tag, fmt, ...) ((void)fprintf(stdout, "D (%s): " fmt "\n", tag, ##__VA_ARGS__))
