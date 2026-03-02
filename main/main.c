#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "oled.h"
#include "mpu6050.h"

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

void i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM,
                       conf.mode,
                       0,
                       0,
                       0);
}

void app_main(void)
{
    i2c_master_init();
    
    oled_init();
    oled_clear();
    oled_display_text("MPU6050 INIT", 1);
    
    // Initialize MPU6050
    if (mpu6050_init() != ESP_OK) {
        oled_clear();
        oled_display_text("SENSOR ERROR", 1);
        printf("MPU6050 initialization failed\n");
        return;
    }
    
    // Verify WHO_AM_I
    uint8_t who_am_i = mpu6050_get_who_am_i();
    printf("MPU6050 WHO_AM_I: 0x%02X (should be 0x68)\n", who_am_i);
    
    oled_clear();
    oled_display_text("READY", 1);
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    oled_clear();
    oled_write_string(0, 0, "MOTION X:");
    oled_write_string(2, 0, "MOTION Y:");
    oled_write_string(4, 0, "MOTION Z:");
    oled_write_string(6, 0, "STATUS:");
    
    mpu6050_data_t sensor_data;
    int motion_count = 0;
    int16_t prev_ax = 0;
    int16_t prev_ay = 0;
    int16_t prev_az = 0;
    int first_sample = 1;
    char value_buf[12];
    
    while(1)
    {
        mpu6050_read_data(&sensor_data);

        snprintf(value_buf, sizeof(value_buf), "%6ld", (long)(sensor_data.accel_x / 100));
        oled_write_string(0, 50, "       ");
        oled_write_string(0, 50, value_buf);

        snprintf(value_buf, sizeof(value_buf), "%6ld", (long)(sensor_data.accel_y / 100));
        oled_write_string(2, 50, "       ");
        oled_write_string(2, 50, value_buf);

        snprintf(value_buf, sizeof(value_buf), "%6ld", (long)(sensor_data.accel_z / 100));
        oled_write_string(4, 50, "       ");
        oled_write_string(4, 50, value_buf);

        int motion_detected = 0;
        if (!first_sample) {
            int32_t delta_sum = abs(sensor_data.accel_x - prev_ax) +
                                abs(sensor_data.accel_y - prev_ay) +
                                abs(sensor_data.accel_z - prev_az);
            if (delta_sum > 500) {
                motion_detected = 1;
            }
        }

        prev_ax = sensor_data.accel_x;
        prev_ay = sensor_data.accel_y;
        prev_az = sensor_data.accel_z;
        first_sample = 0;

        oled_write_string(6, 40, "         ");
        if (motion_detected) {
            motion_count++;
            oled_write_string(6, 40, "MOTION!");
        } else {
            oled_write_string(6, 40, "CALM");
        }
        
        printf("AX:%d AY:%d AZ:%d Motion:%d\n", 
               sensor_data.accel_x, 
               sensor_data.accel_y, 
               sensor_data.accel_z,
               motion_count);
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}