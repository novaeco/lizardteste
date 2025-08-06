/**
 * @file display_driver.c
 * @brief Implémentation du driver d'affichage ST7262
 * @author NovaReptileElevage Team
 */

#include "display_driver.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "Display_Driver";

// Configuration des broches (Waveshare ESP32-S3 Touch LCD 7")
#define PIN_MOSI        11
#define PIN_MISO        13
#define PIN_CLK         12
#define PIN_CS          10
#define PIN_DC          14
#define PIN_RST         21
#define PIN_BCKL        2

// Configuration SPI
#define DISPLAY_SPI_HOST SPI2_HOST
#define SPI_DMA_CHAN    SPI_DMA_CH_AUTO
#define SPI_FREQUENCY   40000000    // 40MHz

// Variables globales
static spi_device_handle_t spi_handle;
static lv_display_t *display;
static lv_color_t *buf1 = NULL;
static lv_color_t *buf2 = NULL;

/**
 * @brief Envoie une commande au contrôleur ST7262
 * @param cmd Commande à envoyer
 */
static void st7262_send_cmd(uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0; // D/C = 0 pour commande
    
    ret = spi_device_polling_transmit(spi_handle, &t);
    assert(ret == ESP_OK);
}

/**
 * @brief Envoie des données au contrôleur ST7262
 * @param data Pointeur vers les données
 * @param len Longueur des données
 */
static void st7262_send_data(const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    
    if (len == 0) return;
    
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*)1; // D/C = 1 pour données
    
    ret = spi_device_polling_transmit(spi_handle, &t);
    assert(ret == ESP_OK);
}

/**
 * @brief Callback pré-transmission pour contrôler la ligne D/C
 * @param t Transaction SPI
 */
static void st7262_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(PIN_DC, dc);
}

/**
 * @brief Initialise le contrôleur ST7262
 */
static void st7262_init(void)
{
    ESP_LOGI(TAG, "Initialisation du contrôleur ST7262");
    
    // Reset du contrôleur
    gpio_set_level(PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Séquence d'initialisation ST7262
    st7262_send_cmd(0x01); // Software Reset
    vTaskDelay(pdMS_TO_TICKS(150));
    
    st7262_send_cmd(0x11); // Sleep Out
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Configuration de l'interface de couleur (RGB565)
    st7262_send_cmd(0x3A);
    uint8_t color_mode = 0x55; // RGB565
    st7262_send_data(&color_mode, 1);
    
    // Configuration de l'orientation
    st7262_send_cmd(0x36);
    uint8_t madctl = 0x00; // Normal orientation
    st7262_send_data(&madctl, 1);
    
    // Configuration de la zone d'affichage
    st7262_send_cmd(0x2A); // Column Address Set
    uint8_t col_data[] = {0x00, 0x00, 0x03, 0x1F}; // 0 to 799
    st7262_send_data(col_data, 4);
    
    st7262_send_cmd(0x2B); // Page Address Set
    uint8_t page_data[] = {0x00, 0x00, 0x01, 0xDF}; // 0 to 479
    st7262_send_data(page_data, 4);
    
    // Activation de l'affichage
    st7262_send_cmd(0x29); // Display On
    vTaskDelay(pdMS_TO_TICKS(100));
    
    ESP_LOGI(TAG, "ST7262 initialisé (800x480 RGB565)");
}

/**
 * @brief Callback LVGL pour le flush de l'affichage
 * @param disp Display LVGL
 * @param area Zone à rafraîchir
 * @param color_p Données de couleur
 */
static void display_flush(lv_display_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
    
    // Configuration de la zone à écrire
    st7262_send_cmd(0x2A);
    uint8_t col_data[] = {
        (area->x1 >> 8) & 0xFF, area->x1 & 0xFF,
        (area->x2 >> 8) & 0xFF, area->x2 & 0xFF
    };
    st7262_send_data(col_data, 4);
    
    st7262_send_cmd(0x2B);
    uint8_t page_data[] = {
        (area->y1 >> 8) & 0xFF, area->y1 & 0xFF,
        (area->y2 >> 8) & 0xFF, area->y2 & 0xFF
    };
    st7262_send_data(page_data, 4);
    
    // Écriture des données de couleur
    st7262_send_cmd(0x2C);
    st7262_send_data((uint8_t*)color_p, size * 2);
    
    lv_display_flush_ready(disp);
}

/**
 * @brief Initialise le PWM pour le rétroéclairage
 */
static esp_err_t init_backlight(void)
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 5000,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    
    esp_err_t ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK) {
        return ret;
    }
    
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_0,
        .duty = 200, // ~80% brightness
        .gpio_num = PIN_BCKL,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0,
    };
    
    ret = ledc_channel_config(&ledc_channel);
    if (ret != ESP_OK) {
        return ret;
    }
    
    ESP_LOGI(TAG, "Rétroéclairage PWM initialisé");
    return ESP_OK;
}

esp_err_t display_driver_init(void)
{
    esp_err_t ret;
    
    ESP_LOGI(TAG, "Initialisation du driver d'affichage ST7262");
    
    // Configuration des GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_DC) | (1ULL << PIN_RST),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    // Configuration SPI
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_MISO,
        .mosi_io_num = PIN_MOSI,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = DISPLAY_BUF_SIZE * 2,
    };
    
    ret = spi_bus_initialize(DISPLAY_SPI_HOST, &buscfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation bus SPI");
        return ret;
    }
    
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = SPI_FREQUENCY,
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 7,
        .pre_cb = st7262_spi_pre_transfer_callback,
    };
    
    ret = spi_bus_add_device(DISPLAY_SPI_HOST, &devcfg, &spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur ajout device SPI");
        return ret;
    }
    
    // Initialisation du contrôleur ST7262
    st7262_init();
    
    // Initialisation du rétroéclairage
    ret = init_backlight();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur initialisation rétroéclairage");
        return ret;
    }
    
    // Allocation des buffers LVGL
    buf1 = heap_caps_malloc(DISPLAY_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    buf2 = heap_caps_malloc(DISPLAY_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    
    if (!buf1 || !buf2) {
        ESP_LOGE(TAG, "Erreur allocation buffers LVGL");
        return ESP_ERR_NO_MEM;
    }
    
    // Configuration LVGL
    display = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    if (!display) {
        ESP_LOGE(TAG, "Erreur création display LVGL");
        return ESP_FAIL;
    }
    
    lv_display_set_flush_cb(display, display_flush);
    lv_display_set_buffers(display, buf1, buf2, DISPLAY_BUF_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    
    ESP_LOGI(TAG, "Driver d'affichage ST7262 initialisé avec succès");
    return ESP_OK;
}

void display_driver_deinit(void)
{
    if (buf1) {
        free(buf1);
        buf1 = NULL;
    }
    if (buf2) {
        free(buf2);
        buf2 = NULL;
    }
    
    spi_bus_remove_device(spi_handle);
    spi_bus_free(DISPLAY_SPI_HOST);
    
    ESP_LOGI(TAG, "Driver d'affichage désactivé");
}

void display_set_brightness(uint8_t brightness)
{
    if (brightness > 100) brightness = 100;
    
    uint32_t duty = (brightness * 255) / 100;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    
    ESP_LOGI(TAG, "Luminosité définie à %d%%", brightness);
}

void display_set_sleep(bool sleep)
{
    if (sleep) {
        st7262_send_cmd(0x10); // Sleep In
        display_set_brightness(0);
        ESP_LOGI(TAG, "Écran en veille");
    } else {
        st7262_send_cmd(0x11); // Sleep Out
        vTaskDelay(pdMS_TO_TICKS(120));
        display_set_brightness(80);
        ESP_LOGI(TAG, "Écran réveillé");
    }
}