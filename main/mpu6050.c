#include "mpu6050.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_NUM I2C_NUM_0

// Write one byte to MPU6050
static esp_err_t mpu6050_write_byte(uint8_t reg_addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 50 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Read one byte from MPU6050
static esp_err_t mpu6050_read_byte(uint8_t reg_addr, uint8_t *data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 50 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Read two bytes from MPU6050 (for 16-bit values)
static esp_err_t mpu6050_read_word(uint8_t reg_addr, int16_t *data)
{
    uint8_t high, low;
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &high, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &low, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 50 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    *data = (int16_t)((high << 8) | low);
    return ret;
}

// Initialize MPU6050
esp_err_t mpu6050_init()
{
    printf("Initializing MPU6050...\n");
    
    // Wake up MPU6050 (clear sleep bit)
    esp_err_t ret = mpu6050_write_byte(MPU6050_PWR_MGMT_1, 0x00);
    if (ret != ESP_OK) {
        printf("Failed to wake up MPU6050\n");
        return ret;
    }
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    // Set accelerometer config (±2g range)
    ret = mpu6050_write_byte(MPU6050_ACCEL_CONFIG, 0x00);
    if (ret != ESP_OK) {
        printf("Failed to configure accelerometer\n");
        return ret;
    }
    
    // Set gyroscope config (±250°/s range)
    ret = mpu6050_write_byte(MPU6050_GYRO_CONFIG, 0x00);
    if (ret != ESP_OK) {
        printf("Failed to configure gyroscope\n");
        return ret;
    }
    
    printf("MPU6050 initialized successfully\n");
    return ESP_OK;
}

// Get WHO_AM_I register (should be 0x68)
uint8_t mpu6050_get_who_am_i()
{
    uint8_t who_am_i = 0;
    mpu6050_read_byte(MPU6050_WHO_AM_I, &who_am_i);
    return who_am_i;
}

// Read all accelerometer and gyroscope data
esp_err_t mpu6050_read_data(mpu6050_data_t *data)
{
    mpu6050_read_word(MPU6050_ACCEL_XOUT_H, &data->accel_x);
    mpu6050_read_word(MPU6050_ACCEL_YOUT_H, &data->accel_y);
    mpu6050_read_word(MPU6050_ACCEL_ZOUT_H, &data->accel_z);
    mpu6050_read_word(MPU6050_GYRO_XOUT_H, &data->gyro_x);
    mpu6050_read_word(MPU6050_GYRO_YOUT_H, &data->gyro_y);
    mpu6050_read_word(MPU6050_GYRO_ZOUT_H, &data->gyro_z);
    
    return ESP_OK;
}

// Detect motion based on acceleration magnitude
int mpu6050_detect_motion()
{
    mpu6050_data_t data;
    mpu6050_read_data(&data);
    
    // Calculate acceleration magnitude (simplified)
    // threshold = 5000 (out of 16000 for ±2g, so about 0.6g)
    int32_t accel_mag = (data.accel_x * data.accel_x) + 
                        (data.accel_y * data.accel_y) + 
                        (data.accel_z * data.accel_z);
    
    if (accel_mag > 5000000) {  // Threshold for motion detection
        return 1;  // Motion detected
    }
    return 0;  // No motion
}
