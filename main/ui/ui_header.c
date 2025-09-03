/**
 * @file ui_header.c
 * @brief Implémentation de la barre de titre
 * @author NovaReptileElevage Team
 */

#include "ui_header.h"
#include "ui_styles.h"
#include "ui_icons.h"
#include "esp_log.h"

static const char *TAG = "UI_Header";

// Composants du header
static lv_obj_t *header_logo;
static lv_obj_t *header_title;
static lv_obj_t *header_connection_indicator;
static lv_obj_t *header_time;
static lv_obj_t *header_profile_btn;
static lv_obj_t *header_settings_btn;
static lv_obj_t *profile_menu;
static lv_obj_t *quick_settings_panel;

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
    ESP_LOGD(TAG, "profile_btn_event_cb code=%d", code);
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGI(TAG, "Bouton profil cliqué");
        esp_err_t ret = ui_header_open_profile_menu();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Ouverture menu profil échouée: %s", esp_err_to_name(ret));
        }
    }
}

/**
 * @brief Callback pour le bouton paramètres
 * @param e Événement LVGL
 */
static void settings_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    ESP_LOGD(TAG, "settings_btn_event_cb code=%d", code);
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGI(TAG, "Bouton paramètres cliqué");
        esp_err_t ret = ui_header_open_quick_settings();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Ouverture réglages rapides échouée: %s", esp_err_to_name(ret));
        }
    }
}

/**
 * @brief Callback commun pour la fermeture des boîtes de dialogue
 * @param e Événement LVGL
 */
static void msgbox_event_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    lv_obj_t *btn = lv_event_get_target(e);
    const char *txt = lv_label_get_text(lv_obj_get_child(btn, 0));
    lv_obj_t *box = lv_obj_get_parent(btn);
    ESP_LOGI(TAG, "Bouton message box: %s", txt);
    lv_obj_del_async(box);
    if (box == profile_menu) profile_menu = NULL;
    else if (box == quick_settings_panel) quick_settings_panel = NULL;
}

esp_err_t ui_header_open_profile_menu(void)
{
    if (profile_menu) {
        ESP_LOGW(TAG, "Menu profil déjà ouvert");
        return ESP_ERR_INVALID_STATE;
    }

    profile_menu = lv_msgbox_create(lv_scr_act());
    if (!profile_menu) {
        ESP_LOGE(TAG, "Création menu profil échouée");
        return ESP_ERR_NO_MEM;
    }

    lv_msgbox_add_title(profile_menu, "Profil");
    lv_msgbox_add_text(profile_menu, "Menu profil");
    lv_obj_t *btn = lv_msgbox_add_footer_button(profile_menu, "Fermer");
    lv_obj_add_event_cb(btn, msgbox_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_center(profile_menu);
    ESP_LOGI(TAG, "Menu profil ouvert");
    return ESP_OK;
}

esp_err_t ui_header_open_quick_settings(void)
{
    if (quick_settings_panel) {
        ESP_LOGW(TAG, "Panneau réglages rapides déjà ouvert");
        return ESP_ERR_INVALID_STATE;
    }

    quick_settings_panel = lv_msgbox_create(lv_scr_act());
    if (!quick_settings_panel) {
        ESP_LOGE(TAG, "Création panneau réglages rapides échouée");
        return ESP_ERR_NO_MEM;
    }

    lv_msgbox_add_title(quick_settings_panel, "Réglages rapides");
    lv_msgbox_add_text(quick_settings_panel, "Panneau en développement");
    lv_obj_t *btn = lv_msgbox_add_footer_button(quick_settings_panel, "Fermer");
    lv_obj_add_event_cb(btn, msgbox_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_center(quick_settings_panel);
    ESP_LOGI(TAG, "Panneau réglages rapides ouvert");
    return ESP_OK;
}

/**
 * @brief Crée le logo de l'application
 * @param parent Conteneur parent
 * @return lv_obj_t* Widget créé
 */
static lv_obj_t* create_logo(lv_obj_t *parent)
{
    lv_obj_t *logo = lv_label_create(parent);
    lv_label_set_text(logo, LV_SYMBOL_IMAGE);
    lv_obj_add_style(logo, ui_styles_get_text_title(), 0);
    lv_obj_set_style_margin_right(logo, 10, 0);

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
    lv_obj_set_flex_grow(title, 1);

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
    lv_obj_set_style_margin_right(indicator, 10, 0);

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
    lv_obj_set_style_margin_right(time_label, 10, 0);

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
    lv_obj_set_style_margin_right(btn, 10, 0);
    lv_obj_add_style(btn, ui_styles_get_button_secondary(), 0);

    lv_obj_t *img = lv_img_create(btn);
    lv_img_set_src(img, &ui_img_profile);
    lv_obj_center(img);

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
    lv_obj_add_style(btn, ui_styles_get_button_primary(), 0);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_SETTINGS);
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

    /* Configuration du layout flex pour le conteneur header */
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(parent, 10, 0);
    lv_obj_set_style_pad_gap(parent, 10, 0);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

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
        ESP_LOGI(TAG, "Heure mise à jour: %s", time_str);
    }
}
