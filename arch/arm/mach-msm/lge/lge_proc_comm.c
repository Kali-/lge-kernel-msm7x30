/*
 * arch/arm/mach-msm/lge/lge_proc_comm.c
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

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <mach/board_lge.h>
#include "../proc_comm.h"
#include "lge_proc_comm.h"

#ifdef CONFIG_LGE_DETECT_PIF_PATCH
unsigned lge_get_pif_info(void)
{
	int err;
	unsigned pif_value = -1;
	unsigned cmd_pif = CUSTOMER_CMD1_GET_BOOT_ON_TYPE;

	err = msm_proc_comm(PCOM_CUSTOMER_CMD1, &pif_value, &cmd_pif);
	if (err < 0) {
		pr_err("%s: msm_proc_comm(PCOM_CUSTOMER_CMD2) failed\n",
		       __func__);
		return err;
	}

	/* 2011-01-26 by baborobo@lge.com
	 * 0 : Boot-on by PMIC VBUS
	       ( it is connected the USB Cable or the TA Charger )
	 * 1 : Boot-on by Normal Power long key or reboot
	 * 2 : Boot-on by PIF or PMIC remote-power-on Port
	 */
	return pif_value;
}
EXPORT_SYMBOL(lge_get_pif_info);

unsigned lge_test_frst_sync(void)
{
	int err;
	unsigned ftst_status = 5;
	unsigned cmd_frst = CUSTOMER_CMD1_TEST_FRST_SYNC;

	err = msm_proc_comm(PCOM_CUSTOMER_CMD1, &ftst_status, &cmd_frst);
	if (err < 0) {
		pr_err("%s: msm_proc_comm(CUSTOMER_CMD1_TEST_FRST_SYNC) failed\n",
		       __func__);
		return err;
	}
	return true;
}
EXPORT_SYMBOL(lge_test_frst_sync);

#endif
