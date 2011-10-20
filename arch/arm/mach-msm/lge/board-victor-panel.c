/* arch/arm/mach-msm/lge/board-victor-mmc.c
 * Copyright (C) 2010 LGE Corporation.
 * Author: SungEun Kim <cleaneye.kim@lge.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/fb.h>
#include <linux/mfd/pmic8058.h>
#include <asm/mach-types.h>
#include <mach/mpp.h>
#include <mach/board.h>
#include <mach/vreg.h>
#include <mach/pmic.h>
#include <mach/rpc_pmapp.h>
#include <mach/board_lge.h>
#ifdef CONFIG_SPI_QSD
#include <linux/spi/spi.h>
#include <mach/msm_spi.h>
#include <mach/dma.h>
#endif
#include "devices.h"
#include "board-victor.h"

static unsigned panel_reset_gpio =
	GPIO_CFG(84, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA);

static int display_common_power(int on)
{
	int rc = 0, flag_on = !!on;
	static int display_common_power_save_on;
	struct vreg *vreg_lcd_3_0_v = NULL ; // L15 : "gp6"
	struct vreg *vreg_lcd_1_8_v = NULL ; // L16 : "gp10"


	if (display_common_power_save_on == flag_on)
		return 0;

	display_common_power_save_on = flag_on;

	if (on) {
		rc = gpio_tlmm_config(panel_reset_gpio, GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s: gpio_tlmm_config(%#x)=%d\n",
				       __func__, panel_reset_gpio, rc);
			return rc;
		}
	}

	vreg_lcd_3_0_v = vreg_get(NULL, "gp6");
	if (IS_ERR(vreg_lcd_3_0_v) ) {
		printk("%s: vreg_get(%s) failed (%ld)\n",__func__, "gp6", PTR_ERR(vreg_lcd_3_0_v));
		return -1;
	}

	vreg_lcd_1_8_v = vreg_get(NULL, "gp10");
	if (IS_ERR(vreg_lcd_1_8_v) ) {
		printk("%s: vreg_get(%s) failed (%ld)\n",__func__, "gp10", PTR_ERR(vreg_lcd_1_8_v));
		return -1;
	}

	rc = vreg_set_level(vreg_lcd_3_0_v, 3000);
	if (rc) {
		printk("%s: vreg_set level failed (%d)\n",	__func__, rc);
	}

	rc = vreg_set_level(vreg_lcd_1_8_v, 1800);
	if (rc) {
		printk("%s: vreg_set level failed (%d)\n",	__func__, rc);
	}

	if (on) {
		rc = vreg_enable(vreg_lcd_3_0_v);
		if (rc) {
			printk("%s: vreg_enable() = %d \n", __func__, rc);
		}

		mdelay(1);

		rc = vreg_enable(vreg_lcd_1_8_v);
		if (rc) {
			printk("%s: vreg_enable() = %d \n", __func__, rc);
		}

		rc = pmapp_display_clock_config(1);
		if (rc) {
			pr_err("%s pmapp_display_clock_config rc=%d\n",
					__func__, rc);
			return rc;
		}

		mdelay(20);

	}
	else {
		rc = pmapp_display_clock_config(0);
		if (rc) {
			pr_err("%s pmapp_display_clock_config rc=%d\n",
					__func__, rc);
			return rc;
		}

		rc = vreg_disable(vreg_lcd_1_8_v);
		if (rc) {
			printk("%s: vreg_disable() = %d \n", __func__, rc);
		}

		mdelay(1);

		rc = vreg_disable(vreg_lcd_3_0_v);
		if (rc) {
			printk("%s: vreg_disable() = %d \n", __func__, rc);
		}

		mdelay(20);
	}
	return rc;
}

static struct msm_gpio lcd_panel_gpios[] = {
/* Workaround, since HDMI_INT is using the same GPIO line (18), and is used as
 * input.  if there is a hardware revision; we should reassign this GPIO to a
 * new open line; and removing it will just ensure that this will be missed in
 * the future.
 */
	{ GPIO_CFG(18, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_grn0" },
	{ GPIO_CFG(19, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_grn1" },
	{ GPIO_CFG(20, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_blu0" },
	{ GPIO_CFG(21, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_blu1" },
	{ GPIO_CFG(22, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_blu2" },
	{ GPIO_CFG(23, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_red0" },
	{ GPIO_CFG(24, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_red1" },
	{ GPIO_CFG(25, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_red2" },
#ifdef CONFIG_SPI_QSD
	{ GPIO_CFG(45, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "spi_clk" },
	{ GPIO_CFG(46, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_UP, GPIO_CFG_2MA), "spi_cs0" },
	{ GPIO_CFG(47, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "spi_mosi" },
	{ GPIO_CFG(48, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "spi_miso" },
#endif
	{ GPIO_CFG(90, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_8MA), "lcdc_pclk" },
	{ GPIO_CFG(91, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_en" },
	{ GPIO_CFG(92, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_vsync" },
	{ GPIO_CFG(93, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_hsync" },
	{ GPIO_CFG(94, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_grn2" },
	{ GPIO_CFG(95, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_grn3" },
	{ GPIO_CFG(96, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_grn4" },
	{ GPIO_CFG(97, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_grn5" },
	{ GPIO_CFG(98, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_grn6" },
	{ GPIO_CFG(99, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_grn7" },
	{ GPIO_CFG(100, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_blu3" },
	{ GPIO_CFG(101, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_blu4" },
	{ GPIO_CFG(102, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_blu5" },
	{ GPIO_CFG(103, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_blu6" },
	{ GPIO_CFG(104, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_blu7" },
	{ GPIO_CFG(105, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_red3" },
	{ GPIO_CFG(106, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_red4" },
	{ GPIO_CFG(107, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_red5" },
	{ GPIO_CFG(108, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_red6" },
	{ GPIO_CFG(109, 1, GPIO_CFG_OUTPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "lcdc_red7" },
};

static struct msm_gpio lcd_panel_off_gpios[] = {
/* Workaround, since HDMI_INT is using the same GPIO line (18), and is used as
 * input.  if there is a hardware revision; we should reassign this GPIO to a
 * new open line; and removing it will just ensure that this will be missed in
 * the future.
 */
	{ GPIO_CFG(18, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_grn0" },
	{ GPIO_CFG(19, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_grn1" },
	{ GPIO_CFG(20, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_blu0" },
	{ GPIO_CFG(21, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_blu1" },
	{ GPIO_CFG(22, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_blu2" },
	{ GPIO_CFG(23, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_red0" },
	{ GPIO_CFG(24, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_red1" },
	{ GPIO_CFG(25, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_red2" },
#ifdef CONFIG_SPI_QSD
 	{ GPIO_CFG(45, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "spi_clk" },
	{ GPIO_CFG(46, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "spi_cs0" },
	{ GPIO_CFG(47, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "spi_mosi" },
	{ GPIO_CFG(48, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "spi_miso" },
#endif
 	{ GPIO_CFG(90, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), "lcdc_pclk" },
	{ GPIO_CFG(91, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_en" },
	{ GPIO_CFG(92, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_vsync" },
	{ GPIO_CFG(93, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_hsync" },
	{ GPIO_CFG(94, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_grn2" },
	{ GPIO_CFG(95, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_grn3" },
	{ GPIO_CFG(96, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_grn4" },
	{ GPIO_CFG(97, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_grn5" },
	{ GPIO_CFG(98, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_grn6" },
	{ GPIO_CFG(99, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_grn7" },
	{ GPIO_CFG(100, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_blu3" },
	{ GPIO_CFG(101, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_blu4" },
	{ GPIO_CFG(102, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_blu5" },
	{ GPIO_CFG(103, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_blu6" },
	{ GPIO_CFG(104, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_blu7" },
	{ GPIO_CFG(105, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_red3" },
	{ GPIO_CFG(106, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_red4" },
	{ GPIO_CFG(107, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_red5" },
	{ GPIO_CFG(108, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_red6" },
	{ GPIO_CFG(109, 0, GPIO_CFG_OUTPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), "lcdc_red7" },
};
static int lcdc_common_panel_power(int on)
{
	int rc, i;
	struct msm_gpio *gp;

	printk(KERN_INFO "%s   on/off[%d]\n",__func__, on);

 	if (on) {
		rc = display_common_power(on);
		if (rc < 0) {
			printk(KERN_ERR "%s display_common_power failed: %d\n",
					__func__, rc);
			return rc;
		}
		rc = msm_gpios_enable(lcd_panel_gpios,
				ARRAY_SIZE(lcd_panel_gpios));
		if (rc < 0) {
			printk(KERN_ERR "%s: gpio enable failed: %d\n",
					__func__, rc);
		}
		printk(KERN_INFO "%s   Power on\n",__func__);
	} else {	/* off */
		rc = msm_gpios_enable(lcd_panel_off_gpios,
				ARRAY_SIZE(lcd_panel_off_gpios));
		if (rc < 0) {
			printk(KERN_ERR "%s: gpio enable failed: %d\n",
					__func__, rc);
		}
		gp = lcd_panel_off_gpios;
		for (i = 0; i < ARRAY_SIZE(lcd_panel_off_gpios); i++) {
			/* ouput low */
			gpio_set_value(GPIO_PIN(gp->gpio_cfg), 0);
			gp++;
		}
		mdelay(20);
 		rc = display_common_power(on);
		if (rc < 0) {
			printk(KERN_ERR "%s display_common_power failed: %d\n",
					__func__, rc);
			return rc;
		}
		printk(KERN_INFO "%s   powe offed\n",__func__);
	}

	return rc;
}


static int lcdc_panel_power(int on)
{
	int flag_on = !!on;
	static int lcdc_power_save_on;

	if (lcdc_power_save_on == flag_on)
		return 0;

	lcdc_power_save_on = flag_on;

	return lcdc_common_panel_power(on);
}

static struct lcdc_platform_data lcdc_pdata = {
	.lcdc_power_save   = lcdc_panel_power,
};

#ifdef CONFIG_SPI_QSD
static struct spi_board_info lcdc_lgdisplay_spi_board_info[] __initdata = {
	{
		.modalias       = "lcdc_lgdislpay_lg4273_oled",
		.mode           = SPI_MODE_3|SPI_CS_HIGH,
		.bus_num        = 0,
		.chip_select    = 0,
		.max_speed_hz   = 9963243,
	}
};
#endif

#ifndef CONFIG_SPI_QSD
static int lcdc_lgdisplay_spi_gpio_num[] = {
	45, /* spi_clk */
	46, /* spi_cs  */
	47, /* spi_mosi */
	48, /* spi_miso */
};

static struct msm_gpio lcdc_gpio_config_data[] = {
	{ GPIO_CFG(45, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "spi_clk" },
	{ GPIO_CFG(46, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "spi_cs0" },
	{ GPIO_CFG(47, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "spi_mosi" },
	{ GPIO_CFG(48, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "spi_miso" },
};

static void lcdc_config_gpios(int enable)
{
	if (enable) {
		msm_gpios_request_enable(lcdc_gpio_config_data,
					      ARRAY_SIZE(
						      lcdc_gpio_config_data));
	}
	else
		msm_gpios_free(lcdc_gpio_config_data,
					    ARRAY_SIZE(
						    lcdc_gpio_config_data));
}
#endif

struct msm_fb_info_st {
	unsigned int width_mm ;
	unsigned int height_mm ;
};

/*write down the mm size of LCD */
static struct msm_fb_info_st msm_fb_info_data = {
	.width_mm = 50,
	.height_mm = 83
};

static int msm_fb_event_notify(struct notifier_block *self,
			      unsigned long action, void *data)
{
	struct fb_event *event = data;
	struct fb_info *info = event->info;
	struct msm_fb_info_st *fb_info_mm = &msm_fb_info_data;
	int ret = 0;

	switch(action) {
	case FB_EVENT_FB_REGISTERED:
		info->var.width = fb_info_mm->width_mm;
		info->var.height = fb_info_mm->height_mm;
		break;
	}
	return ret;
}

static struct msm_panel_common_pdata lcdc_lgdisplay_panel_data = {
#ifndef CONFIG_SPI_QSD
	.panel_config_gpio = lcdc_config_gpios,
	.gpio_num       	= lcdc_lgdisplay_spi_gpio_num,
#endif
	.gpio 				= 84,
};

static struct platform_device lcdc_lgdisplay_panel_device = {
	.name   = "lcdc_lgdisplay_wvga",
	.id     = 0,
	.dev    = {
		.platform_data = &lcdc_lgdisplay_panel_data,
	}
};

static struct msm_panel_common_pdata mdp_pdata = {
	.mdp_core_clk_rate = 122880000,
};

static struct notifier_block msm_fb_event_notifier = {
	.notifier_call	= msm_fb_event_notify,
};

static void __init msm_fb_add_devices(void)
{
	msm_fb_register_device("mdp", &mdp_pdata);
	msm_fb_register_device("lcdc", &lcdc_pdata);
}

void __init lge_add_lcd_devices(void)
{
	fb_register_client(&msm_fb_event_notifier);
	platform_device_register(&lcdc_lgdisplay_panel_device);
	msm_fb_add_devices();
#ifdef CONFIG_SPI_QSD
	msm_qsd_spi_init();
	spi_register_board_info(lcdc_lgdisplay_spi_board_info,
							ARRAY_SIZE(lcdc_lgdisplay_spi_board_info));
#endif
}
