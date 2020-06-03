/*
 * Copyright 2013-2016 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Freescale i.MX6SL EVK board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "mx6_common.h"

#ifdef CONFIG_SPL
#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_MMC_SUPPORT
#include "imx6_spl.h"
#endif

/* uncomment for PLUGIN mode support */
/* #define CONFIG_USE_PLUGIN */

/* uncomment for SECURE mode support */
/* #define CONFIG_SECURE_BOOT */

#ifdef CONFIG_SECURE_BOOT
#ifndef CONFIG_CSF_SIZE
#define CONFIG_CSF_SIZE 0x4000
#endif
#endif

#define MACH_TYPE_MX6SLEVK		4307
#define CONFIG_MACH_TYPE		MACH_TYPE_MX6SLEVK

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(16 * SZ_1M)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT

#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE		UART1_BASE

/* MMC Configs */
#define CONFIG_SYS_FSL_ESDHC_ADDR	USDHC2_BASE_ADDR
#define CONFIG_SYS_FSL_USDHC_NUM	3
#define CONFIG_SYS_MMC_ENV_DEV		1	/* SDHC2*/

/* I2C Configs */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_MXC_I2C1		/* enable I2C bus 1 */
#define CONFIG_SYS_I2C_SPEED		  400000

/* PMIC */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_PFUZE100
#define CONFIG_POWER_PFUZE100_I2C_ADDR	0x08

#define CONFIG_POWER_BQ27XXX
#define CONFIG_POWER_BQ27XXX_I2C_ADDR	0x55

#define CONFIG_PHYLIB
#define CONFIG_PHY_SMSC

#define CONFIG_RATTA_SETTINGS \
		"zImage_rollback=zImage-origin\0"\
		"fdt_file_rollback=ratta-sn100.dtb-origin\0"\
		"rootfspart=2\0"\
		"upgrade_dir=/etc/ratta-upgrade\0"\
		"check_upgrade_flag=ext4load  mmc ${mmcdev}:${rootfspart} ${loadaddr} ${upgrade_dir}/${current_upgrade}-upgrade\0"\
		"check_upgrade_done_flag=ext4load  mmc ${mmcdev}:${rootfspart} ${loadaddr} ${upgrade_dir}/${current_upgrade}-upgrade-compile\0"\
		"write_upgrade_failure_flag=ext4write  mmc ${mmcdev}:${rootfspart} ${loadaddr} ${upgrade_dir}/${current_upgrade}-upgrade-failure 0\0 "\
		"write_start_upgrade_flag=ext4write  mmc ${mmcdev}:${rootfspart} ${loadaddr} ${upgrade_dir}/${current_upgrade}-upgrade-start 0\0 "\
		"upgrade_failure_rollback= if test ${current_upgrade} = zImage; then "\
			"echo  zImage upgrade failure ...;"\
			"setenv image ${zImage_rollback}; "\
		"else "\
			"echo dtb upgrade failure ... "\
			"setenv fdt_file ${fdt_file_rollback}; "\
		"fi; "\
		"run  write_upgrade_failure_flag;\0"\
		"zImage-upgrade=setenv current_upgrade zImage; " \
		"if run check_upgrade_flag;then " \
			"echo upgrade flag ...; " \
			"if test  ${zImage_upgrade_state} = start; then " \
				"if  run check_upgrade_done_flag ; then " \
					"echo has upgrade_done_flag ... ; " \
					"run write_start_upgrade_flag ; " \
				"else " \
					"echo upgrade failure ; " \
					"run upgrade_failure_rollback; " \
					 "setenv zImage_upgrade_state failure; " \
					 "saveenv; " \
				"fi; " \
			"else " \
				"echo upgrade start ...; " \
				"setenv zImage_upgrade_state start; " \
				"saveenv; " \
				"run write_start_upgrade_flag ; " \
			"fi; " \
		"else " \
			"echo no need upgrade ... ; " \
			"if test ${zImage_upgrade_state} != success;then " \
			"setenv zImage_upgrade_state success; " \
			"setenv image zImage ;" \
			"saveenv; " \
			"fi; " \
		"fi;\0" \
		"dtb-upgrade=setenv current_upgrade dtb; " \
		"if run check_upgrade_flag;then " \
			"echo upgrade flag ...; " \
			"if test  ${dtb_upgrade_state} = start; then " \
				"if  run check_upgrade_done_flag ; then " \
					"echo has upgrade_done_flag ... ; " \
					"run write_start_upgrade_flag ; " \
				"else " \
					"echo upgrade failure ; " \
					"run upgrade_failure_rollback; " \
					"setenv dtb_upgrade_state failure; " \
					"saveenv; " \
				"fi; " \
			"else " \
				"echo upgrade start ...; " \
				"setenv dtb_upgrade_state start; " \
				"saveenv; " \
				"run write_start_upgrade_flag ; " \
			"fi; " \
		"else " \
			"echo no need upgrade ... ; " \
			"if test ${dtb_upgrade_state} != success;then " \
			"setenv dtb_upgrade_state success; " \
			"setenv fdt_file ratta-sn100.dtb ; " \
			"saveenv; " \
			"fi; " \
		"fi;\0" \
		"usbargs=g_mass_storage.removable=1 g_mass_storage.idVendor=0x15a2 g_mass_storage.idProduct=0x7b  " \
		"g_mass_storage.iSerialNumber=123456abcdef g_mass_storage.luns=1 \0"
		
#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_RATTA_SETTINGS \
	"epdc_waveform=epdc_splash.bin\0" \
	"script=boot.scr\0" \
	"image=zImage\0" \
	"console=ttymxc0\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"fdt_file=ratta-sn100.dtb\0" \
	"fdt_addr=0x88000000\0" \
	"splashpos=m,m\0" \
	"boot_fdt=try\0" \
	"mmcdev="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"mmcpart=1\0" \
	"mmcroot=" CONFIG_MMCROOT " rootwait rw\0" \
	"mmcautodetect=yes\0" \
	"mmcargs=setenv bootargs console=${console},${baudrate} ${usbargs}  " \
		"root=${mmcroot}\0" \
	"loadbootscript=" \
		"fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"source\0" \
	"loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
	"loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr} ${fdt_file}\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if run loadfdt; then " \
				"bootz ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi; " \
		"else " \
			"bootz; " \
		"fi;\0" \

#define CONFIG_BOOTCOMMAND \
	   "mmc dev ${mmcdev};" \
	   "mmc dev ${mmcdev}; if mmc rescan; then " \
		   "run zImage-upgrade; " \
		   "run dtb-upgrade; " \
		   "if run loadbootscript; then " \
			   "run bootscript; " \
		   "else " \
			   "if run loadimage; then " \
				   "run mmcboot; " \
			   "fi; " \
		   "fi; " \
	   "fi"

/* Miscellaneous configurable options */
#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + SZ_512M)

#define CONFIG_STACKSIZE		SZ_128K

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR
#define PHYS_SDRAM_SIZE			SZ_1G

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* Environment organization */
#define CONFIG_ENV_SIZE			SZ_8K
#define CONFIG_SYS_MMC_ENV_DEV		1   /* USDHC2 */
#define CONFIG_SYS_MMC_ENV_PART		0	/* user partition */
#define CONFIG_MMCROOT			"/dev/mmcblk1p2"  /* USDHC2 */

#define CONFIG_ENV_OFFSET		(12 * SZ_64K)
#define CONFIG_ENV_IS_IN_MMC

/* USB Configs */
#define CONFIG_CMD_USB
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX6 
#define CONFIG_USB_STORAGE
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS		0
#define CONFIG_USB_MAX_CONTROLLER_COUNT	2
#endif

#define CONFIG_SYS_FSL_USDHC_NUM	3
#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_SYS_MMC_ENV_DEV		1	/* SDHC2*/
#endif

#define CONFIG_IMX_THERMAL

/* #define CONFIG_SPLASH_SCREEN */

/*
 * SPLASH SCREEN Configs
 */
#ifdef CONFIG_SPLASH_SCREEN
	/*
	 * Framebuffer and LCD
	 */
	#define CONFIG_CMD_BMP
	#define CONFIG_MXC_EPDC				1
	#define CONFIG_LCD
	#define CONFIG_SPLASH_SOURCE
	#define CONFIG_SPLASH_SCREEN_ALIGN

#ifdef CONFIG_MXC_EPDC
	#undef LCD_TEST_PATTERN
	#define LCD_BPP					LCD_MONOCHROME
	#define CONFIG_WAVEFORM_BUF_SIZE		SZ_4M
#endif
#endif /* CONFIG_SPLASH_SCREEN */

#define CONFIG_CI_UDC
#define CONFIG_USBD_HS
#define CONFIG_USB_GADGET_DUALSPEED

#define CONFIG_USB_GADGET
#define CONFIG_CMD_USB_MASS_STORAGE
#define CONFIG_USB_FUNCTION_MASS_STORAGE
#define CONFIG_USB_GADGET_DOWNLOAD
#define CONFIG_USB_GADGET_VBUS_DRAW	2

#define CONFIG_G_DNL_VENDOR_NUM		0x0525
#define CONFIG_G_DNL_PRODUCT_NUM	0xa4a5
#define CONFIG_G_DNL_MANUFACTURER	"FSL"

#if 0
/* USB Device Firmware Update support */
#define CONFIG_CMD_DFU
#define CONFIG_USB_FUNCTION_DFU
#define CONFIG_DFU_MMC
#define CONFIG_DFU_RAM
#endif
#endif				/* __CONFIG_H */
