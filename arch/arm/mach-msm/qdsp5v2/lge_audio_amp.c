/* arch/arm/mach-msm/qdsp5v2/lge_tpa2055-amp.c
 *
 * Copyright (C) 2010 LGE, Inc.
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

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/device.h>
#include <asm/gpio.h>
#include <asm/system.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <asm/ioctls.h>
#include <mach/debug_mm.h>
#include <mach/board_lge.h>
#include <linux/slab.h>
#include "lge_audio_amp.h"

#define MODULE_NAME	"lge_amp"

static struct amp_data *_data;

/* -------- LGE AMP control API  ---------*/
void lge_amp_spk_stereo_on(void)
{
	if (_data && _data->amp_spk_stereo_on)
		_data->amp_spk_stereo_on();
}

void lge_amp_spk_phone_on(void)
{
	if (_data && _data->amp_spk_phone_on)
		_data->amp_spk_phone_on();
}

void lge_amp_hp_stereo_on(void)
{
	if (_data && _data->amp_hp_stereo_on)
		_data->amp_hp_stereo_on();
}

void lge_amp_hp_stereo_spk_stereo_on(void)
{
	if (_data && _data->amp_hp_stereo_spk_stereo_on)
		_data->amp_hp_stereo_spk_stereo_on();
}

void lge_amp_hp_phone_on(void)
{
	if (_data && _data->amp_hp_phone_on)
		_data->amp_hp_phone_on();
}

void lge_amp_earpiece_on(void)
{
	if (_data && _data->amp_earpiece_on)
		_data->amp_earpiece_on();
}

void lge_amp_spk_off(void)
{
	if (_data && _data->amp_spk_off)
		_data->amp_spk_off();
}

void lge_amp_off_all(void)
{
	if (_data && _data->amp_off_all)
		_data->amp_off_all();
}

void lge_amp_fm_spk_stereo_on(void)
{
	if (_data && _data->amp_fm_spk_stereo_on)
		_data->amp_fm_spk_stereo_on();
}

void lge_amp_fm_hp_stereo_on(void)
{
	if (_data && _data->amp_fm_hp_stereo_on)
		_data->amp_fm_hp_stereo_on();
}

/*  Calibration tool                      */
static long lge_amp_calibration_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int rc = 0;
	if (_data && _data->amp_calibration)
		rc = _data->amp_calibration(cmd, arg);
	return rc;
}

static int amp_open(struct inode *inode, struct file *file)
{
	int rc = 0;
	return rc;
}

static int amp_release(struct inode *inode, struct file *file)
{
	int rc = 0;
	return rc;
}

const struct file_operations lge_tpa_fops = {
	.owner		= THIS_MODULE,
	.open		= amp_open,
	.release	= amp_release,
	.unlocked_ioctl	= lge_amp_calibration_ioctl,
};

struct miscdevice lge_amp_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "lge_amp",
	.fops = &lge_tpa_fops,
};

static int lge_amp_ctl_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct amp_data *data;
	struct i2c_adapter *adapter = client->adapter;
	int err;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_WORD_DATA))	{
		err = -EOPNOTSUPP;
		return err;
	}

	MM_INFO("\n");

	data = kzalloc(sizeof(struct amp_data), GFP_KERNEL);
	if (NULL == data)
		return -ENOMEM;
	_data = data;
	data->client = client;
	i2c_set_clientdata(client, data);

	if (lge_bd_rev >= LGE_REV_B)
		wm9093_amp_init(data);
	else
		tpa2055_amp_init(data);

	MM_ERR("%s chip found : bd rev : %d\n", client->name, lge_bd_rev);

	data->amp_off_all();
	err = misc_register(&lge_amp_misc);

	return 0;
}

static int lge_amp_ctl_remove(struct i2c_client *client)
{
	struct amp_data *data = i2c_get_clientdata(client);
	kfree(data);

	MM_INFO("\n");
	i2c_set_clientdata(client, NULL);
	return 0;
}


static struct i2c_device_id lge_amp_idtable[] = {
	{ "lge_amp", 1 },
};

static struct i2c_driver lge_amp_ctl_driver = {
	.probe = lge_amp_ctl_probe,
	.remove = lge_amp_ctl_remove,
	.id_table = lge_amp_idtable,
	.driver = {
		.name = MODULE_NAME,
	},
};


static int __init lge_amp_ctl_init(void)
{
	return i2c_add_driver(&lge_amp_ctl_driver);
}

static void __exit lge_amp_ctl_exit(void)
{
	return i2c_del_driver(&lge_amp_ctl_driver);
}

module_init(lge_amp_ctl_init);
module_exit(lge_amp_ctl_exit);

MODULE_DESCRIPTION("lge_tpa2055 Amp Control");
MODULE_AUTHOR("Kim EunHye <ehgrace.kim@lge.com>");
MODULE_LICENSE("GPL");
