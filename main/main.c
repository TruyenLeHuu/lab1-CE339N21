#include <stdio.h>
#include "i2c_lib.h"
#include "hdc1080.h"
#include "esp_log.h"

#define BUTTON_PIN      ( 0 )
#define LED_PIN         ( 2 )

static const char *TAG = "app: "; 
static hdc1080_sensor_t* sensor;

void gpios_setup( void )
{
    gpio_pad_select_gpio(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);

    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level( LED_PIN, 0 ); 
}
void app_main(void)
{   
    /*setup gpio pin */
    gpios_setup();
    /*init i2c */
    i2c_init(I2C_BUS, I2C_SCL_PIN, I2C_SDA_PIN, I2C_FREQ);
    /*init sensor */
    sensor = hdc1080_init_sensor (I2C_BUS, HDC1080_ADDR);

    float temperature;
    float humidity;

    if (sensor)
    {
        hdc1080_registers_t registers = hdc1080_get_registers(sensor);
        ESP_LOGI(TAG, "Initialized HDC1080 sensor");
        registers.acquisition_mode = 1;
        
        /*Set mode for sensor*/
        hdc1080_set_registers(sensor, registers);
    }
    else
        printf("Could not initialize HDC1080 sensor\n");
    
    while (1)
    {   
        gpio_set_level(LED_PIN, 0);
        if (gpio_get_level(BUTTON_PIN) == 0)
        {   
            if (hdc1080_read(sensor, &temperature, &humidity)){
                ESP_LOGI(TAG, "HDC1080 Sensor periodic: temperature %.2f oC, humidity %.2f\n", temperature, humidity);
            }
            gpio_set_level(LED_PIN, 1);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
