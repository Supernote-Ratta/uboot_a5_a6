/*
 * BQ27xxx battery driver
 */

#ifndef __BQ27XXX_PMIC_H_
#define __BQ27XXX_PMIC_H_

#define BQ27XXX_MANUFACTURER	"Texas Instruments"

/* BQ27XXX Flags */
#define BQ27XXX_FLAG_DSC	BIT(0)
#define BQ27XXX_FLAG_SOCF	BIT(1) /* State-of-Charge threshold final */
#define BQ27XXX_FLAG_SOC1	BIT(2) /* State-of-Charge threshold 1 */
#define BQ27XXX_FLAG_FC		BIT(9)
#define BQ27XXX_FLAG_OTD	BIT(14)
#define BQ27XXX_FLAG_OTC	BIT(15)
#define BQ27XXX_FLAG_UT		BIT(14)
#define BQ27XXX_FLAG_OT		BIT(15)

/* BQ27000 has different layout for Flags register */
#define BQ27000_FLAG_EDVF	BIT(0) /* Final End-of-Discharge-Voltage flag */
#define BQ27000_FLAG_EDV1	BIT(1) /* First End-of-Discharge-Voltage flag */
#define BQ27000_FLAG_CI		BIT(4) /* Capacity Inaccurate flag */
#define BQ27000_FLAG_FC		BIT(5)
#define BQ27000_FLAG_CHGS	BIT(7) /* Charge state flag */

#define BQ27XXX_RS			(20) /* Resistor sense mOhm */
#define BQ27XXX_POWER_CONSTANT		(29200) /* 29.2 µV^2 * 1000 */
#define BQ27XXX_CURRENT_CONSTANT	(3570) /* 3.57 µV * 1000 */

#define INVALID_REG_ADDR	0xff

/*
 * bq27xxx_reg_index - Register names
 *
 * These are indexes into a device's register mapping array.
 */

/* BQ27XXX registers */
enum  {
	BQ27XXX_REG_CTRL = 0x00,
	BQ27XXX_REG_TEMP = 0x06,
	BQ27XXX_REG_VOLT = 0x08,
	BQ27XXX_REG_RM = 0x10,  //RemainingCapacity()  mAh
	BQ27XXX_REG_AI = 0x14,     
	BQ27XXX_REG_FLAGS = 0x0a,
	BQ27XXX_REG_TTE = 0x16,
	BQ27XXX_REG_TTF = 0x18,
	BQ27XXX_REG_TTES = 0x1c,
	BQ27XXX_REG_NAC = 0x0c,  
	BQ27XXX_REG_FCC = 0x12,
	BQ27XXX_REG_CYCT = 0x2a,
	BQ27XXX_REG_SOC = 0x2c,
	BQ27XXX_REG_DCAP = 0x3c,
	BQ27XXX_REG_RST = 0x41,
	//BQ27XXX_REG_MAX= BQ27XXX_REG_RST +1 /* sentinel */
	BQ27XXX_REG_MAX= 0x62 /* sentinel */
};
#endif
