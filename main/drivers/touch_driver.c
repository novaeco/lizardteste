/**
 * @file touch_driver.c
 * @brief Implémentation du driver tactile GT911
 * @author NovaReptileElevage Team
 */

#include "touch_driver.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "Touch_Driver";

// Configuration des broches I2C (Waveshare ESP32-S3 Touch LCD 7")
#define PIN_SDA 8
#define PIN_SCL 9
#define PIN_INT 18
#define TOUCH_PIN_RST 17

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

// Préparation pour une gestion interruptive future
static void IRAM_ATTR touch_isr_handler(void *arg) {
  // TODO: implémenter la gestion de l'interruption tactile
}

/**
 * @brief Lit des données depuis le GT911 via I2C
 * @param reg_addr Adresse du registre
 * @param data Buffer pour les données lues
 * @param len Nombre d'octets à lire
 * @return esp_err_t Code d'erreur
 */
static esp_err_t gt911_read_reg(uint16_t reg_addr, uint8_t *data, size_t len) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  // Écriture de l'adresse du registre
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (GT911_ADDR << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, (reg_addr >> 8) & 0xFF, true);
  i2c_master_write_byte(cmd, reg_addr & 0xFF, true);

  // Lecture des données
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (GT911_ADDR << 1) | I2C_MASTER_READ, true);
  i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
  i2c_cmd_link_delete(cmd);

  return ret;
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
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (GT911_ADDR << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, (reg_addr >> 8) & 0xFF, true);
  i2c_master_write_byte(cmd, reg_addr & 0xFF, true);
  i2c_master_write(cmd, data, len, true);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
  i2c_cmd_link_delete(cmd);

  return ret;
}

/**
 * @brief Initialise le contrôleur GT911
 * @return esp_err_t Code d'erreur
 */
static esp_err_t gt911_init(void) {
  ESP_LOGI(TAG, "Initialisation du contrôleur GT911");

  // Reset du contrôleur
  gpio_set_level(TOUCH_PIN_RST, 0);
  vTaskDelay(pdMS_TO_TICKS(10));
  gpio_set_level(TOUCH_PIN_RST, 1);
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
  static bool touch_pressed = false;

  if (point_index >= total_points) {
    // Lecture du statut tactile
    uint8_t status;
    esp_err_t ret = gt911_read_reg(GT911_REG_STATUS, &status, 1);

    if (ret != ESP_OK) {
      data->state = LV_INDEV_STATE_REL;
      data->continue_reading = false;
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
        touch_pressed = true;
        last_x = points[0].x;
        last_y = points[0].y;

        ESP_LOGD(TAG, "Touch: points=%d", total_points);
      }

      // Effacement du statut pour préparer la prochaine lecture
      uint8_t clear = 0;
      gt911_write_reg(GT911_REG_STATUS, &clear, 1);
    } else {
      touch_pressed = false;
    }
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

esp_err_t touch_driver_init(void) {
  esp_err_t ret;

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
    return ret;
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
    return ret;
  }

  // Configuration I2C
  i2c_config_t i2c_conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = PIN_SDA,
      .scl_io_num = PIN_SCL,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_FREQUENCY,
  };

  ret = i2c_param_config(I2C_PORT, &i2c_conf);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Erreur configuration I2C");
    return ret;
  }

  ret = i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Erreur installation driver I2C");
    return ret;
  }

  // Initialisation du GT911
  ret = gt911_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Erreur initialisation GT911");
    i2c_driver_delete(I2C_PORT);
    return ret;
  }

  // Configuration du driver d'entrée LVGL
  lv_indev_t *indev = lv_indev_create();
  if (!indev) {
    ESP_LOGE(TAG, "Erreur création device tactile LVGL");
    i2c_driver_delete(I2C_PORT);
    return ESP_FAIL;
  }

  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touch_read);

  touch_initialized = true;
  ESP_LOGI(TAG, "Driver tactile GT911 initialisé avec succès");

  return ESP_OK;
}

void touch_driver_deinit(void) {
  if (touch_initialized) {
    i2c_driver_delete(I2C_PORT);
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
    gpio_set_direction(PIN_INT, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_INT, 0);
    gpio_set_level(TOUCH_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_direction(PIN_INT, GPIO_MODE_INPUT);
  } else {
    ESP_LOGI(TAG, "Tactile désactivé");

    /*
     * Mise en sommeil :
     *  - placer RST à 0 pour couper le contrôleur
     *  - maintenir INT à 0 afin de désactiver l'IRQ
     */
    gpio_set_direction(PIN_INT, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_INT, 0);
    gpio_set_level(TOUCH_PIN_RST, 0);
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
