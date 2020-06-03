/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
//#define DEBUG
#include <common.h>
#include <errno.h>
#include <power/pmic.h>
#include <power/bq27xxx_pmic.h>
#include "../common/bq27xxx.h"

#ifndef CONFIG_DM_PMIC_BQ27XXX
#define BATTERY_435
static int  unseal_fullaccess_cfgupdate(struct pmic *p)
{
	int ret;
	//UNSEAL
	ret = pmic_reg_write(p,0,0x14);
	ret |= pmic_reg_write(p,1,0x04);
	ret |= pmic_reg_write(p,0,0x72);
	ret |= pmic_reg_write(p,1,0x36);

	//enter FULL ACCESS
	ret |= pmic_reg_write(p,0,0xFF);
	ret |= pmic_reg_write(p,1,0xFF);
	ret |= pmic_reg_write(p,0,0xFF);
	ret |= pmic_reg_write(p,1,0xFF);

	//Send ENTER_CGF_UPDATE command
	ret |= pmic_reg_write(p,0,0x90);
	ret |= pmic_reg_write(p,1,0x00);
	
	if(ret < 0)  {		
		debug("%s -- %d err \n",__FUNCTION__,__LINE__);
	}
	
	int temp=0;
	int i;
	for(i=0;i<6;i++)	{
	mdelay(500);
	ret |= pmic_reg_read(p, 0x3b,&temp);
	if(ret < 0)  {		
		debug("%s -- %d err \n",__FUNCTION__,__LINE__);
	}	
	debug(" OperationStatus()=%d  bit2 should set  \n",temp);
	if(temp >= 4)
		break;	
	}
	if(i == 6) {
		debug("%s -- %d err \n",__FUNCTION__,__LINE__);
	}
	
}

static int  seal_exitcfgupdate(struct pmic *p)
{
	int ret,i;
	int temp;
	//Exit CFGUPDATE mode
	ret |= pmic_reg_write(p, 0x0,0x91);
	ret |= pmic_reg_write(p, 0x1,0);	

	for(i=0;i<4;i++)	{
	mdelay(500);
	ret |= pmic_reg_read(p, 0x3b,&temp);
	if(ret < 0)  {		
		debug("%s -- %d err \n",__FUNCTION__,__LINE__);
	}	
	debug(" OperationStatus()=%d  bit2 should clear  \n",temp);
	if(temp < 4)
		break;	
	}

	//return to  SEALED
	ret |= pmic_reg_write(p, 0x0,0x30);
	ret |= pmic_reg_write(p, 0x1,0);

	return ret;
}

static void  display_dcp(struct pmic *p)
{
	int dc;
	dc = gauge_cmd_read(0, BQ27XXX_REG_DCAP);

	printf("Battery Design Capacity :%d mAh\n",dc);
}

static int bq27xxx_set_bat_cedv(struct pmic *p,int attr,int dcp)
{
	int ret;
	int dcp_msb,dcp_lsb;
	int attr_msb,attr_lsb;

	if(dcp < 0 || attr <0)
		return -1;
	dcp_msb = dcp >> 8;
	dcp_lsb  = dcp & 0xff;

	attr_msb = attr >> 8;
	attr_lsb  = attr & 0xff;
	int temp=0;

	//Write 0x9F to 0x3E to access the MSB of Design Capacity.
	ret |= pmic_reg_write(p, 0x3e,attr_lsb);
	//Write 0x92 to 0x3F to access the LSB of Design Capacity.
	ret |= pmic_reg_write(p, 0x3f,attr_msb);;
	
	//Read the 1-byte checksum
	int old_crc;
	ret |= pmic_reg_read(p, 0x60,&old_crc);
	if(ret < 0)  {		
		debug("%s -- %d err \n",__FUNCTION__,__LINE__);
	}	
	debug(" old checksum=0x%x  \n",old_crc);		

	//Read the 1-byte block length
	int blk_len;
	ret |= pmic_reg_read(p, 0x61,&blk_len);
	if(ret < 0)  {		
		debug("%s -- %d err \n",__FUNCTION__,__LINE__);
	}	
	debug(" block length=0x%X  \n",blk_len);	

#if 1 //bugfix
	//Write 0x9F to 0x3E to access the MSB of Design Capacity.
	ret |= pmic_reg_write(p, 0x3e,attr_lsb);
	//Write 0x92 to 0x3F to access the LSB of Design Capacity.
	ret |= pmic_reg_write(p, 0x3f,attr_msb);
#endif

	int old_dc_msb;
	ret |= pmic_reg_read(p, 0x40,&old_dc_msb);
	if(ret < 0)  {		
		debug("%s -- %d err \n",__FUNCTION__,__LINE__);
	}	
	debug(" Old_DC_MSB=0x%x  \n",old_dc_msb);	

	int old_dc_lsb;
	ret |= pmic_reg_read(p, 0x41,&old_dc_lsb);
	if(ret < 0)  {		
		debug("%s -- %d err \n",__FUNCTION__,__LINE__);
	}
	debug(" Old_DC_LSB=0x%x  \n",old_dc_lsb);	
	
	ret |= pmic_reg_write(p, 0x40,dcp_msb);
	ret |= pmic_reg_write(p, 0x41,dcp_lsb);	

	int new_crc;
	temp = ((255-old_crc-old_dc_msb -old_dc_lsb)%256);

	new_crc = 255-((temp+dcp_msb+dcp_lsb)%256) ;
	debug("new_crc=0x%x  \n",new_crc);	
	ret |= pmic_reg_write(p, 0x60,new_crc);	
	
	//	Write the block length.
	ret |= pmic_reg_write(p, 0x61,blk_len);	
	return 0;
}

int bq27xxx_set_bat(struct pmic *p)
{
	int ret;
	ret = unseal_fullaccess_cfgupdate(p);
//	mdelay(10000);
	ret |= bq27xxx_set_bat_cedv(p,0x929d,2500);	//FCC
	ret |= bq27xxx_set_bat_cedv(p,0x929f,2501);  	//DC  
	ret |= bq27xxx_set_bat_cedv(p,0x92a3,3800);	//Design Voltage  3.8v

	/*  */
	ret |= bq27xxx_set_bat_cedv(p,0x9201,160);	//taper current mA
	/**************************
		DOD   0%
		................
		DOD 100%
	***************************/
#ifndef BATTERY_435
	ret |= bq27xxx_set_bat_cedv(p,0x92bd,4167);
	ret |= bq27xxx_set_bat_cedv(p,0x92bf,4073);
	ret |= bq27xxx_set_bat_cedv(p,0x92c1,3989);	
	ret |= bq27xxx_set_bat_cedv(p,0x92c3,3912);	
	ret |= bq27xxx_set_bat_cedv(p,0x92c5,3841);	
	ret |= bq27xxx_set_bat_cedv(p,0x92c7,3803);
	ret |= bq27xxx_set_bat_cedv(p,0x92c9,3773);	
	ret |= bq27xxx_set_bat_cedv(p,0x92cb,3744);	
	ret |= bq27xxx_set_bat_cedv(p,0x92cd,3701);	
	ret |= bq27xxx_set_bat_cedv(p,0x92cf,3662);
	ret |= bq27xxx_set_bat_cedv(p,0x92d1,3041);
	
	ret |= bq27xxx_set_bat_cedv(p,0x92a7,3786);	 //EMF
	ret |= bq27xxx_set_bat_cedv(p,0x92A9,169);	 	//C0
	ret |= bq27xxx_set_bat_cedv(p,0x92AB,2193);	//R0
	ret |= bq27xxx_set_bat_cedv(p,0x92AD,4531);	//T0	
	ret |= bq27xxx_set_bat_cedv(p,0x92AF,756);	 	//R1
	ret |= bq27xxx_set_bat_cedv(p,0x92B1,9);	 	//TC
	ret |= bq27xxx_set_bat_cedv(p,0x92B2,0);		//C1
#else
	#if 1 
	/* charge voltage default 4.2v */
	ret |= bq27xxx_set_bat_cedv(p,0x91fd,4350);  	

	/* full charge voltage threshold 4.35,default 4.2v */
	ret |= bq27xxx_set_bat_cedv(p,0x9288,4350);  	
	/* clean FC voltage threshold 4.35,default 4.2v */
	ret |= bq27xxx_set_bat_cedv(p,0x928A,4220);  	

	/* set TC voltage threshold 4.35,default 4.2v */
	ret |= bq27xxx_set_bat_cedv(p,0x9294,4290);  	
	/* clr TC voltage threshold 4.25,default 4.2v */
	ret |= bq27xxx_set_bat_cedv(p,0x9296,4220);  	
	#endif

	ret |= bq27xxx_set_bat_cedv(p,0x92bd,4279);
	ret |= bq27xxx_set_bat_cedv(p,0x92bf,4161);
	ret |= bq27xxx_set_bat_cedv(p,0x92c1,4060);	
	ret |= bq27xxx_set_bat_cedv(p,0x92c3,3955);	
	ret |= bq27xxx_set_bat_cedv(p,0x92c5,3873);	
	ret |= bq27xxx_set_bat_cedv(p,0x92c7,3813);
	ret |= bq27xxx_set_bat_cedv(p,0x92c9,3769);	
	ret |= bq27xxx_set_bat_cedv(p,0x92cb,3733);	
	ret |= bq27xxx_set_bat_cedv(p,0x92cd,3697);	
	ret |= bq27xxx_set_bat_cedv(p,0x92cf,3647);
	ret |= bq27xxx_set_bat_cedv(p,0x92d1,3014);

	ret |= bq27xxx_set_bat_cedv(p,0x92a7,3804);	 //EMF
	ret |= bq27xxx_set_bat_cedv(p,0x92A9,146);	 	//C0
	ret |= bq27xxx_set_bat_cedv(p,0x92AB,3966);	//R0
	ret |= bq27xxx_set_bat_cedv(p,0x92AD,4378);	//T0	
	ret |= bq27xxx_set_bat_cedv(p,0x92AF,270);	 	//R1
	ret |= bq27xxx_set_bat_cedv(p,0x92B1,9);	 	//TC
	ret |= bq27xxx_set_bat_cedv(p,0x92B2,0);		//C1

#endif
	ret|=seal_exitcfgupdate(p);
	return ret;
}

int battery_check_gpc_cedv(struct pmic *p)
{
	int fcc_value,fdc_value;
	int i;
	for (i=0;i<10;i++)	{
	 fcc_value = gauge_cmd_read(0, BQ27XXX_REG_FCC);
	 printf("Battery:FCC %d mAh \n",fcc_value);	
	 
	 fdc_value = gauge_cmd_read(0, BQ27XXX_REG_DCAP);
	 printf("Battery:FDC %d mAh \n",fdc_value);	
	
	 if(fcc_value > 1000 )
	 	break;
	 else
	 	mdelay(100);
	}

	if(i == 10)
		printf("%s, error FCC = %d \n",__FUNCTION__,fcc_value);
/*
*    history:set
*    sn100 : 2500, 2502 
*    sn078 : 2408
*
*    default 3000; need set ; if == history set ,need update
*/
	if(fdc_value == 3000 || fdc_value == 2503 || fdc_value == 2408 )
		bq27xxx_set_bat(p);	
}

int bq27xxx_get_bat_state(struct pmic *p,  enum battery_state  *bat_s)
{
	int ret;
	int i; 
	int max_err = 10;

	int vol_shutdown_n = 0;
	int dis_charge_flag=0;
	int read_err=0;
	int battery_err=0;

	int volt,soc,chg_flag;
	int volt_not_match_soc=0;	
	int  reg;

reset_onetime_and_rechack:	
	
	/* 1. check  BQ27XXX_REG_VOLT  */
	for(i=0;i<max_err;i++) {
		volt = gauge_cmd_read(0, BQ27XXX_REG_VOLT);
		printf("Battery:VOLT %d mV\n",volt);
		if(volt > 3000  &&  volt < 4500)
			break;
		else
			mdelay(100);
	}
	if(i == max_err)
		return -1;
	
	/* 2. check  BQ27XXX_REG_SOC  */
	for(i=0;i<max_err;i++)  {
		soc = gauge_cmd_read(0, BQ27XXX_REG_SOC);
		printf("Battery:SOC %d %%\n",soc);		
		if( soc> 0 && soc<=100 )
			break;
		else if(soc == 0)     //sometimes soc value get slow.
			mdelay(300);
		else
			mdelay(100);
	}
	if(i == max_err && soc != 0)
		return -1;

	/* 3. check is BQ27XXX_REG_SOC   match BQ27XXX_REG_VOLT */
#ifndef BATTERY_435    
 if((volt > 3851 && soc < 3) || (volt > 4170 && soc < 85) || (volt < 3803 && soc > 80)  )        {
#else
 if((volt > 3851 && soc < 3) || (volt > 4250 && soc < 70) || (volt < 3603 && soc > 80)  )        {
#endif
		volt_not_match_soc++;	
		unseal_fullaccess_cfgupdate(p);	
		gauge_control(0, BQ27XXX_REG_RST);
		bq27xxx_set_bat(p); 
		mdelay(500);

		if(volt_not_match_soc == 1)
			goto reset_onetime_and_rechack;					
	}

	/* 4. if SOC low ,check is charging ... */
	if(soc >= POWER_OFF_CAP)	{
		*bat_s=BAT_NORMAL; 	
		return 0;		
	}		

	/* 5. is charging ... ,is can boot up ... */
	while(1)
	{
		reg = gauge_cmd_read(0, BQ27XXX_REG_FLAGS);
		if ((reg & 0xff) == 0xff || reg < 0)
		{
			printf("%s error =%d\n",__FUNCTION__, reg);
			battery_err++;
			if( battery_err >= 6)
			{
				return -1;
			}
			
			udelay(500000);
			continue;
		}

		if ( reg & BQ27XXX_FLAG_DSC )
			dis_charge_flag++ ;
		else
			dis_charge_flag = 0;

		soc = gauge_cmd_read(0, BQ27XXX_REG_SOC);
		if ( soc < 0 )	{
			read_err++;
			if(read_err >= 3 )
				return -1;
		}
		else
			read_err = 0;
		
		printf("battery_capacity : %d %%\n",soc);
		if(soc < POWER_OFF_CAP ) 
		{
			vol_shutdown_n++;
			 if(vol_shutdown_n >= 7 && dis_charge_flag >= 5) {
				*bat_s=CAPACITY_LOW;
				return 0;
			 }	
		} else {
			*bat_s=BAT_NORMAL;	
			return 0;
		}
	 	udelay(500000);
	}
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
