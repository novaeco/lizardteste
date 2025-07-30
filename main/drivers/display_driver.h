/**
 * @file display_driver.h
 * @brief Driver pour écran ST7262 (800x480)
 * @author NovaReptileElevage Team
 */

#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include "esp_err.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Configuration de l'écran
#define DISPLAY_WIDTH   800
#define DISPLAY_HEIGHT  480
#define DISPLAY_BUF_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT / 10)

/**
 * @brief Initialise le driver d'affichage ST7262
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
 */
void display_set_sleep(bool sleep);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_DRIVER_H