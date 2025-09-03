/**
 * @file ui_icons.c
 * @brief Définition des icônes bitmap utilisées dans l'UI
 */
#include "ui_icons.h"

/* Icône profil 16x16, format 1 bit alpha */
static const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST uint8_t ui_img_profile_map[] = {
    0x00, 0x00,
    0x03, 0xC0,
    0x07, 0xE0,
    0x07, 0xE0,
    0x03, 0xC0,
    0x00, 0x00,
    0x07, 0xE0,
    0x0F, 0xF0,
    0x0F, 0xF0,
    0x0F, 0xF0,
    0x0F, 0xF0,
    0x0F, 0xF0,
    0x1F, 0xF8,
    0x1F, 0xF8,
    0x3F, 0xFC,
    0x00, 0x00
};

const lv_image_dsc_t ui_img_profile = {
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.cf = LV_COLOR_FORMAT_A1,
    .header.flags = 0,
    .header.w = 16,
    .header.h = 16,
    .data_size = sizeof(ui_img_profile_map),
    .data = ui_img_profile_map,
};

