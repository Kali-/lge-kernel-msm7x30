/* arch/arm/mach-msm/lge/board-ku3700-broadcast.c
 * Copyright (C) 2011 LGE, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/types.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/platform_device.h>
#include <asm/setup.h>
#include <mach/gpio.h>
#include <mach/board.h>
#include <mach/msm_iomap.h>
#include <mach/board_lge.h>
#include <linux/delay.h>

#include "board-victor.h"


#define DMB_I2C_SDA_PIN	39
#define DMB_I2C_SCL_PIN	38
#define DMB_RESET_N_PIN	33
#define DMB_PWR_EN_PIN	41
#define DMB_DEVICE_CHIP_ID	(0x80 >> 1)

	/*
		.sda_pin = DMB_I2C_SDA_PIN,
		.scl_pin = DMB_I2C_SCL_PIN,
	*/
static struct gpio_i2c_pin 	dmb_i2c_pin[] = {
	[0] = {
		.sda_pin = DMB_I2C_SCL_PIN,
		.scl_pin = DMB_I2C_SDA_PIN,
		.reset_pin = 0,
		.irq_pin = 0,
	},
};

static struct i2c_gpio_platform_data dmb_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay 			=2,
};

static struct platform_device	dmb_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &dmb_i2c_pdata,
};

void	broadcast_dmb_config_gpio_on(void)
{
}

void broadcast_dmb_config_gpio_off(void)
{
	
}


static void broadcast_dmb_dev_init_gpio(void)
{
	printk("broadcast_dmb_dev_init_gpio\n");
	gpio_tlmm_config(GPIO_CFG(DMB_PWR_EN_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
//	gpio_configure(DMB_PWR_EN_PIN, GPIOF_DRIVE_OUTPUT);
	gpio_direction_output(DMB_PWR_EN_PIN, 1);

//2011-4-4 sangyoun.han
	gpio_set_value(DMB_PWR_EN_PIN, 0);     /* power enable */

	gpio_tlmm_config(GPIO_CFG(DMB_RESET_N_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
//	gpio_configure(DMB_RESET_N_PIN, GPIOF_DRIVE_OUTPUT);
	gpio_direction_output(DMB_RESET_N_PIN, 1);
}

static int broadcast_dmb_dev_power_on(void)
{

	printk("broadcast_dmb_dev_power_on\n");
	gpio_set_value(DMB_RESET_N_PIN, 0);    /* reset low */
	
	gpio_set_value(DMB_PWR_EN_PIN, 1);     /* power enable */

	//udelay(700);
	mdelay(1);

	gpio_set_value(DMB_RESET_N_PIN, 1);   /* reset high */
	mdelay(1);


	gpio_set_value(DMB_RESET_N_PIN, 0);   /* reset low */
//	udelay(700);
	mdelay(1);

	gpio_set_value(DMB_RESET_N_PIN, 1);   /* reset high */
	//udelay(100);
	mdelay(1);
	return 1;
}

static int broadcast_dmb_dev_power_off(void)
{

	
	gpio_set_value(DMB_RESET_N_PIN, 0);
	udelay(100);

	gpio_set_value(DMB_PWR_EN_PIN, 0);
	return 1;
}

static struct broadcast_device_platform_data broadcast_device_data ={
	.dmb_gpio_on = broadcast_dmb_config_gpio_on,
	.dmb_gpio_off = broadcast_dmb_config_gpio_off,
	.dmb_gpio_init = broadcast_dmb_dev_init_gpio,
	.dmb_power_on = broadcast_dmb_dev_power_on,
	.dmb_power_off = broadcast_dmb_dev_power_off,
};

static struct i2c_board_info dmb_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("tdmb_t3900", DMB_DEVICE_CHIP_ID),
		.type = "tdmb_t3900",
		.platform_data = &broadcast_device_data,
	},
};


static void __init broadcast_init_i2c_device(int bus_num)
{
	printk("broadcast_init_i2c_device(bus_num=%d)\n",bus_num);

	dmb_i2c_device.id = bus_num;
	
	init_gpio_i2c_pin(&dmb_i2c_pdata, dmb_i2c_pin[0], &dmb_i2c_bdinfo[0]);
	i2c_register_board_info(bus_num, &dmb_i2c_bdinfo[0], 1);
	platform_device_register(&dmb_i2c_device);

}
void __init lge_add_broadcast_dmb_devices(void)
{
//	broadcast_init_device_gpio( );
	printk("lge_add_broadcast_dmb_devices\n");

	lge_add_gpio_i2c_device(broadcast_init_i2c_device);
}

