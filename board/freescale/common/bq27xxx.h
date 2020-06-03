/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __BQ27XXX_BOARD_HELPER__
#define __BQ27XXX_BOARD_HELPER__

#define POWER_OFF_VOL  3600 

enum battery_state{
	BAT_NORMAL,
	VOLTAGE_LOW,
	CAPACITY_LOW,
	CAP_VOL_LOW,
};
struct pmic *bq27xxx_common_init(unsigned char i2cbus);
int bq27xxx_get_bat_state(struct pmic *p,  enum battery_state  *bat_s);
#endif
