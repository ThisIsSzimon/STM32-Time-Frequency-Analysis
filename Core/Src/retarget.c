#include <stdio.h>
#include "usart.h"   // teraz jest generowany przez CubeMX

int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}
