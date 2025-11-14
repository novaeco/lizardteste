# Legacy placeholder for LVGL sources.
#
# LVGL 9.4 is now provided via the ESP-IDF Component Manager through
# `main/idf_component.yml`. This directory is kept so that downstream
# scripts expecting `components/lvgl/` continue to find a valid path,
# but no local component is registered here.
#
# Returning immediately prevents ESP-IDF from attempting to register a
# component named `lvgl`, which would clash with the managed package.
return()
