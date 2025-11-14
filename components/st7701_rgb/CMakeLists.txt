idf_component_register(
    SRCS "st7701_rgb.c"
    INCLUDE_DIRS "."
    REQUIRES esp_lcd driver esp_driver_spi
)
