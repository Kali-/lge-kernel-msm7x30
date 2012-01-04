
/*
  * arch/arm/mach-msm/include/mach/board_lge.h
  * Copyright (C) 2010 LGE Corporation.
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

#ifndef __ASM_ARCH_MSM_BOARD_LGE_H
#define __ASM_ARCH_MSM_BOARD_LGE_H

#include <linux/types.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/rfkill.h>
#include <linux/platform_device.h>
#include <asm/setup.h>

#if __GNUC__
#define __WEAK __attribute__((weak))
#endif

#define VENDOR_LGE	0x1004

#ifdef CONFIG_ARCH_MSM7X30

/* MSM_PMEM_SF_SIZE PMEM Size 0x1700000 --> 0x1C00000
* 2011-05-07, cheongil.hyun@lge.com
* 2011-08-20, Increas SF PMEM(0x1D00000), munyoung.hwang@lge.com
*/
#define MSM_PMEM_SF_SIZE	0x1D00000
#ifdef CONFIG_FB_MSM_TRIPLE_BUFFER
#define MSM_FB_SIZE             0x780000
#else
#define MSM_FB_SIZE             0x500000
#endif
#define MSM_GPU_PHYS_SIZE       SZ_2M
//[LGE_UPDATE_S] taeyol.kim@lge.com 2011-06-27 : To support 720P and VT rotation, one more preview buffer is needed. => need to increase pmem
#if 0
#define MSM_PMEM_ADSP_SIZE      0x1800000
#else
#define MSM_PMEM_ADSP_SIZE      0x1B00000
#endif
//[LGE_UPDATE_E] taeyol.kim@lge.com 2011-06-27
#define MSM_FLUID_PMEM_ADSP_SIZE	0x2800000
#define PMEM_KERNEL_EBI1_SIZE   0x600000
#define MSM_PMEM_AUDIO_SIZE     0x200000

#ifdef CONFIG_ANDROID_RAM_CONSOLE
#define LGE_RAM_CONSOLE_SIZE    (124 * SZ_1K)
#endif

#ifdef CONFIG_LGE_HANDLE_PANIC
#define LGE_CRASH_LOG_SIZE		(4 * SZ_1K)
#endif

/* board revision information */
enum {
	EVB         = 0,
	LGE_REV_A,
	LGE_REV_B,
	LGE_REV_C,
	LGE_REV_D,
	LGE_REV_E,
	LGE_REV_F,
	LGE_REV_10,
	LGE_REV_11,
	LGE_REV_12,
	LGE_REV_13,
	LGE_REV_TOT_NUM,
};

extern int lge_bd_rev;

/* define gpio pin number of i2c-gpio */
struct gpio_i2c_pin {
	unsigned int sda_pin;
	unsigned int scl_pin;
	unsigned int reset_pin;
	unsigned int irq_pin;
};

/* atcmd virtual keyboard platform data */
struct atcmd_virtual_platform_data {
	unsigned int keypad_row;
	unsigned int keypad_col;
	unsigned char *keycode;
};

struct key_touch_platform_data {
	int (*power)(unsigned char onoff);
	int irq;
	int scl;
	int sda;
	unsigned char *keycode;
	int keycodemax;
};

/* touch screen platform data */
#if defined(CONFIG_TOUCHSCREEN_QT602240) || defined(CONFIG_TOUCHSCREEN_SU610)
struct qt602240_platform_data {
	unsigned int x_line;
	unsigned int y_line;
	unsigned int x_size;
	unsigned int y_size;
	unsigned int blen;
	unsigned int threshold;
	unsigned int voltage;
	unsigned char orient;
	int (*power)(unsigned char onoff);
	int gpio_int;	
	int irq;
	int scl;
	int sda;
	int hw_i2c;	
	int reset;
};
#else
struct touch_platform_data {
	int ts_x_min;
	int ts_x_max;
	int ts_y_min;
	int ts_y_max;
	int ts_y_start;
	int (*power)(unsigned char onoff);
	int irq;
	int gpio_int;
	int hw_i2c;
	int scl;
	int sda;
	int ce;
};
#endif

/* acceleration platform data */
struct acceleration_platform_data {
	int irq_num;
#ifdef CONFIG_SENSOR_BMA250
	int irq_num2;
#endif
	int (*power)(unsigned char onoff);
};

/* proximity platform data */
struct proximity_platform_data {
	int irq_num;
	int (*power)(unsigned char onoff);
	int methods;
	int operation_mode;
	int debounce;
	u8 cycle;
};

/* ecompass platform data */
struct ecom_platform_data {
	int pin_int;
	int pin_rst;
	int (*power)(unsigned char onoff);
	char accelerator_name[20];
	int fdata_sign_x;
        int fdata_sign_y;
        int fdata_sign_z;
	int fdata_order0;
	int fdata_order1;
	int fdata_order2;
	int sensitivity1g;
	s16 *h_layout;
	s16 *a_layout;
};

/* android vibrator platform data */
struct android_vibrator_platform_data {
	int enable_status;
	int (*power_set)(int enable); 		/* LDO Power Set Function */
	int (*pwm_set)(int enable, int gain); 		/* PWM Set Function */
	int (*ic_enable_set)(int enable); 	/* Motor IC Set Function */
	int amp_value;				/* PWM tuning value */
};

/* bt platform data */
struct bluetooth_platform_data {
	int (*bluetooth_power)(int on);
	int (*bluetooth_toggle_radio)(void *data, bool blocked);
};

struct bluesleep_platform_data {
	int bluetooth_port_num;
};

#ifdef CONFIG_LGE_BROADCAST	
struct broadcast_device_platform_data {
	void (*dmb_gpio_on)(void);
	void (*dmb_gpio_off)(void);
	void (*dmb_gpio_init)(void);
	int (*dmb_power_on)(void);
	int (*dmb_power_off)(void);
};
#endif

typedef void (gpio_i2c_init_func_t)(int bus_num);
int __init init_gpio_i2c_pin(struct i2c_gpio_platform_data *i2c_adap_pdata,
		struct gpio_i2c_pin gpio_i2c_pin,
		struct i2c_board_info *i2c_board_info_data);

void __init msm_add_fb_device(void);
void __init msm_add_pmem_devices(void);
void __init msm_add_kgsl_device(void);
void __init msm7x30_allocate_memory_regions(void);
void __init msm_add_usb_devices(void);
void __init msm7x30_init_marimba(void);
#if defined(CONFIG_LGE_MODEL_E739)
void __init register_board_info(void);
#endif

/* implement in board-victor-pm.c */
int __init pmic8058_buses_init(void);
void __init pmic8058_leds_init(void);
void __init lge_pm_set_platform_data(void);

/* implement in board-victor-misc.c */
void __init lge_add_mmc_devices(void);
void __init lge_add_misc_devices(void);

/* implement in board-victor-sound.c */
int __init aux_pcm_gpio_init(void);
void __init lge_victor_audio_init(void);

/* implement in board-victor-mmc.c */
void __init lge_add_mmc_devices(void);

/* implement in board-victor-input.c */
void __init lge_add_input_devices(void);

/* implement in board-victor-bt.c */
void __init lge_add_btpower_devices(void);

void __init lge_add_gpio_i2c_device(gpio_i2c_init_func_t *init_func);
void __init lge_add_gpio_i2c_devices(void);

#ifdef CONFIG_LGE_BROADCAST	
void __init lge_add_broadcast_dmb_devices(void);
#endif

void __init lge_add_lcd_devices(void);
void __init msm_qsd_spi_init(void);

/* implement in board-victor-camera.c */
void __init lge_add_camera_devices(void);

/* for interaction with LK loader */
int __init lge_get_uart_mode(void);

#ifdef CONFIG_ANDROID_RAM_CONSOLE
void __init lge_add_ramconsole_devices(void);
#endif

#if defined(CONFIG_ANDROID_RAM_CONSOLE) && defined(CONFIG_LGE_HANDLE_PANIC)
void __init lge_add_panic_handler_devices(void);
void lge_set_reboot_reason(unsigned int reason);
#endif

#ifdef CONFIG_LGE_DETECT_PIF_PATCH
unsigned lge_get_pif_info(void);
unsigned lge_test_frst_sync(void);
#endif

#ifdef CONFIG_LGE_HIDDEN_RESET_PATCH
//extern int hidden_reset_enable;
extern int hreset_enable_flag;
extern int on_hidden_reset;
void *lge_get_fb_addr(void);
void *lge_get_fb_copy_virt_addr(void);
void *lge_get_fb_copy_phys_addr(void);
void *lge_get_fb_copy_virt_rgb888_addr(void);
unsigned int lge_get_fb_phys_addr(void);
#endif // CONFIG_LGE_HIDDEN_RESET_PATCH

#endif

int board_is_rev(char *);

#endif
