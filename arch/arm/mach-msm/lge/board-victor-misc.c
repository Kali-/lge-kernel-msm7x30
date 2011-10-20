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
#include <linux/power_supply.h>

#include <linux/pwm.h>
#ifdef CONFIG_PMIC8058_PWM
#include <linux/mfd/pmic8058.h>
#include <linux/pmic8058-pwm.h>
#endif

#include <mach/msm_battery.h>
#include <mach/board.h>
#include <mach/board_lge.h>

#include "board-victor.h"

#ifdef CONFIG_PMIC8058_PWM
#define PWM_LEVEL	64
#define PWM_PEROID	40
#define HALF_DUTY   32 /* set duty cycle 50% */
#define ENABLE_DUTY 62 /* set duty cycle  6% */

static struct pwm_device *vib_pwm = 0;
#endif

#ifdef CONFIG_LGE_FUEL_GAUGE
static u32 victor_battery_capacity(u32 current_soc)
{
	if(current_soc > 100)
		current_soc = 100;

	return current_soc;
}
#endif

static struct msm_psy_batt_pdata msm_psy_batt_data = {
	.voltage_min_design 	= 3200,
	.voltage_max_design	= 4200,
	.avail_chg_sources   	= AC_CHG | USB_CHG ,
	.batt_technology        = POWER_SUPPLY_TECHNOLOGY_LION,
#ifdef CONFIG_LGE_FUEL_GAUGE
	.calculate_capacity		= victor_battery_capacity,
#endif
};

static struct platform_device msm_batt_device = {
	.name 		    = "msm-battery",
	.id		    = -1,
	.dev.platform_data  = &msm_psy_batt_data,
};

int victor_vibrator_power_set(int enable)
{
	if (enable)
		gpio_direction_output(MOTER_EN, 1);
	else
		gpio_direction_output(MOTER_EN, 0);

	return 0;
}

int victor_vibrator_pwm_set(int enable, int amp)
{
#ifdef CONFIG_PMIC8058_PWM
	if (vib_pwm == 0)
		vib_pwm = pwm_request(0, "vibrator pwm");

	if (enable) {
		pwm_config(vib_pwm, amp * PWM_PEROID / PWM_LEVEL, PWM_PEROID);
		pwm_enable(vib_pwm);
	} else
	//nthyunjin.yang 110530 for pwm status in sleep mode [START]
	#ifdef CONFIG_LGE_MODEL_SU610
		pwm_disable(vib_pwm);
	#else
		pwm_config(vib_pwm, HALF_DUTY * PWM_PEROID / PWM_LEVEL, PWM_PEROID);
		pwm_enable(vib_pwm);
	#endif
	//nthyunjin.yang 110530 for pwm status in sleep mode [END]
#endif
	return 0;
}

int victor_vibrator_ic_enable_set(int enable)
{
	/* nothing to do, thunder does not using Motor Enable pin */
	return 0;
}

static struct android_vibrator_platform_data victor_vibrator_data = {
	.enable_status = 0,
	.power_set = victor_vibrator_power_set,
	.pwm_set = victor_vibrator_pwm_set,
	.ic_enable_set = victor_vibrator_ic_enable_set,
	.amp_value = ENABLE_DUTY,
};

static struct platform_device android_vibrator_device = {
	.name   = "android-vibrator",
	.id = -1,
	.dev = {
		.platform_data = &victor_vibrator_data,
	},
};


/* misc platform devices */
static struct platform_device *misc_devices[] __initdata = {
	&msm_batt_device,
	&android_vibrator_device,
};

/* main interface */
void __init lge_add_misc_devices(void)
{
	gpio_request(MOTER_EN, "motor enable");
	platform_add_devices(misc_devices, ARRAY_SIZE(misc_devices));
}
