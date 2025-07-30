/**
 * @file main.c
 * @brief Point d'entrée principal pour NovaReptileElevage
 * @author NovaReptileElevage Team
 * @version 1.0
 * @date 2025
 * 
 * Interface LVGL pour ESP32-S3 Touch LCD 7" (800x480)
 * Drivers: ST7262 (Display) + GT911 (Touch)
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#include "lvgl.h"
#include "ui_main.h"
#include "display_driver.h"
#include "touch_driver.h"

static const char *TAG = "NovaReptile_Main";

/**
 * @brief Tâche principale LVGL - Gestion des timers et événements
 * @param pvParameter Paramètres de la tâche (non utilisé)
 */
static void lvgl_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Démarrage de la tâche LVGL");
    
    while (1) {
        // Mise à jour des timers LVGL (recommandé toutes les 1-10ms)
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief Initialisation du système NovaReptileElevage
 * @return esp_err_t Code d'erreur ESP
 */
static esp_err_t nova_reptile_init(void)
{
    esp_err_t ret = ESP_OK;
    
    // Initialisation de la mémoire flash NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialisation de LVGL
    ESP_LOGI(TAG, "Initialisation LVGL v%d.%d.%d", 
             lv_version_major(), lv_version_minor(), lv_version_patch());
    lv_init();
    
    // Initialisation du driver d'affichage ST7262
    ret = display_driver_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation display: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Initialisation du driver tactile GT911
    ret = touch_driver_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation touch: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Initialisation de l'interface utilisateur
    ret = ui_main_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation UI: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "Système NovaReptileElevage initialisé avec succès");
    return ESP_OK;
}

/**
 * @brief Fonction principale de l'application
 */
void app_main(void)
{
    ESP_LOGI(TAG, "=== NovaReptileElevage v1.0 ===");
    ESP_LOGI(TAG, "ESP32-S3 Touch LCD 7\" (800x480)");
    ESP_LOGI(TAG, "LVGL %d.%d.%d + ESP-IDF %s", 
             lv_version_major(), lv_version_minor(), lv_version_patch(),
             esp_get_idf_version());
    
    // Initialisation du système
    ESP_ERROR_CHECK(nova_reptile_init());
    
    // Création de la tâche LVGL (priorité élevée pour la fluidité)
    xTaskCreatePinnedToCore(
        lvgl_task,              // Fonction de la tâche
        "LVGL_Task",            // Nom de la tâche
        8192,                   // Taille de la pile (8KB)
        NULL,                   // Paramètres
        5,                      // Priorité (élevée)
        NULL,                   // Handle de la tâche
        1                       // Core 1 (Core 0 pour WiFi/BT)
    );
    
    ESP_LOGI(TAG, "Application démarrée - Interface prête");
}