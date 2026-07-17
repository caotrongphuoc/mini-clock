#ifndef __QMC5885L_H__
#define __QMC5885L_H__

#include <stdint.h>

#define QMC5885L_ADDR (0x0D)

// Registers
#define QMC5885L_REG_X_LSB (0x00)
#define QMC5885L_REG_STATUS (0x06)
#define QMC5885L_REG_CONTROL_1 (0x09)
#define QMC5885L_REG_CONTROL_2 (0x0A)
#define QMC5885L_REG_SET_RESET (0x0B)

// Control values
#define QMC5885L_MODE_CONTINUOUS (0x01)
#define QMC5885L_ODR_200HZ (0x0C)
#define QMC5885L_RNG_8G (0x10)
#define QMC5885L_OSR_512 (0x00)

typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;

} qmc5885l_raw_t;

void qmc5885l_init(void);

uint8_t qmc5885l_read(
    qmc5885l_raw_t* data);

float qmc5885l_get_heading(
    qmc5885l_raw_t* data);

#endif