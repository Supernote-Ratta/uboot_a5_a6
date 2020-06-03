/*
 * Copyright (C) 2013 - 2016 Freescale Semiconductor, Inc.
 *
 * Author: Fabio Estevam <fabio.estevam@freescale.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
//#define DEBUG
#include <asm/arch/clock.h>
#include <asm/arch/iomux.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx6-ddr.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/spi.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <common.h>
#include <fsl_esdhc.h>
#include <i2c.h>
#include <mmc.h>
#include <netdev.h>
#include <power/pmic.h>
#include <power/pfuze100_pmic.h>
#include "../common/pfuze.h"
#include "../common/bq27xxx.h"
#include <usb.h>
#include <usb/ehci-fsl.h>
#include <splash.h>
#if defined(CONFIG_MXC_EPDC)
#include <lcd.h>
#include <mxc_epdc_fb.h>
#endif
#ifdef CONFIG_FSL_FASTBOOT
#include <fsl_fastboot.h>
#ifdef CONFIG_ANDROID_RECOVERY
#include <recovery.h>
#endif
#endif /*CONFIG_FSL_FASTBOOT*/

DECLARE_GLOBAL_DATA_PTR;

/*uart 1.8v sn078*/
#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |PAD_CTL_LVE_BIT |	\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_22K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define ENET_PAD_CTRL  (PAD_CTL_PKE | PAD_CTL_PUE |             \
	PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED   |             \
	PAD_CTL_DSE_40ohm   | PAD_CTL_HYS)

#define SPI_PAD_CTRL (PAD_CTL_HYS | PAD_CTL_SPEED_MED | \
		      PAD_CTL_DSE_40ohm | PAD_CTL_SRE_FAST)

#define I2C_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE |		\
		      PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |	\
		      PAD_CTL_DSE_40ohm | PAD_CTL_HYS |		\
		      PAD_CTL_ODE | PAD_CTL_SRE_FAST)

#define OTGID_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE |		\
			PAD_CTL_PUS_47K_UP | PAD_CTL_SPEED_LOW |\
			PAD_CTL_DSE_80ohm | PAD_CTL_HYS |	\
			PAD_CTL_SRE_FAST)

#define ELAN_INTR_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE | \
			    PAD_CTL_PUS_47K_UP | PAD_CTL_HYS)

#define EPDC_PAD_CTRL    (PAD_CTL_PKE | PAD_CTL_SPEED_MED |	\
	PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define ETH_PHY_POWER	IMX_GPIO_NR(4, 21)

int dram_init(void)
{
	gd->ram_size = get_ram_size((void *)PHYS_SDRAM, PHYS_SDRAM_SIZE);

	return 0;
}

static iomux_v3_cfg_t const uart1_pads[] = {
	MX6_PAD_UART1_TXD__UART1_TXD | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART1_RXD__UART1_RXD | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc1_pads[] = {
#if 0	
	/* 8 bit SD */
	MX6_PAD_SD1_CLK__USDHC1_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_CMD__USDHC1_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT0__USDHC1_DAT0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT1__USDHC1_DAT1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT2__USDHC1_DAT2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT3__USDHC1_DAT3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT4__USDHC1_DAT4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT5__USDHC1_DAT5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT6__USDHC1_DAT6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DAT7__USDHC1_DAT7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	/*CD pin*/
	MX6_PAD_KEY_ROW7__GPIO_4_7 | MUX_PAD_CTRL(NO_PAD_CTRL),
#endif
};

#if 0
static iomux_v3_cfg_t const usdhc2_pads[] = {
	MX6_PAD_SD2_CLK__USDHC2_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_CMD__USDHC2_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT0__USDHC2_DAT0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT1__USDHC2_DAT1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT2__USDHC2_DAT2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT3__USDHC2_DAT3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	/*CD pin*/
	MX6_PAD_SD2_DAT7__GPIO_5_0 | MUX_PAD_CTRL(NO_PAD_CTRL),
};
#else
/* EMMC */
static iomux_v3_cfg_t const usdhc2_pads[] = {
	MX6_PAD_SD2_CLK__USDHC2_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_CMD__USDHC2_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT0__USDHC2_DAT0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT1__USDHC2_DAT1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT2__USDHC2_DAT2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT3__USDHC2_DAT3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT4__USDHC2_DAT4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT5__USDHC2_DAT5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT6__USDHC2_DAT6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT7__USDHC2_DAT7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	/* RST_B */
	MX6_PAD_SD2_RST__USDHC2_RST | MUX_PAD_CTRL(USDHC_PAD_CTRL | PAD_CTL_LVE),
};
#endif

static iomux_v3_cfg_t const usdhc3_pads[] = {
	MX6_PAD_SD3_CLK__USDHC3_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_CMD__USDHC3_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT0__USDHC3_DAT0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT1__USDHC3_DAT1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT2__USDHC3_DAT2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT3__USDHC3_DAT3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	/*CD pin*/
	MX6_PAD_REF_CLK_32K__GPIO_3_22 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static iomux_v3_cfg_t const fec_pads[] = {
	MX6_PAD_FEC_MDC__FEC_MDC | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_MDIO__FEC_MDIO | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_CRS_DV__FEC_RX_DV | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_RXD0__FEC_RX_DATA0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_RXD1__FEC_RX_DATA1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_TX_EN__FEC_TX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_TXD0__FEC_TX_DATA0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_TXD1__FEC_TX_DATA1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_REF_CLK__FEC_REF_OUT | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_RX_ER__GPIO_4_19 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_FEC_TX_CLK__GPIO_4_21 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static iomux_v3_cfg_t const elan_pads[] = {
	MX6_PAD_KEY_COL6__GPIO_4_4 | MUX_PAD_CTRL(EPDC_PAD_CTRL),
};

static iomux_v3_cfg_t const epdc_enable_pads[] = {
	MX6_PAD_EPDC_D0__EPDC_SDDO_0	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D1__EPDC_SDDO_1	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D2__EPDC_SDDO_2	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D3__EPDC_SDDO_3	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D4__EPDC_SDDO_4	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D5__EPDC_SDDO_5	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D6__EPDC_SDDO_6	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D7__EPDC_SDDO_7	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D8__EPDC_SDDO_8	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D9__EPDC_SDDO_9	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D10__EPDC_SDDO_10	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D11__EPDC_SDDO_11	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D12__EPDC_SDDO_12	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D13__EPDC_SDDO_13	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D14__EPDC_SDDO_14	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_D15__EPDC_SDDO_15	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_GDCLK__EPDC_GDCLK	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_GDSP__EPDC_GDSP	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_GDOE__EPDC_GDOE	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_SDCLK__EPDC_SDCLK	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_SDOE__EPDC_SDOE	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_SDLE__EPDC_SDLE	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_BDR0__EPDC_BDR_0	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_SDCE0__EPDC_SDCE_0	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
#if 0
	MX6_PAD_EPDC_SDSHR__EPDC_SDSHR	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_GDRL__EPDC_GDRL	| MUX_PAD_CTRL(EPDC_PAD_CTRL),	
	MX6_PAD_EPDC_SDCE1__EPDC_SDCE_1	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
	MX6_PAD_EPDC_SDCE2__EPDC_SDCE_2	| MUX_PAD_CTRL(EPDC_PAD_CTRL),
#endif
};

static iomux_v3_cfg_t const epdc_disable_pads[] = {	
	MX6_PAD_EPDC_D0__GPIO_1_7,
	MX6_PAD_EPDC_D1__GPIO_1_8,
	MX6_PAD_EPDC_D2__GPIO_1_9,
	MX6_PAD_EPDC_D3__GPIO_1_10,
	MX6_PAD_EPDC_D4__GPIO_1_11,
	MX6_PAD_EPDC_D5__GPIO_1_12,
	MX6_PAD_EPDC_D6__GPIO_1_13,
	MX6_PAD_EPDC_D7__GPIO_1_14,
	MX6_PAD_EPDC_D8__GPIO_1_15,
	MX6_PAD_EPDC_D9__GPIO_1_16,
	MX6_PAD_EPDC_D10__GPIO_1_17,
	MX6_PAD_EPDC_D11__GPIO_1_18,
	MX6_PAD_EPDC_D12__GPIO_1_19,
	MX6_PAD_EPDC_D13__GPIO_1_20,
	MX6_PAD_EPDC_D14__GPIO_1_21,
	MX6_PAD_EPDC_D15__GPIO_1_22,	
	MX6_PAD_EPDC_GDCLK__GPIO_1_31,
	MX6_PAD_EPDC_GDSP__GPIO_2_2,
	MX6_PAD_EPDC_GDOE__GPIO_2_0,
	MX6_PAD_EPDC_SDCLK__GPIO_1_23,
	MX6_PAD_EPDC_SDOE__GPIO_1_25,
	MX6_PAD_EPDC_SDLE__GPIO_1_24,
	MX6_PAD_EPDC_BDR0__GPIO_2_5,
	MX6_PAD_EPDC_SDCE0__GPIO_1_27,
#if 0
	MX6_PAD_EPDC_SDSHR__GPIO_1_26,
	MX6_PAD_EPDC_GDRL__GPIO_2_1,
	MX6_PAD_EPDC_SDCE1__GPIO_1_28,
	MX6_PAD_EPDC_SDCE2__GPIO_1_29,
#endif
};

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
}

static void setup_iomux_fec(void)
{
	imx_iomux_v3_setup_multiple_pads(fec_pads, ARRAY_SIZE(fec_pads));

	/* Power up LAN8720 PHY */
	gpio_direction_output(ETH_PHY_POWER , 1);
	udelay(15000);
}

#define USDHC1_CD_GPIO	IMX_GPIO_NR(4, 7)
#define USDHC2_CD_GPIO	IMX_GPIO_NR(5, 0)
#define USDHC3_CD_GPIO	IMX_GPIO_NR(3, 22)

static struct fsl_esdhc_cfg usdhc_cfg[3] = {
	{USDHC1_BASE_ADDR},
	{USDHC2_BASE_ADDR, 0, 4},
	{USDHC3_BASE_ADDR, 0, 4},
};

int board_mmc_get_env_dev(int devno)
{
	return devno;
}

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
	case USDHC1_BASE_ADDR:
		ret = !gpio_get_value(USDHC1_CD_GPIO);
		break;
	case USDHC2_BASE_ADDR:
		ret = !gpio_get_value(USDHC2_CD_GPIO);
		break;
	case USDHC3_BASE_ADDR:
		ret = !gpio_get_value(USDHC3_CD_GPIO);
		break;
	}

	return ret;
}

int board_mmc_init(bd_t *bis)
{
#ifndef CONFIG_SPL_BUILD
	int i, ret;

	/*
	 * According to the board_mmc_init() the following map is done:
	 * (U-Boot device node)    (Physical Port)
	 * mmc0                    USDHC1
	 * mmc1                    USDHC2
	 * mmc2                    USDHC3
	 */
	for (i = 0; i < CONFIG_SYS_FSL_USDHC_NUM; i++) {
		switch (i) {
		case 0:
			imx_iomux_v3_setup_multiple_pads(
				usdhc1_pads, ARRAY_SIZE(usdhc1_pads));
			gpio_direction_input(USDHC1_CD_GPIO);
			usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC_CLK);
			break;
		case 1:
			imx_iomux_v3_setup_multiple_pads(
				usdhc2_pads, ARRAY_SIZE(usdhc2_pads));
			gpio_direction_input(USDHC2_CD_GPIO);
			usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);
			break;
		case 2:
			imx_iomux_v3_setup_multiple_pads(
				usdhc3_pads, ARRAY_SIZE(usdhc3_pads));
			gpio_direction_input(USDHC3_CD_GPIO);
			usdhc_cfg[2].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
			break;
		default:
			printf("Warning: you configured more USDHC controllers"
				"(%d) than supported by the board\n", i + 1);
			return -EINVAL;
			}

			ret = fsl_esdhc_initialize(bis, &usdhc_cfg[i]);
			if (ret) {
				printf("Warning: failed to initialize "
					"mmc dev %d\n", i);
				return ret;
			}
	}

	return 0;
#else
	struct src *src_regs = (struct src *)SRC_BASE_ADDR;
	u32 val;
	u32 port;

	val = readl(&src_regs->sbmr1);

	/* Boot from USDHC */
	port = (val >> 11) & 0x3;
	switch (port) {
	case 0:
		imx_iomux_v3_setup_multiple_pads(usdhc1_pads,
						 ARRAY_SIZE(usdhc1_pads));
		gpio_direction_input(USDHC1_CD_GPIO);
		usdhc_cfg[0].esdhc_base = USDHC1_BASE_ADDR;
		usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC_CLK);
		break;
	case 1:
		imx_iomux_v3_setup_multiple_pads(usdhc2_pads,
						 ARRAY_SIZE(usdhc2_pads));
		gpio_direction_input(USDHC2_CD_GPIO);
		usdhc_cfg[0].esdhc_base = USDHC2_BASE_ADDR;
		usdhc_cfg[0].max_bus_width = 4;
		usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);
		break;
	case 2:
		imx_iomux_v3_setup_multiple_pads(usdhc3_pads,
						 ARRAY_SIZE(usdhc3_pads));
		gpio_direction_input(USDHC3_CD_GPIO);
		usdhc_cfg[0].esdhc_base = USDHC3_BASE_ADDR;
		usdhc_cfg[0].max_bus_width = 4;
		usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
		break;
	}

	gd->arch.sdhc_clk = usdhc_cfg[0].sdhc_clk;
	return fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
#endif
}

#ifdef CONFIG_SYS_I2C_MXC
#define PC	MUX_PAD_CTRL(I2C_PAD_CTRL)
/* I2C1 for PMIC */
struct i2c_pads_info i2c_pad_info1 = {
	.sda = {
		.i2c_mode = MX6_PAD_I2C1_SDA__I2C1_SDA | PC,
		.gpio_mode = MX6_PAD_I2C1_SDA__GPIO_3_13 | PC,
		.gp = IMX_GPIO_NR(3, 13),
	},
	.scl = {
		.i2c_mode = MX6_PAD_I2C1_SCL__I2C1_SCL | PC,
		.gpio_mode = MX6_PAD_I2C1_SCL__GPIO_3_12 | PC,
		.gp = IMX_GPIO_NR(3, 12),
	},
};

static int pmic_power_init_board(void)
{
	struct pmic *pfuze;
	unsigned int reg;
	int ret;

	pfuze = pfuze_common_init(I2C_PMIC);
	if (!pfuze)
		return -ENODEV;

	ret = pfuze_mode_init(pfuze, APS_PFM);
	if (ret < 0)
		return ret;

	/* set SW1AB staby volatage 0.975V */
	pmic_reg_read(pfuze, PFUZE100_SW1ABSTBY, &reg);
	reg &= ~0x3f;
	reg |= 0x1b;
	pmic_reg_write(pfuze, PFUZE100_SW1ABSTBY, reg);

	/* set SW1AB/VDDARM step ramp up time from 16us to 4us/25mV */
	pmic_reg_read(pfuze, PFUZE100_SW1ABCONF, &reg);
	reg &= ~0xc0;
	reg |= 0x40;
	pmic_reg_write(pfuze, PFUZE100_SW1ABCONF, reg);

	/* set SW1C staby volatage 0.975V */
	pmic_reg_read(pfuze, PFUZE100_SW1CSTBY, &reg);
	reg &= ~0x3f;
	reg |= 0x1b;
	pmic_reg_write(pfuze, PFUZE100_SW1CSTBY, reg);

	/* set SW1C/VDDSOC step ramp up time to from 16us to 4us/25mV */
	pmic_reg_read(pfuze, PFUZE100_SW1CCONF, &reg);
	reg &= ~0xc0;
	reg |= 0x40;
	pmic_reg_write(pfuze, PFUZE100_SW1CCONF, reg);

	return 0;
}


#define   SNVS_LPPGDR    0X20CC064
#define   SNVS_LPSR             0X20CC04C
#define   IMX_RGE_SNVS_LPCR      0x020cc038
static void mx6_snvs_poweroff(void)
{
	int datalong;
	int i;
	int data;
	writel(0x41736166,SNVS_LPPGDR);    //这个是重点
	data=readl( SNVS_LPSR);
	printf("SNVS_LPSR:%x\n",data);
	writel(data,SNVS_LPSR); 
	//清标志位，标志位是写1清0，即某BIT被写入1，该BIT就被设置为0
	writel(0x69 , IMX_RGE_SNVS_LPCR);	//关机操作。
}

/*
led red 3-23
led green 3-20
*/
static void  set_led_red(bool on)
{
	if(on)
		gpio_direction_output(IMX_GPIO_NR(3, 23), 1);
	else
		gpio_direction_output(IMX_GPIO_NR(3, 23), 0);

}

static  int  leds_blinks(int num,int delay_on_ms,int delay_off_ms)
{
	int i;
	for (i=0;i<num;i++)	{
		set_led_red(1);
		udelay(delay_on_ms*1000);
		set_led_red(0);
		udelay(delay_off_ms*1000);
	}

	return 0;
}

/*
* poweroff function is failed: not poweroff but loop reboot ...
*  so poweroff at  load bq27220 driver ... 
*/
static void action_power_off(void)
{
	/*1. powerdown*/
	printf("power off  ... \n");	
	mx6_snvs_poweroff();
	printf("power off  delay 2s ... \n");
	udelay(2000000);
	printf("power off  failed... \n");
	while(1);
}

static struct pmic *g_bq27xxx;
static void battery_check_state(struct pmic *battery)
{
	int ret;
	enum battery_state bat_s;
	ret = bq27xxx_get_bat_state(battery,  &bat_s);
	if(ret < 0 )
	{
		printf("get bat state err , power off ... \n");
		return -ENODEV;
	}

	if(bat_s != BAT_NORMAL)
	{
		if(bat_s == VOLTAGE_LOW)
			printf("VOLTAGE_LOW < %d , power off ... \n",POWER_OFF_VOL);
		else
			printf("CAPACITY_LOW < %d , power off ... \n",POWER_OFF_CAP);	
		
		leds_blinks(5,200,200);  		
		action_power_off();
	}

}
static int pmic_battery_init_board(void)
{
	struct pmic *bq27xxx;
	int ret;
	bq27xxx = bq27xxx_common_init(I2C_PMIC);
	if (!bq27xxx)
	{
		printf("%s error ... \n",__FUNCTION__);	
		return -ENODEV;
	}

   /*1. check  gpc_cedv has set or not ?  */
   	battery_check_gpc_cedv(bq27xxx);     
	battery_check_state(bq27xxx);
	
	g_bq27xxx = bq27xxx;
	return 0;
}

int power_init_board(void)
{
	debug("%s -- %d \n",__FUNCTION__,__LINE__);	
	int ret;
	ret = pmic_power_init_board();
	debug("%s -- %d \n",__FUNCTION__,__LINE__);
	set_led_red(1);	

#ifdef CONFIG_POWER_BQ27XXX
	ret |= pmic_battery_init_board();
	debug("%s -- %d \n",__FUNCTION__,__LINE__);	
#endif
	/* pmic or bq27220 error */
	if( ret  != 0 )	{
		leds_blinks(10,50,50);  
		action_power_off();
	}

	/* close led */
	set_led_red(0);
	return ret;
}

#ifdef CONFIG_LDO_BYPASS_CHECK
void ldo_mode_set(int ldo_bypass)
{
	u32 value;
	int is_400M;
	struct pmic *p = pmic_get("PFUZE100");

	if (!p) {
		printf("No pmic!\n");
		return;
	}

	/* swith to ldo_bypass mode */
	if (ldo_bypass) {
		prep_anatop_bypass();

		/* decrease VDDARM to 1.1V */
		pmic_reg_read(p, PFUZE100_SW1ABVOL, &value);
		value &= ~0x3f;
		value |= 0x20;
		pmic_reg_write(p, PFUZE100_SW1ABVOL, value);

		/* increase VDDSOC to 1.3V */
		pmic_reg_read(p, PFUZE100_SW1CVOL, &value);
		value &= ~0x3f;
		value |= 0x28;
		pmic_reg_write(p, PFUZE100_SW1CVOL, value);

		is_400M = set_anatop_bypass(0);

		/*
		 * MX6SL: VDDARM:1.175V@800M; VDDSOC:1.175V@800M
		 *        VDDARM:0.975V@400M; VDDSOC:1.175V@400M
		 */
		pmic_reg_read(p, PFUZE100_SW1ABVOL, &value);
		value &= ~0x3f;
		if (is_400M)
			value |= 0x1b;
		else
			value |= 0x23;
		pmic_reg_write(p, PFUZE100_SW1ABVOL, value);

		/* decrease VDDSOC to 1.175V */
		pmic_reg_read(p, PFUZE100_SW1CVOL, &value);
		value &= ~0x3f;
		value |= 0x23;
		pmic_reg_write(p, PFUZE100_SW1CVOL, value);

		finish_anatop_bypass();
		printf("switch to ldo_bypass mode!\n");
	}
}
#endif

#endif

#ifdef CONFIG_FEC_MXC
int board_eth_init(bd_t *bis)
{
	setup_iomux_fec();

	return cpu_eth_init(bis);
}

static int setup_fec(void)
{
	struct iomuxc *iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;

	/* clear gpr1[14], gpr1[18:17] to select anatop clock */
	clrsetbits_le32(&iomuxc_regs->gpr[1], IOMUX_GPR1_FEC_MASK, 0);

	return enable_fec_anatop_clock(0, ENET_50MHZ);
}
#endif

#ifdef CONFIG_USB_EHCI_MX6
#define USB_OTHERREGS_OFFSET	0x800
#define UCTRL_PWR_POL		(1 << 9)

static iomux_v3_cfg_t const usb_otg_pads[] = {
	/* OTG1 */
/*	MX6_PAD_KEY_COL4__USB_USBOTG1_PWR | MUX_PAD_CTRL(NO_PAD_CTRL),*/
	MX6_PAD_EPDC_PWRCOM__ANATOP_USBOTG1_ID | MUX_PAD_CTRL(OTGID_PAD_CTRL),
	/* OTG2 */
/*	MX6_PAD_KEY_COL5__USB_USBOTG2_PWR | MUX_PAD_CTRL(NO_PAD_CTRL) */
};

static void setup_usb(void)
{
	imx_iomux_v3_setup_multiple_pads(usb_otg_pads,
					 ARRAY_SIZE(usb_otg_pads));
}

int board_usb_phy_mode(int port)
{
	if (port == 1)
		return USB_INIT_HOST;
	else
		return usb_phy_mode(port);
}

int board_ehci_hcd_init(int port)
{
	u32 *usbnc_usb_ctrl;

	if (port > 1)
		return -EINVAL;

	usbnc_usb_ctrl = (u32 *)(USB_BASE_ADDR + USB_OTHERREGS_OFFSET +
				 port * 4);

	/* Set Power polarity */
	setbits_le32(usbnc_usb_ctrl, UCTRL_PWR_POL);

	return 0;
}
#endif

int board_early_init_f(void)
{
	setup_iomux_uart();
	return 0;
}

static struct splash_location splash_locations[] = {
	{
		.name = "mmc_fs",
		.storage = SPLASH_STORAGE_MMC,
		.flags = SPLASH_STORAGE_FS,
		.devpart = "1:1",
	},
};

int splash_screen_prepare(void)
{

	setenv("splashfile", "splash.bmp");

	return splash_source_load(splash_locations, ARRAY_SIZE(splash_locations));
}

#ifdef CONFIG_MXC_EPDC
vidinfo_t panel_info = {
	.vl_col = 1872,
	.vl_row = 1404,
	.vl_left_margin = 32,
	.vl_right_margin = 326,
	.vl_upper_margin = 4,
	.vl_lower_margin = 12,
	.vl_hsync = 44,
	.vl_vsync = 1,
	.vl_bpix = 3,
	.cmap = 0,
};

struct epdc_timing_params panel_timings = {
	.vscan_holdoff = 4,
	.sdoed_width = 10,
	.sdoed_delay = 20,
	.sdoez_width = 10,
	.sdoez_delay = 20,
	.gdclk_hp_offs = 1042,
	.gdsp_offs = 768,
	.gdoe_offs = 0,
	.gdclk_offs = 91,
	.num_ce = 1,
};

static void setup_epdc_power(void)
{
	/* Setup epdc voltage */

	/* EPDC_PWRSTAT - GPIO2[13] for PWR_GOOD status */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRSTAT__GPIO_2_13 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	gpio_direction_input(IMX_GPIO_NR(2, 13));

	/* EPDC_VCOM0 - GPIO2[3] for VCOM control */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_VCOM0__GPIO_2_3 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));

	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2, 3), 1);

#if 1
	/* EPDC_PWRWAKEUP - GPIO2[9] for EPD PMIC WAKEUP */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRCTRL2__GPIO_2_9 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2, 9), 1);
#endif
	/* EPDC_PWRCTRL0 - GPIO2[7] for EPD PWR CTL0 */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRCTRL0__GPIO_2_7 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2, 7), 1);

	/* EPDC_PWRup - GPIO2[10] for EPD PWR UP */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRCTRL3__GPIO_2_10 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2,10), 1);


	/* EPDC_PWR EN - GPIO2[8] for EPD PWR EN */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRCTRL1__GPIO_2_8 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2,8), 1);
}


static void setup_epdc_powerpin_in(void)
{
	/* Setup epdc voltage */

	/* EPDC_PWRSTAT - GPIO2[13] for PWR_GOOD status */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRSTAT__GPIO_2_13 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	gpio_direction_input(IMX_GPIO_NR(2, 13));

	/* EPDC_VCOM0 - GPIO2[3] for VCOM control */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_VCOM0__GPIO_2_3 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));

	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2, 3), 0);

#if 1
	/* EPDC_PWRWAKEUP - GPIO2[9] for EPD PMIC WAKEUP */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRCTRL2__GPIO_2_9 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2, 9), 0);
#endif
	/* EPDC_PWRCTRL0 - GPIO2[7] for EPD PWR CTL0 */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRCTRL0__GPIO_2_7 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2, 7), 0);

	/* EPDC_PWRup - GPIO2[10] for EPD PWR UP */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRCTRL3__GPIO_2_10 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2,10), 0);


	/* EPDC_PWR EN - GPIO2[8] for EPD PWR EN */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRCTRL1__GPIO_2_8 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
	/* Set as output */
	gpio_direction_output(IMX_GPIO_NR(2,8), 0);
}

static void epdc_enable_pins(void)
{
	/* epdc iomux settings */
	imx_iomux_v3_setup_multiple_pads(epdc_enable_pads,
				ARRAY_SIZE(epdc_enable_pads));
}

static void epdc_disable_pins(void)
{
	/* Configure MUX settings for EPDC pins to GPIO  and drive to 0 */
	imx_iomux_v3_setup_multiple_pads(epdc_disable_pads,
				ARRAY_SIZE(epdc_disable_pads));
}

static void setup_epdc(void)
{
	unsigned int reg;
	struct mxc_ccm_reg *ccm_regs = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	/*** epdc Maxim PMIC settings ***/

	/* EPDC PWRSTAT - GPIO2[13] for PWR_GOOD status */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRSTAT__GPIO_2_13 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));

	/* EPDC VCOM0 - GPIO2[3] for VCOM control */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_VCOM0__GPIO_2_3 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
#if 1
	/* - GPIO2[9] for EPD PMIC WAKEUP */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRCTRL2__GPIO_2_9 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));
#endif
	/* EIM_A18 - GPIO2[7] for EPD PWR CTL0 */
	imx_iomux_v3_setup_pad(MX6_PAD_EPDC_PWRCTRL0__GPIO_2_7 |
				MUX_PAD_CTRL(EPDC_PAD_CTRL));


	/* ft7 fpga pwr ctrl*/
	imx_iomux_v3_setup_pad(MX6_PAD_SD1_DAT3__GPIO5_IO06 |MUX_PAD_CTRL(EPDC_PAD_CTRL));


	/*** Set pixel clock rates for EPDC ***/

	/* EPDC AXI clk from PFD_400M, set to 396/2 = 198MHz */
	reg = readl(&ccm_regs->chsccdr);
	reg &= ~0x3F000;
	reg |= (0x4 << 15) | (1 << 12);
	writel(reg, &ccm_regs->chsccdr);

	/* EPDC AXI clk enable */
	reg = readl(&ccm_regs->CCGR3);
	reg |= 0x0030;
	writel(reg, &ccm_regs->CCGR3);

	/* EPDC PIX clk from PFD_540M, set to 540/4/5 = 27MHz */
	reg = readl(&ccm_regs->cscdr2);
	reg &= ~0x03F000;
	reg |= (0x5 << 15) | (4 << 12);
	writel(reg, &ccm_regs->cscdr2);

	reg = readl(&ccm_regs->cbcmr);
	reg &= ~0x03800000;
	reg |= (0x3 << 23);
	writel(reg, &ccm_regs->cbcmr);

	/* EPDC PIX clk enable */
	reg = readl(&ccm_regs->CCGR3);
	reg |= 0x0C00;
	writel(reg, &ccm_regs->CCGR3);

	panel_info.epdc_data.wv_modes.mode_init = 0;
	panel_info.epdc_data.wv_modes.mode_du = 1;
	panel_info.epdc_data.wv_modes.mode_gc4 = 3;
	panel_info.epdc_data.wv_modes.mode_gc8 = 2;
	panel_info.epdc_data.wv_modes.mode_gc16 = 2;
	panel_info.epdc_data.wv_modes.mode_gc32 = 2;

	panel_info.epdc_data.epdc_timings = panel_timings;

//	setup_epdc_power();
}

void epdc_power_on(void)
{
	unsigned int reg;
	int tries = 50;
	struct gpio_regs *gpio_regs = (struct gpio_regs *)GPIO2_BASE_ADDR;

	gpio_set_value(IMX_GPIO_NR(2, 8), 1);
#if 1	
	/* Set EPD_PWR_CTL0 to high - enable EINK_VDD (3.15) */
	gpio_set_value(IMX_GPIO_NR(2, 7), 1);
	udelay(1000);
	/* Enable epdc signal pin */
	setup_epdc_power();
	epdc_enable_pins();	

	/* Set PMIC Wakeup to high - enable Display power */
	gpio_set_value(IMX_GPIO_NR(2, 9), 1);
	udelay(3000);
	gpio_set_value(IMX_GPIO_NR(2, 10), 1);


#if 1	
	/* Wait for PWRGOOD == 1 */
	while (tries--) {
		reg = gpio_get_value(IMX_GPIO_NR(2, 13));
		if (reg) {
			break;
		}
		udelay(1000);
	}
	if(tries <= 0)
		printf("power good error ...\n");
	else
		printf("power good tries_times = %d ...\n",50-tries);

#endif
	/* Enable VCOM */
	gpio_set_value(IMX_GPIO_NR(2, 3), 1);
#endif	
	udelay(500);

}

void epdc_power_off(void)
{
	/* Set PMIC Wakeup to low - disable Display power */
//	gpio_set_value(IMX_GPIO_NR(2, 9), 0);
	udelay(500);
	gpio_set_value(IMX_GPIO_NR(2, 3), 0);
	udelay(1000);
	gpio_set_value(IMX_GPIO_NR(2, 10), 0);
	/* Disable VCOM */
//	gpio_set_value(IMX_GPIO_NR(2, 3), 0);
//	epdc_disable_pins();
	setup_epdc_powerpin_in();
	/* Set EPD_PWR_CTL0 to low - disable EINK_VDD (3.15) */
	gpio_set_value(IMX_GPIO_NR(2, 9), 0);
	gpio_set_value(IMX_GPIO_NR(2, 7), 0);
	gpio_set_value(IMX_GPIO_NR(2, 8), 0);
	
	udelay(1000);
	return;
}
#endif

int board_init(void)
{

	debug("%s -- %d \n",__FUNCTION__,__LINE__); 
//	set_led_red(1);
	debug("%s -- %d \n",__FUNCTION__,__LINE__); 

	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#ifdef CONFIG_SYS_I2C_MXC
	setup_i2c(0, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info1);
#endif

#ifdef	CONFIG_MXC_EPDC
	debug("%s -- %d \n",__FUNCTION__,__LINE__);	
	setup_epdc();
#endif
	gpio_direction_output(IMX_GPIO_NR(5, 6), 1);
	gpio_direction_output(IMX_GPIO_NR(4, 15), 1);

	return 0;
}

int board_late_init(void)
{
#ifdef CONFIG_ENV_IS_IN_MMC
	board_late_mmc_env_init();
#endif
	return 0;
}

int checkboard(void)
{
	puts("Board: SN078-M02A\n");

	return 0;
}

#ifdef CONFIG_MXC_KPD
#define MX6SL_KEYPAD_CTRL (PAD_CTL_HYS | PAD_CTL_PKE | PAD_CTL_PUE | \
			   PAD_CTL_PUS_100K_UP | PAD_CTL_DSE_120ohm)

iomux_v3_cfg_t const mxc_kpd_pads[] = {
	(MX6_PAD_KEY_COL0__KPP_COL_0 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	(MX6_PAD_KEY_COL1__KPP_COL_1 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	(MX6_PAD_KEY_COL2__KPP_COL_2 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	(MX6_PAD_KEY_COL3__KPP_COL_3 | MUX_PAD_CTRL(NO_PAD_CTRL)),

	(MX6_PAD_KEY_ROW0__KPP_ROW_0 | MUX_PAD_CTRL(MX6SL_KEYPAD_CTRL)),
	(MX6_PAD_KEY_ROW1__KPP_ROW_1 | MUX_PAD_CTRL(MX6SL_KEYPAD_CTRL)),
	(MX6_PAD_KEY_ROW2__KPP_ROW_2 | MUX_PAD_CTRL(MX6SL_KEYPAD_CTRL)),
	(MX6_PAD_KEY_ROW3__KPP_ROW_3 | MUX_PAD_CTRL(MX6SL_KEYPAD_CTRL)),
};
int setup_mxc_kpd(void)
{
	imx_iomux_v3_setup_multiple_pads(mxc_kpd_pads,
					 ARRAY_SIZE(mxc_kpd_pads));

	return 0;
}
#endif /*CONFIG_MXC_KPD*/

#ifdef CONFIG_FSL_FASTBOOT
void board_fastboot_setup(void)
{
	switch (get_boot_device()) {
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD1_BOOT:
	case MMC1_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc0");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "boota mmc0");
		break;
	case SD2_BOOT:
	case MMC2_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc1");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "boota mmc1");
		break;
	case SD3_BOOT:
	case MMC3_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc2");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "boota mmc2");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
	default:
		printf("unsupported boot devices\n");
		break;
	}

}

#ifdef CONFIG_ANDROID_RECOVERY
int check_recovery_cmd_file(void)
{
    return recovery_check_and_clean_flag();
}

void board_recovery_setup(void)
{
	int bootdev = get_boot_device();

	/*current uboot BSP only supports USDHC2*/
	switch (bootdev) {
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD1_BOOT:
	case MMC1_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
					"boota mmc0 recovery");
		break;
	case SD2_BOOT:
	case MMC2_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
					"boota mmc1 recovery");
		break;
	case SD3_BOOT:
	case MMC3_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
					"boota mmc2 recovery");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
	default:
		printf("Unsupported bootup device for recovery: dev: %d\n",
			bootdev);
		return;
	}

	printf("setup env for recovery..\n");
	setenv("bootcmd", "run bootcmd_android_recovery");
}

#endif /*CONFIG_ANDROID_RECOVERY*/

#endif /*CONFIG_FSL_FASTBOOT*/


#ifdef CONFIG_SPL_BUILD
#include <spl.h>
#include <libfdt.h>

const struct mx6sl_iomux_ddr_regs mx6_ddr_ioregs = {
	.dram_sdqs0 = 0x00003030,
	.dram_sdqs1 = 0x00003030,
	.dram_sdqs2 = 0x00003030,
	.dram_sdqs3 = 0x00003030,
	.dram_dqm0 = 0x00000030,
	.dram_dqm1 = 0x00000030,
	.dram_dqm2 = 0x00000030,
	.dram_dqm3 = 0x00000030,
	.dram_cas  = 0x00000030,
	.dram_ras  = 0x00000030,
	.dram_sdclk_0 = 0x00000028,
	.dram_reset = 0x00000030,
	.dram_sdba2 = 0x00000000,
	.dram_odt0 = 0x00000008,
	.dram_odt1 = 0x00000008,
};

const struct mx6sl_iomux_grp_regs mx6_grp_ioregs = {
	.grp_b0ds = 0x00000030,
	.grp_b1ds = 0x00000030,
	.grp_b2ds = 0x00000030,
	.grp_b3ds = 0x00000030,
	.grp_addds = 0x00000030,
	.grp_ctlds = 0x00000030,
	.grp_ddrmode_ctl = 0x00020000,
	.grp_ddrpke = 0x00000000,
	.grp_ddrmode = 0x00020000,
	.grp_ddr_type = 0x00080000,
};

const struct mx6_mmdc_calibration mx6_mmcd_calib = {
	.p0_mpdgctrl0 =  0x20000000,
	.p0_mpdgctrl1 =  0x00000000,
	.p0_mprddlctl =  0x4241444a,
	.p0_mpwrdlctl =  0x3030312b,
	.mpzqlp2ctl = 0x1b4700c7,
};

static struct mx6_lpddr2_cfg mem_ddr = {
	.mem_speed = 800,
	.density = 4,
	.width = 32,
	.banks = 8,
	.rowaddr = 14,
	.coladdr = 10,
	.trcd_lp = 2000,
	.trppb_lp = 2000,
	.trpab_lp = 2250,
	.trasmin = 4200,
};

static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	writel(0xFFFFFFFF, &ccm->CCGR0);
	writel(0xFFFFFFFF, &ccm->CCGR1);
	writel(0xFFFFFFFF, &ccm->CCGR2);
	writel(0xFFFFFFFF, &ccm->CCGR3);
	writel(0xFFFFFFFF, &ccm->CCGR4);
	writel(0xFFFFFFFF, &ccm->CCGR5);
	writel(0xFFFFFFFF, &ccm->CCGR6);

	writel(0x00260324, &ccm->cbcmr);
}

static void spl_dram_init(void)
{
	struct mx6_ddr_sysinfo sysinfo = {
		.dsize = mem_ddr.width / 32,
		.cs_density = 20,
		.ncs = 2,
		.cs1_mirror = 0,
		.walat = 0,
		.ralat = 2,
		.mif3_mode = 3,
		.bi_on = 1,
		.rtt_wr = 0,        /* LPDDR2 does not need rtt_wr rtt_nom */
		.rtt_nom = 0,
		.sde_to_rst = 0,    /* LPDDR2 does not need this field */
		.rst_to_cke = 0x10, /* JEDEC value for LPDDR2: 200us */
		.ddr_type = DDR_TYPE_LPDDR2,
	};
	mx6sl_dram_iocfg(32, &mx6_ddr_ioregs, &mx6_grp_ioregs);
	mx6_dram_cfg(&sysinfo, &mx6_mmcd_calib, &mem_ddr);
}

void board_init_f(ulong dummy)
{
	/* setup AIPS and disable watchdog */
	arch_cpu_init();

	ccgr_init();

	/* iomux and setup of i2c */
	board_early_init_f();

	/* setup GP timer */
	timer_init();

	/* UART clocks enabled and gd valid - init serial console */
	preloader_console_init();

	/* DDR initialization */
	spl_dram_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* load/boot image from boot device */
	board_init_r(NULL, 0);
}
#endif
