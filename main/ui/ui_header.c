/**
 * @file ui_header.c
 * @brief Implémentation de la barre de titre
 * @author NovaReptileElevage Team
 */

#include "ui_header.h"
#include "ui_styles.h"
#include "esp_log.h"

static const char *TAG = "UI_Header";

// Composants du header
static lv_obj_t *header_logo;
static lv_obj_t *header_title;
static lv_obj_t *header_connection_indicator;
static lv_obj_t *header_time;
static lv_obj_t *header_profile_btn;
static lv_obj_t *header_settings_btn;

static lv_style_t style_connected;
static lv_style_t style_disconnected;

static void init_header_styles(void)
{
    lv_style_init(&style_connected);
    lv_style_set_bg_color(&style_connected, COLOR_ACCENT_GREEN);
    lv_style_set_bg_opa(&style_connected, LV_OPA_COVER);
    lv_style_set_radius(&style_connected, 6);
    lv_style_set_border_width(&style_connected, 0);

    lv_style_init(&style_disconnected);
    lv_style_set_bg_color(&style_disconnected, COLOR_ACCENT_ORANGE);
    lv_style_set_bg_opa(&style_disconnected, LV_OPA_COVER);
    lv_style_set_radius(&style_disconnected, 6);
    lv_style_set_border_width(&style_disconnected, 0);
}

/**
 * @brief Callback pour le bouton profil
 * @param e Événement LVGL
 */
static void profile_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGI(TAG, "Bouton profil cliqué");
        // TODO: Ouvrir menu profil
    }
}

/**
 * @brief Callback pour le bouton paramètres
 * @param e Événement LVGL
 */
static void settings_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGI(TAG, "Bouton paramètres cliqué");
        // TODO: Ouvrir paramètres rapides
    }
}

/**
 * @brief Crée le logo de l'application
 * @param parent Conteneur parent
 * @return lv_obj_t* Widget créé
 */
static lv_obj_t* create_logo(lv_obj_t *parent)
{
    lv_obj_t *logo = lv_label_create(parent);
    lv_label_set_text(logo, "🦎"); // Icône emoji temporaire
    lv_obj_add_style(logo, ui_styles_get_text_title(), 0);
    lv_obj_set_pos(logo, 10, 10);
    
    return logo;
}

/**
 * @brief Crée le titre principal
 * @param parent Conteneur parent
 * @return lv_obj_t* Widget créé
 */
static lv_obj_t* create_title(lv_obj_t *parent)
{
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "NovaReptileElevage");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);
    lv_obj_set_pos(title, 60, 15);
    
    return title;
}

/**
 * @brief Crée l'indicateur de connexion
 * @param parent Conteneur parent
 * @return lv_obj_t* Widget créé
 */
static lv_obj_t* create_connection_indicator(lv_obj_t *parent)
{
    lv_obj_t *indicator = lv_obj_create(parent);
    lv_obj_set_size(indicator, 12, 12);
    lv_obj_set_pos(indicator, 680, 24);

    lv_obj_add_style(indicator, &style_connected, 0);

    return indicator;
}

/**
 * @brief Crée l'affichage de l'heure
 * @param parent Conteneur parent
 * @return lv_obj_t* Widget créé
 */
static lv_obj_t* create_time_display(lv_obj_t *parent)
{
    lv_obj_t *time_label = lv_label_create(parent);
    lv_label_set_text(time_label, "12:34");
    lv_obj_add_style(time_label, ui_styles_get_text_body(), 0);
    lv_obj_set_pos(time_label, 700, 20);
    
    return time_label;
}

/**
 * @brief Crée le bouton profil
 * @param parent Conteneur parent
 * @return lv_obj_t* Widget créé
 */
static lv_obj_t* create_profile_button(lv_obj_t *parent)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 40, 40);
    lv_obj_set_pos(btn, 720, 10);
    lv_obj_add_style(btn, ui_styles_get_button_secondary(), 0);
    
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "👤"); // Icône profil temporaire
    lv_obj_center(label);
    
    lv_obj_add_event_cb(btn, profile_btn_event_cb, LV_EVENT_CLICKED, NULL);
    
    return btn;
}

/**
 * @brief Crée le bouton paramètres
 * @param parent Conteneur parent
 * @return lv_obj_t* Widget créé
 */
static lv_obj_t* create_settings_button(lv_obj_t *parent)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 40, 40);
    lv_obj_set_pos(btn, 770, 10);
    lv_obj_add_style(btn, ui_styles_get_button_primary(), 0);
    
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "⚙️"); // Icône paramètres temporaire
    lv_obj_center(label);
    
    lv_obj_add_event_cb(btn, settings_btn_event_cb, LV_EVENT_CLICKED, NULL);
    
    return btn;
}

esp_err_t ui_header_init(lv_obj_t *parent)
{
    if (!parent) {
        ESP_LOGE(TAG, "Conteneur parent invalide");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Initialisation du header");

    init_header_styles();

    // Création des composants du header
    header_logo = create_logo(parent);
    header_title = create_title(parent);
    header_connection_indicator = create_connection_indicator(parent);
    header_time = create_time_display(parent);
    header_profile_btn = create_profile_button(parent);
    header_settings_btn = create_settings_button(parent);
    
    // Vérification de la création
    if (!header_logo || !header_title || !header_connection_indicator || 
        !header_time || !header_profile_btn || !header_settings_btn) {
        ESP_LOGE(TAG, "Erreur création composants header");
        return ESP_ERR_NO_MEM;
    }
    
    ESP_LOGI(TAG, "Header initialisé avec succès");
    return ESP_OK;
}

void ui_header_set_title(const char *title)
{
    if (header_title && title) {
        lv_label_set_text(header_title, title);
        ESP_LOGI(TAG, "Titre mis à jour: %s", title);
    }
}

void ui_header_set_connection_status(bool connected)
{
    if (header_connection_indicator) {
        lv_obj_add_style(header_connection_indicator,
                         connected ? &style_connected : &style_disconnected,
                         0);
        ESP_LOGI(TAG, "État connexion: %s", connected ? "Connecté" : "Déconnecté");
    }
}

void ui_header_set_time(const char *time_str)
{
    if (header_time && time_str) {
        lv_label_set_text(header_time, time_str);
    }
}
