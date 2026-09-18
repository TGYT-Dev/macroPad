#pragma once

// OLED (SSD1306) over I2C
// Confirmed wiring: OLED SDA -> XIAO D4 (GP6), OLED SCL -> XIAO D5 (GP7)
// GP6/GP7 form a valid RP2040 I2C1 SDA/SCL pair.
#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP6
#define I2C1_SCL_PIN GP7

// Debounce (ms) for direct-wired switches
#define DEBOUNCE 5

// Vial required defines
