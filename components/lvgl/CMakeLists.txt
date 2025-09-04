file(GLOB_RECURSE SOURCES "lvgl/src/*.c")

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "lvgl/src" 
                                 "lvgl"
                                 "."
                    REQUIRES esp_timer
                             driver)

# Désactiver les warnings de format pour LVGL (bibliothèque externe)
target_compile_options(__idf_lvgl PRIVATE -Wno-format)

# Ajouter la définition pour que LVGL trouve lv_conf.h
target_compile_definitions(__idf_lvgl PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
