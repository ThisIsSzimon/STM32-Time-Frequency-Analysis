#ifndef ADXL_H
#define ADXL_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"   // lub odpowiedni nagłówek HAL dla Twojej serii

#ifdef __cplusplus
extern "C" {
#endif

/* --- Rejestry ADXL345 --- */
#define ADXL_REG_DEVID        0x00
#define ADXL_REG_BW_RATE      0x2C
#define ADXL_REG_POWER_CTL    0x2D
#define ADXL_REG_DATA_FORMAT  0x31
#define ADXL_REG_DATAX0       0x32   // ... do 0x37
#define ADXL_REG_INT_ENABLE   0x2E
#define ADXL_REG_INT_MAP      0x2F
#define ADXL_REG_INT_SOURCE   0x30
#define ADXL_REG_FIFO_CTL     0x38
#define ADXL_REG_FIFO_STATUS  0x39

/* --- Stałe --- */
#define ADXL_DEVID_EXPECTED   0xE5

/* --- Kody ODR (BW_RATE) --- */
typedef enum {
  ADXL_ODR_6_25HZ  = 0x06,
  ADXL_ODR_12_5HZ  = 0x07,
  ADXL_ODR_25HZ    = 0x08,
  ADXL_ODR_50HZ    = 0x09,
  ADXL_ODR_100HZ   = 0x0A,
  ADXL_ODR_200HZ   = 0x0B,
  ADXL_ODR_400HZ   = 0x0C,
  ADXL_ODR_800HZ   = 0x0D,
  ADXL_ODR_1600HZ  = 0x0E,
  ADXL_ODR_3200HZ  = 0x0F
} ADXL_ODR_t;

/* --- Zakres (DATA_FORMAT.RANGE) --- */
typedef enum {
  ADXL_RANGE_2G  = 0x00,
  ADXL_RANGE_4G  = 0x01,
  ADXL_RANGE_8G  = 0x02,
  ADXL_RANGE_16G = 0x03
} ADXL_Range_t;

/* --- Uchwyt urządzenia --- */
typedef struct {
  SPI_HandleTypeDef *hspi;
  GPIO_TypeDef      *cs_port;
  uint16_t           cs_pin;

  /* informacyjne / pomocnicze */
  bool               full_res;   // DATA_FORMAT.FULL_RES
  ADXL_Range_t       range;
  float              lsb_to_g;   // współczynnik przeliczenia
} ADXL_Handle;

/* --- API wysokiego poziomu --- */
bool ADXL_Init(ADXL_Handle *dev, ADXL_ODR_t odr, ADXL_Range_t range, bool full_res);
bool ADXL_SetODR(ADXL_Handle *dev, ADXL_ODR_t odr);
bool ADXL_SetRange(ADXL_Handle *dev, ADXL_Range_t range, bool full_res);
bool ADXL_ReadXYZ_raw(ADXL_Handle *dev, int16_t *x, int16_t *y, int16_t *z);
bool ADXL_ReadXYZ_g(ADXL_Handle *dev, float *gx, float *gy, float *gz);
bool ADXL_EnableDataReady(ADXL_Handle *dev, bool enable);
bool ADXL_ReadXYZ_wait_dr(ADXL_Handle *dev, int16_t *x, int16_t *y, int16_t *z, uint32_t timeout_ms);
bool ADXL_FifoStreamEnable(ADXL_Handle *dev, uint8_t watermark_samples);
int  ADXL_FifoReadSamples(ADXL_Handle *dev, int16_t *xyz_buf, int max_samples);
uint8_t ADXL_ReadID(ADXL_Handle *dev);

/* --- Niskopoziomowe (przydatne czasem) --- */
bool ADXL_WriteReg(ADXL_Handle *dev, uint8_t reg, uint8_t val);
bool ADXL_ReadRegs(ADXL_Handle *dev, uint8_t start_reg, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif /* ADXL_H */
