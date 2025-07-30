/**
 * @file ui_styles.h
 * @brief Styles personnalisés pour l'interface NovaReptileElevage
 * @author NovaReptileElevage Team
 */

#ifndef UI_STYLES_H
#define UI_STYLES_H

#include "lvgl.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Palette de couleurs NovaReptileElevage
#define COLOR_BG_MAIN        lv_color_hex(0xFAFAFA)  // Blanc cassé
#define COLOR_BG_LIGHT       lv_color_hex(0xF5F5F5)  // Gris clair
#define COLOR_BG_PASTEL      lv_color_hex(0xE8F4F8)  // Bleu-gris pastel
#define COLOR_ACCENT_BLUE    lv_color_hex(0x4A90E2)  // Bleu doux
#define COLOR_ACCENT_GREEN   lv_color_hex(0x7ED321)  // Vert nature
#define COLOR_ACCENT_ORANGE  lv_color_hex(0xF5A623)  // Orange alertes
#define COLOR_TEXT_DARK      lv_color_hex(0x2C3E50)  // Gris anthracite
#define COLOR_TEXT_MEDIUM    lv_color_hex(0x5D6D7E)  // Gris moyen
#define COLOR_TEXT_LIGHT     lv_color_hex(0x85929E)  // Gris clair
#define COLOR_WHITE          lv_color_hex(0xFFFFFF)  // Blanc pur
#define COLOR_SHADOW         lv_color_hex(0x000000)  // Noir pour ombres

// Constantes de style
#define RADIUS_SMALL    8
#define RADIUS_MEDIUM   12
#define RADIUS_LARGE    16
#define SHADOW_WIDTH    8
#define SHADOW_SPREAD   2

/**
 * @brief Initialise tous les styles personnalisés
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_styles_init(void);

/**
 * @brief Libère les ressources des styles
 */
void ui_styles_deinit(void);

// Getters pour les styles principaux
lv_style_t* ui_styles_get_main_bg(void);
lv_style_t* ui_styles_get_header_bg(void);
lv_style_t* ui_styles_get_sidebar_bg(void);
lv_style_t* ui_styles_get_content_bg(void);
lv_style_t* ui_styles_get_footer_bg(void);

// Getters pour les styles de composants
lv_style_t* ui_styles_get_button_primary(void);
lv_style_t* ui_styles_get_button_secondary(void);
lv_style_t* ui_styles_get_button_danger(void);
lv_style_t* ui_styles_get_card_style(void);
lv_style_t* ui_styles_get_text_title(void);
lv_style_t* ui_styles_get_text_subtitle(void);
lv_style_t* ui_styles_get_text_body(void);
lv_style_t* ui_styles_get_text_small(void);

// Getters pour les styles de navigation
lv_style_t* ui_styles_get_nav_item_normal(void);
lv_style_t* ui_styles_get_nav_item_active(void);
lv_style_t* ui_styles_get_nav_item_hover(void);

#ifdef __cplusplus
}
#endif

#endif // UI_STYLES_H