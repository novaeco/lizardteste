idf_component_register(INCLUDE_DIRS "." REQUIRES lvgl)

idf_component_get_property(lvgl_lib lvgl COMPONENT_LIB)
if(lvgl_lib)
    target_include_directories(${lvgl_lib} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(${lvgl_lib} PUBLIC LV_CONF_INCLUDE_SIMPLE=1)
endif()
