/**
 * @file ui_footer.c
 * @brief Implémentation de la barre d'état
 * @author NovaReptileElevage Team
 */

#include "ui_footer.h"
#include "ui_styles.h"
#include "esp_log.h"
#include "esp_random.h"
#include <stdio.h>

static const char *TAG = "UI_Footer";

// Composants du footer
static lv_obj_t *footer_wifi_icon;
static lv_obj_t *footer_wifi_text;
static lv_obj_t *footer_notifications;
static lv_obj_t *footer_datetime;
static lv_obj_t *footer_system_info;

/**
 * @brief Crée l'indicateur Wi-Fi
 * @param parent Conteneur parent
 * @return esp_err_t Code d'erreur
 */
static esp_err_t create_wifi_indicator(lv_obj_t *parent)
{
    lv_obj_t *cont = lv_obj_create(parent);
    if (!cont) {
        return ESP_ERR_NO_MEM;
    }

    lv_obj_remove_style_all(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_gap(cont, 5, 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);

    // Icône Wi-Fi
    footer_wifi_icon = lv_label_create(cont);
    lv_label_set_text(footer_wifi_icon, LV_SYMBOL_WIFI);
    lv_obj_add_style(footer_wifi_icon, ui_styles_get_text_small(), 0);

    // Texte statut Wi-Fi
    footer_wifi_text = lv_label_create(cont);
    lv_label_set_text(footer_wifi_text, "Wi-Fi: Connecté");
    lv_obj_add_style(footer_wifi_text, ui_styles_get_text_small(), 0);

    return ESP_OK;
}

/**
 * @brief Crée l'indicateur de notifications
 * @param parent Conteneur parent
 * @return esp_err_t Code d'erreur
 */
static esp_err_t create_notifications_indicator(lv_obj_t *parent)
{
    footer_notifications = lv_label_create(parent);
    lv_label_set_text(footer_notifications, LV_SYMBOL_BELL " 2 notifications");
    lv_obj_add_style(footer_notifications, ui_styles_get_text_small(), 0);

    return ESP_OK;
}

/**
 * @brief Crée l'affichage de date/heure
 * @param parent Conteneur parent
 * @return esp_err_t Code d'erreur
 */
static esp_err_t create_datetime_display(lv_obj_t *parent)
{
    footer_datetime = lv_label_create(parent);
    lv_label_set_text(footer_datetime, "15 Jan 2025 - 14:32:15");
    lv_obj_add_style(footer_datetime, ui_styles_get_text_small(), 0);

    return ESP_OK;
}

/**
 * @brief Crée l'affichage des informations système
 * @param parent Conteneur parent
 * @return esp_err_t Code d'erreur
 */
static esp_err_t create_system_info(lv_obj_t *parent)
{
    footer_system_info = lv_label_create(parent);
    lv_label_set_text(footer_system_info, "CPU: 45% | RAM: 62% | Temp: 38°C");
    lv_obj_add_style(footer_system_info, ui_styles_get_text_small(), 0);

    return ESP_OK;
}

esp_err_t ui_footer_init(lv_obj_t *parent)
{
    if (!parent) {
        ESP_LOGE(TAG, "Conteneur parent invalide");
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Initialisation du footer");
    
    // Mise en place du layout flex pour le footer
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(parent, 10, 0);
    lv_obj_set_style_pad_gap(parent, 20, 0);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Création des composants
    esp_err_t ret;

    ret = create_wifi_indicator(parent);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur création indicateur Wi-Fi");
        return ret;
    }

    /* Espaceur pour pousser les éléments restants à droite */
    lv_obj_t *spacer = lv_obj_create(parent);
    lv_obj_remove_style_all(spacer);
    lv_obj_set_flex_grow(spacer, 1);

    ret = create_notifications_indicator(parent);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur création indicateur notifications");
        return ret;
    }

    ret = create_datetime_display(parent);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur création affichage date/heure");
        return ret;
    }

    ret = create_system_info(parent);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur création informations système");
        return ret;
    }
    
    ESP_LOGI(TAG, "Footer initialisé avec succès");
    return ESP_OK;
}

void ui_footer_update_status(void)
{
    // Mise à jour des informations système
    if (footer_system_info) {
        // Simulation de données système
        static int cpu_usage = 45;
        static int ram_usage = 62;
        static float cpu_temp = 38.2f;
        
        // Variation légère pour simuler l'activité
        cpu_usage += (esp_random() % 10) - 5;
        if (cpu_usage < 20) cpu_usage = 20;
        if (cpu_usage > 80) cpu_usage = 80;
        
        ram_usage += (esp_random() % 6) - 3;
        if (ram_usage < 40) ram_usage = 40;
        if (ram_usage > 85) ram_usage = 85;
        
        cpu_temp += ((float)(esp_random() % 20) - 10) / 10.0f;
        if (cpu_temp < 35.0f) cpu_temp = 35.0f;
        if (cpu_temp > 45.0f) cpu_temp = 45.0f;
        
        char sys_info[100];
        snprintf(sys_info, sizeof(sys_info), 
                "CPU: %d%% | RAM: %d%% | Temp: %.1f°C", 
                cpu_usage, ram_usage, cpu_temp);
        
        lv_label_set_text(footer_system_info, sys_info);
    }
    
    ESP_LOGD(TAG, "Statut mis à jour");
}

void ui_footer_set_wifi_status(bool connected, int signal_strength)
{
    if (footer_wifi_text) {
        char wifi_status[50];
        
        if (connected) {
            snprintf(wifi_status, sizeof(wifi_status), 
                    "Wi-Fi: Connecté (%d%%)", signal_strength);
        } else {
            snprintf(wifi_status, sizeof(wifi_status), "Wi-Fi: Déconnecté");
        }
        
        lv_label_set_text(footer_wifi_text, wifi_status);
        ESP_LOGI(TAG, "Statut Wi-Fi: %s", wifi_status);
    }
    
    if (footer_wifi_icon) {
        if (connected) {
            if (signal_strength > 75) {
                lv_label_set_text(footer_wifi_icon, LV_SYMBOL_WIFI);
            } else if (signal_strength > 50) {
                lv_label_set_text(footer_wifi_icon, LV_SYMBOL_WIFI);
            } else if (signal_strength > 25) {
                lv_label_set_text(footer_wifi_icon, LV_SYMBOL_WIFI);
            } else {
                lv_label_set_text(footer_wifi_icon, LV_SYMBOL_WIFI);
            }
        } else {
            lv_label_set_text(footer_wifi_icon, LV_SYMBOL_CLOSE);
        }
    }
}

void ui_footer_set_notification_count(int count)
{
    if (footer_notifications) {
        char notif_text[50];
        
        if (count > 0) {
            snprintf(notif_text, sizeof(notif_text), LV_SYMBOL_BELL " %d notification%s",
                    count, count > 1 ? "s" : "");
        } else {
            snprintf(notif_text, sizeof(notif_text), LV_SYMBOL_BELL " Aucune notification");
        }
        
        lv_label_set_text(footer_notifications, notif_text);
        ESP_LOGI(TAG, "Notifications: %d", count);
    }
}

void ui_footer_set_datetime(const char *time_str)
{
    if (footer_datetime && time_str) {
        lv_label_set_text(footer_datetime, time_str);
    }
}