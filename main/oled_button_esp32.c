#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <intr_types.h>
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "frames.h"
#define BUTTON_UP 27
#define BUTTON_RIGHT 12
#define BUTTON_SELECT 26
#define BUTTON_LEFT 33
#define BUTTON_DOWN 32
#define LED 2
#define WITH_VEHICLE 1
static const char *TAG = "Debug";
bool enableBlink = false;
bool enableIncrease = true;
uint8_t button_state = 0x00; // 0000 0010
#define button_up (button_state & 0x10)
#define button_right (button_state & 0x02)
#define button_select (button_state & 0x01)
#define button_left (button_state & 0x04)
#define button_down (button_state & 0x08)

void configure_gpio(void);
void button_task(void *pvParameter);
void screen_task(void *pvParameter);
void app_main(void)
{
    // xTaskCreate(test_task, "test", 2048, NULL, 5, NULL);
    configure_gpio();
    xTaskCreate(button_task, "button", 2048, NULL, 5, NULL);
    xTaskCreate(screen_task, "screen", 8 * 1024, NULL, 5, NULL);
}
void screen_task(void *pvParameter)
{
    SSD1306_t dev;
#if CONFIG_I2C_INTERFACE
    ESP_LOGI(TAG, "INTERFACE is i2c");
    ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d", CONFIG_SDA_GPIO);
    ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d", CONFIG_SCL_GPIO);
    ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d", CONFIG_RESET_GPIO);
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE
#if CONFIG_SSD1306_128x64
    ESP_LOGI(TAG, "Panel is 128x64");
    ssd1306_init(&dev, 128, 64);
#endif // CONFIG_SSD1306_128x64
    uint8_t *wk = (uint8_t *)malloc(1024);
    if (wk == NULL)
    {
        ESP_LOGE(TAG, "malloc failed");
        while (1)
        {
            vTaskDelay(1);
        }
    }

    // Allocate memory for save frame
    uint8_t *save = (uint8_t *)malloc(1024);
    if (save == NULL)
    {
        ESP_LOGE(TAG, "malloc failed");
        while (1)
        {
            vTaskDelay(1);
        }
    }

    // Allocate memory for background frame
    uint8_t *buffer = (uint8_t *)malloc(1024 * 2);
    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "malloc failed");
        while (1)
        {
            vTaskDelay(1);
        }
    }

    // Allocate memory for vehicle frame
    uint8_t *vehicle = (uint8_t *)malloc(1024 * 3);
    if (vehicle == NULL)
    {
        ESP_LOGE(TAG, "malloc failed");
        while (1)
        {
            vTaskDelay(1);
        }
    }
    int threading = 0;
    bool config = true;
    ssd1306_contrast(&dev, 0xff);
    gpio_set_level(LED, 0);
    int frameIndex = 1;
    int page = 0;
    int vehicleHorizontalPosition[3] = {20, 52, 84};
    int vehicleVerticalPosition[3] = {-8, 1, -14};
    int vehicleVerticalMove[3] = {1, 0, 1};
    while (1)
    {
        if (true)
        {
            ssd1306_wrap_arround(&dev, PAGE_SCROLL_DOWN, 0, 0, -1);
            vTaskDelay(1);
            if (threading == 0)
            {
                if (config)
                {
                    gpio_set_level(LED, 0);
                    ssd1306_clear_screen(&dev, false);
                    ssd1306_bitmaps(&dev, 0, 0, vehicle1, 24, 48, true);
                    ssd1306_get_buffer(&dev, &vehicle[1024 * 0]);
                    vTaskDelay(pdMS_TO_TICKS(3000));
                    gpio_set_level(LED, 1);
                    config = false;
                }

                if (button_select)
                {
                    threading = 1;
                    config = true;
                }
            }
            else if (threading == 1)
            {
                if (config)
                {
                    gpio_set_level(LED, 0);
                    ssd1306_clear_screen(&dev, false);
                    ssd1306_bitmaps(&dev, 0, 0, vehicle2, 24, 48, true);
                    ssd1306_get_buffer(&dev, &vehicle[1024 * 1]);
                    vTaskDelay(pdMS_TO_TICKS(3000));
                    gpio_set_level(LED, 1);
                    config = false;
                }

                if (button_select)
                {
                    threading = 2;
                    config = true;
                }
            }
            else if (threading == 2)
            {
                if (config)
                {
                    gpio_set_level(LED, 0);
                    ssd1306_clear_screen(&dev, false);
                    ssd1306_bitmaps(&dev, 0, 0, vehicle3, 24, 48, true);
                    ssd1306_get_buffer(&dev, &vehicle[1024 * 2]);
                    vTaskDelay(pdMS_TO_TICKS(3000));
                    gpio_set_level(LED, 1);
                    config = false;
                }

                if (button_select)
                {
                    threading = 3;
                    config = true;
                }
            }
            else if (threading == 3)
            {
                if (config)
                {
                    gpio_set_level(LED, 0);
                    ssd1306_clear_screen(&dev, false);
                    ssd1306_bitmaps(&dev, 0, 0, highway_width1, 128, 64, false);
                    ssd1306_get_buffer(&dev, &buffer[1024 * 0]);
                    vTaskDelay(pdMS_TO_TICKS(3000));
                    gpio_set_level(LED, 1);
                    config = false;
                }

                if (button_select)
                {
                    threading = 4;
                    config = true;
                }
            }
            else if (threading == 4)
            {
                if (config)
                {
                    gpio_set_level(LED, 0);
                    ssd1306_clear_screen(&dev, false);
                    ssd1306_bitmaps(&dev, 0, 0, highway_width2, 128, 64, false);
                    ssd1306_get_buffer(&dev, &buffer[1024 * 1]);
                    vTaskDelay(pdMS_TO_TICKS(3000));
                    config = false;
                    gpio_set_level(LED, 1);
                }

                if (button_select)
                {
                    threading = 5;
                    config = true;
                }
            }
            else if (threading == 5)
            {
                if (config)
                {
                    gpio_set_level(LED, 0);
                    // Show initial background
                    ssd1306_set_buffer(&dev, &buffer[1024 * 0]);
                    ssd1306_show_buffer(&dev);
                    vTaskDelay(pdMS_TO_TICKS(3000));
                    gpio_set_level(LED, 1);
                    config = false;
                }

                if (button_select)
                {
                    threading = 6;
                    config = true;
                }
            }
            else if (threading == 6)
            {
                if (config)
                {
                    gpio_set_level(LED, 0);
                    // Show initial background
                    ssd1306_set_buffer(&dev, &buffer[1024 * 1]);
                    ssd1306_show_buffer(&dev);
                    vTaskDelay(pdMS_TO_TICKS(3000));
                    gpio_set_level(LED, 1);
                    config = false;
                }

                if (button_select)
                {
                    threading = 7;
                    config = true;
                }
            }
            else if (threading == 7)
            {
                if (config)
                {
                    gpio_set_level(LED, 0);
                    ssd1306_wrap_arround(&dev, PAGE_SCROLL_DOWN, 0, 0, -1);

                    // Get from internal buffer to local buffer
                    // buffer is [8][128] 8 page 128 pixel
                    ssd1306_get_buffer(&dev, wk);

                    // Update buffer
                    int frameOffset = frameIndex * 1024;
                    int dst_index = 0;
                    int src_index = (7 - page) * 128 + frameOffset;
                    // printf("frameIndex=%d page=%d src_index=%d\n", frameIndex, page, src_index);
                    for (int seg = 0; seg < 128; seg++)
                    {
                        wk[dst_index++] = buffer[src_index++];
                    }

                    // Set frame without vehicle
                    ssd1306_set_buffer(&dev, wk);

#if WITH_VEHICLE
                    // Get frame without vehicle
                    ssd1306_get_buffer(&dev, save);

                    // Set vehicle image
                    for (int vehicleFrame = 0; vehicleFrame < 3; vehicleFrame++)
                    {
                        int startPage = vehicleVerticalPosition[vehicleFrame];
                        int endPage = startPage + 6;
                        ESP_LOGD(TAG, "page=%d vehicleVerticalPosition1=%d startPage=%d endPage=%d",
                                page, vehicleVerticalPosition[vehicleFrame], startPage, endPage);
                        int vehicleIndex = vehicleFrame * 1024;
                        int vehicleOffset = 0;
                        for (int _page = startPage; _page < endPage; _page++)
                        {
                            for (int seg = 0; seg < 24; seg++)
                            {
                                int bufferIndex = _page * 128 + seg + vehicleHorizontalPosition[vehicleFrame];
                                if (bufferIndex >= 0 && bufferIndex < 1023)
                                    wk[bufferIndex] = vehicle[vehicleIndex + vehicleOffset * 128 + seg];
                            }
                            vehicleOffset++;
                        }
                    }

                    ssd1306_set_buffer(&dev, wk);
#endif

                    // Show internal buffer
                    ssd1306_show_buffer(&dev);

#if WITH_VEHICLE
                    // Set frame without vehicle
                    ssd1306_set_buffer(&dev, save);
#endif
                    page++;
                    if (page > 7)
                    {
                        page = 0;
                        for (int vehicleFrame = 0; vehicleFrame < 3; vehicleFrame++)
                        {
                            vehicleVerticalPosition[vehicleFrame] = vehicleVerticalPosition[vehicleFrame] + vehicleVerticalMove[vehicleFrame];
                            if (vehicleVerticalPosition[vehicleFrame] > 8)
                            {
                                vehicleVerticalPosition[vehicleFrame] = -8;
                            }
                        }
                        frameIndex++;
                        if (frameIndex > 1)
                            frameIndex = 0;
                    }
                    gpio_set_level(LED, 1);
                    // config = false;
                    // vTaskDelay(pdMS_TO_TICKS(200));
                }
                // if (button_select)
                // {
                //     vTaskDelay(pdMS_TO_TICKS(20));
                //     config = true;
                //     while (button_select)
                //         ;
                // }
            }
        }
    }
}

void button_task(void *pvParameter)
{
    while (true)
    {
        button_state = (!gpio_get_level(BUTTON_UP) << 4) | (!gpio_get_level(BUTTON_DOWN) << 3) | (!gpio_get_level(BUTTON_LEFT) << 2) | (!gpio_get_level(BUTTON_RIGHT) << 1) | !gpio_get_level(BUTTON_SELECT);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
void configure_gpio(void)
{
    // Configure output pin
    gpio_config_t output_config = {
        .pin_bit_mask = (1ULL << LED),         // Bit mask of the pin
        .mode = GPIO_MODE_OUTPUT,              // Set as output mode
        .pull_up_en = GPIO_PULLUP_DISABLE,     // Disable pull-up resistor
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // Disable pull-down resistor
        .intr_type = GPIO_INTR_DISABLE         // No interrupts
    };
    gpio_config(&output_config);

    // Configure input pin
    gpio_config_t input_config = {
        .pin_bit_mask = (1ULL << BUTTON_UP) | (1ULL << BUTTON_LEFT) | (1ULL << BUTTON_SELECT) | (1ULL << BUTTON_RIGHT) | (1ULL << BUTTON_DOWN), // Bit mask of the pin
        .mode = GPIO_MODE_INPUT,                                                                                                                // Set as input mode
        .pull_up_en = GPIO_PULLUP_ENABLE,                                                                                                       // Enable pull-up resistor
        .pull_down_en = GPIO_PULLDOWN_DISABLE,                                                                                                  // Disable pull-down resistor
        .intr_type = GPIO_INTR_DISABLE                                                                                                          // No interrupts
    };
    gpio_config(&input_config);
}