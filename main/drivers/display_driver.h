/**
 * @file display_driver.h
 * @brief Driver pour écran ST7701 (1024x600)
 * @author NovaReptileElevage Team
 */

#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <stdbool.h>

#include "esp_err.h"
#include "lvgl.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

// Configuration de l'écran
#define DISPLAY_WIDTH   1024
#define DISPLAY_HEIGHT  600

/*
 * Taille du tampon LVGL :
 * - Avec PSRAM, on peut se permettre 1/4 de l'écran pour gagner en fluidité
 * - Sans PSRAM, on réduit à 1/6 afin d'économiser la RAM interne
 */
#if CONFIG_SPIRAM
#define DISPLAY_BUF_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT / 4)
#else
#define DISPLAY_BUF_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT / 6)
#endif

/**
 * @brief Initialise le driver d'affichage ST7701
 * @return esp_err_t Code d'erreur ESP
 */
esp_err_t display_driver_init(void);

/**
 * @brief Désactive le driver d'affichage
 */
void display_driver_deinit(void);

/**
 * @brief Contrôle du rétroéclairage
 * @param brightness Luminosité (0-100)
 */
void display_set_brightness(uint8_t brightness);

/**
 * @brief Mise en veille de l'écran
 * @param sleep true pour mettre en veille, false pour réveiller
 * @return esp_err_t Code d'erreur ESP
 */
esp_err_t display_set_sleep(bool sleep);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_DRIVER_H
