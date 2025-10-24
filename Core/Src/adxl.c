#include "adxl.h"

/* Makra CS */
static inline void cs_low(ADXL_Handle *d)  { HAL_GPIO_WritePin(d->cs_port, d->cs_pin, GPIO_PIN_RESET); }
static inline void cs_high(ADXL_Handle *d) { HAL_GPIO_WritePin(d->cs_port, d->cs_pin, GPIO_PIN_SET); }

/* Pomocniczo: współczynnik LSB->g dla trybu 10-bit (FULL_RES=0) */
static float lsb_to_g_10bit(ADXL_Range_t r)
{
  switch (r) {
    case ADXL_RANGE_2G:  return 0.0039f;   // 3.9 mg/LSB
    case ADXL_RANGE_4G:  return 0.0078f;
    case ADXL_RANGE_8G:  return 0.0156f;
    case ADXL_RANGE_16G: return 0.0312f;
    default:             return 0.0039f;
  }
}

/* --- Niskopoziomowe --- */
bool ADXL_WriteReg(ADXL_Handle *dev, uint8_t reg, uint8_t val)
{
  uint8_t frame[2] = { (uint8_t)(reg & 0x3F), val }; // RW=0, MB=0
  cs_low(dev);
  HAL_StatusTypeDef st = HAL_SPI_Transmit(dev->hspi, frame, sizeof(frame), HAL_MAX_DELAY);
  cs_high(dev);
  return (st == HAL_OK);
}

bool ADXL_ReadRegs(ADXL_Handle *dev, uint8_t start_reg, uint8_t *buf, uint16_t len)
{
  uint8_t cmd = 0x80 | ((len > 1) ? 0x40 : 0x00) | (start_reg & 0x3F); // RW=1, MB=1 jeśli >1
  cs_low(dev);
  HAL_StatusTypeDef st = HAL_SPI_Transmit(dev->hspi, &cmd, 1, HAL_MAX_DELAY);
  if (st == HAL_OK) st = HAL_SPI_Receive(dev->hspi, buf, len, HAL_MAX_DELAY);
  cs_high(dev);
  return (st == HAL_OK);
}

/* --- Wysokopoziomowe --- */
uint8_t ADXL_ReadID(ADXL_Handle *dev)
{
  uint8_t id = 0;
  (void)ADXL_ReadRegs(dev, ADXL_REG_DEVID, &id, 1);
  return id;
}

bool ADXL_SetODR(ADXL_Handle *dev, ADXL_ODR_t odr)
{
  return ADXL_WriteReg(dev, ADXL_REG_BW_RATE, (uint8_t)odr);
}

bool ADXL_SetRange(ADXL_Handle *dev, ADXL_Range_t range, bool full_res)
{
  /* SPI=0 (4-wire), INT_INVERT=0, JUSTIFY=0, FULL_RES wg parametru, RANGE wg parametru */
  uint8_t fmt = (full_res ? 0x08 : 0x00) | (uint8_t)range;
  if (!ADXL_WriteReg(dev, ADXL_REG_DATA_FORMAT, fmt)) return false;

  dev->range   = range;
  dev->full_res = full_res;
  dev->lsb_to_g = full_res ? 0.0039f : lsb_to_g_10bit(range);
  return true;
}

bool ADXL_Init(ADXL_Handle *dev, ADXL_ODR_t odr, ADXL_Range_t range, bool full_res)
{
  /* CS w spoczynku w stanie wysokim */
  cs_high(dev);

  /* Sprawdź ID */
  if (ADXL_ReadID(dev) != ADXL_DEVID_EXPECTED) return false;

  /* Ustaw ODR */
  if (!ADXL_SetODR(dev, odr)) return false;

  /* Ustaw zakres i tryb rozdzielczości */
  if (!ADXL_SetRange(dev, range, full_res)) return false;

  /* Włącz pomiar (POWER_CTL.Measure=1) */
  if (!ADXL_WriteReg(dev, ADXL_REG_POWER_CTL, 0x08)) return false;

  return true;
}

bool ADXL_ReadXYZ_raw(ADXL_Handle *dev, int16_t *x, int16_t *y, int16_t *z)
{
  uint8_t b[6];
  if (!ADXL_ReadRegs(dev, ADXL_REG_DATAX0, b, 6)) return false;

  *x = (int16_t)((b[1] << 8) | b[0]);
  *y = (int16_t)((b[3] << 8) | b[2]);
  *z = (int16_t)((b[5] << 8) | b[4]);
  return true;
}

bool ADXL_ReadXYZ_g(ADXL_Handle *dev, float *gx, float *gy, float *gz)
{
  int16_t x, y, z;
  if (!ADXL_ReadXYZ_raw(dev, &x, &y, &z)) return false;

  *gx = x * dev->lsb_to_g;
  *gy = y * dev->lsb_to_g;
  *gz = z * dev->lsb_to_g;
  return true;
}