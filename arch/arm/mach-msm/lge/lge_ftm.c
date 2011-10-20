/* arch/arm/mach-msm/lge/lge_ftm.c
 *
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

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h>
#include <mach/board_lge.h>
#include "proc_comm.h"
#include "lge_proc_comm.h"

static int lge_ftm_proc_comm(int cmd, int *cmd_result)
{
	int ret;

	if(!cmd_result) return -1;

	ret = msm_proc_comm(PCOM_CUSTOMER_CMD1, cmd_result, &cmd);
	if (ret < 0) {
		pr_err("%s: msm_proc_comm(PCOM_CUSTOMER_CMD1) failed(%d)\n",
			   __func__, ret);
		return ret;
	}
	pr_info("%s: cmd(%d), result(%d)\n", __func__, cmd, *cmd_result);
	return ret;
}

static int lge_ftm_get_qem(void)
{
	int qem = 0;
	if(lge_ftm_proc_comm(CUSTOMER_CMD1_GET_QEM_ENABLED, &qem) < 0)
		return 0;

	return qem;
}

static void lge_ftm_set_frst_status(int status)
{
	lge_ftm_proc_comm(CUSTOMER_CMD1_SET_FRST_STATUS, &status);
}

static int lge_ftm_get_frst_status(void)
{
	int status = 0;
	if(lge_ftm_proc_comm(CUSTOMER_CMD1_GET_FRST_STATUS, &status) < 0)
		return 0;
	return status;
}

static ssize_t
show_frst_status(struct device *dev, struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n",	lge_ftm_get_frst_status());
}

static ssize_t
store_frst_status(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t count)
{
	int value;

	sscanf(buf, "%d", &value);
	pr_info("%s: set frst status:%d!\n", __func__, value);
	lge_ftm_set_frst_status(value);

	return count;
}

static ssize_t
show_qem(struct device *dev, struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", lge_ftm_get_qem());
}

static DEVICE_ATTR(frst_status, S_IRUGO | S_IWUSR, show_frst_status, store_frst_status);
static DEVICE_ATTR(qem, S_IRUGO | S_IWUSR, show_qem, NULL);
static struct attribute *dev_attrs[] = {
	&dev_attr_frst_status.attr,
	&dev_attr_qem.attr,
	NULL,
};
static struct attribute_group dev_attr_grp = {
	.attrs = dev_attrs,
};

static int lge_ftm_probe(struct platform_device *pdev)
{
	int ret;

	ret = sysfs_create_group(&pdev->dev.kobj, &dev_attr_grp);
	if(ret < 0)
		pr_err("%s: cannot create sysfs\n", __func__);

	return 0;
}

static struct platform_driver lge_ftm_driver = {
	.probe = lge_ftm_probe,
	.driver = {
		.name = "lge-ftm",
	},
};

static struct platform_device lge_ftm_device = {
	.name   = "lge-ftm",
	.id = 0,
};

static int __init lge_ftm_init(void)
{
	printk(KERN_INFO "LGE FTM Driver Init\n");

	if(platform_driver_register(&lge_ftm_driver) < 0) {
		printk(KERN_ERR"%s: lge_ftm_driver register failed\n", __func__);
		return -1;
	}

	if(platform_device_register(&lge_ftm_device) < 0) {
		printk(KERN_ERR"%s: lge_ftm_device register failed\n", __func__);
		return -1;
	}

	return 0;
}

module_init(lge_ftm_init);

MODULE_DESCRIPTION("LGE Factory test module");
MODULE_AUTHOR("Munyoung Hwang <munyoung.hwang@lge.com>")
MODULE_LICENSE("GPL v2");
