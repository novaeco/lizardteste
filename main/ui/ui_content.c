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
 * @param width Largeur
 * @param height Hauteur
 * @return lv_obj_t* Carte créée
 */
static lv_obj_t* create_info_card(lv_obj_t *parent, const char *title,
                                  const char *value, const char *unit,
                                  int width, int height)
{
    // Conteneur de la carte
    lv_obj_t *card = lv_obj_create(parent);
    if (!card) {
        ESP_LOGE(TAG, "Erreur création carte info");
        return NULL;
    }
    lv_obj_remove_style_all(card);
    lv_obj_add_style(card, ui_styles_get_card_style(), 0);
    lv_obj_set_size(card, width, height);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(card, 12, 0);
    lv_obj_set_style_pad_gap(card, 10, 0);

    // Titre de la carte
    lv_obj_t *title_label = lv_label_create(card);
    lv_label_set_text(title_label, title);
    lv_obj_add_style(title_label, ui_styles_get_text_small(), 0);

    // Valeur principale
    lv_obj_t *value_label = lv_label_create(card);
    char value_text[64];
    snprintf(value_text, sizeof(value_text), "%s %s", value, unit);
    lv_label_set_text(value_label, value_text);
    lv_obj_add_style(value_label, ui_styles_get_text_title(), 0);

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
    if (!screen) {
        ESP_LOGE(TAG, "Erreur création écran dashboard");
        return NULL;
    }
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_set_style_pad_gap(screen, 12, 0);

    // Titre de l'écran
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Tableau de Bord");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);

    // Conteneur des cartes d'informations
    lv_obj_t *cards = lv_obj_create(screen);
    if (!cards) {
        ESP_LOGE(TAG, "Erreur création conteneur cartes");
        return NULL;
    }
    lv_obj_remove_style_all(cards);
    lv_obj_set_size(cards, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cards, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_gap(cards, 10, 0);

    if (!create_info_card(cards, "Température Moyenne", "24.5", "°C", 140, 80) ||
        !create_info_card(cards, "Humidité Moyenne", "65", "%", 140, 80) ||
        !create_info_card(cards, "Reptiles Actifs", "8", "/12", 140, 80) ||
        !create_info_card(cards, "Alertes Actives", "2", "", 140, 80) ||
        !create_info_card(cards, "Terrariums", "6", "unités", 140, 80) ||
        !create_info_card(cards, "Éclairage", "ON", "", 140, 80) ||
        !create_info_card(cards, "Chauffage", "AUTO", "", 140, 80) ||
        !create_info_card(cards, "Ventilation", "75", "%", 140, 80)) {
        ESP_LOGE(TAG, "Erreur création cartes info");
        return NULL;
    }

    // Zone pour graphique ou informations détaillées
    lv_obj_t *info_panel = lv_obj_create(screen);
    if (!info_panel) {
        ESP_LOGE(TAG, "Erreur création panneau info dashboard");
        return NULL;
    }
    lv_obj_remove_style_all(info_panel);
    lv_obj_add_style(info_panel, ui_styles_get_card_style(), 0);
    lv_obj_set_size(info_panel, lv_pct(100), 160);
    lv_obj_set_flex_flow(info_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(info_panel, 16, 0);
    lv_obj_set_style_pad_gap(info_panel, 8, 0);

    lv_obj_t *panel_title = lv_label_create(info_panel);
    lv_label_set_text(panel_title, "Activité Récente");
    lv_obj_add_style(panel_title, ui_styles_get_text_subtitle(), 0);

    lv_obj_t *panel_content = lv_label_create(info_panel);
    lv_label_set_text(panel_content,
        "• 14:30 - Température terrarium #3: 26.2°C\n"
        "• 14:25 - Alimentation Python Royal - OK\n"
        "• 14:20 - Nettoyage terrarium #1 - Terminé\n"
        "• 14:15 - Humidité terrarium #2: 68%");
    lv_obj_add_style(panel_content, ui_styles_get_text_body(), 0);

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
    if (!screen) {
        ESP_LOGE(TAG, "Erreur création écran reptiles");
        return NULL;
    }
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(screen, 12, 0);

    // En-tête avec titre et bouton d'ajout
    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_remove_style_all(header);
    lv_obj_set_size(header, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_gap(header, 10, 0);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "Gestion des Reptiles");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);

    lv_obj_t *add_btn = lv_btn_create(header);
    lv_obj_add_style(add_btn, ui_styles_get_button_primary(), 0);
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

    lv_obj_t *list = lv_obj_create(screen);
    lv_obj_remove_style_all(list);
    lv_obj_set_size(list, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(list, 10, 0);

    for (int i = 0; i < 4; i++) {
        lv_obj_t *reptile_card = lv_obj_create(list);
        if (!reptile_card) {
            ESP_LOGE(TAG, "Erreur création carte reptile %d", i);
            return NULL;
        }
        lv_obj_remove_style_all(reptile_card);
        lv_obj_add_style(reptile_card, ui_styles_get_card_style(), 0);
        lv_obj_set_size(reptile_card, lv_pct(100), LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(reptile_card, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_all(reptile_card, 16, 0);
        lv_obj_set_style_pad_gap(reptile_card, 10, 0);

        lv_obj_t *text_cont = lv_obj_create(reptile_card);
        lv_obj_remove_style_all(text_cont);
        lv_obj_set_flex_flow(text_cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_grow(text_cont, 1);

        lv_obj_t *reptile_name = lv_label_create(text_cont);
        lv_label_set_text(reptile_name, reptiles_data[i]);
        lv_obj_add_style(reptile_name, ui_styles_get_text_subtitle(), 0);

        lv_obj_t *reptile_status = lv_label_create(text_cont);
        lv_label_set_text(reptile_status, "État: Actif • Dernière alimentation: 2j");
        lv_obj_add_style(reptile_status, ui_styles_get_text_small(), 0);

        lv_obj_t *actions = lv_obj_create(reptile_card);
        lv_obj_remove_style_all(actions);
        lv_obj_set_flex_flow(actions, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_gap(actions, 10, 0);

        lv_obj_t *edit_btn = lv_btn_create(actions);
        lv_obj_add_style(edit_btn, ui_styles_get_button_secondary(), 0);
        lv_obj_set_size(edit_btn, 60, 30);
        lv_obj_t *edit_label = lv_label_create(edit_btn);
        lv_label_set_text(edit_label, "Éditer");
        lv_obj_center(edit_label);

        lv_obj_t *view_btn = lv_btn_create(actions);
        lv_obj_add_style(view_btn, ui_styles_get_button_primary(), 0);
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
    if (!screen) {
        ESP_LOGE(TAG, "Erreur création écran terrariums");
        return NULL;
    }
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(screen, 12, 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Gestion des Terrariums");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);

    // Conteneur grille 2x3
    lv_obj_t *grid = lv_obj_create(screen);
    lv_obj_remove_style_all(grid);
    lv_obj_set_size(grid, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);
    lv_obj_set_style_pad_column(grid, 10, 0);
    lv_obj_set_style_pad_row(grid, 10, 0);

    for (int i = 0; i < 6; i++) {
        lv_obj_t *terrarium_card = lv_obj_create(grid);
        if (!terrarium_card) {
            ESP_LOGE(TAG, "Erreur création carte terrarium %d", i);
            return NULL;
        }
        lv_obj_remove_style_all(terrarium_card);
        lv_obj_add_style(terrarium_card, ui_styles_get_card_style(), 0);
        lv_obj_set_size(terrarium_card, LV_PCT(100), 110);
        lv_obj_set_grid_cell(terrarium_card, LV_GRID_ALIGN_STRETCH, i % 2, 1,
                              LV_GRID_ALIGN_START, i / 2, 1);
        lv_obj_set_flex_flow(terrarium_card, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(terrarium_card, 12, 0);
        lv_obj_set_style_pad_gap(terrarium_card, 8, 0);

        char title_text[32];
        snprintf(title_text, sizeof(title_text), "Terrarium #%d", i + 1);
        lv_obj_t *ter_title = lv_label_create(terrarium_card);
        lv_label_set_text(ter_title, title_text);
        lv_obj_add_style(ter_title, ui_styles_get_text_subtitle(), 0);

        // Données simulées
        char temp_text[64], hum_text[64];
        snprintf(temp_text, sizeof(temp_text), "Temp: %.1f°C", 24.0 + (i * 0.5));
        snprintf(hum_text, sizeof(hum_text), "Hum: %d%%", 60 + (i * 2));

        lv_obj_t *data_row = lv_obj_create(terrarium_card);
        lv_obj_remove_style_all(data_row);
        lv_obj_set_flex_flow(data_row, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_gap(data_row, 10, 0);

        lv_obj_t *temp_label = lv_label_create(data_row);
        lv_label_set_text(temp_label, temp_text);
        lv_obj_add_style(temp_label, ui_styles_get_text_body(), 0);

        lv_obj_t *hum_label = lv_label_create(data_row);
        lv_label_set_text(hum_label, hum_text);
        lv_obj_add_style(hum_label, ui_styles_get_text_body(), 0);

        lv_obj_t *status_indicator = lv_obj_create(data_row);
        if (!status_indicator) {
            ESP_LOGE(TAG, "Erreur création indicateur terrarium %d", i);
            return NULL;
        }
        lv_obj_set_size(status_indicator, 60, 25);
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
    if (!screen) {
        ESP_LOGE(TAG, "Erreur création écran statistiques");
        return NULL;
    }
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(screen, 12, 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Statistiques et Graphiques");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);

    // Placeholder pour un graphique (nécessiterait l'activation de LV_USE_CHART)
    lv_obj_t *chart_placeholder = lv_obj_create(screen);
    if (!chart_placeholder) {
        ESP_LOGE(TAG, "Erreur création placeholder graphique");
        return NULL;
    }
    lv_obj_remove_style_all(chart_placeholder);
    lv_obj_add_style(chart_placeholder, ui_styles_get_card_style(), 0);
    lv_obj_set_size(chart_placeholder, lv_pct(100), 200);
    lv_obj_set_flex_flow(chart_placeholder, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(chart_placeholder, 16, 0);
    lv_obj_set_style_pad_gap(chart_placeholder, 8, 0);

    lv_obj_t *chart_title = lv_label_create(chart_placeholder);
    lv_label_set_text(chart_title, "Évolution Température (24h)");
    lv_obj_add_style(chart_title, ui_styles_get_text_subtitle(), 0);

    lv_obj_t *chart_info = lv_label_create(chart_placeholder);
    lv_label_set_text(chart_info, "Graphique des températures moyennes\npar terrarium sur les dernières 24h");
    lv_obj_add_style(chart_info, ui_styles_get_text_body(), 0);

    // Statistiques résumées
    lv_obj_t *cards = lv_obj_create(screen);
    lv_obj_remove_style_all(cards);
    lv_obj_set_size(cards, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cards, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_gap(cards, 10, 0);

    if (!create_info_card(cards, "Temp. Min", "22.1", "°C", 140, 80) ||
        !create_info_card(cards, "Temp. Max", "27.8", "°C", 140, 80) ||
        !create_info_card(cards, "Hum. Moyenne", "66.5", "%", 140, 80) ||
        !create_info_card(cards, "Uptime", "99.2", "%", 140, 80)) {
        ESP_LOGE(TAG, "Erreur création cartes statistiques");
        return NULL;
    }

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
    if (!screen) {
        ESP_LOGE(TAG, "Erreur création écran alertes");
        return NULL;
    }
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(screen, 12, 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Alertes et Notifications");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);

    // Alertes actives
    const char *alerts_data[][3] = {
        {"CRITIQUE", "Température terrarium #2 élevée", "28.5°C (Max: 26°C)"},
        {"ATTENTION", "Humidité terrarium #4 faible", "45% (Min: 50%)"},
        {"INFO", "Maintenance programmée demain", "Nettoyage système filtration"}
    };

    lv_obj_t *list = lv_obj_create(screen);
    lv_obj_remove_style_all(list);
    lv_obj_set_size(list, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(list, 10, 0);

    for (int i = 0; i < 3; i++) {
        lv_obj_t *alert_card = lv_obj_create(list);
        if (!alert_card) {
            ESP_LOGE(TAG, "Erreur création carte alerte %d", i);
            return NULL;
        }
        lv_obj_remove_style_all(alert_card);
        lv_obj_add_style(alert_card, ui_styles_get_card_style(), 0);
        lv_obj_set_size(alert_card, lv_pct(100), 80);
        lv_obj_set_flex_flow(alert_card, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_all(alert_card, 10, 0);
        lv_obj_set_style_pad_gap(alert_card, 10, 0);

        // Indicateur de niveau
        lv_obj_t *level_indicator = lv_obj_create(alert_card);
        lv_obj_set_size(level_indicator, 6, 60);
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

        lv_obj_t *text_cont = lv_obj_create(alert_card);
        lv_obj_remove_style_all(text_cont);
        lv_obj_set_flex_flow(text_cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_grow(text_cont, 1);
        lv_obj_set_style_pad_gap(text_cont, 5, 0);

        lv_obj_t *level_label = lv_label_create(text_cont);
        lv_label_set_text(level_label, alerts_data[i][0]);
        lv_obj_add_style(level_label, ui_styles_get_text_small(), 0);

        lv_obj_t *alert_title = lv_label_create(text_cont);
        lv_label_set_text(alert_title, alerts_data[i][1]);
        lv_obj_add_style(alert_title, ui_styles_get_text_subtitle(), 0);

        lv_obj_t *alert_details = lv_label_create(text_cont);
        lv_label_set_text(alert_details, alerts_data[i][2]);
        lv_obj_add_style(alert_details, ui_styles_get_text_body(), 0);

        // Bouton d'action
        lv_obj_t *action_btn = lv_btn_create(alert_card);
        lv_obj_add_style(action_btn, ui_styles_get_button_danger(), 0);
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
    if (!screen) {
        ESP_LOGE(TAG, "Erreur création écran paramètres");
        return NULL;
    }
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(screen, 12, 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Paramètres Système");
    lv_obj_add_style(title, ui_styles_get_text_title(), 0);

    // Sections de paramètres
    const char *settings_sections[] = {
        "Réseau et Connectivité",
        "Capteurs et Surveillance",
        "Notifications et Alertes",
        "Maintenance et Sauvegardes"
    };

    lv_obj_t *list = lv_obj_create(screen);
    lv_obj_remove_style_all(list);
    lv_obj_set_size(list, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(list, 10, 0);

    for (int i = 0; i < 4; i++) {
        lv_obj_t *section_card = lv_obj_create(list);
        if (!section_card) {
            ESP_LOGE(TAG, "Erreur création carte section %d", i);
            return NULL;
        }
        lv_obj_remove_style_all(section_card);
        lv_obj_add_style(section_card, ui_styles_get_card_style(), 0);
        lv_obj_set_size(section_card, lv_pct(100), 70);
        lv_obj_set_flex_flow(section_card, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_all(section_card, 16, 0);
        lv_obj_set_style_pad_gap(section_card, 10, 0);

        lv_obj_t *text_cont = lv_obj_create(section_card);
        lv_obj_remove_style_all(text_cont);
        lv_obj_set_flex_flow(text_cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_grow(text_cont, 1);
        lv_obj_set_style_pad_gap(text_cont, 5, 0);

        lv_obj_t *section_title = lv_label_create(text_cont);
        lv_label_set_text(section_title, settings_sections[i]);
        lv_obj_add_style(section_title, ui_styles_get_text_subtitle(), 0);

        lv_obj_t *section_desc = lv_label_create(text_cont);
        lv_label_set_text(section_desc, "Configuration des paramètres de cette section");
        lv_obj_add_style(section_desc, ui_styles_get_text_small(), 0);

        lv_obj_t *config_btn = lv_btn_create(section_card);
        lv_obj_add_style(config_btn, ui_styles_get_button_primary(), 0);
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
