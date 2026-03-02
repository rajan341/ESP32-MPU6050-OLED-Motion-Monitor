#ifndef OLED_H
#define OLED_H

#include <stdint.h>

// OLED I2C address and dimensions
#define OLED_I2C_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_PAGES (OLED_HEIGHT / 8)

// Function prototypes
void oled_init(void);
void oled_clear(void);
void oled_write_string(uint8_t page, uint8_t col, const char *str);
void oled_display_number(uint8_t page, uint8_t col, int32_t num);
void oled_display_text(const char *str, uint8_t page);
void oled_set_cursor(uint8_t page, uint8_t col);
void oled_write_char(char c);

#endif