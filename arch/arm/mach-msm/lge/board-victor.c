/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/gpio.h>

#include <linux/mfd/marimba.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <mach/board.h>
#include <mach/dma.h>
#include <mach/msm_hsusb.h>
#include <mach/rpc_hsusb.h>
#ifdef CONFIG_USB_ANDROID
#include <linux/usb/android_composite.h>
#endif

#include <mach/socinfo.h>
#include <mach/board_lge.h>

#include "devices.h"
#include "timer.h"
#include "spm.h"

//LGE_FW_TDMB [START]
#if defined(CONFIG_TSIF) || defined(CONFIG_TSIF_MODULE) 
#include <mach/msm_tsif.h>
#endif
//LGE_FW_TDMB [END]
/* board-specific usb data definitions */
/* QCT originals are in device_lge.c, not here */
#ifdef CONFIG_USB_ANDROID
/* LGE_CHANGE
 * Currently, LG Android host driver has 2 USB bind orders as following;
 * - Android Platform : MDM + DIAG + GPS + UMS + ADB
 * - Android Net      : DIAG + NDIS + MDM + GPS + UMS
 *
 * To bind LG AndroidNet, we add ACM function named "acm2".
 * Please refer to drivers/usb/gadget/f_acm.c.
 * 2011-01-12, hyunhui.park@lge.com
 */

/* The binding list for LGE Android USB */
char *usb_functions_lge_all[] = {
#ifdef CONFIG_USB_ANDROID_MTP
	"mtp",
#endif
#ifdef CONFIG_USB_ANDROID_RNDIS
	"rndis",
#endif
#ifdef CONFIG_USB_ANDROID_ACM
	"acm",
#endif
#ifdef CONFIG_USB_ANDROID_DIAG
	"diag",
#endif
#ifdef CONFIG_USB_ANDROID_CDC_ECM
	"ecm",
	"acm2",
#endif
#ifdef CONFIG_USB_F_SERIAL
	"nmea",
#endif
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
	"usb_cdrom_storage",
	"charge_only",
#endif
	"usb_mass_storage",
	"adb",
};


/* LG Android Platform */
char *usb_functions_lge_android_plat[] = {
	"acm", "diag", "nmea", "usb_mass_storage",
};

char *usb_functions_lge_android_plat_adb[] = {
	"acm", "diag", "nmea", "usb_mass_storage", "adb",
};

#ifdef CONFIG_USB_ANDROID_CDC_ECM
/* LG AndroidNet */
char *usb_functions_lge_android_net[] = {
	"diag", "ecm", "acm2", "nmea", "usb_mass_storage",
};

char *usb_functions_lge_android_net_adb[] = {
	"diag", "ecm", "acm2", "nmea", "usb_mass_storage", "adb",
};
#endif

#ifdef CONFIG_USB_ANDROID_RNDIS
/* LG AndroidNet RNDIS ver */
char *usb_functions_lge_android_rndis[] = {
	"rndis",
};

char *usb_functions_lge_android_rndis_adb[] = {
	"rndis", "adb",
};
#endif

#ifdef CONFIG_USB_ANDROID_MTP
/* LG AndroidNet MTP (in future use) */
char *usb_functions_lge_android_mtp[] = {
	"mtp",
};

char *usb_functions_lge_android_mtp_adb[] = {
	"mtp", "adb",
};
#endif

/* LG Manufacturing mode */
char *usb_functions_lge_manufacturing[] = {
	"acm", "diag",
};

/* Mass storage only mode */
char *usb_functions_lge_mass_storage_only[] = {
	"usb_mass_storage",
};

#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
/* CDROM storage only mode(Autorun default mode) */
char *usb_functions_lge_cdrom_storage_only[] = {
	"usb_cdrom_storage",
};

char *usb_functions_lge_cdrom_storage_adb[] = {
	"usb_cdrom_storage", "adb",
};

char *usb_functions_lge_charge_only[] = {
	"charge_only",
};
#endif

/* QCT original's composition array is existed in device_lge.c */
struct android_usb_product usb_products[] = {
	{
		.product_id = 0x618E,
		.num_functions = ARRAY_SIZE(usb_functions_lge_android_plat),
		.functions = usb_functions_lge_android_plat,
	},
	{
		.product_id = 0x618E,
		.num_functions = ARRAY_SIZE(usb_functions_lge_android_plat_adb),
		.functions = usb_functions_lge_android_plat_adb,
	},
#ifdef CONFIG_USB_ANDROID_CDC_ECM
	{
		.product_id = 0x61A2,
		.num_functions = ARRAY_SIZE(usb_functions_lge_android_net),
		.functions = usb_functions_lge_android_net,
	},
	{
		.product_id = 0x61A1,
		.num_functions = ARRAY_SIZE(usb_functions_lge_android_net_adb),
		.functions = usb_functions_lge_android_net_adb,
	},
#endif
#ifdef CONFIG_USB_ANDROID_RNDIS
	{
		.product_id = 0x61DA,
		.num_functions = ARRAY_SIZE(usb_functions_lge_android_rndis),
		.functions = usb_functions_lge_android_rndis,
	},
	{
		.product_id = 0x61D9,
		.num_functions = ARRAY_SIZE(usb_functions_lge_android_rndis_adb),
		.functions = usb_functions_lge_android_rndis_adb,
	},
#endif
#ifdef CONFIG_USB_ANDROID_MTP
	/* FIXME: These pids are experimental.
	 * Don't use them in official version.
	 */
	{
		.product_id = 0x61C7,
		.num_functions = ARRAY_SIZE(usb_functions_lge_android_mtp),
		.functions = usb_functions_lge_android_mtp,
	},
	{
		.product_id = 0x61F9,
		.num_functions = ARRAY_SIZE(usb_functions_lge_android_mtp_adb),
		.functions = usb_functions_lge_android_mtp_adb,
	},
#endif
	{
		.product_id = 0x6000,
		.num_functions = ARRAY_SIZE(usb_functions_lge_manufacturing),
		.functions = usb_functions_lge_manufacturing,
	},
	{
		.product_id = 0x61C5,
		.num_functions = ARRAY_SIZE(usb_functions_lge_mass_storage_only),
		.functions = usb_functions_lge_mass_storage_only,
	},
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
	{
		/* FIXME: This pid is just for test */
		.product_id = 0x91C8,
		.num_functions = ARRAY_SIZE(usb_functions_lge_cdrom_storage_only),
		.functions = usb_functions_lge_cdrom_storage_only,
	},
	{
		.product_id = 0x61A6,
		.num_functions = ARRAY_SIZE(usb_functions_lge_cdrom_storage_adb),
		.functions = usb_functions_lge_cdrom_storage_adb,
	},
	{
		/* Charge only doesn't have no specific pid */
		.product_id = 0xFFFF,
		.num_functions = ARRAY_SIZE(usb_functions_lge_charge_only),
		.functions = usb_functions_lge_charge_only,
	},
#endif
};

struct usb_mass_storage_platform_data mass_storage_pdata = {
	.nluns      = 1,
	.vendor     = "LGE",
	.product    = "Android Platform",
	.release    = 0x0100,
};

struct platform_device usb_mass_storage_device = {
	.name   = "usb_mass_storage",
	.id 	= -1,
	.dev    = {
		.platform_data = &mass_storage_pdata,
	},
};

struct usb_ether_platform_data rndis_pdata = {
	/* ethaddr is filled by board_serialno_setup */
	.vendorID   	= 0x1004,
	.vendorDescr    = "LG Electronics Inc.",
};

struct platform_device rndis_device = {
	.name   = "rndis",
	.id 	= -1,
	.dev    = {
		.platform_data = &rndis_pdata,
	},
};


#ifdef CONFIG_USB_ANDROID_CDC_ECM
/* LGE_CHANGE
 * To bind LG AndroidNet, add platform data for CDC ECM.
 * 2011-01-12, hyunhui.park@lge.com
 */
struct usb_ether_platform_data ecm_pdata = {
	/* ethaddr is filled by board_serialno_setup */
	.vendorID   	= 0x1004,
	.vendorDescr    = "LG Electronics Inc.",
};

struct platform_device ecm_device = {
	.name   = "ecm",
	.id 	= -1,
	.dev    = {
		.platform_data = &ecm_pdata,
	},
};
#endif

#ifdef CONFIG_USB_ANDROID_ACM
/* LGE_CHANGE
 * To bind LG AndroidNet, add platform data for CDC ACM.
 * 2011-01-12, hyunhui.park@lge.com
 */
struct acm_platform_data acm_pdata = {
	.num_inst	    = 1,
};

struct platform_device acm_device = {
	.name   = "acm",
	.id 	= -1,
	.dev    = {
		.platform_data = &acm_pdata,
	},
};
#endif

//LGE_FW_TDMB [START]
#if defined(CONFIG_TSIF) || defined(CONFIG_TSIF_MODULE)
//SU570
//#define TSIF_B_SYNC      	GPIO_CFG(87, 5, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)
//#define TSIF_B_DATA		GPIO_CFG(36, 3, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)
//#define TSIF_B_EN		GPIO_CFG(35, 3, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)
//#define TSIF_B_CLK		GPIO_CFG(34, 4, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)

#define TSIF_B_DATA	GPIO_CFG(36, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)
#define TSIF_B_EN		GPIO_CFG(35, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)
#define TSIF_B_CLK	GPIO_CFG(34, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA)

static const struct msm_gpio tsif_gpios[] = {
	{ .gpio_cfg = TSIF_B_CLK,  .label =  "tsif_clk", },
	{ .gpio_cfg = TSIF_B_EN,   .label =  "tsif_en", },
	{ .gpio_cfg = TSIF_B_DATA, .label =  "tsif_data", },
	//{ .gpio_cfg = TSIF_B_SYNC, .label =  "tsif_sync", },
};

static struct msm_tsif_platform_data tsif_platform_data = {
	.num_gpios = ARRAY_SIZE(tsif_gpios),
	.gpios = tsif_gpios,
	.tsif_pclk = "tsif_pclk",
	.tsif_ref_clk = "tsif_ref_clk",
};
#endif 
//LGE_FW_TDMB [END]
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
/* LGE_CHANGE
 * Add platform data and device for cdrom storage function.
 * It will be used in Autorun feature.
 * 2011-03-02, hyunhui.park@lge.com
 */
struct usb_cdrom_storage_platform_data cdrom_storage_pdata = {
	.nluns      = 1,
	.vendor     = "LGE",
	.product    = "Android Platform",
	.release    = 0x0100,
};

struct platform_device usb_cdrom_storage_device = {
	.name   = "usb_cdrom_storage",
	.id = -1,
	.dev    = {
		.platform_data = &cdrom_storage_pdata,
	},
};
#endif

struct android_usb_platform_data android_usb_pdata = {
	.vendor_id  = 0x1004,
	.product_id = 0x618E,
	.version    = 0x0100,
	.product_name       = "LGE Android Phone",
	.manufacturer_name  = "LG Electronics Inc.",
	.num_products = ARRAY_SIZE(usb_products),
	.products = usb_products,
	.num_functions = ARRAY_SIZE(usb_functions_lge_all),
	.functions = usb_functions_lge_all,
	.serial_number = "LG_ANDROID_VICTOR__",
};

static int __init board_serialno_setup(char *serialno)
{
	int i;
	char *src = serialno;

	/* create a fake MAC address from our serial number.
	 * first byte is 0x02 to signify locally administered.
	 */
	rndis_pdata.ethaddr[0] = 0x02;
	for (i = 0; *src; i++) {
		/* XOR the USB serial across the remaining bytes */
		rndis_pdata.ethaddr[i % (ETH_ALEN - 1) + 1] ^= *src++;
	}

	android_usb_pdata.serial_number = serialno;
	return 1;
}
__setup("androidboot.serialno=", board_serialno_setup);
#endif

static struct platform_device *devices[] __initdata = {
	&msm_device_smd,
	&msm_device_dmov,
#ifdef CONFIG_I2C_SSBI
	&msm_device_ssbi6,
	&msm_device_ssbi7,
#endif
	&msm_device_i2c,
	&msm_device_i2c_2,
	&msm_device_uart_dm1,
	&qup_device_i2c,
	&msm_ebi0_thermal,
	&msm_ebi1_thermal
//LGE_FW_TDMB[START]
	#if defined(CONFIG_TSIF) || defined(CONFIG_TSIF_MODULE)
	,&msm_device_tsif,
	#endif
//LGE_FW_TDMB [END]
};

static struct msm_gpio msm_i2c_gpios_hw[] = {
	{ GPIO_CFG(70, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "i2c_scl" },
	{ GPIO_CFG(71, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "i2c_sda" },
};

static struct msm_gpio msm_i2c_gpios_io[] = {
	{ GPIO_CFG(70, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "i2c_scl" },
	{ GPIO_CFG(71, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "i2c_sda" },
};

static struct msm_gpio qup_i2c_gpios_io[] = {
	{ GPIO_CFG(16, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "qup_scl" },
	{ GPIO_CFG(17, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "qup_sda" },
};
static struct msm_gpio qup_i2c_gpios_hw[] = {
	{ GPIO_CFG(16, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "qup_scl" },
	{ GPIO_CFG(17, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), "qup_sda" },
};

static void
msm_i2c_gpio_config(int adap_id, int config_type)
{
	struct msm_gpio *msm_i2c_table;

	/* Each adapter gets 2 lines from the table */
	if (adap_id > 0)
		return;
	if (config_type)
		msm_i2c_table = &msm_i2c_gpios_hw[adap_id*2];
	else
		msm_i2c_table = &msm_i2c_gpios_io[adap_id*2];
	msm_gpios_enable(msm_i2c_table, 2);
}
/*This needs to be enabled only for OEMS*/
#ifndef CONFIG_QUP_EXCLUSIVE_TO_CAMERA
static struct vreg *qup_vreg;
#endif
static void
qup_i2c_gpio_config(int adap_id, int config_type)
{
	int rc = 0;
	struct msm_gpio *qup_i2c_table;
	/* Each adapter gets 2 lines from the table */
	if (adap_id != 4)
		return;
	if (config_type)
		qup_i2c_table = qup_i2c_gpios_hw;
	else
		qup_i2c_table = qup_i2c_gpios_io;
	rc = msm_gpios_enable(qup_i2c_table, 2);
	if (rc < 0)
		printk(KERN_ERR "QUP GPIO enable failed: %d\n", rc);
	/*This needs to be enabled only for OEMS*/
#ifndef CONFIG_QUP_EXCLUSIVE_TO_CAMERA
	if (qup_vreg) {
		int rc = vreg_set_level(qup_vreg, 1800);
		if (rc) {
			pr_err("%s: vreg LVS1 set level failed (%d)\n",
			__func__, rc);
		}
		rc = vreg_enable(qup_vreg);
		if (rc) {
			pr_err("%s: vreg_enable() = %d \n",
			__func__, rc);
		}
	}
#endif
}

static struct msm_i2c_platform_data msm_i2c_pdata = {
	.clk_freq = 100000,
	.pri_clk = 70,
	.pri_dat = 71,
	.rmutex  = 1,
	.rsl_id = "D:I2C02000021",
	.msm_i2c_config_gpio = msm_i2c_gpio_config,
};

static void __init msm_device_i2c_init(void)
{
	if (msm_gpios_request(msm_i2c_gpios_hw, ARRAY_SIZE(msm_i2c_gpios_hw)))
		pr_err("failed to request I2C gpios\n");

	msm_device_i2c.dev.platform_data = &msm_i2c_pdata;
}

static struct msm_i2c_platform_data msm_i2c_2_pdata = {
	.clk_freq = 100000,
	.rmutex  = 1,
	.rsl_id = "D:I2C02000022",
	.msm_i2c_config_gpio = msm_i2c_gpio_config,
};

static void __init msm_device_i2c_2_init(void)
{
	msm_device_i2c_2.dev.platform_data = &msm_i2c_2_pdata;
}

static struct msm_i2c_platform_data qup_i2c_pdata = {
	.clk_freq = 384000,
	.pclk = "camif_pad_pclk",
	.msm_i2c_config_gpio = qup_i2c_gpio_config,
};

static void __init qup_device_i2c_init(void)
{
	if (msm_gpios_request(qup_i2c_gpios_hw, ARRAY_SIZE(qup_i2c_gpios_hw)))
		pr_err("failed to request I2C gpios\n");

	qup_device_i2c.dev.platform_data = &qup_i2c_pdata;
	/*This needs to be enabled only for OEMS*/
#ifndef CONFIG_QUP_EXCLUSIVE_TO_CAMERA
	qup_vreg = vreg_get(NULL, "lvsw0");
	if (IS_ERR(qup_vreg)) {
		printk(KERN_ERR "%s: vreg get failed (%ld)\n",
			__func__, PTR_ERR(qup_vreg));
	}
#endif
}

#ifdef CONFIG_I2C_SSBI
static struct msm_ssbi_platform_data msm_i2c_ssbi6_pdata = {
	.rsl_id = "D:PMIC_SSBI",
	.controller_type = MSM_SBI_CTRL_SSBI2,
};

static struct msm_ssbi_platform_data msm_i2c_ssbi7_pdata = {
	.rsl_id = "D:CODEC_SSBI",
	.controller_type = MSM_SBI_CTRL_SSBI,
};
#endif

#ifdef CONFIG_SERIAL_MSM_CONSOLE
static struct msm_gpio uart2_config_data[] = {
	{ GPIO_CFG(51, 2, GPIO_CFG_INPUT,   GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "UART2_Rx"},
	{ GPIO_CFG(52, 2, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "UART2_Tx"},
};

static void msm7x30_init_uart2(void)
{
	msm_gpios_request_enable(uart2_config_data,
			ARRAY_SIZE(uart2_config_data));

}
#endif

static struct msm_spm_platform_data msm_spm_data __initdata = {
	.reg_base_addr = MSM_SAW_BASE,

	.reg_init_values[MSM_SPM_REG_SAW_CFG] = 0x05,
	.reg_init_values[MSM_SPM_REG_SAW_SPM_CTL] = 0x18,
	.reg_init_values[MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY] = 0x00006666,
	.reg_init_values[MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY] = 0xFF000666,

	.reg_init_values[MSM_SPM_REG_SAW_SLP_CLK_EN] = 0x01,
	.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN] = 0x03,
	.reg_init_values[MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN] = 0x00,

	.reg_init_values[MSM_SPM_REG_SAW_SLP_CLMP_EN] = 0x01,
	.reg_init_values[MSM_SPM_REG_SAW_SLP_RST_EN] = 0x00,
	.reg_init_values[MSM_SPM_REG_SAW_SPM_MPM_CFG] = 0x00,

	.awake_vlevel = 0xF2,
	.retention_vlevel = 0xE0,
	.collapse_vlevel = 0x72,
	.retention_mid_vlevel = 0xE0,
	.collapse_mid_vlevel = 0xE0,

	.vctl_timeout_us = 50,
};

static struct msm_acpu_clock_platform_data msm7x30_clock_data = {
	.acpu_switch_time_us = 50,
	.vdd_switch_time_us = 62,
};

static void __init msm7x30_init_irq(void)
{
	msm_init_irq();
}

static void __init msm7x30_init(void)
{
	uint32_t soc_version = 0;

	if (socinfo_init() < 0)
		printk(KERN_ERR "%s: socinfo_init() failed!\n",
		       __func__);

	soc_version = socinfo_get_version();

	msm_clock_init(msm_clocks_7x30, msm_num_clocks_7x30);
#ifdef CONFIG_SERIAL_MSM_CONSOLE
	if (lge_get_uart_mode())
		msm7x30_init_uart2();
#endif
	msm_spm_init(&msm_spm_data, 1);
	msm_acpu_clock_init(&msm7x30_clock_data);

	msm_add_pmem_devices();
	msm_add_fb_device();
	msm_add_kgsl_device();

	msm_add_usb_devices();

#if defined(CONFIG_LGE_MODEL_E739)
	msm7x30_init_marimba();
#endif

#ifdef CONFIG_MSM7KV2_AUDIO
	lge_victor_audio_init();
	msm_snddev_init();
	aux_pcm_gpio_init();
#endif
	if (lge_get_uart_mode())
		platform_device_register(&msm_device_uart2);
//LGE_FW_TDMB [START]
#if defined(CONFIG_TSIF) || defined(CONFIG_TSIF_MODULE)
	msm_device_tsif.dev.platform_data = &tsif_platform_data;
#endif
//LGE_FW_TDMB [END]

	platform_add_devices(devices, ARRAY_SIZE(devices));

	msm_device_i2c_init();
	msm_device_i2c_2_init();
	qup_device_i2c_init();

#if defined(CONFIG_LGE_MODEL_E739)
    register_board_info();
#else
	msm7x30_init_marimba();

#endif


#ifdef CONFIG_I2C_SSBI
	msm_device_ssbi6.dev.platform_data = &msm_i2c_ssbi6_pdata;
	msm_device_ssbi7.dev.platform_data = &msm_i2c_ssbi7_pdata;
#endif

	/* initialize pm */
	pmic8058_buses_init();
	lge_pm_set_platform_data();

	/* add lcd devices */
	lge_add_lcd_devices();

	/* add mmc devices */
	lge_add_mmc_devices();

	/* add misc devices */
	lge_add_misc_devices();

	/* add input devices */
	lge_add_input_devices();
//LGE_FW_TDMB [START]
#ifdef CONFIG_LGE_BROADCAST	
	lge_add_broadcast_dmb_devices();
#endif
//LGE_FW_TDMB [END]

	/* gpio i2c devices should be registered at latest point */
	lge_add_gpio_i2c_devices();

	/* add buletooth devices */
	lge_add_btpower_devices();

	/* add camera devices */
	lge_add_camera_devices();

	/* add ram console device */
#ifdef CONFIG_ANDROID_RAM_CONSOLE
	lge_add_ramconsole_devices();
#endif

#if defined(CONFIG_ANDROID_RAM_CONSOLE) && defined(CONFIG_LGE_HANDLE_PANIC)
	lge_add_panic_handler_devices();
#endif
}

static void __init msm7x30_map_io(void)
{
	msm_shared_ram_phys = 0x00100000;
	msm_map_msm7x30_io();
	msm7x30_allocate_memory_regions();
}

MACHINE_START(MSM8X55_VICTOR, "LGE MSM8X55 VICTOR")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io  = MSM_DEBUG_UART_PHYS,
	.io_pg_offst = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.boot_params = PHYS_OFFSET + 0x100,
	.map_io = msm7x30_map_io,
	.init_irq = msm7x30_init_irq,
	.init_machine = msm7x30_init,
	.timer = &msm_timer,
MACHINE_END
