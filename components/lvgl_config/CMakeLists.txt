idf_component_register(INCLUDE_DIRS ".")

# Rendez la configuration LVGL accessible à l'ensemble du projet, y compris au
# paquet LVGL récupéré via l'ESP-IDF Component Manager.
idf_build_set_property(INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}" APPEND)
