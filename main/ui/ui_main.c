/**
 * @file ui_main.c
 * @brief Implémentation de l'interface principale
 * @author NovaReptileElevage Team
 */

#include "ui_main.h"
#include "ui_header.h"
#include "ui_sidebar.h"
#include "ui_content.h"
#include "ui_footer.h"
#include "ui_styles.h"
#include "esp_log.h"

static const char *TAG = "UI_Main";
static nova_ui_t g_nova_ui = {0};
static nova_screen_t g_current_screen = SCREEN_DASHBOARD;

/**
 * @brief Crée la structure principale de l'interface
 * @return esp_err_t Code d'erreur
 */
static esp_err_t create_main_layout(void)
{
    // Création de l'écran principal
    g_nova_ui.main_screen = lv_obj_create(NULL);
    if (!g_nova_ui.main_screen) {
        ESP_LOGE(TAG, "Erreur création écran principal");
        return ESP_ERR_NO_MEM;
    }
    
    lv_obj_remove_style_all(g_nova_ui.main_screen);
    lv_obj_add_style(g_nova_ui.main_screen, ui_styles_get_main_bg(), 0);

    /* Mise en place du layout grid principal pour éviter les positions absolues */
    static lv_coord_t col_dsc[] = {SIDEBAR_WIDTH, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {HEADER_HEIGHT, LV_GRID_FR(1), FOOTER_HEIGHT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_layout(g_nova_ui.main_screen, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(g_nova_ui.main_screen, col_dsc, row_dsc);

    // Création du conteneur header (barre de titre)
    g_nova_ui.header_container = lv_obj_create(g_nova_ui.main_screen);
    lv_obj_remove_style_all(g_nova_ui.header_container);
    lv_obj_add_style(g_nova_ui.header_container, ui_styles_get_header_bg(), 0);
    lv_obj_set_grid_cell(g_nova_ui.header_container, LV_GRID_ALIGN_STRETCH, 0, 2,
                         LV_GRID_ALIGN_STRETCH, 0, 1);

    // Création du conteneur sidebar (menu latéral)
    g_nova_ui.sidebar_container = lv_obj_create(g_nova_ui.main_screen);
    lv_obj_remove_style_all(g_nova_ui.sidebar_container);
    lv_obj_add_style(g_nova_ui.sidebar_container, ui_styles_get_sidebar_bg(), 0);
    lv_obj_set_grid_cell(g_nova_ui.sidebar_container, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 1, 1);

    // Création du conteneur de contenu principal
    g_nova_ui.content_container = lv_obj_create(g_nova_ui.main_screen);
    lv_obj_remove_style_all(g_nova_ui.content_container);
    lv_obj_add_style(g_nova_ui.content_container, ui_styles_get_content_bg(), 0);
    lv_obj_set_grid_cell(g_nova_ui.content_container, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_STRETCH, 1, 1);

    // Création du conteneur footer (barre d'état)
    g_nova_ui.footer_container = lv_obj_create(g_nova_ui.main_screen);
    lv_obj_remove_style_all(g_nova_ui.footer_container);
    lv_obj_add_style(g_nova_ui.footer_container, ui_styles_get_footer_bg(), 0);
    lv_obj_set_grid_cell(g_nova_ui.footer_container, LV_GRID_ALIGN_STRETCH, 0, 2,
                         LV_GRID_ALIGN_STRETCH, 2, 1);
    
    ESP_LOGI(TAG, "Layout principal créé avec succès");
    return ESP_OK;
}

/**
 * @brief Initialise tous les composants de l'interface
 * @return esp_err_t Code d'erreur
 */
esp_err_t ui_main_init(void)
{
    esp_err_t ret;
    
    ESP_LOGI(TAG, "Initialisation de l'interface NovaReptileElevage");
    
    // Initialisation des styles personnalisés
    ret = ui_styles_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation styles");
        return ret;
    }
    
    // Création du layout principal
    ret = create_main_layout();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur création layout principal");
        return ret;
    }
    
    // Initialisation du header (barre de titre)
    ret = ui_header_init(g_nova_ui.header_container);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation header");
        return ret;
    }
    
    // Initialisation de la sidebar (menu latéral)
    ret = ui_sidebar_init(g_nova_ui.sidebar_container);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation sidebar");
        return ret;
    }
    
    // Initialisation du contenu principal
    ret = ui_content_init(g_nova_ui.content_container);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation content");
        return ret;
    }
    
    // Initialisation du footer (barre d'état)
    ret = ui_footer_init(g_nova_ui.footer_container);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation footer");
        return ret;
    }
    
    // Chargement de l'écran principal
    lv_scr_load(g_nova_ui.main_screen);
    
    // Affichage initial du tableau de bord
    ui_main_set_screen(SCREEN_DASHBOARD);
    
    ESP_LOGI(TAG, "Interface initialisée avec succès");
    return ESP_OK;
}

nova_ui_t* ui_main_get_instance(void)
{
    return &g_nova_ui;
}

esp_err_t ui_main_set_screen(nova_screen_t screen)
{
    if (screen >= SCREEN_COUNT) {
        ESP_LOGE(TAG, "Écran invalide: %d", screen);
        return ESP_ERR_INVALID_ARG;
    }
    
    g_current_screen = screen;
    
    // Notification à la sidebar pour mettre à jour la sélection
    ui_sidebar_set_active_item(screen);
    
    // Chargement du contenu correspondant
    esp_err_t ret = ui_content_load_screen(screen);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur chargement écran %d", screen);
        return ret;
    }
    
    ESP_LOGI(TAG, "Écran changé vers: %d", screen);
    return ESP_OK;
}

nova_screen_t ui_main_get_current_screen(void)
{
    return g_current_screen;
}

void ui_main_update_realtime_data(void)
{
    // Mise à jour des données temps réel
    ui_footer_update_status();
    
    // Mise à jour du contenu si nécessaire
    if (g_current_screen == SCREEN_DASHBOARD || g_current_screen == SCREEN_STATISTICS) {
        ui_content_update_realtime_data();
    }
}