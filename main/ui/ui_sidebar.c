/**
 * @file ui_sidebar.c
 * @brief Implémentation du menu latéral
 * @author NovaReptileElevage Team
 */

#include "ui_sidebar.h"
#include "ui_styles.h"
#include "ui_main.h"
#include "esp_log.h"
#include "ui_data.h"

static const char *TAG = "UI_Sidebar";

// Structure pour les éléments de menu
typedef struct {
    lv_obj_t *container;
    lv_obj_t *icon;
    lv_obj_t *label;
    lv_obj_t *indicator;
    nova_screen_t screen_type;
    bool is_active;
} sidebar_item_t;

// Tableau des éléments de menu
static sidebar_item_t menu_items[SCREEN_COUNT];
static lv_obj_t *sidebar_container;

/**
 * @brief Callback pour les clics sur les éléments de menu
 * @param e Événement LVGL
 */
static void menu_item_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    
    if (code == LV_EVENT_CLICKED) {
        // Recherche de l'élément cliqué
        for (size_t i = 0; i < g_ui_menu_items_count; i++) {
            if (menu_items[i].container == obj) {
                ESP_LOGI(TAG, "Menu cliqué: %s", g_ui_menu_items[i].label);
                ui_main_set_screen(menu_items[i].screen_type);
                break;
            }
        }
    } else if (code == LV_EVENT_PRESSED) {
        // Effet visuel de pression
        lv_obj_add_style(obj, ui_styles_get_nav_item_hover(), 0);
    } else if (code == LV_EVENT_RELEASED) {
        // Retour à l'état normal si pas actif
        for (size_t i = 0; i < g_ui_menu_items_count; i++) {
            if (menu_items[i].container == obj && !menu_items[i].is_active) {
                lv_obj_remove_style(obj, ui_styles_get_nav_item_hover(), 0);
                break;
            }
        }
    }
}

/**
 * @brief Crée un élément de menu
 * @param parent Conteneur parent
 * @param index Index de l'élément
 * @param y_pos Position Y
 * @return esp_err_t Code d'erreur
 */
static esp_err_t create_menu_item(lv_obj_t *parent, int index)
{
    sidebar_item_t *item = &menu_items[index];
    
    // Conteneur principal de l'élément
    item->container = lv_obj_create(parent);
    if (!item->container) {
        ESP_LOGE(TAG, "Erreur création conteneur menu %d", index);
        return ESP_ERR_NO_MEM;
    }
    
    lv_obj_remove_style_all(item->container);
    lv_obj_add_style(item->container, ui_styles_get_nav_item_normal(), 0);
    lv_obj_set_size(item->container, LV_PCT(100), 50);
    lv_obj_set_style_margin_bottom(item->container, 10, 0);
    lv_obj_add_flag(item->container, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_flex_flow(item->container, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(item->container, 8, 0);
    lv_obj_set_style_pad_gap(item->container, 10, 0);
    
    // Icône
    item->icon = lv_label_create(item->container);
    lv_label_set_text(item->icon, g_ui_menu_items[index].icon);
    lv_obj_add_style(item->icon, ui_styles_get_text_subtitle(), 0);
    
    // Label
    item->label = lv_label_create(item->container);
    lv_label_set_text(item->label, g_ui_menu_items[index].label);
    lv_obj_add_style(item->label, ui_styles_get_text_body(), 0);
    lv_obj_set_flex_grow(item->label, 1);
    
    // Indicateur (pour notifications, compteurs, etc.)
    item->indicator = lv_obj_create(item->container);
    if (!item->indicator) {
        ESP_LOGE(TAG, "Erreur création indicateur menu %d", index);
        return ESP_ERR_NO_MEM;
    }
    lv_obj_set_size(item->indicator, 8, 8);
    lv_obj_add_flag(item->indicator, LV_OBJ_FLAG_HIDDEN); // Caché par défaut
    lv_obj_add_style(item->indicator, ui_styles_get_nav_indicator(), 0);
    
    // Configuration de l'élément
    item->screen_type = g_ui_menu_items[index].screen;
    item->is_active = false;
    
    // Ajout du callback d'événement
    lv_obj_add_event_cb(item->container, menu_item_event_cb, LV_EVENT_ALL, NULL);
    
    return ESP_OK;
}

esp_err_t ui_sidebar_init(lv_obj_t *parent)
{
    if (!parent) {
        ESP_LOGE(TAG, "Conteneur parent invalide");
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Initialisation de la sidebar");
    
    sidebar_container = parent;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(parent, 12, 0);
    lv_obj_set_style_pad_gap(parent, 10, 0);

    // Création des éléments de menu
    for (size_t i = 0; i < g_ui_menu_items_count; i++) {
        esp_err_t ret = create_menu_item(parent, i);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Erreur création élément menu %d", i);

            // Nettoyage des éléments déjà créés
            if (menu_items[i].container) {
                lv_obj_del(menu_items[i].container);
                menu_items[i].container = NULL;
            }
            for (size_t j = 0; j < i; j++) {
                if (menu_items[j].container) {
                    lv_obj_del(menu_items[j].container);
                    menu_items[j].container = NULL;
                }
            }

            return ret;
        }
    }
    
    // Activation du premier élément par défaut
    ui_sidebar_set_active_item(SCREEN_DASHBOARD);
    
    ESP_LOGI(TAG, "Sidebar initialisée avec succès");
    return ESP_OK;
}

void ui_sidebar_set_active_item(nova_screen_t screen_type)
{
    for (size_t i = 0; i < g_ui_menu_items_count; i++) {
        sidebar_item_t *item = &menu_items[i];
        
        if (item->screen_type == screen_type) {
            // Activation de l'élément
            if (!item->is_active) {
                lv_obj_remove_style_all(item->container);
                lv_obj_add_style(item->container, ui_styles_get_nav_item_active(), 0);
                item->is_active = true;
                ESP_LOGI(TAG, "Élément activé: %s", g_ui_menu_items[i].label);
            }
        } else {
            // Désactivation des autres éléments
            if (item->is_active) {
                lv_obj_remove_style_all(item->container);
                lv_obj_add_style(item->container, ui_styles_get_nav_item_normal(), 0);
                item->is_active = false;
            }
        }
    }
}

void ui_sidebar_update_indicators(void)
{
    // Exemple: Affichage d'indicateur pour les alertes
    sidebar_item_t *alerts_item = &menu_items[SCREEN_ALERTS];
    
    // Simulation d'une alerte active
    static bool has_alerts = true;
    
    if (has_alerts) {
        lv_obj_clear_flag(alerts_item->indicator, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(alerts_item->indicator, LV_OBJ_FLAG_HIDDEN);
    }
    
    ESP_LOGD(TAG, "Indicateurs mis à jour");
}
