/*
 *  arch/arm/mach-msm/lge/lge_headset_MAX14579.c
 *
 *  LGE 3.5 PI Headset detection driver.
 *  HSD is HeadSet Detection with one GPIO
 *
 * Copyright (C) 2008 Google, Inc.
 * Author: Mike Lockwood <lockwood@android.com>
 *
 * Copyright (C) 2009 ~ 2010 LGE, Inc.
 * Author: Lee SungYoung < lsy@lge.com>
 *
 * Copyright (C) 2010 LGE, Inc.
 * Author: Cho Younghyun < bob.cho@lge.com>
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

/* The Android Donut's Headset Detection Interface is following;
 * source file is android/frameworks/base/services/java/com/android/server/HeadsetObserver.java
 * HEADSET_UEVENT_MATCH = "DEVPATH=/sys/devices/virtual/switch/h2w"
 * HEADSET_STATE_PATH = /sys/class/switch/h2w/state
 * HEADSET_NAME_PATH = /sys/class/switch/h2w/name
 * INPUT = SW_HEADPHONE_INSERT ==> KEY_MEDIA
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/switch.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/mutex.h>
#include <linux/hrtimer.h>
#include <linux/input.h>
#include <linux/wakelock.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <mach/board.h>
#include <mach/vreg.h>
#include <linux/slab.h>
#include <mach/board_lge.h>

#if defined (CONFIG_LGE_DOMESTIC) || defined(CONFIG_LGE_MODEL_E739)
#define	OLD_REV		LGE_REV_B
#else
#define	OLD_REV		LGE_REV_C
#endif

#undef  LGE_HSD_DEBUG_PRINT
#define LGE_HSD_DEBUG_PRINT
#undef  LGE_HSD_ERROR_PRINT
#define LGE_HSD_ERROR_PRINT

#if defined(LGE_HSD_DEBUG_PRINT)
#define HSD_DBG(fmt, args...) printk(KERN_INFO "HSD[%-18s:%5d]" fmt, __func__, __LINE__, ## args)
#else
#define HSD_DBG(fmt, args...) do {} while (0)
#endif

#if defined(LGE_HSD_ERROR_PRINT)
#define HSD_ERR(fmt, args...) printk(KERN_ERR "HSD[%-18s:%5d]" fmt, __func__, __LINE__, ## args)
#else
#define HSD_ERR(fmt, args...) do {} while (0)
#endif

static void hs_detect_work_func(struct work_struct *work);

struct hsd_info {
   struct switch_dev sdev;
   struct input_dev *input;
   struct mutex mutex_lock;
   const char *name_on;
   const char *name_off;
   const char *state_on;
   const char *state_off;
   unsigned hs_detect_gpio;
   unsigned hook_switch_gpio;
   unsigned hs_mode_gpio;
   unsigned int hs_detect_irq;
   unsigned int hook_switch_irq;
   atomic_t btn_state;
   int old_btn_status;
   int on_headset_detect;
   struct wake_lock headset_wake_lock;

   struct delayed_work hs_work;
   struct workqueue_struct *hs_work_queue;
   struct hrtimer btn_timer;
   unsigned int hsd_debounce_time;
   ktime_t btnup_debounce_time;
   ktime_t btndn_debounce_time;
};

static struct hsd_info *hi;

enum {
   NO_DEVICE   = 0,
   LGE_HEADSET = 1,
   LGE_HEADSET_NO_MIC = 2,
};

enum {
   BUTTON_RELEASED	= 1,
   BUTTON_PRESSED	= 0,
};

#define LGE_HEADSET_DETECT_GPIO  26

static ssize_t lge_hsd_print_name(struct switch_dev *sdev, char *buf)
{
	switch (switch_get_state(&hi->sdev)) {
	case NO_DEVICE:
		return sprintf(buf, "No Device\n");
	case LGE_HEADSET:
	case LGE_HEADSET_NO_MIC:
		return sprintf(buf, "Headset\n");
	}

	return -EINVAL;
}

static ssize_t lge_hsd_print_state(struct switch_dev *sdev, char *buf)
{
	const char *state;

	if (switch_get_state(&hi->sdev))
		state = hi->state_on;
	else
		state = hi->state_off;

	if (state)
		return sprintf(buf, "%s\n", state);
	return -1;
}

static void button_pressed(void)
{
	HSD_DBG("button_pressed \n");

	/* set button status */
	atomic_set(&hi->btn_state, BUTTON_PRESSED);

	/* post input event */
	input_report_key(hi->input, KEY_MEDIA, 1);
	/* remove : input_sync(hi->input);*/

	if (wake_lock_active(&hi->headset_wake_lock))
		wake_unlock(&hi->headset_wake_lock);
	wake_lock_timeout(&hi->headset_wake_lock, 1.5*HZ);
}

static void button_released(void)
{
	HSD_DBG("button_released \n");

	/* set button status */
	atomic_set(&hi->btn_state, BUTTON_RELEASED);

	/* post input event */
	input_report_key(hi->input, KEY_MEDIA, 0);
	/* remove : input_sync(hi->input);*/
	if (wake_lock_active(&hi->headset_wake_lock))
		wake_unlock(&hi->headset_wake_lock);
	wake_lock_timeout(&hi->headset_wake_lock, 1.5*HZ);
}

static void remove_headset(void)
{
	unsigned long irq_flags;

	HSD_DBG("remove_headset \n");

	if (switch_get_state(&hi->sdev) == LGE_HEADSET)	{
		/* disable irq for hook key*/
		local_irq_save(irq_flags);
		disable_irq(hi->hook_switch_irq);
		set_irq_wake(hi->hook_switch_irq, 0);
		local_irq_restore(irq_flags);
	}

	/* change switch status to no-device */
	switch_set_state(&hi->sdev, NO_DEVICE);

	/* when button is pressed , will be release*/
	atomic_set(&hi->btn_state, BUTTON_RELEASED);

	/* post input event */
	input_report_switch(hi->input, SW_HEADPHONE_INSERT, 0);
	if (switch_get_state(&hi->sdev) == LGE_HEADSET)
		input_report_switch(hi->input, SW_MICROPHONE_INSERT, 0);
	/* remove : input_sync(hi->input);*/
	gpio_set_value(hi->hs_mode_gpio, 0);	
}

static void insert_headset(bool has_hook)
{
	unsigned long irq_flags;

	HSD_DBG("insert_headset : %d\n", has_hook);

	/* change switch status to heaset */
	if (has_hook)	{
		gpio_set_value(hi->hs_mode_gpio, 1);	/* will be changed */

		/* post mic insert event */
		input_report_switch(hi->input, SW_HEADPHONE_INSERT, 1);
		input_report_switch(hi->input, SW_MICROPHONE_INSERT, 1);
		/* remove : input_sync(hi->input);*/

		mutex_lock(&hi->mutex_lock);
		switch_set_state(&hi->sdev, LGE_HEADSET);
		mutex_unlock(&hi->mutex_lock);

		/* enable irq to detect hook key*/
		atomic_set(&hi->btn_state, BUTTON_RELEASED);
		local_irq_save(irq_flags);
		enable_irq(hi->hook_switch_irq);
		set_irq_wake(hi->hook_switch_irq, 1);
		local_irq_restore(irq_flags);
	} else	{
		gpio_set_value(hi->hs_mode_gpio, 1);
		
		/* post mic insert event */
		input_report_switch(hi->input, SW_HEADPHONE_INSERT, 1);

		mutex_lock(&hi->mutex_lock);
		switch_set_state(&hi->sdev, LGE_HEADSET_NO_MIC);
		mutex_unlock(&hi->mutex_lock);
	}
}

static void hs_detect_work_func(struct work_struct *work)
{

	int state = 0;
	bool earjack_type;

	/* to ignore button irp when headset is removed and inserted in debounce delay*/
	hi->on_headset_detect	= 0;		

	/* Rev old 	: HIGH - INSERT, LOW - OUT */
	/* Rev new	: HIGH - OUT,		LOW - INSERT */
	if (lge_bd_rev > OLD_REV)
	{
		if(gpio_get_value(hi->hs_detect_gpio))
			state = 0;
		else
			state = 1;
	}
	else
		state = gpio_get_value(hi->hs_detect_gpio);		

	if (((switch_get_state(&hi->sdev) ? 1 : 0) ^ state) == 0 && work != NULL)	{
		if(switch_get_state(&hi->sdev) == NO_DEVICE)
			gpio_set_value(hi->hs_mode_gpio, 0);
		goto invalid_status;
	}	

	if (state)	{
		/* when headset inserted, check earjack type (3 or 4 polarity) */
		earjack_type = gpio_get_value(hi->hook_switch_gpio);
		if (earjack_type)
			insert_headset(true);
		else
			insert_headset(false);
	} else	{
		/* when headset removed*/
		remove_headset();
	}

invalid_status:
	return;
}

static enum hrtimer_restart button_event_timer_func(struct hrtimer *data)
{
	int value;
	unsigned long irq_flags;

	/* to ignore button irp when headset is removed and inserted in debounce delay*/
	if (hi->on_headset_detect)
		goto invalid_status;

	/* skip if has no key */
	if (switch_get_state(&hi->sdev) != LGE_HEADSET)
		goto invalid_status;

	/* skip if button status is same */
	value = gpio_get_value(hi->hook_switch_gpio);
	if (hi->old_btn_status != value)
		goto invalid_status;
	if ((atomic_read(&hi->btn_state)^value) == 0)
		goto invalid_status;
	
	/* button check */
	if (value == BUTTON_PRESSED)
		button_pressed();
	else
		button_released();

invalid_status:
	/* enable irq to detect hook key*/
	local_irq_save(irq_flags);
	enable_irq(hi->hook_switch_irq);
	local_irq_restore(irq_flags);

	return HRTIMER_NORESTART;
}


static irqreturn_t headset_detect_irq_handler(int irq, void *dev_id)
{
	int state = 0;

	/* Rev old 	: HIGH - INSERT, LOW - OUT */
	/* Rev new	: HIGH - OUT,		LOW - INSERT */
	if (lge_bd_rev > OLD_REV)
	{
		if(gpio_get_value(hi->hs_detect_gpio))
			state = 0;
		else
			state = 1;
	}
	else
		state = gpio_get_value(hi->hs_detect_gpio);	

	/* there is change in headset status*/
	if (((switch_get_state(&hi->sdev) ? 1 : 0) ^ state))	{
		/* to ignore button irp when headset is removed and inserted in debounce delay*/
		if(switch_get_state(&hi->sdev) == NO_DEVICE)
			gpio_set_value(hi->hs_mode_gpio, 1);
		hi->on_headset_detect	= 1;
		/* start work queue with debounce delay */
		queue_delayed_work(hi->hs_work_queue, &hi->hs_work, msecs_to_jiffies(hi->hsd_debounce_time));
		
		if (wake_lock_active(&hi->headset_wake_lock))
			wake_unlock(&hi->headset_wake_lock);
		wake_lock_timeout(&hi->headset_wake_lock, 5*HZ);
	}
	return IRQ_HANDLED;
}

static irqreturn_t hook_switch_irq_handler(int irq, void *dev_id)
{
	/* to ignore button irp when headset is removed and inserted in debounce delay*/
	if (hi->on_headset_detect)
		return IRQ_HANDLED;
	
	/*disable irp*/
	disable_irq_nosync(hi->hook_switch_irq);
	hrtimer_cancel(&hi->btn_timer);
	hi->old_btn_status = gpio_get_value(hi->hook_switch_gpio);

	/* skip if button status is same */
	hrtimer_start(&hi->btn_timer, 
					(hi->old_btn_status == BUTTON_PRESSED)? hi->btndn_debounce_time : hi->btnup_debounce_time, 
					HRTIMER_MODE_REL);

	return IRQ_HANDLED;
}

static int __init lge_hsd_probe(struct platform_device *pdev)
{
	int ret;

	/* alloc platform data */
	hi = kzalloc(sizeof(struct hsd_info), GFP_KERNEL);
	if (!hi) {
		HSD_ERR("Failed to allloate headset per device info\n");
		return -ENOMEM;
	}

	/* initialization */
	atomic_set(&hi->btn_state, BUTTON_RELEASED);
	mutex_init(&hi->mutex_lock);

	/* switch dev register */
	hi->sdev.name = "h2w";
	hi->sdev.print_state = lge_hsd_print_state;
	hi->sdev.print_name = lge_hsd_print_name;
	hi->hs_detect_gpio = 26;
	hi->hook_switch_gpio = 145;
	hi->hs_mode_gpio = 31;
	hi->hsd_debounce_time = 300; /* 300 ms */
	hi->btndn_debounce_time = ktime_set(0, 100000000); /* 100 ms */
	hi->btnup_debounce_time = ktime_set(0, 30000000); /* 100 ms */
	
	/* init headset wake lock */
	wake_lock_init(&hi->headset_wake_lock, WAKE_LOCK_SUSPEND, "lge_headset");

	/* headset mode : Low Power Mode  */
	gpio_set_value(hi->hs_mode_gpio, 0);

	/* switch dev register */
	ret = switch_dev_register(&hi->sdev);
	if (ret < 0) {
		HSD_ERR("Failed to register switch device\n");
		goto err_switch_dev_register;
	}

	/* create work queue for headset detecting */
	INIT_DELAYED_WORK(&hi->hs_work, hs_detect_work_func);
	hi->hs_work_queue = create_singlethread_workqueue("headset_detect");
	if (hi->hs_work_queue  == NULL) {
		HSD_ERR("Failed to create workqueue\n");
		goto err_create_work_queue;
	}

	/* add timer for handling hook key */
	hrtimer_init(&hi->btn_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hi->btn_timer.function = button_event_timer_func;

	/* add gpio/irq  for headset detecting */
	gpio_tlmm_config(GPIO_CFG(hi->hs_detect_gpio, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL,
	       GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	ret = gpio_request(hi->hs_detect_gpio, hi->sdev.name);
	if (ret < 0) {
		HSD_ERR("Failed to request gpio%d\n", hi->hs_detect_gpio);
		goto err_request_detect_gpio;
	}

	ret = gpio_direction_input(hi->hs_detect_gpio);
	if (ret < 0) {
		HSD_ERR("Failed to set gpio%d as input\n", hi->hs_detect_gpio);
		goto err_set_detect_gpio;
	}

	hi->hs_detect_irq = gpio_to_irq(hi->hs_detect_gpio);
	ret = request_irq(hi->hs_detect_irq, headset_detect_irq_handler,
	   IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, hi->sdev.name, NULL);
	if (ret < 0) {
		HSD_ERR("Failed to request interrupt handler\n");
		goto err_request_detect_irq;
	}

	ret = set_irq_wake(hi->hs_detect_irq, 1);
	if (ret < 0) {
		HSD_ERR("Failed to set interrupt wake\n");
		goto err_request_input_dev;
	}

	/* add gpio/irq  for hook key detecting */
	gpio_tlmm_config(GPIO_CFG(hi->hook_switch_gpio, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL,
	       GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	ret = gpio_request(hi->hook_switch_gpio, hi->sdev.name);
	if (ret < 0) {
		HSD_ERR("Failed to request gpio%d\n", hi->hook_switch_gpio);
		goto err_request_detect_gpio;
	}

	ret = gpio_direction_input(hi->hook_switch_gpio);
	if (ret < 0) {
		HSD_ERR("Failed to set gpio%d as input\n", hi->hook_switch_gpio);
		goto err_set_detect_gpio;
	}

	hi->hook_switch_irq = gpio_to_irq(hi->hook_switch_gpio);
	ret = request_irq(hi->hook_switch_irq, hook_switch_irq_handler,
	   IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, hi->sdev.name, NULL);
	if (ret < 0) {
		HSD_ERR("Failed to request interrupt handler\n");
		goto err_request_detect_irq;
	}
	disable_irq(hi->hook_switch_irq);

	ret = set_irq_wake(hi->hook_switch_irq, 0);
	if (ret < 0) {
		HSD_ERR("Failed to set interrupt wake\n");
		goto err_request_input_dev;
	}

	/* add input device  */
	hi->input = input_allocate_device();
	if (!hi->input) {
		HSD_ERR("Failed to allocate input device\n");
		ret = -ENOMEM;
		goto err_request_input_dev;
	}

	hi->input->name = "hs_detect";
	hi->input->id.vendor	= 0x0001;
	hi->input->id.product	= 1;
	hi->input->id.version	= 1;

	set_bit(EV_SYN, hi->input->evbit);
	set_bit(EV_KEY, hi->input->evbit);
	set_bit(EV_SW, hi->input->evbit);
	set_bit(KEY_MEDIA, hi->input->keybit);
	set_bit(SW_HEADPHONE_INSERT, hi->input->swbit);
	set_bit(SW_MICROPHONE_INSERT, hi->input->swbit);

	ret = input_register_device(hi->input);
	if (ret) {
		HSD_ERR("Failed to register input device\n");
		goto err_register_input_dev;
	}

	switch_set_state(&hi->sdev, NO_DEVICE);
	/* Perform initial detection : headset chek on boot */
	hs_detect_work_func(NULL);

	return 0;

err_register_input_dev:
	input_free_device(hi->input);
err_request_input_dev:
	free_irq(hi->hook_switch_irq, 0);
err_request_detect_irq:
err_set_detect_gpio:
	gpio_free(hi->hook_switch_gpio);
err_request_detect_gpio:
	destroy_workqueue(hi->hs_work_queue);
err_create_work_queue:
	switch_dev_unregister(&hi->sdev);
err_switch_dev_register:
	HSD_ERR("Failed to register driver\n");

	return ret;
}

static int lge_hsd_remove(struct platform_device *pdev)
{
	if (switch_get_state(&hi->sdev))
		remove_headset();

	input_unregister_device(hi->input);
	gpio_free(hi->hs_detect_gpio);
	gpio_free(hi->hook_switch_gpio);
	free_irq(hi->hs_detect_irq, 0);
	free_irq(hi->hook_switch_irq, 0);
	destroy_workqueue(hi->hs_work_queue);
	switch_dev_unregister(&hi->sdev);

	return 0;
}

static struct platform_driver lge_hsd_driver __refdata = {
	.probe		= lge_hsd_probe,
	.remove		= lge_hsd_remove,
	.driver		= {
		.name	= "lge_headset",
		.owner	= THIS_MODULE,
	},
};

static int __init lge_hsd_init(void)
{
	platform_driver_probe(&lge_hsd_driver, lge_hsd_probe);
	return 0;
}

static void __exit lge_hsd_exit(void)
{
	platform_driver_unregister(&lge_hsd_driver);
}

module_init(lge_hsd_init);
module_exit(lge_hsd_exit);

MODULE_AUTHOR("Cho Younghyun <bob.cho@lge.com>");
MODULE_DESCRIPTION("LGE Headset detection driver");
MODULE_LICENSE("GPL");
