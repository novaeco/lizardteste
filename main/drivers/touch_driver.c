/**
 * @file touch_driver.c
 * @brief Implémentation du driver tactile GT911
 * @author NovaReptileElevage Team
 */

#include "touch_driver.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "Touch_Driver";

// Configuration des broches I2C (Waveshare ESP32-S3 Touch LCD 7")
#define PIN_SDA 8
#define PIN_SCL 9
// Affectation de broches libres hors bus LCD pour l'interface tactile
#define PIN_INT 15        // Broche d'interruption GT911
#define TOUCH_PIN_RST 16  // Broche de reset GT911

// Configuration I2C
#define I2C_PORT I2C_NUM_0
#define I2C_FREQUENCY 400000 // 400kHz
#define GT911_ADDR 0x5D      // Adresse I2C du GT911

// Registres GT911
#define GT911_REG_STATUS 0x814E
#define GT911_REG_ID 0x8140
#define GT911_REG_POINT1 0x814F
#define GT911_REG_CONFIG 0x8047

// Structure pour les données tactiles
typedef struct {
  uint16_t x;
  uint16_t y;
  uint8_t size;
  uint8_t track_id;
} gt911_point_t;

static bool touch_initialized = false;
static volatile bool touch_pressed = false;
static i2c_master_bus_handle_t i2c_bus = NULL;
static i2c_master_dev_handle_t gt911_dev = NULL;

/**
 * @brief ISR appelée sur front descendant de la ligne INT du GT911.
 *
 * Cette routine se contente de mémoriser qu'un évènement tactile est
 * disponible. Le traitement complet est réalisé dans le contexte de
 * `touch_read` afin de limiter la charge de l'ISR.
 */
static void IRAM_ATTR touch_isr_handler(void *arg) {
  (void)arg;
  touch_pressed = true;
}

/**
 * @brief Lit des données depuis le GT911 via I2C
 * @param reg_addr Adresse du registre
 * @param data Buffer pour les données lues
 * @param len Nombre d'octets à lire
 * @return esp_err_t Code d'erreur
 */
static esp_err_t gt911_read_reg(uint16_t reg_addr, uint8_t *data, size_t len) {
  uint8_t reg[2] = {(uint8_t)((reg_addr >> 8) & 0xFF), (uint8_t)(reg_addr & 0xFF)};
  return i2c_master_transmit_receive(gt911_dev, reg, sizeof(reg), data, len,
                                     1000);
}

/**
 * @brief Écrit des données vers le GT911 via I2C
 * @param reg_addr Adresse du registre
 * @param data Données à écrire
 * @param len Nombre d'octets à écrire
 * @return esp_err_t Code d'erreur
 */
static esp_err_t gt911_write_reg(uint16_t reg_addr, const uint8_t *data,
                                 size_t len) {
  uint8_t *buf = malloc(2 + len);
  if (buf == NULL) {
    return ESP_ERR_NO_MEM;
  }
  buf[0] = (reg_addr >> 8) & 0xFF;
  buf[1] = reg_addr & 0xFF;
  memcpy(&buf[2], data, len);
  esp_err_t ret = i2c_master_transmit(gt911_dev, buf, 2 + len, 1000);
  free(buf);
  return ret;
}

/**
 * @brief Initialise le contrôleur GT911
 * @return esp_err_t Code d'erreur
 */
static esp_err_t gt911_init(void) {
  ESP_LOGI(TAG, "Initialisation du contrôleur GT911");

  // Reset du contrôleur
  esp_err_t err = gpio_set_level(TOUCH_PIN_RST, 0);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "gpio_set_level(TOUCH_PIN_RST, 0) failed: %s",
             esp_err_to_name(err));
    return err;
  }
  ESP_ERROR_CHECK(err);

  vTaskDelay(pdMS_TO_TICKS(10));

  err = gpio_set_level(TOUCH_PIN_RST, 1);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "gpio_set_level(TOUCH_PIN_RST, 1) failed: %s",
             esp_err_to_name(err));
    return err;
  }
  ESP_ERROR_CHECK(err);
  vTaskDelay(pdMS_TO_TICKS(100));

  // Lecture de l'ID du contrôleur
  uint8_t id_data[4];
  esp_err_t ret = gt911_read_reg(GT911_REG_ID, id_data, 4);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Erreur lecture ID GT911");
    return ret;
  }

  ESP_LOGI(TAG, "GT911 ID: %c%c%c%c", id_data[0], id_data[1], id_data[2],
           id_data[3]);

  // Vérification de l'ID (doit être "911")
  if (id_data[0] != '9' || id_data[1] != '1' || id_data[2] != '1') {
    ESP_LOGE(TAG, "ID GT911 invalide");
    return ESP_ERR_NOT_FOUND;
  }

  ESP_LOGI(TAG, "GT911 initialisé avec succès");
  return ESP_OK;
}

/**
 * @brief Callback LVGL pour la lecture des données tactiles
 * @param indev Device d'entrée LVGL
 * @param data Structure de données tactiles
 */
static void touch_read(lv_indev_t *indev, lv_indev_data_t *data) {
  static gt911_point_t points[TOUCH_MAX_POINTS];
  static uint8_t total_points = 0;
  static uint8_t point_index = 0;
  static uint16_t last_x = 0, last_y = 0;

  if (point_index >= total_points) {
    if (!touch_pressed) {
      data->state = LV_INDEV_STATE_REL;
      data->continue_reading = false;
      return;
    }

    // Lecture du statut tactile
    uint8_t status;
    esp_err_t ret = gt911_read_reg(GT911_REG_STATUS, &status, 1);

    if (ret != ESP_OK) {
      data->state = LV_INDEV_STATE_REL;
      data->continue_reading = false;
      touch_pressed = false;
      return;
    }

    uint8_t point_count = status & 0x0F;
    total_points = 0;

    if (point_count > 0 && (status & 0x80)) {
      if (point_count > TOUCH_MAX_POINTS) {
        point_count = TOUCH_MAX_POINTS;
      }

      // Lecture des données de tous les points actifs
      uint8_t point_data[8 * TOUCH_MAX_POINTS];
      ret = gt911_read_reg(GT911_REG_POINT1, point_data, point_count * 8);

      if (ret == ESP_OK) {
        for (uint8_t i = 0; i < point_count; i++) {
          uint8_t *p = &point_data[i * 8];
          uint16_t x = ((uint16_t)p[1] << 8) | p[0];
          uint16_t y = ((uint16_t)p[3] << 8) | p[2];

          // Adaptation aux dimensions de l'écran
          x = (x * TOUCH_WIDTH) / 800;
          y = (y * TOUCH_HEIGHT) / 480;

          points[i].x = x;
          points[i].y = y;
        }
        total_points = point_count;
        point_index = 0;
        last_x = points[0].x;
        last_y = points[0].y;

        ESP_LOGD(TAG, "Touch: points=%d", total_points);
      }

      // Effacement du statut pour préparer la prochaine lecture
      uint8_t clear = 0;
      gt911_write_reg(GT911_REG_STATUS, &clear, 1);
    }

    touch_pressed = false;
  }

  if (point_index < total_points) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = points[point_index].x;
    data->point.y = points[point_index].y;
    last_x = data->point.x;
    last_y = data->point.y;
    point_index++;
    data->continue_reading = point_index < total_points;
  } else {
    data->state = LV_INDEV_STATE_REL;
    data->point.x = last_x;
    data->point.y = last_y;
    data->continue_reading = false;
    total_points = 0;
    point_index = 0;
  }
}

/**
 * @brief Initialise le driver tactile GT911.
 *
 * En cas d'échec, une séquence de rollback est exécutée afin de laisser le
 * matériel dans un état sûr :
 *   1. suppression du handler et du service ISR ;
 *   2. reconfiguration de PIN_INT et TOUCH_PIN_RST en entrée pull-up.
 */
esp_err_t touch_driver_init(void) {
  esp_err_t ret;
  bool isr_service_installed = false;
  bool isr_handler_added = false;

  ESP_LOGI(TAG, "Initialisation du driver tactile GT911");

  if (touch_initialized) {
    ESP_LOGW(TAG, "Driver tactile déjà initialisé");
    return ESP_OK;
  }

  // Configuration de la broche RST (sortie)
  gpio_config_t rst_conf = {
      .pin_bit_mask = (1ULL << TOUCH_PIN_RST),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  ret = gpio_config(&rst_conf);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Erreur configuration GPIO RST");
    goto fail;
  }

  // Configuration de la broche INT (entrée avec pull-up)
  gpio_config_t int_conf = {
      .pin_bit_mask = (1ULL << PIN_INT),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_NEGEDGE, // Préparation pour une ISR
  };
  ret = gpio_config(&int_conf);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Erreur configuration GPIO INT");
    goto fail;
  }

  ret = gpio_install_isr_service(0);
  if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
    ESP_LOGE(TAG, "Erreur installation service ISR");
    goto fail;
  }
  if (ret == ESP_OK)
    isr_service_installed = true;

  ret = gpio_isr_handler_add(PIN_INT, touch_isr_handler, NULL);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Erreur ajout handler ISR");
    goto fail;
  }
  isr_handler_added = true;

  // Configuration du bus I2C et ajout du périphérique GT911
  i2c_master_bus_config_t bus_conf = {
      .i2c_port = I2C_PORT,
      .sda_io_num = PIN_SDA,
      .scl_io_num = PIN_SCL,
      .clk_source = I2C_CLK_SRC_DEFAULT, // ou I2C_CLK_SRC_PLL_F80 selon la carte
      .flags = {
          .enable_internal_pullup = true,
      },
  };

  // Activation explicite des pull-ups si non gérés par l'API
  gpio_pullup_en(PIN_SDA);
  gpio_pullup_en(PIN_SCL);

  ret = i2c_new_master_bus(&bus_conf, &i2c_bus);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Erreur création bus I2C");
    goto fail;
  }

  i2c_device_config_t dev_conf = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = GT911_ADDR,
      .scl_speed_hz = I2C_FREQUENCY,
  };

  ret = i2c_master_bus_add_device(i2c_bus, &dev_conf, &gt911_dev);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Erreur ajout device I2C");
    i2c_del_master_bus(i2c_bus);
    i2c_bus = NULL;
    goto fail;
  }

  // Initialisation du GT911
  ret = gt911_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Erreur initialisation GT911");
    i2c_master_bus_rm_device(gt911_dev);
    i2c_del_master_bus(i2c_bus);
    gt911_dev = NULL;
    i2c_bus = NULL;
    goto fail;
  }

  // Configuration du driver d'entrée LVGL
  lv_indev_t *indev = lv_indev_create();
  if (!indev) {
    ESP_LOGE(TAG, "Erreur création device tactile LVGL");
    i2c_master_bus_rm_device(gt911_dev);
    i2c_del_master_bus(i2c_bus);
    gt911_dev = NULL;
    i2c_bus = NULL;
    ret = ESP_FAIL;
    goto fail;
  }

  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touch_read);

  touch_initialized = true;
  ESP_LOGI(TAG, "Driver tactile GT911 initialisé avec succès");

  return ESP_OK;

fail:
  /*
   * Séquence de rollback :
   *  - suppression du handler et du service ISR
   *  - reconfiguration de PIN_INT et TOUCH_PIN_RST en entrée pull-up
   */
  if (isr_handler_added)
    gpio_isr_handler_remove(PIN_INT);
  if (isr_service_installed)
    gpio_uninstall_isr_service();
  gpio_config_t rollback_conf = {
      .pin_bit_mask = (1ULL << PIN_INT) | (1ULL << TOUCH_PIN_RST),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  gpio_config(&rollback_conf);
  return ret;
}

void touch_driver_deinit(void) {
  if (touch_initialized) {
    gpio_isr_handler_remove(PIN_INT);
    gpio_uninstall_isr_service();
    if (gt911_dev) {
      i2c_master_bus_rm_device(gt911_dev);
      gt911_dev = NULL;
    }
    if (i2c_bus) {
      i2c_del_master_bus(i2c_bus);
      i2c_bus = NULL;
    }
    touch_initialized = false;
    ESP_LOGI(TAG, "Driver tactile désactivé");
  }
}

/**
 * @brief Active ou met en sommeil le GT911 en pilotant RST et INT.
 *
 * Lorsque @p enable vaut true, la broche RST est relâchée et INT est
 * configurée en entrée afin de réactiver les interruptions. Lorsque
 * @p enable vaut false, RST est maintenue à l'état bas et INT est forcée à 0
 * pour mettre le contrôleur en veille et bloquer les IRQ.
 */
void touch_set_enable(bool enable) {
  if (!touch_initialized)
    return;
  if (enable) {
    ESP_LOGI(TAG, "Tactile activé");

    /*
     * Séquence de réveil :
     *  - forcer INT à 0 pour éviter une interruption parasite
     *  - relâcher RST pour sortir le contrôleur du reset
     *  - reconfigurer INT en entrée (IRQ)
     */
    esp_err_t err = gpio_set_direction(PIN_INT, GPIO_MODE_OUTPUT);
    if (err != ESP_OK) {
      ESP_LOGE(TAG,
               "gpio_set_direction(PIN_INT, GPIO_MODE_OUTPUT) failed: %s",
               esp_err_to_name(err));
      return;
    }
    ESP_ERROR_CHECK(err);

    err = gpio_set_level(PIN_INT, 0);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "gpio_set_level(PIN_INT, 0) failed: %s",
               esp_err_to_name(err));
      return;
    }
    ESP_ERROR_CHECK(err);

    err = gpio_set_level(TOUCH_PIN_RST, 1);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "gpio_set_level(TOUCH_PIN_RST, 1) failed: %s",
               esp_err_to_name(err));
      return;
    }
    ESP_ERROR_CHECK(err);
    vTaskDelay(pdMS_TO_TICKS(50));
    err = gpio_set_direction(PIN_INT, GPIO_MODE_INPUT);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "gpio_set_direction(PIN_INT, GPIO_MODE_INPUT) failed: %s",
               esp_err_to_name(err));
      return;
    }
    ESP_ERROR_CHECK(err);
  } else {
    ESP_LOGI(TAG, "Tactile désactivé");

    /*
     * Mise en sommeil :
     *  - placer RST à 0 pour couper le contrôleur
     *  - maintenir INT à 0 afin de désactiver l'IRQ
     */
    esp_err_t err = gpio_set_direction(PIN_INT, GPIO_MODE_OUTPUT);
    if (err != ESP_OK) {
      ESP_LOGE(TAG,
               "gpio_set_direction(PIN_INT, GPIO_MODE_OUTPUT) failed: %s",
               esp_err_to_name(err));
      return;
    }
    ESP_ERROR_CHECK(err);

    err = gpio_set_level(PIN_INT, 0);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "gpio_set_level(PIN_INT, 0) failed: %s",
               esp_err_to_name(err));
      return;
    }
    ESP_ERROR_CHECK(err);

    err = gpio_set_level(TOUCH_PIN_RST, 0);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "gpio_set_level(TOUCH_PIN_RST, 0) failed: %s",
               esp_err_to_name(err));
      return;
    }
    ESP_ERROR_CHECK(err);
  }
}

esp_err_t touch_calibrate(void) {
  if (!touch_initialized) {
    return ESP_ERR_INVALID_STATE;
  }

  ESP_LOGI(TAG, "Calibration tactile - Non implémentée pour GT911");
  // Le GT911 est généralement auto-calibré

  return ESP_OK;
}
