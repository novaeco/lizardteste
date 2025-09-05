#include "ui_data.h"

// Données par défaut pour les éléments de menu
static const ui_menu_item_t default_menu_items[] = {
    {LV_SYMBOL_HOME, "Accueil", SCREEN_DASHBOARD},
    {LV_SYMBOL_EYE_OPEN, "Reptiles", SCREEN_REPTILES},
    {LV_SYMBOL_DIRECTORY, "Terrariums", SCREEN_TERRARIUMS},
    {LV_SYMBOL_BARS, "Statistiques", SCREEN_STATISTICS},
    {LV_SYMBOL_WARNING, "Alertes", SCREEN_ALERTS},
    {LV_SYMBOL_SETTINGS, "Paramètres", SCREEN_SETTINGS},
};

// Données par défaut pour les reptiles
static const char *default_reptiles[] = {
    "Python Royal",
    "Iguane Vert",
    "Gecko Léopard",
    "Boa Constrictor",
    "Caméléon",
    "Tortue d'Hermann",
};

// Données par défaut pour les alertes
static const ui_alert_item_t default_alerts[] = {
    {"CRITIQUE", "Température terrarium #2 élevée", "28.5°C (Max: 26°C)"},
    {"ATTENTION", "Humidité terrarium #4 faible", "45% (Min: 50%)"},
    {"INFO", "Maintenance programmée demain", "Nettoyage système filtration"},
};

// Sections de paramètres par défaut
static const char *default_settings_sections[] = {
    "Réseau et Connectivité",
    "Capteurs et Surveillance",
    "Notifications et Alertes",
    "Maintenance et Sauvegardes",
};

// Instances configurables exposées
ui_menu_item_t g_ui_menu_items[sizeof(default_menu_items)/sizeof(default_menu_items[0])];
size_t g_ui_menu_items_count = sizeof(default_menu_items)/sizeof(default_menu_items[0]);

const char *g_ui_reptiles[sizeof(default_reptiles)/sizeof(default_reptiles[0])];
size_t g_ui_reptiles_count = sizeof(default_reptiles)/sizeof(default_reptiles[0]);

ui_alert_item_t g_ui_alerts[sizeof(default_alerts)/sizeof(default_alerts[0])];
size_t g_ui_alerts_count = sizeof(default_alerts)/sizeof(default_alerts[0]);

const char *g_ui_settings_sections[sizeof(default_settings_sections)/sizeof(default_settings_sections[0])];
size_t g_ui_settings_sections_count = sizeof(default_settings_sections)/sizeof(default_settings_sections[0]);

void ui_data_load_defaults(void)
{
    for (size_t i = 0; i < g_ui_menu_items_count; ++i) {
        g_ui_menu_items[i] = default_menu_items[i];
    }
    for (size_t i = 0; i < g_ui_reptiles_count; ++i) {
        g_ui_reptiles[i] = default_reptiles[i];
    }
    for (size_t i = 0; i < g_ui_alerts_count; ++i) {
        g_ui_alerts[i] = default_alerts[i];
    }
    for (size_t i = 0; i < g_ui_settings_sections_count; ++i) {
        g_ui_settings_sections[i] = default_settings_sections[i];
    }
}

void ui_data_reload(void)
{
    // Dans une implémentation future, cette fonction pourrait charger les
    // données depuis le stockage ou un service distant. Ici, on recharge les
    // valeurs par défaut pour simuler une mise à jour dynamique.
    ui_data_load_defaults();
}
