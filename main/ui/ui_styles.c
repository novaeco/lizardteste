/**
 * @file ui_styles.c
 * @brief Implémentation des styles personnalisés
 * @author NovaReptileElevage Team
 */

#include "ui_styles.h"
#include "esp_log.h"

static const char *TAG = "UI_Styles";

// Styles pour les conteneurs principaux
static lv_style_t style_main_bg;
static lv_style_t style_header_bg;
static lv_style_t style_sidebar_bg;
static lv_style_t style_content_bg;
static lv_style_t style_footer_bg;

// Styles pour les boutons
static lv_style_t style_btn_primary;
static lv_style_t style_btn_secondary;
static lv_style_t style_btn_danger;

// Styles pour les cartes et conteneurs
static lv_style_t style_card;

// Styles pour le texte
static lv_style_t style_text_title;
static lv_style_t style_text_subtitle;
static lv_style_t style_text_body;
static lv_style_t style_text_small;

// Styles pour la navigation
static lv_style_t style_nav_normal;
static lv_style_t style_nav_active;
static lv_style_t style_nav_hover;

// Styles pour les indicateurs et états
static lv_style_t style_nav_indicator;
static lv_style_t style_status_ok;
static lv_style_t style_alert_level_critical;
static lv_style_t style_alert_level_warning;
static lv_style_t style_alert_level_info;

/**
 * @brief Initialise les styles des conteneurs principaux
 */
static void init_container_styles(void)
{
    // Style fond principal
    lv_style_init(&style_main_bg);
    lv_style_set_bg_color(&style_main_bg, COLOR_BG_MAIN);
    lv_style_set_bg_opa(&style_main_bg, LV_OPA_COVER);
    lv_style_set_pad_all(&style_main_bg, 0);
    lv_style_set_border_width(&style_main_bg, 0);
    
    // Style header
    lv_style_init(&style_header_bg);
    lv_style_set_bg_color(&style_header_bg, COLOR_WHITE);
    lv_style_set_bg_opa(&style_header_bg, LV_OPA_COVER);
    lv_style_set_pad_all(&style_header_bg, 8);
    lv_style_set_border_width(&style_header_bg, 0);
    lv_style_set_shadow_width(&style_header_bg, SHADOW_WIDTH);
    lv_style_set_shadow_color(&style_header_bg, COLOR_SHADOW);
    lv_style_set_shadow_opa(&style_header_bg, LV_OPA_10);
    lv_style_set_shadow_spread(&style_header_bg, SHADOW_SPREAD);
    
    // Style sidebar
    lv_style_init(&style_sidebar_bg);
    lv_style_set_bg_color(&style_sidebar_bg, COLOR_BG_PASTEL);
    lv_style_set_bg_opa(&style_sidebar_bg, LV_OPA_COVER);
    lv_style_set_pad_all(&style_sidebar_bg, 12);
    lv_style_set_border_width(&style_sidebar_bg, 0);
    lv_style_set_shadow_width(&style_sidebar_bg, SHADOW_WIDTH);
    lv_style_set_shadow_color(&style_sidebar_bg, COLOR_SHADOW);
    lv_style_set_shadow_opa(&style_sidebar_bg, LV_OPA_10);
    
    // Style contenu principal
    lv_style_init(&style_content_bg);
    lv_style_set_bg_color(&style_content_bg, COLOR_BG_LIGHT);
    lv_style_set_bg_opa(&style_content_bg, LV_OPA_COVER);
    lv_style_set_pad_all(&style_content_bg, 16);
    lv_style_set_border_width(&style_content_bg, 0);
    
    // Style footer
    lv_style_init(&style_footer_bg);
    lv_style_set_bg_color(&style_footer_bg, COLOR_WHITE);
    lv_style_set_bg_opa(&style_footer_bg, LV_OPA_COVER);
    lv_style_set_pad_hor(&style_footer_bg, 12);
    lv_style_set_pad_ver(&style_footer_bg, 4);
    lv_style_set_border_width(&style_footer_bg, 0);
    lv_style_set_shadow_width(&style_footer_bg, SHADOW_WIDTH);
    lv_style_set_shadow_color(&style_footer_bg, COLOR_SHADOW);
    lv_style_set_shadow_opa(&style_footer_bg, LV_OPA_10);
    lv_style_set_shadow_spread(&style_footer_bg, -SHADOW_SPREAD);
}

/**
 * @brief Initialise les styles des boutons
 */
static void init_button_styles(void)
{
    // Bouton principal (bleu)
    lv_style_init(&style_btn_primary);
    lv_style_set_bg_color(&style_btn_primary, COLOR_ACCENT_BLUE);
    lv_style_set_bg_opa(&style_btn_primary, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn_primary, COLOR_WHITE);
    lv_style_set_radius(&style_btn_primary, RADIUS_MEDIUM);
    lv_style_set_pad_hor(&style_btn_primary, 20);
    lv_style_set_pad_ver(&style_btn_primary, 12);
    lv_style_set_border_width(&style_btn_primary, 0);
    lv_style_set_shadow_width(&style_btn_primary, SHADOW_WIDTH);
    lv_style_set_shadow_color(&style_btn_primary, COLOR_ACCENT_BLUE);
    lv_style_set_shadow_opa(&style_btn_primary, LV_OPA_30);
    
    // Bouton secondaire (vert)
    lv_style_init(&style_btn_secondary);
    lv_style_set_bg_color(&style_btn_secondary, COLOR_ACCENT_GREEN);
    lv_style_set_bg_opa(&style_btn_secondary, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn_secondary, COLOR_WHITE);
    lv_style_set_radius(&style_btn_secondary, RADIUS_MEDIUM);
    lv_style_set_pad_hor(&style_btn_secondary, 20);
    lv_style_set_pad_ver(&style_btn_secondary, 12);
    lv_style_set_border_width(&style_btn_secondary, 0);
    lv_style_set_shadow_width(&style_btn_secondary, SHADOW_WIDTH);
    lv_style_set_shadow_color(&style_btn_secondary, COLOR_ACCENT_GREEN);
    lv_style_set_shadow_opa(&style_btn_secondary, LV_OPA_30);
    
    // Bouton danger (orange)
    lv_style_init(&style_btn_danger);
    lv_style_set_bg_color(&style_btn_danger, COLOR_ACCENT_ORANGE);
    lv_style_set_bg_opa(&style_btn_danger, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn_danger, COLOR_WHITE);
    lv_style_set_radius(&style_btn_danger, RADIUS_MEDIUM);
    lv_style_set_pad_hor(&style_btn_danger, 20);
    lv_style_set_pad_ver(&style_btn_danger, 12);
    lv_style_set_border_width(&style_btn_danger, 0);
    lv_style_set_shadow_width(&style_btn_danger, SHADOW_WIDTH);
    lv_style_set_shadow_color(&style_btn_danger, COLOR_ACCENT_ORANGE);
    lv_style_set_shadow_opa(&style_btn_danger, LV_OPA_30);
}

/**
 * @brief Initialise les styles des cartes
 */
static void init_card_styles(void)
{
    lv_style_init(&style_card);
    lv_style_set_bg_color(&style_card, COLOR_WHITE);
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_radius(&style_card, RADIUS_MEDIUM);
    lv_style_set_pad_all(&style_card, 16);
    lv_style_set_border_width(&style_card, 0);
    lv_style_set_shadow_width(&style_card, SHADOW_WIDTH);
    lv_style_set_shadow_color(&style_card, COLOR_SHADOW);
    lv_style_set_shadow_opa(&style_card, LV_OPA_10);
    lv_style_set_shadow_spread(&style_card, SHADOW_SPREAD);
}

/**
 * @brief Initialise les styles de texte
 */
static void init_text_styles(void)
{
    // Titre principal
    lv_style_init(&style_text_title);
    lv_style_set_text_color(&style_text_title, COLOR_TEXT_DARK);
    lv_style_set_text_font(&style_text_title, &lv_font_montserrat_24);
    
    // Sous-titre
    lv_style_init(&style_text_subtitle);
    lv_style_set_text_color(&style_text_subtitle, COLOR_TEXT_MEDIUM);
    lv_style_set_text_font(&style_text_subtitle, &lv_font_montserrat_18);
    
    // Texte corps
    lv_style_init(&style_text_body);
    lv_style_set_text_color(&style_text_body, COLOR_TEXT_DARK);
    lv_style_set_text_font(&style_text_body, &lv_font_montserrat_14);
    
    // Petit texte
    lv_style_init(&style_text_small);
    lv_style_set_text_color(&style_text_small, COLOR_TEXT_LIGHT);
    lv_style_set_text_font(&style_text_small, &lv_font_montserrat_12);
}

/**
 * @brief Initialise les styles de navigation
 */
static void init_navigation_styles(void)
{
    // Item de navigation normal
    lv_style_init(&style_nav_normal);
    lv_style_set_bg_opa(&style_nav_normal, LV_OPA_TRANSP);
    lv_style_set_text_color(&style_nav_normal, COLOR_TEXT_MEDIUM);
    lv_style_set_pad_hor(&style_nav_normal, 16);
    lv_style_set_pad_ver(&style_nav_normal, 12);
    lv_style_set_radius(&style_nav_normal, RADIUS_SMALL);
    lv_style_set_border_width(&style_nav_normal, 0);
    
    // Item de navigation actif
    lv_style_init(&style_nav_active);
    lv_style_set_bg_color(&style_nav_active, COLOR_ACCENT_BLUE);
    lv_style_set_bg_opa(&style_nav_active, LV_OPA_20);
    lv_style_set_text_color(&style_nav_active, COLOR_ACCENT_BLUE);
    lv_style_set_pad_hor(&style_nav_active, 16);
    lv_style_set_pad_ver(&style_nav_active, 12);
    lv_style_set_radius(&style_nav_active, RADIUS_SMALL);
    lv_style_set_border_width(&style_nav_active, 2);
    lv_style_set_border_color(&style_nav_active, COLOR_ACCENT_BLUE);
    lv_style_set_border_opa(&style_nav_active, LV_OPA_30);
    
    // Item de navigation survol
    lv_style_init(&style_nav_hover);
    lv_style_set_bg_color(&style_nav_hover, COLOR_ACCENT_BLUE);
    lv_style_set_bg_opa(&style_nav_hover, LV_OPA_10);
    lv_style_set_text_color(&style_nav_hover, COLOR_ACCENT_BLUE);
    lv_style_set_pad_hor(&style_nav_hover, 16);
    lv_style_set_pad_ver(&style_nav_hover, 12);
    lv_style_set_radius(&style_nav_hover, RADIUS_SMALL);
    lv_style_set_border_width(&style_nav_hover, 0);
}

/**
 * @brief Initialise les styles d'indicateurs et d'états
 */
static void init_indicator_styles(void)
{
    // Indicateur de notification pour la sidebar
    lv_style_init(&style_nav_indicator);
    lv_style_set_bg_color(&style_nav_indicator, COLOR_ACCENT_ORANGE);
    lv_style_set_bg_opa(&style_nav_indicator, LV_OPA_COVER);
    lv_style_set_radius(&style_nav_indicator, 4);
    lv_style_set_border_width(&style_nav_indicator, 0);

    // Indicateur de statut OK
    lv_style_init(&style_status_ok);
    lv_style_set_bg_color(&style_status_ok, COLOR_ACCENT_GREEN);
    lv_style_set_bg_opa(&style_status_ok, LV_OPA_COVER);
    lv_style_set_radius(&style_status_ok, 12);
    lv_style_set_border_width(&style_status_ok, 0);

    // Niveaux d'alerte
    lv_style_init(&style_alert_level_critical);
    lv_style_set_bg_color(&style_alert_level_critical, COLOR_ACCENT_ORANGE);
    lv_style_set_bg_opa(&style_alert_level_critical, LV_OPA_COVER);
    lv_style_set_radius(&style_alert_level_critical, 3);
    lv_style_set_border_width(&style_alert_level_critical, 0);

    lv_style_init(&style_alert_level_warning);
    lv_style_set_bg_color(&style_alert_level_warning, COLOR_ACCENT_ORANGE);
    lv_style_set_bg_opa(&style_alert_level_warning, LV_OPA_COVER);
    lv_style_set_radius(&style_alert_level_warning, 3);
    lv_style_set_border_width(&style_alert_level_warning, 0);

    lv_style_init(&style_alert_level_info);
    lv_style_set_bg_color(&style_alert_level_info, COLOR_ACCENT_BLUE);
    lv_style_set_bg_opa(&style_alert_level_info, LV_OPA_COVER);
    lv_style_set_radius(&style_alert_level_info, 3);
    lv_style_set_border_width(&style_alert_level_info, 0);
}

esp_err_t ui_styles_init(void)
{
    ESP_LOGI(TAG, "Initialisation des styles NovaReptileElevage");
    
    init_container_styles();
    init_button_styles();
    init_card_styles();
    init_text_styles();
    init_navigation_styles();
    init_indicator_styles();
    
    ESP_LOGI(TAG, "Styles initialisés avec succès");
    return ESP_OK;
}

void ui_styles_deinit(void)
{
    // Libération des ressources styles si nécessaire
    ESP_LOGI(TAG, "Styles libérés");
}

// Getters pour les styles principaux
lv_style_t* ui_styles_get_main_bg(void) { return &style_main_bg; }
lv_style_t* ui_styles_get_header_bg(void) { return &style_header_bg; }
lv_style_t* ui_styles_get_sidebar_bg(void) { return &style_sidebar_bg; }
lv_style_t* ui_styles_get_content_bg(void) { return &style_content_bg; }
lv_style_t* ui_styles_get_footer_bg(void) { return &style_footer_bg; }

// Getters pour les styles de composants
lv_style_t* ui_styles_get_button_primary(void) { return &style_btn_primary; }
lv_style_t* ui_styles_get_button_secondary(void) { return &style_btn_secondary; }
lv_style_t* ui_styles_get_button_danger(void) { return &style_btn_danger; }
lv_style_t* ui_styles_get_card_style(void) { return &style_card; }
lv_style_t* ui_styles_get_text_title(void) { return &style_text_title; }
lv_style_t* ui_styles_get_text_subtitle(void) { return &style_text_subtitle; }
lv_style_t* ui_styles_get_text_body(void) { return &style_text_body; }
lv_style_t* ui_styles_get_text_small(void) { return &style_text_small; }

// Getters pour les styles de navigation
lv_style_t* ui_styles_get_nav_item_normal(void) { return &style_nav_normal; }
lv_style_t* ui_styles_get_nav_item_active(void) { return &style_nav_active; }
lv_style_t* ui_styles_get_nav_item_hover(void) { return &style_nav_hover; }

// Getters pour les styles d'indicateurs et d'états
lv_style_t* ui_styles_get_nav_indicator(void) { return &style_nav_indicator; }
lv_style_t* ui_styles_get_status_ok(void) { return &style_status_ok; }
lv_style_t* ui_styles_get_alert_level_critical(void) { return &style_alert_level_critical; }
lv_style_t* ui_styles_get_alert_level_warning(void) { return &style_alert_level_warning; }
lv_style_t* ui_styles_get_alert_level_info(void) { return &style_alert_level_info; }