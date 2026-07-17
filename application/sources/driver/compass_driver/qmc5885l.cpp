#include "qmc5885l.h"

#include "Wire.h"
#include <math.h>

static void qmc5885l_write(
    uint8_t reg,
    uint8_t value)
{
	Wire.beginTransmission(QMC5885L_ADDR);

	Wire.write(reg);
	Wire.write(value);

	Wire.endTransmission();
}

static uint8_t qmc5885l_read_regs(
    uint8_t reg,
    uint8_t* data,
    uint8_t len)
{
	Wire.beginTransmission(QMC5885L_ADDR);

	Wire.write(reg);

	if (Wire.endTransmission() != 0)
	{
		return 0;
	}

	uint8_t count =
	    Wire.requestFrom(
	        (uint8_t)QMC5885L_ADDR,
	        len);

	if (count != len)
	{
		return 0;
	}

	for (uint8_t i = 0; i < len; i++)
	{
		data[i] = Wire.read();
	}

	return 1;
}

void qmc5885l_init(void)
{
	/*
	 * Continuous measurement
	 *
	 * OSR = 512
	 * RNG = 8G
	 * ODR = 200Hz
	 */

	qmc5885l_write(
	    QMC5885L_REG_CONTROL_1,
	    QMC5885L_OSR_512 |
	        QMC5885L_ODR_200HZ |
	        QMC5885L_RNG_8G |
	        QMC5885L_MODE_CONTINUOUS);

	qmc5885l_write(
	    QMC5885L_REG_SET_RESET,
	    0x01);
}

uint8_t qmc5885l_read(
    qmc5885l_raw_t* data)
{
	uint8_t buf[6];

	if (!qmc5885l_read_regs(
	        QMC5885L_REG_X_LSB,
	        buf,
	        6))
	{
		return 0;
	}

	data->x =
	    (int16_t)((buf[1] << 8) |
	              buf[0]);

	data->y =
	    (int16_t)((buf[3] << 8) |
	              buf[2]);

	data->z =
	    (int16_t)((buf[5] << 8) |
	              buf[4]);

	return 1;
}

float qmc5885l_get_heading(
    qmc5885l_raw_t* data)
{
	float heading;

	heading =
	    atan2(
	        (float)data->y,
	        (float)data->x) *
	    180.0f / 3.14159265f;

	if (heading < 0)
	{
		heading += 360.0f;
	}

	return heading;
}