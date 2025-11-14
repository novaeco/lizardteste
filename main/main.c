/**
 * @file main.c
 * @brief Point d'entrée principal pour NovaReptileElevage
 * @author NovaReptileElevage Team
 * @version 1.0
 * @date 2025
 * 
 * Interface LVGL pour ESP32-S3 Touch LCD 7B (1024x600)
 * Drivers: ST7701 (Display) + GT911 (Touch)
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_timer.h"
#include "esp_psram.h"

#include "lvgl.h"
#include "ui_main.h"
#include "ui_styles.h"
#include "display_driver.h"
#include "touch_driver.h"
#include "ch422g.h"
#include "i2c_bus.h"

static const char *TAG = "NovaReptile_Main";

/**
 * @brief Callback du timer haute résolution pour LVGL
 *
 * Incrémente le tick LVGL toutes les 1 ms afin d'assurer une gestion
 * temporelle précise des timers internes de la bibliothèque.
 */
static void lv_tick_cb(void *arg)
{
    lv_tick_inc(1);
}

/** Handle du timer haute résolution LVGL */
static esp_timer_handle_t lvgl_tick_timer;

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
 *
 * Les différentes sous-systèmes sont initialisés séquentiellement. En cas
 * d'échec, les ressources sont libérées en ordre inverse de leur allocation :
 * interface (ui_styles), driver tactile, driver d'affichage puis cœur LVGL.
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

    // Initialisation et vérification de la PSRAM
    ret = esp_psram_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation PSRAM: %s", esp_err_to_name(ret));
        return ret;
    }

    size_t psram_size = esp_psram_get_size();
    if (psram_size == 0) {
        ESP_LOGE(TAG, "Aucune PSRAM détectée - initialisation annulée");
        return ESP_ERR_NO_MEM;
    }

    // Initialisation de LVGL
    ESP_LOGI(TAG, "Initialisation LVGL v%d.%d.%d",
             lv_version_major(), lv_version_minor(), lv_version_patch());
    lv_init();

    // Initialisation du bus I2C partagé
    ret = i2c_bus_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation I2C: %s", esp_err_to_name(ret));
        lv_deinit();
        return ret;
    }

    // Initialisation du pilote CH422G
    ret = ch422g_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation CH422G: %s", esp_err_to_name(ret));
        lv_deinit();
        return ret;
    }

    // Initialisation du driver d'affichage ST7701
    ret = display_driver_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation display: %s", esp_err_to_name(ret));
        // Libération en ordre inverse: ch422g -> LVGL
        lv_deinit();
        return ret;
    }

    // Initialisation du driver tactile GT911
    ret = touch_driver_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation touch: %s", esp_err_to_name(ret));
        // Libération en ordre inverse: display -> LVGL
        display_driver_deinit();
        i2c_bus_deinit();
        lv_deinit();
        return ret;
    }

    // Initialisation de l'interface utilisateur
    ret = ui_main_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation UI: %s", esp_err_to_name(ret));
        // Libération en ordre inverse: styles -> touch -> display -> LVGL
        ui_styles_deinit();
        touch_driver_deinit();
        display_driver_deinit();
        i2c_bus_deinit();
        lv_deinit();
        return ret;
    }
    
    ESP_LOGI(TAG, "Système NovaReptileElevage initialisé avec succès");
    return ESP_OK;
}

/**
 * @brief Déinitialisation globale du système NovaReptileElevage
 *
 * Libère l'ensemble des sous-systèmes initialisés par nova_reptile_init()
 * et arrête le timer haute résolution utilisé par LVGL.
 */
static void nova_reptile_deinit(void)
{
    if (lvgl_tick_timer) {
        ESP_ERROR_CHECK(esp_timer_stop(lvgl_tick_timer));
        ESP_ERROR_CHECK(esp_timer_delete(lvgl_tick_timer));
        lvgl_tick_timer = NULL;
    }

    ui_styles_deinit();
    touch_driver_deinit();
    display_driver_deinit();
    i2c_bus_deinit();
    lv_deinit();

    ESP_LOGI(TAG, "Système NovaReptileElevage déinitialisé");
}

/**
 * @brief Fonction principale de l'application
 */
void app_main(void)
{
    ESP_LOGI(TAG, "=== NovaReptileElevage v1.0 ===");
    ESP_LOGI(TAG, "ESP32-S3 Touch LCD 7B (1024x600)");
    ESP_LOGI(TAG, "LVGL %d.%d.%d + ESP-IDF %s",
             lv_version_major(), lv_version_minor(), lv_version_patch(),
             esp_get_idf_version());

    // Initialisation du système
    ESP_ERROR_CHECK(nova_reptile_init());

    // Initialisation du timer haute résolution LVGL (1 ms)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &lv_tick_cb,
        .name = "lvgl_tick"
    };
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 1000));

    // Création de la tâche LVGL (priorité élevée pour la fluidité)
    // Si la création échoue, l'erreur est journalisée et le système redémarre
    BaseType_t task_ret = xTaskCreatePinnedToCore(
        lvgl_task,              // Fonction de la tâche
        "LVGL_Task",            // Nom de la tâche
        8192,                   // Taille de la pile (8KB)
        NULL,                   // Paramètres
        5,                      // Priorité (élevée)
        NULL,                   // Handle de la tâche
        1                       // Core 1 (Core 0 pour WiFi/BT)
    );

    if (task_ret != pdPASS) {
        ESP_LOGE(TAG, "Échec création tâche LVGL: %ld", task_ret);
        nova_reptile_deinit();
        esp_restart();
    }
    
    ESP_LOGI(TAG, "Application démarrée - Interface prête");
}
