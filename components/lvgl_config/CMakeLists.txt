idf_component_register(INCLUDE_DIRS ".")

# Rendez la configuration LVGL accessible à l'ensemble du build (y compris au package Component Manager).
idf_build_set_property(INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}" APPEND)
