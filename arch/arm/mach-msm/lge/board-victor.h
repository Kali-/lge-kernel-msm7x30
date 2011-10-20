/* arch/arm/mach-msm/include/mach/board-victor.h
 * Copyright (C) 2009 LGE, Inc.
 * Author: SungEun Kim <cleaneye@lge.com>
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
#ifndef __ARCH_MSM_BOARD_VICTOR_H
#define __ARCH_MSM_BOARD_VICTOR_H

#include <linux/types.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <asm/setup.h>

#include <mach/msm_iomap.h>

#include "pm.h"

/* MSM_PMEM_SF_SIZE PMEM Size 0x1700000 --> 0x1C00000
* 2011-05-07, cheongil.hyun@lge.com
* 2011-08-20, Increas SF PMEM(0x1D00000), munyoung.hwang@lge.com
*/
#define MSM_PMEM_SF_SIZE	0x1D00000
#define MSM_FB_SIZE		0x500000
//[LGE_UPDATE_S] taeyol.kim@lge.com 2011-06-27 : To support 720P and VT rotation, one more preview buffer is needed. => need to increase pmem
#if 0
#define MSM_PMEM_ADSP_SIZE      0x1800000
#else
#define MSM_PMEM_ADSP_SIZE      0x1B00000
#endif
//[LGE_UPDATE_S] taeyol.kim@lge.com 2011-06-27
#define MSM_FLUID_PMEM_ADSP_SIZE	0x2800000
#define PMEM_KERNEL_EBI1_SIZE   0x600000
#define MSM_PMEM_AUDIO_SIZE     0x200000

#if 1	// kh.tak MicroSD Detection
#define SYS_GPIO_SD_DET    42  /* SYS SD Detection GPIO Number 42 */
//#define SYS_GPIO_SD_EN_N   87 // 56 /* SYS SD Enable GPIO Number 56*/
#endif

#define PMIC_GPIO_INT		27
#define PMIC_VREG_WLAN_LEVEL	2900
#define PMIC_GPIO_SD_DET	36
#define PMIC_GPIO_SDC4_EN	17  /* PMIC GPIO Number 18 */
#define PMIC_GPIO_HDMI_5V_EN_V3 32  /* PMIC GPIO for V3 H/W */
#define PMIC_GPIO_HDMI_5V_EN_V2 39 /* PMIC GPIO for V2 H/W */


#define FPGA_SDCC_STATUS       0x8E0001A8

#define FPGA_OPTNAV_GPIO_ADDR	0x8E000026
#define OPTNAV_I2C_SLAVE_ADDR	(0xB0 >> 1)
#define OPTNAV_IRQ		20
#define OPTNAV_CHIP_SELECT	19

/* Macros assume PMIC GPIOs start at 0 */
#define PM8058_GPIO_PM_TO_SYS(pm_gpio)     (pm_gpio + NR_GPIO_IRQS)
#define PM8058_GPIO_SYS_TO_PM(sys_gpio)    (sys_gpio - NR_GPIO_IRQS)

#define PMIC_GPIO_HAP_ENABLE   16  /* PMIC GPIO Number 17 */

#define HAP_LVL_SHFT_MSM_GPIO 24

#define PMIC_GPIO_QUICKVX_CLK 37 /* PMIC GPIO 38 */

#define	PM_FLIP_MPP 5 /* PMIC MPP 06 */

#ifdef CONFIG_LGE_MODEL_SU610
//seokmin for NFC
/* nfc */
#define	NFC_GPIO_VEN			167
#define NFC_GPIO_I2C_SDA		165
#define NFC_GPIO_I2C_SCL		166
//#define NFC_GPIO_CLK_REQ		167
//#define NFC_GPIO_SW_UPGRADE		168
#define NFC_GPIO_SW_UPGRADE		157 // REV B
#define NFC_GPIO_IRQ			162
#define NFC_I2C_SLAVE_ADDR		0x28
#endif

/* virtual key */
#define ATCMD_VIRTUAL_KEYPAD_ROW	8
#define ATCMD_VIRTUAL_KEYPAD_COL	8

/* key_touch */
#define KEY_TOUCH_GPIO_INT      176
#define KEY_TOUCH_GPIO_I2C_SCL  175
#define KEY_TOUCH_GPIO_I2C_SDA  174
#define KEY_TOUCH_GPIO_LDO  	124

#define SO340010_I2C_ADDRESS   0x2C

/* touch-screen */
#define MAIN_TS_X_MIN				0
#define MAIN_TS_X_MAX				480
#define MAIN_TS_Y_MIN				0
#define MAIN_TS_Y_MAX				800
#define MAIN_TS_Y_START				0
#define MAIN_TS_GPIO_RESET			121
#define MAIN_TS_GPIO_I2C_SDA		71
#define MAIN_TS_GPIO_I2C_SCL		70
#define MAIN_TS_GPIO_INT			43
#define MAIN_TS_GPIO_IRQ			MSM_GPIO_TO_INT(MAIN_TS_GPIO_INT)

#define MXT224_TS_I2C_SLAVE_ADDR	0x4A

/* accelerometer */
#define ACCEL_GPIO_INT	 		171
#define ACCEL_GPIO_INT2	 		168
#define ACCEL_GPIO_I2C_SCL  	178
#define ACCEL_GPIO_I2C_SDA  	177
#define ACCEL_I2C_ADDRESS		0x18

/*Ecompass*/
#define ECOM_GPIO_I2C_SCL		173
#define ECOM_GPIO_I2C_SDA		172
#define ECOM_GPIO_INT			123
#define ECOM_I2C_ADDRESS		0x0E /* slave address 7bit */

/* proximity */
#define PROXI_GPIO_I2C_SCL		149
#define PROXI_GPIO_I2C_SDA		150
#define PROXI_GPIO_DOUT			50
#define PROXI_I2C_ADDRESS		0x44 /*slave address 7bit*/

/* vibrator */
#define MOTER_EN				125

#define GP_MN_CLK_MDIV_REG		0x004C
#define GP_MN_CLK_NDIV_REG		0x0050
#define GP_MN_CLK_DUTY_REG		0x0054

/* about 22.93 kHz, should be checked */
#define GPMN_M_DEFAULT			21
#define GPMN_N_DEFAULT			4500
/* default duty cycle = disable motor ic */
#define GPMN_D_DEFAULT			(GPMN_N_DEFAULT >> 1) 
#define PWM_MAX_HALF_DUTY		((GPMN_N_DEFAULT >> 1) - 80) /* minimum operating spec. should be checked */

#define GPMN_M_MASK				0x01FF
#define GPMN_N_MASK				0x1FFF
#define GPMN_D_MASK				0x1FFF

/* Audio Subsystem(AMP) I2C Number */
#define GPIO_AMP_I2C_SDA	89
#define GPIO_AMP_I2C_SCL	88

#define REG_WRITEL(value, reg)	writel(value, (MSM_WEB_BASE + reg))
/* LGE_FW_TDMB [START] */
#ifdef CONFIG_LGE_BROADCAST
enum 
{
	DMB_I2C_SDA 	= 39,
	DMB_I2C_SCL 	= 38,
	DMB_RESET_N 	= 33,
	DMB_PWR_EN 	= 41,
};
#endif
/* LGE_FW_TDMB {END] */
#endif
