/*
 * Copyright (C) 2014 Gateworks Corporation
 * Tim Harvey <tharvey@gateworks.com>
 *
 * SPDX-License-Identifier:      GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <i2c.h>
#include <power/pmic.h>
#include <power/bq27xxx_pmic.h>

int power_bq27xxx_init(unsigned char bus)
{
	static const char name[] = "BQ27XXX";
	struct pmic *p = pmic_alloc();

	if (!p) {
		printf("%s: POWER allocation error!\n", __func__);
		return -ENOMEM;
	}

	p->name = name;
	p->interface = PMIC_I2C;
	p->number_of_regs = BQ27XXX_REG_MAX;
	p->hw.i2c.addr = CONFIG_POWER_BQ27XXX_I2C_ADDR;
	p->hw.i2c.tx_num = 1;
	p->sensor_byte_order=PMIC_SENSOR_BYTE_ORDER_BIG;
	p->bus = bus;

	return 0;
}
