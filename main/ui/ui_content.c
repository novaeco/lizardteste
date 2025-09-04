/**
 * @file ui_content.c
 * @brief Implémentation de la zone de contenu principal
 * @author NovaReptileElevage Team
 */

#include "ui_content.h"
#include "ui_styles.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "UI_Content";

static lv_obj_t *content_container;
static lv_obj_t *current_screen_container;
static nova_screen_t current_screen = SCREEN_DASHBOARD;

// Prototypes des fonctions de création d'écrans
static lv_obj_t* create_dashboard_screen(lv_obj_t *parent);
static lv_obj_t* create_reptiles_screen(lv_obj_t *parent);
static lv_obj_t* create_terrariums_screen(lv_obj_t *parent);
static lv_obj_t* create_statistics_screen(lv_obj_t *parent);
static lv_obj_t* create_alerts_screen(lv_obj_t *parent);
static lv_obj_t* create_settings_screen(lv_obj_t *parent);

/**
 * @brief Crée une carte d'information (widget réutilisable)
 * @param parent Conteneur parent
 * @param title Titre de la carte
 * @param value Valeur à afficher
 * @param unit Unité de mesure
 * @param x Position X
 * @param y Position Y
 * @param width Largeur
 * @param height Hauteur
 * @return lv_obj_t* Carte créée
 */
static lv_obj_t* create_info_card(lv_obj_t *parent, const char *title, 
                                  const char *value, const char *unit,
                                  int x, int y, int width, int height)
{
    // Conteneur de la carte
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_remove_style_all(card);
    lv_obj_add_style(card, ui_styles_get_card_style(), 0);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, width, height);
    
    // Titre de la carte
    lv_obj_t *title_label = lv_label_create(card);
    lv_label_set_text(title_label, title);
    lv_obj_add_style(title_label, ui_styles_get_text_small(), 0);
    lv_obj_set_pos(title_label, 12, 8);
    
    // Valeur principale
    lv_obj_t *value_label = lv_label_create(card);
    char value_text[64];
    snprintf(value_text, sizeof(value_text), "%s %s", value, unit);
    lv_label_set_text(value_label, value_text);
    lv_obj_add_style(value_label, ui_styles_get_text_title(), 0);
    lv_obj_set_pos(value_label, 12, 30);
    
    return card;
}

/**
 * @brief Crée l'écran tableau de bord principal
 * @param parent Conteneur parent
 * @return lv_obj_t* Écran créé
 */
static lv_obj_t* create_dashboard_screen(lv_obj_t *parent)
{
    lv_obj_t *screen = lv_obj_create(parent);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    
    // Titre de l'écran
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Tableau de Bord");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);
    lv_obj_set_pos(title, 0, 0);
    
    // Cartes d'informations - première rangée
    create_info_card(screen, "Température Moyenne", "24.5", "°C", 0, 40, 140, 80);
    create_info_card(screen, "Humidité Moyenne", "65", "%", 150, 40, 140, 80);
    create_info_card(screen, "Reptiles Actifs", "8", "/12", 300, 40, 140, 80);
    create_info_card(screen, "Alertes Actives", "2", "", 450, 40, 140, 80);
    
    // Cartes d'informations - deuxième rangée
    create_info_card(screen, "Terrariums", "6", "unités", 0, 130, 140, 80);
    create_info_card(screen, "Éclairage", "ON", "", 150, 130, 140, 80);
    create_info_card(screen, "Chauffage", "AUTO", "", 300, 130, 140, 80);
    create_info_card(screen, "Ventilation", "75", "%", 450, 130, 140, 80);
    
    // Zone pour graphique ou informations détaillées
    lv_obj_t *info_panel = lv_obj_create(screen);
    lv_obj_remove_style_all(info_panel);
    lv_obj_add_style(info_panel, ui_styles_get_card_style(), 0);
    lv_obj_set_pos(info_panel, 0, 220);
    lv_obj_set_size(info_panel, 590, 160);
    
    lv_obj_t *panel_title = lv_label_create(info_panel);
    lv_label_set_text(panel_title, "Activité Récente");
    lv_obj_add_style(panel_title, ui_styles_get_text_subtitle(), 0);
    lv_obj_set_pos(panel_title, 16, 8);
    
    lv_obj_t *panel_content = lv_label_create(info_panel);
    lv_label_set_text(panel_content, 
        "• 14:30 - Température terrarium #3: 26.2°C\n"
        "• 14:25 - Alimentation Python Royal - OK\n"
        "• 14:20 - Nettoyage terrarium #1 - Terminé\n"
        "• 14:15 - Humidité terrarium #2: 68%");
    lv_obj_add_style(panel_content, ui_styles_get_text_body(), 0);
    lv_obj_set_pos(panel_content, 16, 35);
    
    ESP_LOGI(TAG, "Écran tableau de bord créé");
    return screen;
}

/**
 * @brief Crée l'écran de gestion des reptiles
 * @param parent Conteneur parent
 * @return lv_obj_t* Écran créé
 */
static lv_obj_t* create_reptiles_screen(lv_obj_t *parent)
{
    lv_obj_t *screen = lv_obj_create(parent);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    
    // Titre de l'écran
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Gestion des Reptiles");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);
    lv_obj_set_pos(title, 0, 0);
    
    // Bouton d'ajout
    lv_obj_t *add_btn = lv_btn_create(screen);
    lv_obj_add_style(add_btn, ui_styles_get_button_primary(), 0);
    lv_obj_set_pos(add_btn, 450, 0);
    lv_obj_set_size(add_btn, 140, 35);
    
    lv_obj_t *add_label = lv_label_create(add_btn);
    lv_label_set_text(add_label, "+ Nouveau Reptile");
    lv_obj_center(add_label);
    
    // Liste des reptiles (simulation)
    const char *reptiles_data[] = {
        "Python Royal - Mâle - 3 ans",
        "Gecko Léopard - Femelle - 2 ans", 
        "Pogona Vitticeps - Mâle - 1 an",
        "Boa Constrictor - Femelle - 5 ans"
    };
    
    for (int i = 0; i < 4; i++) {
        lv_obj_t *reptile_card = lv_obj_create(screen);
        lv_obj_remove_style_all(reptile_card);
        lv_obj_add_style(reptile_card, ui_styles_get_card_style(), 0);
        lv_obj_set_pos(reptile_card, 0, 50 + i * 80);
        lv_obj_set_size(reptile_card, 590, 70);
        
        lv_obj_t *reptile_name = lv_label_create(reptile_card);
        lv_label_set_text(reptile_name, reptiles_data[i]);
        lv_obj_add_style(reptile_name, ui_styles_get_text_subtitle(), 0);
        lv_obj_set_pos(reptile_name, 16, 10);
        
        lv_obj_t *reptile_status = lv_label_create(reptile_card);
        lv_label_set_text(reptile_status, "État: Actif • Dernière alimentation: 2j");
        lv_obj_add_style(reptile_status, ui_styles_get_text_small(), 0);
        lv_obj_set_pos(reptile_status, 16, 35);
        
        // Boutons d'action
        lv_obj_t *edit_btn = lv_btn_create(reptile_card);
        lv_obj_add_style(edit_btn, ui_styles_get_button_secondary(), 0);
        lv_obj_set_pos(edit_btn, 450, 15);
        lv_obj_set_size(edit_btn, 60, 30);
        
        lv_obj_t *edit_label = lv_label_create(edit_btn);
        lv_label_set_text(edit_label, "Éditer");
        lv_obj_center(edit_label);
        
        lv_obj_t *view_btn = lv_btn_create(reptile_card);
        lv_obj_add_style(view_btn, ui_styles_get_button_primary(), 0);
        lv_obj_set_pos(view_btn, 520, 15);
        lv_obj_set_size(view_btn, 60, 30);
        
        lv_obj_t *view_label = lv_label_create(view_btn);
        lv_label_set_text(view_label, "Voir");
        lv_obj_center(view_label);
    }
    
    ESP_LOGI(TAG, "Écran reptiles créé");
    return screen;
}

/**
 * @brief Crée l'écran de gestion des terrariums
 * @param parent Conteneur parent
 * @return lv_obj_t* Écran créé
 */
static lv_obj_t* create_terrariums_screen(lv_obj_t *parent)
{
    lv_obj_t *screen = lv_obj_create(parent);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Gestion des Terrariums");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);
    lv_obj_set_pos(title, 0, 0);
    
    // Grille de terrariums (2x3)
    for (int i = 0; i < 6; i++) {
        int x = (i % 2) * 295;
        int y = 50 + (i / 2) * 120;
        
        lv_obj_t *terrarium_card = lv_obj_create(screen);
        lv_obj_remove_style_all(terrarium_card);
        lv_obj_add_style(terrarium_card, ui_styles_get_card_style(), 0);
        lv_obj_set_pos(terrarium_card, x, y);
        lv_obj_set_size(terrarium_card, 285, 110);
        
        char title_text[32];
        snprintf(title_text, sizeof(title_text), "Terrarium #%d", i + 1);
        lv_obj_t *ter_title = lv_label_create(terrarium_card);
        lv_label_set_text(ter_title, title_text);
        lv_obj_add_style(ter_title, ui_styles_get_text_subtitle(), 0);
        lv_obj_set_pos(ter_title, 12, 8);
        
        // Données simulées
        char temp_text[64], hum_text[64];
        snprintf(temp_text, sizeof(temp_text), "Temp: %.1f°C", 24.0 + (i * 0.5));
        snprintf(hum_text, sizeof(hum_text), "Hum: %d%%", 60 + (i * 2));
        
        lv_obj_t *temp_label = lv_label_create(terrarium_card);
        lv_label_set_text(temp_label, temp_text);
        lv_obj_add_style(temp_label, ui_styles_get_text_body(), 0);
        lv_obj_set_pos(temp_label, 12, 35);
        
        lv_obj_t *hum_label = lv_label_create(terrarium_card);
        lv_label_set_text(hum_label, hum_text);
        lv_obj_add_style(hum_label, ui_styles_get_text_body(), 0);
        lv_obj_set_pos(hum_label, 12, 55);
        
        // Indicateur d'état
        lv_obj_t *status_indicator = lv_obj_create(terrarium_card);
        lv_obj_set_size(status_indicator, 60, 25);
        lv_obj_set_pos(status_indicator, 210, 35);
        lv_obj_add_style(status_indicator, ui_styles_get_status_ok(), 0);
        
        lv_obj_t *status_text = lv_label_create(status_indicator);
        lv_label_set_text(status_text, "OK");
        lv_obj_add_style(status_text, ui_styles_get_text_small(), 0);
        lv_obj_center(status_text);
    }
    
    ESP_LOGI(TAG, "Écran terrariums créé");
    return screen;
}

/**
 * @brief Crée l'écran des statistiques
 * @param parent Conteneur parent
 * @return lv_obj_t* Écran créé
 */
static lv_obj_t* create_statistics_screen(lv_obj_t *parent)
{
    lv_obj_t *screen = lv_obj_create(parent);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Statistiques et Graphiques");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);
    lv_obj_set_pos(title, 0, 0);
    
    // Placeholder pour un graphique (nécessiterait l'activation de LV_USE_CHART)
    lv_obj_t *chart_placeholder = lv_obj_create(screen);
    lv_obj_remove_style_all(chart_placeholder);
    lv_obj_add_style(chart_placeholder, ui_styles_get_card_style(), 0);
    lv_obj_set_pos(chart_placeholder, 0, 50);
    lv_obj_set_size(chart_placeholder, 590, 200);
    
    lv_obj_t *chart_title = lv_label_create(chart_placeholder);
    lv_label_set_text(chart_title, "Évolution Température (24h)");
    lv_obj_add_style(chart_title, ui_styles_get_text_subtitle(), 0);
    lv_obj_set_pos(chart_title, 16, 8);
    
    lv_obj_t *chart_info = lv_label_create(chart_placeholder);
    lv_label_set_text(chart_info, "Graphique des températures moyennes\npar terrarium sur les dernières 24h");
    lv_obj_add_style(chart_info, ui_styles_get_text_body(), 0);
    lv_obj_set_pos(chart_info, 16, 40);
    
    // Statistiques résumées
    create_info_card(screen, "Temp. Min", "22.1", "°C", 0, 270, 140, 80);
    create_info_card(screen, "Temp. Max", "27.8", "°C", 150, 270, 140, 80);
    create_info_card(screen, "Hum. Moyenne", "66.5", "%", 300, 270, 140, 80);
    create_info_card(screen, "Uptime", "99.2", "%", 450, 270, 140, 80);
    
    ESP_LOGI(TAG, "Écran statistiques créé");
    return screen;
}

/**
 * @brief Crée l'écran des alertes
 * @param parent Conteneur parent
 * @return lv_obj_t* Écran créé
 */
static lv_obj_t* create_alerts_screen(lv_obj_t *parent)
{
    lv_obj_t *screen = lv_obj_create(parent);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Alertes et Notifications");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);
    lv_obj_set_pos(title, 0, 0);
    
    // Alertes actives
    const char *alerts_data[][3] = {
        {"CRITIQUE", "Température terrarium #2 élevée", "28.5°C (Max: 26°C)"},
        {"ATTENTION", "Humidité terrarium #4 faible", "45% (Min: 50%)"},
        {"INFO", "Maintenance programmée demain", "Nettoyage système filtration"}
    };
    
    for (int i = 0; i < 3; i++) {
        lv_obj_t *alert_card = lv_obj_create(screen);
        lv_obj_remove_style_all(alert_card);
        lv_obj_add_style(alert_card, ui_styles_get_card_style(), 0);
        lv_obj_set_pos(alert_card, 0, 50 + i * 90);
        lv_obj_set_size(alert_card, 590, 80);
        
        // Indicateur de niveau
        lv_obj_t *level_indicator = lv_obj_create(alert_card);
        lv_obj_set_size(level_indicator, 6, 60);
        lv_obj_set_pos(level_indicator, 8, 10);
        
        switch (i) {
            case 0:
                lv_obj_add_style(level_indicator, ui_styles_get_alert_level_critical(), 0);
                break;
            case 1:
                lv_obj_add_style(level_indicator, ui_styles_get_alert_level_warning(), 0);
                break;
            default:
                lv_obj_add_style(level_indicator, ui_styles_get_alert_level_info(), 0);
                break;
        }
        
        // Niveau d'alerte
        lv_obj_t *level_label = lv_label_create(alert_card);
        lv_label_set_text(level_label, alerts_data[i][0]);
        lv_obj_add_style(level_label, ui_styles_get_text_small(), 0);
        lv_obj_set_pos(level_label, 25, 8);
        
        // Titre de l'alerte
        lv_obj_t *alert_title = lv_label_create(alert_card);
        lv_label_set_text(alert_title, alerts_data[i][1]);
        lv_obj_add_style(alert_title, ui_styles_get_text_subtitle(), 0);
        lv_obj_set_pos(alert_title, 25, 25);
        
        // Détails
        lv_obj_t *alert_details = lv_label_create(alert_card);
        lv_label_set_text(alert_details, alerts_data[i][2]);
        lv_obj_add_style(alert_details, ui_styles_get_text_body(), 0);
        lv_obj_set_pos(alert_details, 25, 50);
        
        // Bouton d'action
        lv_obj_t *action_btn = lv_btn_create(alert_card);
        lv_obj_add_style(action_btn, ui_styles_get_button_danger(), 0);
        lv_obj_set_pos(action_btn, 480, 25);
        lv_obj_set_size(action_btn, 90, 30);
        
        lv_obj_t *action_label = lv_label_create(action_btn);
        lv_label_set_text(action_label, "Résoudre");
        lv_obj_center(action_label);
    }
    
    ESP_LOGI(TAG, "Écran alertes créé");
    return screen;
}

/**
 * @brief Crée l'écran des paramètres
 * @param parent Conteneur parent
 * @return lv_obj_t* Écran créé
 */
static lv_obj_t* create_settings_screen(lv_obj_t *parent)
{
    lv_obj_t *screen = lv_obj_create(parent);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Paramètres Système");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);
    lv_obj_set_pos(title, 0, 0);
    
    // Sections de paramètres
    const char *settings_sections[] = {
        "Réseau et Connectivité",
        "Capteurs et Surveillance", 
        "Notifications et Alertes",
        "Maintenance et Sauvegardes"
    };
    
    for (int i = 0; i < 4; i++) {
        lv_obj_t *section_card = lv_obj_create(screen);
        lv_obj_remove_style_all(section_card);
        lv_obj_add_style(section_card, ui_styles_get_card_style(), 0);
        lv_obj_set_pos(section_card, 0, 50 + i * 80);
        lv_obj_set_size(section_card, 590, 70);
        
        lv_obj_t *section_title = lv_label_create(section_card);
        lv_label_set_text(section_title, settings_sections[i]);
        lv_obj_add_style(section_title, ui_styles_get_text_subtitle(), 0);
        lv_obj_set_pos(section_title, 16, 10);
        
        lv_obj_t *section_desc = lv_label_create(section_card);
        lv_label_set_text(section_desc, "Configuration des paramètres de cette section");
        lv_obj_add_style(section_desc, ui_styles_get_text_small(), 0);
        lv_obj_set_pos(section_desc, 16, 35);
        
        lv_obj_t *config_btn = lv_btn_create(section_card);
        lv_obj_add_style(config_btn, ui_styles_get_button_primary(), 0);
        lv_obj_set_pos(config_btn, 480, 20);
        lv_obj_set_size(config_btn, 90, 30);
        
        lv_obj_t *config_label = lv_label_create(config_btn);
        lv_label_set_text(config_label, "Configurer");
        lv_obj_center(config_label);
    }
    
    ESP_LOGI(TAG, "Écran paramètres créé");
    return screen;
}

esp_err_t ui_content_init(lv_obj_t *parent)
{
    if (!parent) {
        ESP_LOGE(TAG, "Conteneur parent invalide");
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Initialisation du contenu principal");
    
    content_container = parent;
    current_screen_container = NULL;
    
    ESP_LOGI(TAG, "Contenu principal initialisé");
    return ESP_OK;
}

esp_err_t ui_content_load_screen(nova_screen_t screen_type)
{
    if (screen_type >= SCREEN_COUNT) {
        ESP_LOGE(TAG, "Type d'écran invalide: %d", screen_type);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Suppression de l'écran précédent
    if (current_screen_container) {
        lv_obj_del(current_screen_container);
        current_screen_container = NULL;
    }
    
    // Création du nouvel écran
    switch (screen_type) {
        case SCREEN_DASHBOARD:
            current_screen_container = create_dashboard_screen(content_container);
            break;
        case SCREEN_REPTILES:
            current_screen_container = create_reptiles_screen(content_container);
            break;
        case SCREEN_TERRARIUMS:
            current_screen_container = create_terrariums_screen(content_container);
            break;
        case SCREEN_STATISTICS:
            current_screen_container = create_statistics_screen(content_container);
            break;
        case SCREEN_ALERTS:
            current_screen_container = create_alerts_screen(content_container);
            break;
        case SCREEN_SETTINGS:
            current_screen_container = create_settings_screen(content_container);
            break;
        default:
            ESP_LOGE(TAG, "Écran non implémenté: %d", screen_type);
            return ESP_ERR_NOT_SUPPORTED;
    }
    
    if (!current_screen_container) {
        ESP_LOGE(TAG, "Erreur création écran %d", screen_type);
        return ESP_ERR_NO_MEM;
    }
    
    current_screen = screen_type;
    ESP_LOGI(TAG, "Écran chargé: %d", screen_type);
    
    return ESP_OK;
}

void ui_content_update_realtime_data(void)
{
    // Mise à jour des données temps réel selon l'écran actuel
    if (current_screen == SCREEN_DASHBOARD || current_screen == SCREEN_STATISTICS) {
        // Mise à jour des valeurs dans les cartes
        ESP_LOGD(TAG, "Mise à jour données temps réel");
    }
}

lv_obj_t* ui_content_get_container(void)
{
    return content_container;
}
