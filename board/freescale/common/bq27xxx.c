/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <errno.h>
#include <power/pmic.h>
#include <power/bq27xxx_pmic.h>
#include "../common/bq27xxx.h"

#ifndef CONFIG_DM_PMIC_BQ27XXX

int bq27xxx_get_bat_state(struct pmic *p,  enum battery_state  *bat_s)
{
	int ret;
	unsigned int  reg;
	int vol_shutdown_n = 0;
	int dis_charge_flag=0;

	while(1)
	{
		ret = pmic_reg_read(p, BQ27XXX_REG_FLAGS, &reg);
		if ((reg & 0xff) == 0xff || ret < 0)
		{
			printf("%s error =%d\n",__FUNCTION__, reg);
			return -1;
		}

		if ( reg & BQ27XXX_FLAG_FC || reg & BQ27XXX_FLAG_DSC )
			dis_charge_flag++ ;
		else
			dis_charge_flag = 0;

//		printf("%s flag =%d , dis_charge_flag = %d \n",__FUNCTION__, reg,dis_charge_flag);		
		ret = pmic_reg_read(p, BQ27XXX_REG_VOLT, &reg);
		if ( ret < 0 )
			return -1;
		
		printf("battery voltage = %d \n",reg);
		if(reg < POWER_OFF_VOL ) 
		{
			vol_shutdown_n++;
			 if(vol_shutdown_n >= 7 && dis_charge_flag >= 5) {
				*bat_s=VOLTAGE_LOW;
				return 0;
			 }	
		} else {
			vol_shutdown_n = 0 ;
		}
		
		if( 0  ==   vol_shutdown_n )
			break;

	 	udelay(500000);
	}

	*bat_s=BAT_NORMAL; 	
	return 0;
}

struct pmic *bq27xxx_common_init(unsigned char i2cbus)
{
	struct pmic *p;
	int ret;
	ret = power_bq27xxx_init(i2cbus);
	if (ret)
		return NULL;

	p = pmic_get("BQ27XXX");
	ret = pmic_probe(p);
	if (ret)
		return NULL;
	
	return p;
}
#endif
