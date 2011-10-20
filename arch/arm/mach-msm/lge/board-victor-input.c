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
#include <linux/platform_device.h>
#include <linux/gpio_event.h>

#include <mach/vreg.h>
#include <mach/rpc_server_handset.h>
#include <mach/board.h>
#include <mach/board_lge.h>
#ifdef CONFIG_LGE_MODEL_SU610
#include <linux/pn544.h>	//seokmin for NFC
#endif
#include "board-victor.h"


#define GPIO_VOL_UP		180
#define GPIO_VOL_DOWN	147
	
static struct gpio_event_direct_entry victor_keypad_switch_map[] = {
	{ GPIO_VOL_UP,			KEY_VOLUMEUP		},
	{ GPIO_VOL_DOWN,		KEY_VOLUMEDOWN		},
};

static void victor_gpio_event_input_init(void)
{
	gpio_tlmm_config(GPIO_CFG(GPIO_VOL_UP, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(GPIO_VOL_DOWN, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	// add wakeup source vol up/down key  [younchan.kim@lge.com]
	enable_irq_wake(MSM_GPIO_TO_INT(GPIO_VOL_UP));
	enable_irq_wake(MSM_GPIO_TO_INT(GPIO_VOL_DOWN));
}

static struct gpio_event_input_info victor_keypad_switch_info = {
	.info.func = gpio_event_input_func,
	/* irq for side key do not disable when suspend because of volume control on BGM
	 * 2010-05-10, bob.cho@lge.com
	 */	
	.info.no_suspend = true,
	.flags = 0,
	.type = EV_KEY,
	.keymap = victor_keypad_switch_map,
	.keymap_size = ARRAY_SIZE(victor_keypad_switch_map)
};

static struct gpio_event_info *victor_keypad_info[] = {
	&victor_keypad_switch_info.info,
};

static int victor_gpio_keypad_power(
		const struct gpio_event_platform_data *pdata, bool on)
{
	/* this is dummy function to make gpio_event driver register suspend function
	 * 2010-01-29, cleaneye.kim@lge.com
	 */

	return 0;
}

static struct gpio_event_platform_data victor_keypad_data = {
	.name = "gpio-side-keypad",
	.info = victor_keypad_info,
	.info_count = ARRAY_SIZE(victor_keypad_info),
	.power = victor_gpio_keypad_power,
};

static struct platform_device victor_gpio_keypad_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 1,
	.dev        = {
		.platform_data  = &victor_keypad_data,
	},
};

/* atcmd virtual device : AT%FKPD */
static unsigned short atcmd_virtual_keycode[ATCMD_VIRTUAL_KEYPAD_ROW][ATCMD_VIRTUAL_KEYPAD_COL] = {
	{KEY_1, 		KEY_8, 				KEY_Q,  	 KEY_I,          KEY_D,      	KEY_HOME,	KEY_B,          KEY_UP},
	{KEY_2, 		KEY_9, 		  		KEY_W,		 KEY_O,       	 KEY_F,		 	KEY_RIGHTSHIFT, 	KEY_N,			KEY_DOWN},
	{KEY_3, 		KEY_0, 		  		KEY_E,		 KEY_P,          KEY_G,      	KEY_Z,        	KEY_M, 			KEY_UNKNOWN},
	{KEY_4, 		KEY_BACK,  			KEY_R,		 KEY_SEARCH,     KEY_H,			KEY_X,    		KEY_LEFTSHIFT,	KEY_UNKNOWN},
	{KEY_5, 		KEY_BACKSPACE, 		KEY_T,		 KEY_LEFTALT,    KEY_J,      	KEY_C,     		KEY_REPLY,    KEY_CAMERA},
	{KEY_6, 		KEY_ENTER,  		KEY_Y,  	 KEY_A,		     KEY_K,			KEY_V,  	    KEY_RIGHT,     	KEY_UNKNOWN},
	{KEY_7, 		KEY_MENU,	KEY_U,  	 KEY_S,    		 KEY_L, 	    KEY_SPACE,      KEY_LEFT,     	KEY_SEND},
	{KEY_UNKNOWN, 	KEY_UNKNOWN,  		KEY_UNKNOWN, KEY_UNKNOWN, 	 KEY_UNKNOWN,	KEY_UNKNOWN,    KEY_UNKNOWN,      	KEY_UNKNOWN},

};

static struct atcmd_virtual_platform_data atcmd_virtual_pdata = {
	.keypad_row = ATCMD_VIRTUAL_KEYPAD_ROW,
	.keypad_col = ATCMD_VIRTUAL_KEYPAD_COL,
	.keycode = (unsigned char *)atcmd_virtual_keycode,
};

static struct platform_device atcmd_virtual_device = {
	.name = "atcmd_virtual_kbd",
	.id = -1,
	.dev = {
		.platform_data = &atcmd_virtual_pdata,
	},
};

// START [sangki.hyun@lge.com] [dom_testmode] 20100710 LAB1_FW TESTMODE_INPUT {
// TESTMODE
#ifdef CONFIG_MSM_RPCSERVER_TESTMODE
struct testmode_input_platform_data {
	const char *name;
};

static struct testmode_input_platform_data testmode_input_pdata = {
	.name = "testmode_input",
};

static struct platform_device testmode_input_device = {
	.name = "testmode_input",
	.id = -1,
	.dev    = {
		.platform_data = &testmode_input_pdata
	},
};
#endif
// END [sangki.hyun@lge.com] [dom_testmode] 20100710 LAB1_FW }

/* head set device */
static struct msm_handset_platform_data hs_platform_data = {
	.hs_name = "7k_handset",
	.pwr_key_delay_ms = 500, /* 0 will disable end key */
};

static struct platform_device hs_device = {
	.name   = "msm-handset",
	.id     = -1,
	.dev    = {
		.platform_data = &hs_platform_data,
	},
};

static struct platform_device headset_device = {
	.name   = "lge_headset",
	.id     = -1,
};

static uint32_t key_touch_ldo_config =
  GPIO_CFG(KEY_TOUCH_GPIO_LDO, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA);

static int kt_set_vreg(unsigned char onoff)
{
	struct vreg *vreg_kt;
	int rc;

	printk(KERN_INFO"[KeyTouch] %s() status: %d\n", __func__, onoff);

	gpio_tlmm_config(key_touch_ldo_config,	GPIO_CFG_ENABLE);
	
	vreg_kt = vreg_get(0, "lvsw1");

#ifdef CONFIG_LGE_DOMESTIC
printk("[touch power] %s : %d\n",__func__,__LINE__);
#endif

	if (IS_ERR(vreg_kt)) {
		printk(KERN_ERR"[KeyTouch] vreg_get fail : touch\n");
		return -1;
	}

#ifdef CONFIG_LGE_DOMESTIC
printk("[touch power] %s : %d\n",__func__,__LINE__);
#endif

	if (onoff) {
#ifdef CONFIG_LGE_DOMESTIC
printk("[touch power] %s : %d\n",__func__,__LINE__);
#endif
		rc = vreg_set_level(vreg_kt, 1800);
		
		if (rc != 0) {
			printk(KERN_ERR"[KeyTouch] Controller vreg_set_level failed\n");
			return -1;
		}
		vreg_enable(vreg_kt);
		
		gpio_set_value(KEY_TOUCH_GPIO_LDO, 1);
		
	} else {
#ifdef CONFIG_LGE_DOMESTIC
printk("[touch power] %s : %d\n",__func__,__LINE__);
#endif
		vreg_disable(vreg_kt);

		gpio_set_value(KEY_TOUCH_GPIO_LDO, 0);
	}
#ifdef CONFIG_LGE_DOMESTIC
printk("[touch power] %s : %d\n",__func__,__LINE__);
#endif
	return 0;
}

static struct gpio_i2c_pin key_touch_i2c_pin[] = {
	[0] = {
		.sda_pin    = KEY_TOUCH_GPIO_I2C_SDA,
		.scl_pin    = KEY_TOUCH_GPIO_I2C_SCL,
		.reset_pin  = 0,
		.irq_pin    = KEY_TOUCH_GPIO_INT,
	},
};

static struct i2c_gpio_platform_data key_touch_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device key_touch_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &key_touch_i2c_pdata,
};

static unsigned short key_touch_map[] = {
	KEY_BACK,
	KEY_HOME,
	KEY_MENU,
	KEY_RESERVED,
};

static struct key_touch_platform_data kt_pdata = {
	.power 	= kt_set_vreg,
	.irq   	= KEY_TOUCH_GPIO_INT,
	.scl  	= KEY_TOUCH_GPIO_I2C_SCL,
	.sda  	= KEY_TOUCH_GPIO_I2C_SDA,
	.keycode = (unsigned char *)key_touch_map,
	.keycodemax = (ARRAY_SIZE(key_touch_map) * 2),
};

static struct i2c_board_info key_touch_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("so340010", SO340010_I2C_ADDRESS),
		.type = "so340010",
		.platform_data = &kt_pdata,
	},
};

static int main_ts_set_vreg(unsigned char onoff)
{
	struct vreg *vreg_touch_avdd, *vreg_touch_vdd;
	int rc;

	printk(KERN_INFO"[MainTouch] %s() onoff:%d\n", __func__, onoff);

	vreg_touch_avdd = vreg_get(0, "wlan");
	vreg_touch_vdd = vreg_get(0, "wlan2");

	if (IS_ERR(vreg_touch_avdd) || IS_ERR(vreg_touch_vdd)) {
		printk(KERN_INFO"[MainTouch] vreg_get fail : touch\n");
		return -1;
	}

	if (onoff) {
		rc = vreg_set_level(vreg_touch_avdd, 2700);
		if (rc != 0) {
			printk(KERN_INFO"[MainTouch] avdd vreg_set_level failed\n");
			return -1;
		}

		rc = vreg_enable(vreg_touch_avdd);
		if (rc != 0)
			printk(KERN_INFO"[MainTouch] avdd vreg_enable failed\n");

		rc = vreg_set_level(vreg_touch_vdd, 1800);
		if (rc != 0) {
			printk(KERN_INFO"[MainTouch] vdd vreg_set_level failed\n");
			return -1;
		}

		rc = vreg_enable(vreg_touch_vdd);

		if (rc != 0)
			printk(KERN_INFO"[MainTouch] vdd vreg_enable failed\n");

	} else {
		rc = vreg_disable(vreg_touch_avdd);
		if (rc != 0)
			printk(KERN_INFO"[MainTouch] avdd vreg_disable failed\n");

		rc = vreg_disable(vreg_touch_vdd);
		if (rc != 0)
			printk(KERN_INFO"[MainTouch] vdd vreg_disable failed\n");
	}

	return 0;
}

#if defined(CONFIG_TOUCHSCREEN_QT602240) || defined(CONFIG_TOUCHSCREEN_SU610)
static struct qt602240_platform_data main_ts_pdata = {
	.x_line		= 19,
	.y_line		= 11,
	.x_size		= 800,
	.y_size		= 480,
	.blen		= 32,
	.threshold	= 55,
	.voltage	= 2700000,
	.orient		= 7,
	.power		= main_ts_set_vreg,
	.gpio_int	= MAIN_TS_GPIO_INT,
	.irq		= MAIN_TS_GPIO_IRQ,
	.scl		= MAIN_TS_GPIO_I2C_SCL,
	.sda		= MAIN_TS_GPIO_I2C_SDA,
	.reset		= MAIN_TS_GPIO_RESET,
};
#else
static struct touch_platform_data main_ts_pdata = {
	.ts_x_min   = MAIN_TS_X_MIN,
	.ts_x_max   = MAIN_TS_X_MAX,
	.ts_y_min   = MAIN_TS_Y_MIN,
	.ts_y_max   = MAIN_TS_Y_MAX,
    .ts_y_start = MAIN_TS_Y_START,
	.power      = main_ts_set_vreg,
	.gpio_int   = MAIN_TS_GPIO_INT,
	.irq        = MAIN_TS_GPIO_IRQ,
	.scl        = MAIN_TS_GPIO_I2C_SCL,
	.sda        = MAIN_TS_GPIO_I2C_SDA,
	.hw_i2c     = 1,
};
#endif

static struct i2c_board_info touch_panel_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("qt602240_ts", MXT224_TS_I2C_SLAVE_ADDR),
		.type = "qt602240_ts",
		.platform_data = &main_ts_pdata,
		.irq = MAIN_TS_GPIO_IRQ,
	},
};

#define MASK_ACCEL		0x01
#define MASK_ECOM		0x02
#define MASK_PROC		0x04
static int gp7_vreg_contorl(unsigned char onoff, unsigned char mask)
{
	int ret = 0;
	static unsigned char gp7_on_mask = 0;
	struct vreg *gp7_vreg = vreg_get(0, "gp7");

	if (onoff) {
		if(gp7_on_mask == 0) {
			vreg_set_level(gp7_vreg, 1800);
			vreg_enable(gp7_vreg);
		}

		gp7_on_mask |= mask;
	} else {
		gp7_on_mask &= ~mask;

		if(gp7_on_mask == 0)
			vreg_disable(gp7_vreg);
	}

	return ret;
}

static int xo_ou_vreg_contorl(unsigned char onoff, unsigned char mask)
{
	int ret = 0;
	static unsigned char xo_out_on_mask = 0;
	struct vreg *xo_out_vreg = vreg_get(0, "xo_out");

	if (onoff) {
		if(xo_out_on_mask == 0) {
			vreg_set_level(xo_out_vreg, 2600);
			vreg_enable(xo_out_vreg);
		}
		xo_out_on_mask |= mask;
	} else
	{
		xo_out_on_mask &= ~mask;

		if(xo_out_on_mask == 0)
			vreg_disable(xo_out_vreg);
	}

	return ret;
}

/* accelerometer */
static int accel_power(unsigned char onoff)
{
	int ret = 0;

	if (onoff) {
		gp7_vreg_contorl( 1, MASK_ACCEL);
		xo_ou_vreg_contorl( 1, MASK_ACCEL);
	} else
	{
		xo_ou_vreg_contorl( 0, MASK_ACCEL);
		gp7_vreg_contorl( 0, MASK_ACCEL);
	}

	return ret;
}

struct acceleration_platform_data bma250 = {
#ifdef CONFIG_SENSOR_BMA250
	.irq_num	= ACCEL_GPIO_INT,
	.irq_num2	= ACCEL_GPIO_INT2,
#endif
	.power = accel_power,
};

static struct gpio_i2c_pin accel_i2c_pin[] = {
	[0] = {
		.sda_pin	= ACCEL_GPIO_I2C_SDA,
		.scl_pin	= ACCEL_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= ACCEL_GPIO_INT,
	},
};

static struct i2c_gpio_platform_data accel_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device accel_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &accel_i2c_pdata,
};

static struct i2c_board_info accel_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("bma250", ACCEL_I2C_ADDRESS),
		.type = "bma250",
		.platform_data = &bma250,
	},
};

static void __init victor_init_i2c_acceleration(int bus_num)
{
	accel_i2c_device.id = bus_num;

	init_gpio_i2c_pin(&accel_i2c_pdata, accel_i2c_pin[0], &accel_i2c_bdinfo[0]);

	i2c_register_board_info(bus_num, &accel_i2c_bdinfo[0], 1);

	platform_device_register(&accel_i2c_device);
}

/* ecompass */
static int ecom_power_set(unsigned char onoff)
{
	int ret = 0;

	if (onoff) {
		gp7_vreg_contorl( 1, MASK_ECOM);
		xo_ou_vreg_contorl( 1, MASK_ECOM);
	} else
	{
		gp7_vreg_contorl( 0, MASK_ECOM);
		xo_ou_vreg_contorl( 0, MASK_ECOM);
	}

	return ret;
}

static struct ecom_platform_data ecom_pdata = {
	.pin_int        = ECOM_GPIO_INT,
	.pin_rst		= 0,
	.power          = ecom_power_set,
};

static struct i2c_board_info ecom_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("ami304_sensor", ECOM_I2C_ADDRESS),
		.type = "ami304_sensor",
		.platform_data = &ecom_pdata,
	},
};

static struct gpio_i2c_pin ecom_i2c_pin[] = {
	[0] = {
		.sda_pin	= ECOM_GPIO_I2C_SDA,
		.scl_pin	= ECOM_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= ECOM_GPIO_INT,
	},
};

static struct i2c_gpio_platform_data ecom_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device ecom_i2c_device = {
		.name = "i2c-gpio",
		.dev.platform_data = &ecom_i2c_pdata,
};

static void __init victor_init_i2c_ecom(int bus_num)
{
	ecom_i2c_device.id = bus_num;

	init_gpio_i2c_pin(&ecom_i2c_pdata, ecom_i2c_pin[0], &ecom_i2c_bdinfo[0]);

	i2c_register_board_info(bus_num, &ecom_i2c_bdinfo[0], 1);
	platform_device_register(&ecom_i2c_device);
}

/* proximity */
static int prox_power_set(unsigned char onoff)
{
	int ret = 0;

	if (onoff) {
		gp7_vreg_contorl( 1, MASK_PROC);
		xo_ou_vreg_contorl( 1, MASK_PROC);
	} else
	{
		gp7_vreg_contorl( 0, MASK_PROC);
		xo_ou_vreg_contorl( 0, MASK_PROC);
	}

	return ret;
}

static struct proximity_platform_data proxi_pdata = {
	
	//hyunjee.yoon@lge.com 2011-06-23
	#ifdef CONFIG_LGE_MODEL_SU610
	.irq_num	= PROXI_GPIO_DOUT,
	.power		= prox_power_set,
	.methods		= 0,
	.operation_mode		= 0,
	.debounce	 = 0,
	.cycle = 0,
		#else 
	.irq_num	= PROXI_GPIO_DOUT,
	.power		= prox_power_set,
	.methods		= 0,
	.operation_mode		= 1,
	.debounce	 = 0,
	.cycle = 2,
	#endif
	//hyunjee.yoon@lge.com 2011-06-23
};

static struct i2c_board_info prox_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("proximity_gp2ap", PROXI_I2C_ADDRESS),
		.type = "proximity_gp2ap",
		.platform_data = &proxi_pdata,
	},
};

static struct gpio_i2c_pin proxi_i2c_pin[] = {
	[0] = {
		.sda_pin	= PROXI_GPIO_I2C_SDA,
		.scl_pin	= PROXI_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= PROXI_GPIO_DOUT,
	},
};

static struct i2c_gpio_platform_data proxi_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device proxi_i2c_device = {
    .name = "i2c-gpio",
    .dev.platform_data = &proxi_i2c_pdata,
};


static void __init victor_init_i2c_prox(int bus_num)
{
	proxi_i2c_device.id = bus_num;

	init_gpio_i2c_pin(&proxi_i2c_pdata, proxi_i2c_pin[0], &prox_i2c_bdinfo[0]);

	i2c_register_board_info(bus_num, &prox_i2c_bdinfo[0], 1);
	platform_device_register(&proxi_i2c_device);
}

#ifdef CONFIG_LGE_MODEL_SU610
//seokmin for NFC
static struct gpio_i2c_pin nfc_i2c_pin[] = {
	[0] = {
		.sda_pin		= NFC_GPIO_I2C_SDA,
		.scl_pin		= NFC_GPIO_I2C_SCL,
		.reset_pin		= NFC_GPIO_VEN,		
		.irq_pin		= NFC_GPIO_IRQ,
	},
};

static struct i2c_gpio_platform_data nfc_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device nfc_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &nfc_i2c_pdata,
};

static struct pn544_i2c_platform_data nfc_pdata = {
	.ven_gpio 		= NFC_GPIO_VEN,
	.irq_gpio 	 	= NFC_GPIO_IRQ,
	.scl_gpio		= NFC_GPIO_I2C_SCL,
	.sda_gpio		= NFC_GPIO_I2C_SDA,
	.firm_gpio		= NFC_GPIO_SW_UPGRADE,
};

static struct i2c_board_info nfc_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("pn544", NFC_I2C_SLAVE_ADDR),
		.type = "pn544",
		.platform_data = &nfc_pdata,
	},
};

static void __init victor_init_i2c_nfc(int bus_num)
{
	int ret;
	nfc_i2c_device.id = bus_num;

	printk("[seokmin]victor_init_i2c_nfc in\n");
	ret = init_gpio_i2c_pin(&nfc_i2c_pdata, nfc_i2c_pin[0],	&nfc_i2c_bdinfo[0]);
  
	ret = i2c_register_board_info(bus_num, &nfc_i2c_bdinfo[0], 1);

	platform_device_register(&nfc_i2c_device);
}
#endif

static void __init key_touch(int bus_num)
{
    key_touch_i2c_device.id = bus_num;

	kt_set_vreg(1);

	init_gpio_i2c_pin(&key_touch_i2c_pdata, key_touch_i2c_pin[0], &key_touch_i2c_bdinfo[0]);

	i2c_register_board_info(bus_num, &key_touch_i2c_bdinfo[0], 1);  /* KT */

	platform_device_register(&key_touch_i2c_device);
}

static void __init touch_panel(int bus_num)
{
	main_ts_set_vreg(1);

	/* Fix Bus Num to 0, Because Touch SCL, SDA Use H/W I2C */
	i2c_register_board_info(0, &touch_panel_i2c_bdinfo[0], 1);
}

/* input platform device */
static struct platform_device *victor_input_devices[] __initdata = {
	&hs_device,
	&headset_device,
	&victor_gpio_keypad_device,
	&atcmd_virtual_device,
// START [sangki.hyun@lge.com][dom_testmode] 20100710 LAB1_FW TESTMODE_INPUT {
#ifdef CONFIG_MSM_RPCSERVER_TESTMODE
	&testmode_input_device,
#endif
// END [sangki.hyun@lge.com][dom_testmode] 20100710 LAB1_FW }
};

/* common function */
void __init lge_add_input_devices(void)
{
	victor_gpio_event_input_init();
    platform_add_devices(victor_input_devices, ARRAY_SIZE(victor_input_devices));

	lge_add_gpio_i2c_device(touch_panel);
	lge_add_gpio_i2c_device(key_touch);
	lge_add_gpio_i2c_device(victor_init_i2c_acceleration);
	lge_add_gpio_i2c_device(victor_init_i2c_ecom);
	lge_add_gpio_i2c_device(victor_init_i2c_prox);
#ifdef CONFIG_LGE_MODEL_SU610
	lge_add_gpio_i2c_device(victor_init_i2c_nfc);	//seokmin for NFC
//	gpio_tlmm_config(GPIO_CFG(NFC_GPIO_SW_UPGRADE, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);	//seokmin for NFC
#endif
}
