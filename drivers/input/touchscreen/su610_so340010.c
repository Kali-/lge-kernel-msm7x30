/*
 * drivers/input/touchscreen/so340010.c - Touch keypad driver
 *
 * Copyright (C) 2010 LGE, Inc
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

#include <linux/module.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/wakelock.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <mach/vreg.h>
#include <mach/board_lge.h>

#define TSKEY_REG_INTER			(0x00)
#define TSKEY_REG_GEN			(0x01)
#define TSKEY_REG_BUT_ENABLE	(0x04)
#define TSKEY_REG_SEN1			(0x10)
#define TSKEY_REG_SEN2			(0x11)
#define TSKEY_REG_GPIO_CTRL		(0x0E)
#define TSKEY_REG_LED_EN		(0x22)
#define TSKEY_REG_LED_PERIOD	(0x23)
#define TSKEY_REG_LED_CTRL1		(0x24)
#define TSKEY_REG_LED_CTRL2		(0x25)

#define TSKEY_REG_BUT_STATE1	(0x01)
#define TSKEY_REG_BUT_STATE2	(0x09)

#define TSKEY_REG_DUMMY			(0x00)

#define TSKEY_VAL_SEN0			(0xC8)  /*Back*/
#define TSKEY_VAL_SEN1			(0xC8)  /*Home*/
#define TSKEY_VAL_SEN2			(0xDC)  /*Menu*/
#define TSKEY_VAL_SEN3			(0x00)
#define TSKEY_VAL_PERIOD_A		(0x00)
#define TSKEY_VAL_PERIOD_B		(0x00)

#define PRESS 	1
#define RELEASE 0

struct class *touch_key_class;
EXPORT_SYMBOL(touch_key_class);

struct device *sen_test;
EXPORT_SYMBOL(sen_test);

static u8 touch_sen0, touch_sen1, touch_sen2;
static bool ch = RELEASE;

struct so340010_device {
	struct i2c_client		*client;		/* i2c client for adapter */
	struct input_dev		*input_dev;		/* input device for android */
	struct work_struct		key_work;			/* work for touch bh */
	spinlock_t				lock;			/* protect resources */
	int	irq;
	int gpio_irq;
	struct early_suspend 	earlysuspend;
};

static struct key_touch_platform_data *so340010_pdata;
static struct so340010_device *so340010_pdev = NULL;
static struct workqueue_struct *so340010_wq;

extern u8 su610_hardkey_lock;

static int so340010_i2c_write(u8 reg_h, u8 reg_l, u8 val_h, u8 val_l)
{
	int ret;
	u8 buf[4];
	struct i2c_msg msg = {
		so340010_pdev->client->addr, 0, sizeof(buf), buf
	};

	buf[0] = reg_h;
	buf[1] = reg_l;
	buf[2] = val_h;
	buf[3] = val_l;

	if ((ret = i2c_transfer(so340010_pdev->client->adapter, &msg, 1)) < 0)
		dev_err(&so340010_pdev->client->dev, "i2c write error\n");

	return ret;
}

static u16 so340010_i2c_read(u8 reg_h, u8 reg_l, u16 *ret)
{
	int err;
	u8 buf[2];

	struct i2c_msg msg[2] = {
		{ so340010_pdev->client->addr, 0, sizeof(buf), buf },
		{ so340010_pdev->client->addr, I2C_M_RD, sizeof(buf), (__u8 *)ret }
	};

	buf[0] = reg_h;
	buf[1] = reg_l;

	if ((err = i2c_transfer(so340010_pdev->client->adapter, msg, 2)) < 0)
		dev_err(&so340010_pdev->client->dev, "i2c read error\n");

	return err;
}

struct reg_code_table {
	u8 val1;
	u8 val2;
	u8 val3;
	u8 val4;
};

#if defined (CONFIG_LGE_TUNE_610)
/* 2011-03-22 by xwolf@lge.com
 * skl6100 maps buttons at s1,s2,s3
 * and has another sensor tunning values
 */
static struct reg_code_table initial_code_table[] = {
	{TSKEY_REG_DUMMY, 	TSKEY_REG_INTER, 		TSKEY_REG_DUMMY, 	0x07},
	{TSKEY_REG_DUMMY, 	TSKEY_REG_GEN, 			TSKEY_REG_DUMMY, 	0x30},
	{TSKEY_REG_DUMMY, 	TSKEY_REG_BUT_ENABLE, 	TSKEY_REG_DUMMY, 	0x07},
	{TSKEY_REG_DUMMY,	TSKEY_REG_SEN1, 		0xE6,				0xE6},
	{TSKEY_REG_DUMMY,	TSKEY_REG_SEN2, 		0x00,				0xE6},
	{TSKEY_REG_DUMMY,	TSKEY_REG_LED_EN,		TSKEY_REG_DUMMY,	0x00},
	{TSKEY_REG_DUMMY,	TSKEY_REG_LED_PERIOD,	TSKEY_VAL_PERIOD_A, TSKEY_VAL_PERIOD_B},
};
#else
static struct reg_code_table initial_code_table[] = {
	{TSKEY_REG_DUMMY, 	TSKEY_REG_INTER, 		TSKEY_REG_DUMMY, 	0x07},
	{TSKEY_REG_DUMMY, 	TSKEY_REG_GEN, 			TSKEY_REG_DUMMY, 	0x30},
	{TSKEY_REG_DUMMY, 	TSKEY_REG_BUT_ENABLE, 	TSKEY_REG_DUMMY, 	0x07},
	{TSKEY_REG_DUMMY, 	TSKEY_REG_SEN1, 		TSKEY_VAL_SEN1, 	TSKEY_VAL_SEN0},
	{TSKEY_REG_DUMMY, 	TSKEY_REG_SEN2, 		TSKEY_VAL_SEN3, 	TSKEY_VAL_SEN2},
	{TSKEY_REG_DUMMY, 	TSKEY_REG_LED_EN,		TSKEY_REG_DUMMY, 	0x00},
	{TSKEY_REG_DUMMY, 	TSKEY_REG_LED_PERIOD,	TSKEY_VAL_PERIOD_A,	TSKEY_VAL_PERIOD_B},
};
#endif /* CONFIG_LGE_TUNE_610 */

static struct reg_code_table suspend_code_table[] = {
	{TSKEY_REG_DUMMY,   TSKEY_REG_GEN,          TSKEY_REG_DUMMY,    0xB0},
};

#ifndef CONFIG_HAS_EARLYSUSPEND
static struct reg_code_table resume_code_table[] = {
	{TSKEY_REG_DUMMY,   TSKEY_REG_GEN,          TSKEY_REG_DUMMY,    0x30},
};
#endif

static int so340010_initialize(void)
{
	int ret = 0;
	u16 data;
	int i;

	if (so340010_pdev == NULL) {
		printk(KERN_INFO"%s: so340010 device is null\n", __func__);
		return -1;
	}

	for (i = 0; i < ARRAY_SIZE(initial_code_table); i++) {
		ret = so340010_i2c_write(
				initial_code_table[i].val1,
				initial_code_table[i].val2,
				initial_code_table[i].val3,
				initial_code_table[i].val4);
		if (ret < 0)
			goto end_device_init;
	}

	ret = so340010_i2c_read(TSKEY_REG_BUT_STATE1, TSKEY_REG_BUT_STATE2, &data);

 	return ret;

end_device_init:
	printk(KERN_INFO"%s: failed to initailise\n", __func__);

	return ret;

}

static void so340010_report_event(u16 state)
{
	int keycode = 0;
	static unsigned char old_keycode = 0;

	if (state == 0) {
		ch = RELEASE;
		keycode = old_keycode;
	} else if(state == 0x100) {
		ch = PRESS;
		keycode = so340010_pdata->keycode[0];
	} else if(state == 0x200) {
		ch = PRESS;
		keycode = so340010_pdata->keycode[2];
	} else if(state == 0x400) {
		ch = PRESS;
		keycode = so340010_pdata->keycode[4];
	} else if(state == 0x800) {
		ch = PRESS;
		keycode = so340010_pdata->keycode[6];
	} else {
		printk(KERN_INFO"Unknown key type(0x%x)\n", state);
	}

	old_keycode = keycode;

	if(state == 0 || state == 0x100 || state == 0x200 || state == 0x400 ||state == 0x800) {
		if(!su610_hardkey_lock){
			input_report_key(so340010_pdev->input_dev, keycode, ch);
			input_sync(so340010_pdev->input_dev);
		}
	}
}

/*
 * interrupt service routine
 */
static irqreturn_t so340010_irq_handler(int irq, void *dev_id)
{
	struct so340010_device *pdev = dev_id;
	int state;
	unsigned long flags;

	spin_lock_irqsave(&pdev->lock, flags);
	state = gpio_get_value(pdev->gpio_irq);
	if(state == 0)
		queue_work(so340010_wq, &pdev->key_work);

	spin_unlock_irqrestore(&pdev->lock, flags);

	return IRQ_HANDLED;
}

static void so340010_key_work_func(struct work_struct *work)
{
	u16 state;
	int ret;

	ret = so340010_i2c_read(TSKEY_REG_BUT_STATE1, TSKEY_REG_BUT_STATE2, &state);

	so340010_report_event(state);
}

static int so340010_i2c_suspend(struct i2c_client *i2c_dev, pm_message_t state)
{
	struct so340010_device *pdev = i2c_get_clientdata(i2c_dev);

	cancel_work_sync(&pdev->key_work);

	flush_workqueue(so340010_wq);

	so340010_i2c_write(
			suspend_code_table[0].val1,
			suspend_code_table[0].val2,
			suspend_code_table[0].val3,
			suspend_code_table[0].val4);

	return 0;
}

#ifndef CONFIG_HAS_EARLYSUSPEND
static int so340010_i2c_resume(struct i2c_client *i2c_dev)
{
	so340010_i2c_write(
			resume_code_table[0].val1,
			resume_code_table[0].val2,
			resume_code_table[0].val3,
			resume_code_table[0].val4);

	return 0;
}
#endif

#ifdef CONFIG_MACH_MSM8X55_VICTOR
static void so340010_power_down(void)
{
	gpio_tlmm_config(GPIO_CFG(so340010_pdata->sda, 0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_DISABLE);
	gpio_tlmm_config(GPIO_CFG(so340010_pdata->scl, 0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_DISABLE);
	gpio_tlmm_config(GPIO_CFG(so340010_pdata->irq, 0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_DISABLE);

	so340010_pdata->power(0);
}

static void so340010_power_up(void)
{
	so340010_pdata->power(1);

	gpio_tlmm_config(GPIO_CFG(so340010_pdata->scl, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(so340010_pdata->sda, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(so340010_pdata->irq, 0, GPIO_CFG_INPUT,
				GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	msleep(50);
}
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
static void so340010_early_suspend(struct early_suspend *h)
{
	struct so340010_device *pdev = container_of(h, struct so340010_device, earlysuspend);

	disable_irq(pdev->client->irq);

	so340010_i2c_suspend(pdev->client, PMSG_SUSPEND);
	so340010_power_down();

	return;
}

static void so340010_late_resume(struct early_suspend *h)
{
	struct so340010_device *pdev = container_of(h, struct so340010_device, earlysuspend);
	int ret = 0;

	so340010_power_up();

	ret = so340010_initialize();
	
	if (ret < 0)
		printk(KERN_INFO"%s: failed to init\n", __func__);

	enable_irq(pdev->client->irq);

	return;
}
#endif

static ssize_t so340010_set_sen_show(struct device *dev,
					struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "BACK[%d] HOME[%d] MENU[%d]\n", touch_sen0, touch_sen1, touch_sen2);
}

static ssize_t so340010_set_sen_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	int cmd_no, config_value = 0;
	unsigned long after;

	strict_strtoul(buf, 10, &after);

	printk(KERN_INFO "[TSP] %s\n", __func__);
	cmd_no = (int)(after / 1000);
	config_value = (int)(after % 1000);

	if (cmd_no == 0) {
		touch_sen0 = config_value;
		printk(KERN_INFO"[SO340010] Set Back Key Sen[%d]\n", touch_sen0);
		so340010_i2c_write(
			TSKEY_REG_DUMMY,
			TSKEY_REG_SEN1,
			touch_sen1,
			touch_sen0);
	} else if (cmd_no == 1) {
		touch_sen1 = config_value;
		printk(KERN_INFO"[SO340010] Set Home Key Sen[%d]\n", touch_sen1);
		so340010_i2c_write(
			TSKEY_REG_DUMMY,
			TSKEY_REG_SEN1,
			touch_sen1,
			touch_sen0);
	} else if (cmd_no == 2) {
		touch_sen2 = config_value;
		printk(KERN_INFO"[SO340010] Set Menu Key Sen[%d]\n", touch_sen2);
		so340010_i2c_write(
			TSKEY_REG_DUMMY,
			TSKEY_REG_SEN2,
			TSKEY_VAL_SEN3,
			touch_sen2);
	} else
		printk(KERN_INFO"[%s] unknown CMD\n", __func__);

	return size;
}

static DEVICE_ATTR(set_sen, S_IRUGO | S_IWUSR | S_IXOTH, so340010_set_sen_show, so340010_set_sen_store);

static int so340010_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret = 0;
	int i;
	unsigned keycode = KEY_UNKNOWN;

	printk(KERN_INFO"%s: start\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk(KERN_INFO"%s: it is not support I2C_FUNC_I2C.\n", __func__);
		return -ENODEV;
	}

	so340010_pdev = kzalloc(sizeof(struct so340010_device), GFP_KERNEL);
	if (so340010_pdev == NULL) {
		printk(KERN_INFO"%s: failed to allocation\n", __func__);
		return -ENOMEM;
	}
	so340010_pdev->client = client;
	so340010_pdata = client->dev.platform_data;
	i2c_set_clientdata(so340010_pdev->client, so340010_pdev);

	so340010_pdev->input_dev = input_allocate_device();
	if (NULL == so340010_pdev->input_dev) {
		dev_err(&client->dev, "failed to allocation\n");
		goto err_input_allocate_device;
	}

	/* initialize input device for so34001000200F */
	so340010_pdev->input_dev->name = "touch_keypad";
	so340010_pdev->input_dev->phys = "touch_keypad/i2c";

	so340010_pdev->input_dev->id.vendor = VENDOR_LGE;
	so340010_pdev->input_dev->evbit[0] = BIT_MASK(EV_KEY);
	so340010_pdev->input_dev->keycode = so340010_pdata->keycode;
	so340010_pdev->input_dev->keycodesize = sizeof(unsigned short);
	so340010_pdev->input_dev->keycodemax = so340010_pdata->keycodemax;
	for (i = 0; i < so340010_pdata->keycodemax; i++) {
		keycode = so340010_pdata->keycode[2 * i];
		set_bit(keycode, so340010_pdev->input_dev->keybit);
	}
	ret = input_register_device(so340010_pdev->input_dev);
	if (ret < 0) {
		printk(KERN_INFO"%s: failed to register input\n", __func__);
		goto err_input_register_device;
	}

	touch_sen0 = TSKEY_VAL_SEN0;
	touch_sen1 = TSKEY_VAL_SEN1;
	touch_sen2 = TSKEY_VAL_SEN2;

	touch_key_class = class_create(THIS_MODULE, "touch_key");
	if (IS_ERR(touch_key_class))
		printk(KERN_INFO"%s: class_create fail\n", __func__);

	sen_test = device_create(touch_key_class, NULL, 0, NULL, "sen");
	if (IS_ERR(sen_test))
		printk(KERN_INFO"%s: device_create fail\n", __func__);


	if (device_create_file(sen_test, &dev_attr_set_sen) < 0)
		printk(KERN_INFO"%s: device_create_file fail\n", __func__);

	so340010_pdev->gpio_irq = so340010_pdata->irq;
	so340010_pdev->irq = gpio_to_irq(so340010_pdata->irq);

	INIT_WORK(&so340010_pdev->key_work, so340010_key_work_func);
	spin_lock_init(&so340010_pdev->lock);

	ret = so340010_initialize();
	if (ret < 0) {
		printk(KERN_INFO"%s: failed to init\n", __func__);
		goto err_so340010_initialize;
	}

	/* register interrupt handler */
	ret = request_irq(so340010_pdev->irq, so340010_irq_handler,
			IRQF_TRIGGER_FALLING, "so340010_irq", (void *)so340010_pdev);
	if (ret < 0) {
		printk(KERN_INFO"%s: failed to register irq\n", __func__);
		goto err_irq_request;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	so340010_pdev->earlysuspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN - 1;
	so340010_pdev->earlysuspend.suspend = so340010_early_suspend;
	so340010_pdev->earlysuspend.resume = so340010_late_resume;
	register_early_suspend(&so340010_pdev->earlysuspend);
#endif

	return 0;

err_irq_request:
	input_unregister_device(so340010_pdev->input_dev);
err_so340010_initialize:
err_input_register_device:
	input_free_device(so340010_pdev->input_dev);
err_input_allocate_device:
	kfree(so340010_pdev);

	return ret;
}

static int so340010_i2c_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id so340010_i2c_ids[] = {
		{"so340010", 0 },
		{ },
};

static struct i2c_driver so340010_i2c_driver = {
	.probe		= so340010_i2c_probe,
	.remove		= so340010_i2c_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend	= so340010_i2c_suspend,
	.resume		= so340010_i2c_resume,
#endif
	.id_table	= so340010_i2c_ids,
	.driver = {
		.name	= "so340010",
		.owner	= THIS_MODULE,
	},
};

static int __init so340010_init(void)
{
	int ret;

	printk(KERN_INFO"%s: start...\n", __func__);

	so340010_wq = create_singlethread_workqueue("so340010_wq");
	if (!so340010_wq) {
		printk(KERN_INFO"failed to create singlethread workqueue\n");
		return -ENOMEM;
	}

	ret = i2c_add_driver(&so340010_i2c_driver);
	if (ret < 0) {
		printk(KERN_INFO"%s: failed to i2c_add_driver\n", __func__);
		destroy_workqueue(so340010_wq);
		return ret;
	}

	return 0;
}

static void __exit so340010_exit(void)
{
	return;
}

module_init(so340010_init);
module_exit(so340010_exit);

MODULE_DESCRIPTION("synaptics so340010 touch key driver");
MODULE_LICENSE("GPL");
