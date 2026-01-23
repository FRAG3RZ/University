#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Basic RGB565 helpers
#define RGB565(r,g,b) (uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))

void SSD1963_Init(void);
void SSD1963_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void SSD1963_Fill(uint16_t rgb565);

#ifdef __cplusplus
}
#endif
