/*
 * AT42QT602240/ATMXT224 Touchscreen driver
 *
 * Copyright (C) 2010 Samsung Electronics Co.Ltd
 * Author: Joonyoung Shim <jy0922.shim@samsung.com>
 *
 * 2010 Modified by LG Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/i2c.h>
#include <linux/i2c/qt602240_ts.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#ifndef CONFIG_MACH_MSM8X55_VICTOR
	#define QT602240_USE_FIRMWARE_CLASS
#endif

#ifndef QT602240_USE_FIRMWARE_CLASS
#include "qt602240_firmware_2_0.h"
#endif

#ifdef CONFIG_MACH_MSM8X55_VICTOR
/* LGE_CHANGE
  * For qt602240_ts bring up for Victor
  * 2011-01-25, guilbert.lee@lge.com
  */
#include <linux/gpio.h>
#include <mach/board_lge.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#endif

/* Version */
#define QT602240_VER_20			20
#define QT602240_VER_21			21
#define QT602240_VER_22			22
#ifdef CONFIG_MACH_MSM8X55_VICTOR
	#define QT602240_VER_32		32
#endif

/* Slave addresses */
#define QT602240_APP_LOW		0x4a
#define QT602240_APP_HIGH		0x4b
#define QT602240_BOOT_LOW		0x24
#define QT602240_BOOT_HIGH		0x25

/* Firmware */
#define QT602240_FW_NAME		"qt602240.fw"

/* Registers */
#define QT602240_FAMILY_ID		0x00
#define QT602240_VARIANT_ID		0x01
#define QT602240_VERSION		0x02
#define QT602240_BUILD			0x03
#define QT602240_MATRIX_X_SIZE		0x04
#define QT602240_MATRIX_Y_SIZE		0x05
#define QT602240_OBJECT_NUM		0x06
#define QT602240_OBJECT_START		0x07

#define QT602240_OBJECT_SIZE		6

/* Object types */
#define QT602240_GEN_MESSAGE		5
#define QT602240_GEN_COMMAND		6
#define QT602240_GEN_POWER			7
#define QT602240_GEN_ACQUIRE		8
#define QT602240_TOUCH_MULTI		9
#define QT602240_TOUCH_KEYARRAY		15
#define QT602240_TOUCH_PROXIMITY	23
#define QT602240_PROCI_GRIPFACE		20
#define QT602240_PROCG_NOISE		22
#define QT602240_PROCI_ONETOUCH		24
#define QT602240_PROCI_TWOTOUCH		27
#define QT602240_SPT_COMMSCONFIG	18	/* firmware ver 21 over */
#define QT602240_SPT_GPIOPWM		19
#define QT602240_SPT_SELFTEST		25
#define QT602240_SPT_CTECONFIG		28
#define QT602240_DEBUG_DIAGNOSTIC	37
#define QT602240_SPT_USERDATA		38	/* firmware ver 21 over */

/* QT602240_GEN_COMMAND field */
#define QT602240_COMMAND_RESET		0
#define QT602240_COMMAND_BACKUPNV	1
#define QT602240_COMMAND_CALIBRATE	2
#define QT602240_COMMAND_REPORTALL	3
#define QT602240_COMMAND_DIAGNOSTIC	5

/* QT602240_GEN_POWER field */
#define QT602240_POWER_IDLEACQINT	0
#define QT602240_POWER_ACTVACQINT	1
#define QT602240_POWER_ACTV2IDLETO	2

/* QT602240_GEN_ACQUIRE field */
#define QT602240_ACQUIRE_CHRGTIME	0
#define QT602240_ACQUIRE_TCHDRIFT	2
#define QT602240_ACQUIRE_DRIFTST	3
#define QT602240_ACQUIRE_TCHAUTOCAL	4
#define QT602240_ACQUIRE_SYNC		5
#define QT602240_ACQUIRE_ATCHCALST	6
#define QT602240_ACQUIRE_ATCHCALSTHR	7

/* QT602240_TOUCH_MULTI field */
#define QT602240_TOUCH_CTRL		0
#define QT602240_TOUCH_XORIGIN		1
#define QT602240_TOUCH_YORIGIN		2
#define QT602240_TOUCH_XSIZE		3
#define QT602240_TOUCH_YSIZE		4
#define QT602240_TOUCH_BLEN			6
#define QT602240_TOUCH_TCHTHR		7
#define QT602240_TOUCH_TCHDI		8
#define QT602240_TOUCH_ORIENT		9
#define QT602240_TOUCH_MOVHYSTI		11
#define QT602240_TOUCH_MOVHYSTN		12
#define QT602240_TOUCH_NUMTOUCH		14
#define QT602240_TOUCH_MRGHYST		15
#define QT602240_TOUCH_MRGTHR		16
#define QT602240_TOUCH_AMPHYST		17
#define QT602240_TOUCH_XRANGE_LSB	18
#define QT602240_TOUCH_XRANGE_MSB	19
#define QT602240_TOUCH_YRANGE_LSB	20
#define QT602240_TOUCH_YRANGE_MSB	21
#define QT602240_TOUCH_XLOCLIP		22
#define QT602240_TOUCH_XHICLIP		23
#define QT602240_TOUCH_YLOCLIP		24
#define QT602240_TOUCH_YHICLIP		25
#define QT602240_TOUCH_XEDGECTRL	26
#define QT602240_TOUCH_XEDGEDIST	27
#define QT602240_TOUCH_YEDGECTRL	28
#define QT602240_TOUCH_YEDGEDIST	29
#define QT602240_TOUCH_JUMPLIMIT	30	/* firmware ver 22 over */

/* QT602240_PROCI_GRIPFACE field */
#define QT602240_GRIPFACE_CTRL		0
#define QT602240_GRIPFACE_XLOGRIP	1
#define QT602240_GRIPFACE_XHIGRIP	2
#define QT602240_GRIPFACE_YLOGRIP	3
#define QT602240_GRIPFACE_YHIGRIP	4
#define QT602240_GRIPFACE_MAXTCHS	5
#define QT602240_GRIPFACE_SZTHR1	7
#define QT602240_GRIPFACE_SZTHR2	8
#define QT602240_GRIPFACE_SHPTHR1	9
#define QT602240_GRIPFACE_SHPTHR2	10
#define QT602240_GRIPFACE_SUPEXTTO	11

/* QT602240_PROCI_NOISE field */
#define QT602240_NOISE_CTRL		0
#define QT602240_NOISE_OUTFLEN		1
#define QT602240_NOISE_GCAFUL_LSB	3
#define QT602240_NOISE_GCAFUL_MSB	4
#define QT602240_NOISE_GCAFLL_LSB	5
#define QT602240_NOISE_GCAFLL_MSB	6
#define QT602240_NOISE_ACTVGCAFVALID	7
#define QT602240_NOISE_NOISETHR		8
#define QT602240_NOISE_FREQHOPSCALE	10
#define QT602240_NOISE_FREQ0		11
#define QT602240_NOISE_FREQ1		12
#define QT602240_NOISE_FREQ2		13
#define QT602240_NOISE_FREQ3		14
#define QT602240_NOISE_FREQ4		15
#define QT602240_NOISE_IDLEGCAFVALID	16

/* QT602240_SPT_COMMSCONFIG */
#define QT602240_COMMS_CTRL		0
#define QT602240_COMMS_CMD		1

/* QT602240_SPT_CTECONFIG field */
#define QT602240_CTE_CTRL		0
#define QT602240_CTE_CMD		1
#define QT602240_CTE_MODE		2
#define QT602240_CTE_IDLEGCAFDEPTH	3
#define QT602240_CTE_ACTVGCAFDEPTH	4
#define QT602240_CTE_VOLTAGE		5	/* firmware ver 21 over */

#define QT602240_VOLTAGE_DEFAULT	2700000
#define QT602240_VOLTAGE_STEP		10000

/* Define for QT602240_GEN_COMMAND */
#define QT602240_BOOT_VALUE		0xa5
#define QT602240_BACKUP_VALUE		0x55
#define QT602240_BACKUP_TIME		25	/* msec */
#define QT602240_RESET_TIME		65	/* msec */

#define QT602240_FWRESET_TIME		175	/* msec */

/* Command to unlock bootloader */
#define QT602240_UNLOCK_CMD_MSB		0xaa
#define QT602240_UNLOCK_CMD_LSB		0xdc

/* Bootloader mode status */
#define QT602240_WAITING_BOOTLOAD_CMD	0xc0	/* valid 7 6 bit only */
#define QT602240_WAITING_FRAME_DATA	0x80	/* valid 7 6 bit only */
#define QT602240_FRAME_CRC_CHECK	0x02
#define QT602240_FRAME_CRC_FAIL		0x03
#define QT602240_FRAME_CRC_PASS		0x04
#define QT602240_APP_CRC_FAIL		0x40	/* valid 7 8 bit only */
#define QT602240_BOOT_STATUS_MASK	0x3f

/* Touch status */
#define QT602240_SUPPRESS		(1 << 1)
#define QT602240_AMP			(1 << 2)
#define QT602240_VECTOR			(1 << 3)
#define QT602240_MOVE			(1 << 4)
#define QT602240_RELEASE		(1 << 5)
#define QT602240_PRESS			(1 << 6)
#define QT602240_DETECT			(1 << 7)

/* Touchscreen absolute values */
#ifdef CONFIG_MACH_MSM8X55_VICTOR
/* LGE_CHANGE
  * For qt602240_ts bring up for Victor
  * 2011-01-25, guilbert.lee@lge.com
  */
#define QT602240_MAX_XC			0x1E0
#define QT602240_MAX_YC			0x320
#define QT602240_MAX_AREA		0x1E
#define QT602240_MAX_WIDTH		0x1E

#define QT602240_MAX_FINGER		10
#else
#define QT602240_MAX_XC			0x3ff
#define QT602240_MAX_YC			0x3ff
#define QT602240_MAX_AREA		0xff

#define QT602240_MAX_FINGER		10
#endif

/* Initial register values recommended from chip vendor */
static const u8 init_vals_ver_20[] = {
	/* QT602240_GEN_COMMAND(6) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_GEN_POWER(7) */
	0x20, 0xff, 0x32,
	/* QT602240_GEN_ACQUIRE(8) */
	0x08, 0x05, 0x05, 0x00, 0x00, 0x00, 0x05, 0x14,
	/* QT602240_TOUCH_MULTI(9) */
	0x00, 0x00, 0x00, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x00, 0x01, 0x01, 0x0e, 0x0a, 0x0a, 0x0a, 0x0a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x64,
	/* QT602240_TOUCH_KEYARRAY(15) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	/* QT602240_SPT_GPIOPWM(19) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00,
	/* QT602240_PROCI_GRIPFACE(20) */
	0x00, 0x64, 0x64, 0x64, 0x64, 0x00, 0x00, 0x1e, 0x14, 0x04,
	0x1e, 0x00,
	/* QT602240_PROCG_NOISE(22) */
	0x05, 0x00, 0x00, 0x19, 0x00, 0xe7, 0xff, 0x04, 0x32, 0x00,
	0x01, 0x0a, 0x0f, 0x14, 0x00, 0x00, 0xe8,
	/* QT602240_TOUCH_PROXIMITY(23) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	/* QT602240_PROCI_ONETOUCH(24) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_SPT_SELFTEST(25) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	/* QT602240_PROCI_TWOTOUCH(27) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_SPT_CTECONFIG(28) */
	0x00, 0x00, 0x00, 0x04, 0x08,
};

static const u8 init_vals_ver_21[] = {
	/* QT602240_GEN_COMMAND(6) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_GEN_POWER(7) */
	0x20, 0xff, 0x32,
	/* QT602240_GEN_ACQUIRE(8) */
	0x0a, 0x00, 0x05, 0x00, 0x00, 0x00, 0x09, 0x23,
	/* QT602240_TOUCH_MULTI(9) */
	0x00, 0x00, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x00, 0x01, 0x01, 0x0e, 0x0a, 0x0a, 0x0a, 0x0a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_TOUCH_KEYARRAY(15) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	/* QT602240_SPT_GPIOPWM(19) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_PROCI_GRIPFACE(20) */
	0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x28, 0x04,
	0x0f, 0x0a,
	/* QT602240_PROCG_NOISE(22) */
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x23, 0x00,
	0x00, 0x05, 0x0f, 0x19, 0x23, 0x2d, 0x03,
	/* QT602240_TOUCH_PROXIMITY(23) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	/* QT602240_PROCI_ONETOUCH(24) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_SPT_SELFTEST(25) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	/* QT602240_PROCI_TWOTOUCH(27) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_SPT_CTECONFIG(28) */
	0x00, 0x00, 0x00, 0x08, 0x10, 0x00,
};

#ifdef CONFIG_MACH_MSM8X55_VICTOR
/* LGE_CHANGE
  * For qt602240_ts bring up for Victor
  * 2011-01-25, guilbert.lee@lge.com
  */
static const u8 init_vals_ver_22[] = {
	/* QT602240_GEN_COMMAND(6) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_GEN_POWER(7) */
	0xFF, 0xff, 0x32,
	/* QT602240_GEN_ACQUIRE(8) */
	0x07, 0x05, 0x05, 0x00, 0x00, 0x00, 0x09, 0x23,
	/* QT602240_TOUCH_MULTI(9) */
	0x8F, 0x00, 0x00, 0x13, 0x0b, 0x00, 0x10, 0x32, 0x02, 0x07,
	0x00, 0x03, 0x01, 0x00, 0x05, 0x0a, 0x0a, 0x0a, 0x1F, 0x03,
	0xDF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0A,
	/* QT602240_TOUCH_KEYARRAY(15) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	/* QT602240_SPT_GPIOPWM(19) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xA9, 0x7F, 0x9A, 0x0E,
	/* QT602240_PROCI_GRIPFACE(20) */
	0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00,
	/* QT602240_PROCG_NOISE(22) */
	0x0D, 0x00, 0x00, 0x28, 0x00, 0x28, 0x00, 0x03, 0x1E, 0x00,
	0x03, 0x0A, 0x0F, 0x14, 0x19, 0x1E, 0x00,
	/* QT602240_TOUCH_PROXIMITY(23) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_PROCI_ONETOUCH(24) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_SPT_SELFTEST(25) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	/* QT602240_PROCI_TWOTOUCH(27) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_SPT_CTECONFIG(28) */
	0x00, 0x00, 0x03, 0x10, 0x20, 0x0A,
};

static const u8 init_vals_ver_32[] = {
	/* QT602240_GEN_COMMAND(6) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_GEN_POWER(7) : Must sync with qt602240_start*/
	0x40, 0xFF, 0x32,
	/* QT602240_GEN_ACQUIRE(8) */
	0x0A, 0x05, 0x05, 0x01, 0x00, 0x00, 0x09, 0x03, 0x7F, 0x7F,
	/* QT602240_TOUCH_MULTI(9) */
	0x8F, 0x00, 0x00, 0x13, 0x0B, 0x00, 0x20, 0x37, 0x01, 0x07,
	0x00, 0x00, 0x00, 0x2E, 0x0A, 0x0A, 0x0A, 0x0A, 0x1F, 0x03,
	0xDF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x88, 0x28, 0x8C, 0x50,
	0x0F, 0x00,
	/* QT602240_TOUCH_KEYARRAY(15) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	/* QT602240_SPT_GPIOPWM(19) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xA9, 0x7F, 0x9A, 0x0E,
	/* QT602240_PROCI_GRIPFACE(20) */
	0x1B, 0x00, 0x00, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00,
	/* QT602240_PROCG_NOISE(22) */
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x19, 0x00,
	0x00, 0x0B, 0x0E, 0x11, 0x14, 0x18, 0x03,
	/* QT602240_TOUCH_PROXIMITY(23) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_PROCI_ONETOUCH(24) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_SPT_SELFTEST(25) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	/* QT602240_SPT_CTECONFIG(28) */
	0x00, 0x00, 0x03, 0x20, 0x3F, 0x00,
};

#else
static const u8 init_vals_ver_22[] = {
	/* QT602240_GEN_COMMAND(6) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_GEN_POWER(7) */
	0x20, 0xff, 0x32,
	/* QT602240_GEN_ACQUIRE(8) */
	0x0a, 0x00, 0x05, 0x00, 0x00, 0x00, 0x09, 0x23,
	/* QT602240_TOUCH_MULTI(9) */
	0x00, 0x00, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x00, 0x01, 0x01, 0x0e, 0x0a, 0x0a, 0x0a, 0x0a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	/* QT602240_TOUCH_KEYARRAY(15) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	/* QT602240_SPT_GPIOPWM(19) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_PROCI_GRIPFACE(20) */
	0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x28, 0x04,
	0x0f, 0x0a,
	/* QT602240_PROCG_NOISE(22) */
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x23, 0x00,
	0x00, 0x05, 0x0f, 0x19, 0x23, 0x2d, 0x03,
	/* QT602240_TOUCH_PROXIMITY(23) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_PROCI_ONETOUCH(24) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_SPT_SELFTEST(25) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	/* QT602240_PROCI_TWOTOUCH(27) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* QT602240_SPT_CTECONFIG(28) */
	0x00, 0x00, 0x00, 0x10, 0x20, 0x00,
};
#endif

#ifdef CONFIG_MACH_MSM8X55_VICTOR
enum Firm_Status_ID {
	NO_FIRM_UP = 0,
	UPDATE_FIRM_UP,
	SUCCESS_FIRM_UP,
	FAIL_FIRM_UP,
};

static u8 firmware_status;
static u8 esd_check;
static uint8_t qt602240_cal_check = 0;
#endif

struct qt602240_info {
	u8 family_id;
	u8 variant_id;
	u8 version;
	u8 build;
	u8 matrix_xsize;
	u8 matrix_ysize;
	u8 object_num;
};

struct qt602240_object {
	u8 type;
	u16 start_address;
	u8 size;
	u8 instances;
	u8 num_report_ids;

	/* to map object and message */
	u8 max_reportid;
};

struct qt602240_message {
	u8 reportid;
	u8 message[7];
	u8 checksum;
};

struct qt602240_finger {
	int status;
	int x;
	int y;
	int area;
};

/* Each client has this additional data */
struct qt602240_data {
	struct i2c_client *client;
	struct input_dev *input_dev;
	const struct qt602240_platform_data *pdata;
	struct qt602240_object *object_table;
	struct qt602240_info info;
	struct qt602240_finger finger[QT602240_MAX_FINGER];
	unsigned int irq;
#ifdef CONFIG_MACH_MSM8X55_VICTOR
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend	early_suspend;
#endif
#endif
};

#ifdef CONFIG_MACH_MSM8X55_VICTOR
#ifdef CONFIG_HAS_EARLYSUSPEND
static void qt602240_early_suspend(struct early_suspend *);
static void qt602240_late_resume(struct early_suspend *);
#endif	/* USE_TSP_EARLY_SUSPEND */
#endif


static bool qt602240_object_readable(unsigned int type)
{
	switch (type) {
	case QT602240_GEN_MESSAGE:
	case QT602240_GEN_COMMAND:
	case QT602240_GEN_POWER:
	case QT602240_GEN_ACQUIRE:
	case QT602240_TOUCH_MULTI:
	case QT602240_TOUCH_KEYARRAY:
	case QT602240_TOUCH_PROXIMITY:
	case QT602240_PROCI_GRIPFACE:
	case QT602240_PROCG_NOISE:
	case QT602240_PROCI_ONETOUCH:
	case QT602240_PROCI_TWOTOUCH:
	case QT602240_SPT_COMMSCONFIG:
	case QT602240_SPT_GPIOPWM:
	case QT602240_SPT_SELFTEST:
	case QT602240_SPT_CTECONFIG:
	case QT602240_SPT_USERDATA:
		return true;
	default:
		return false;
	}
}

static bool qt602240_object_writable(unsigned int type)
{
	switch (type) {
	case QT602240_GEN_COMMAND:
	case QT602240_GEN_POWER:
	case QT602240_GEN_ACQUIRE:
	case QT602240_TOUCH_MULTI:
	case QT602240_TOUCH_KEYARRAY:
	case QT602240_TOUCH_PROXIMITY:
	case QT602240_PROCI_GRIPFACE:
	case QT602240_PROCG_NOISE:
	case QT602240_PROCI_ONETOUCH:
#ifndef CONFIG_MACH_MSM8X55_VICTOR
	case QT602240_PROCI_TWOTOUCH:
#endif
	case QT602240_SPT_GPIOPWM:
	case QT602240_SPT_SELFTEST:
	case QT602240_SPT_CTECONFIG:
		return true;
	default:
		return false;
	}
}

static void qt602240_dump_message(struct device *dev,
				  struct qt602240_message *message)
{
	printk(KERN_INFO"reportid:\t0x%x\n", message->reportid);
	printk(KERN_INFO"message1:\t0x%x\n", message->message[0]);
#ifndef CONFIG_MACH_MSM8X55_VICTOR
	printk(KERN_INFO"message2:\t0x%x\n", message->message[1]);
	printk(KERN_INFO"message3:\t0x%x\n", message->message[2]);
	printk(KERN_INFO"message4:\t0x%x\n", message->message[3]);
	printk(KERN_INFO"message5:\t0x%x\n", message->message[4]);
	printk(KERN_INFO"message6:\t0x%x\n", message->message[5]);
	printk(KERN_INFO"message7:\t0x%x\n", message->message[6]);
	printk(KERN_INFO"checksum:\t0x%x\n", message->checksum);
#endif
}

#ifndef CONFIG_MACH_MSM8X55_VICTOR
static int qt602240_check_bootloader(struct i2c_client *client,
				     unsigned int state)
{
	u8 val;

recheck:
	if (i2c_master_recv(client, &val, 1) != 1) {
		dev_err(&client->dev, "%s: i2c recv failed\n", __func__);
		return -EIO;
	}

	switch (state) {
	case QT602240_WAITING_BOOTLOAD_CMD:
	case QT602240_WAITING_FRAME_DATA:
		val &= ~QT602240_BOOT_STATUS_MASK;
		break;
	case QT602240_FRAME_CRC_PASS:
		if (val == QT602240_FRAME_CRC_CHECK)
			goto recheck;
		break;
	default:
		return -EINVAL;
	}

	if (val != state) {
		dev_err(&client->dev, "Unvalid bootloader mode state\n");
		return -EINVAL;
	}

	return 0;
}
#endif

static int qt602240_unlock_bootloader(struct i2c_client *client)
{
	u8 buf[2];

	buf[0] = QT602240_UNLOCK_CMD_LSB;
	buf[1] = QT602240_UNLOCK_CMD_MSB;

	if (i2c_master_send(client, buf, 2) != 2) {
		dev_err(&client->dev, "%s: i2c send failed\n", __func__);
		return -EIO;
	}

	return 0;
}

static int qt602240_fw_write(struct i2c_client *client,
			     const u8 *data, unsigned int frame_size)
{
	if (i2c_master_send(client, data, frame_size) != frame_size) {
		dev_err(&client->dev, "%s: i2c send failed\n", __func__);
		return -EIO;
	}

	return 0;
}

static int __qt602240_read_reg(struct i2c_client *client,
			       u16 reg, u16 len, void *val)
{
	struct i2c_msg xfer[2];
	u8 buf[2];

	buf[0] = reg & 0xff;
	buf[1] = (reg >> 8) & 0xff;

	/* Write register */
	xfer[0].addr = client->addr;
	xfer[0].flags = 0;
	xfer[0].len = 2;
	xfer[0].buf = buf;

	/* Read data */
	xfer[1].addr = client->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = len;
	xfer[1].buf = val;

	if (i2c_transfer(client->adapter, xfer, 2) != 2) {
		dev_err(&client->dev, "%s: i2c transfer failed\n", __func__);
		return -EIO;
	}

	return 0;
}

static int qt602240_read_reg(struct i2c_client *client, u16 reg, u8 *val)
{
	return __qt602240_read_reg(client, reg, 1, val);
}

static int qt602240_write_reg(struct i2c_client *client, u16 reg, u8 val)
{
	u8 buf[3];

	buf[0] = reg & 0xff;
	buf[1] = (reg >> 8) & 0xff;
	buf[2] = val;

	if (i2c_master_send(client, buf, 3) != 3) {
		dev_err(&client->dev, "%s: i2c send failed\n", __func__);
		return -EIO;
	}

	return 0;
}

static int qt602240_read_object_table(struct i2c_client *client,
				      u16 reg, u8 *object_buf)
{
	return __qt602240_read_reg(client, reg, QT602240_OBJECT_SIZE,
				   object_buf);
}

static struct qt602240_object *
qt602240_get_object(struct qt602240_data *data, u8 type)
{
	struct qt602240_object *object;
	int i;

	for (i = 0; i < data->info.object_num; i++) {
		object = data->object_table + i;
		if (object->type == type)
			return object;
	}

	dev_err(&data->client->dev, "Invalid object type\n");
	return NULL;
}

static int qt602240_read_message(struct qt602240_data *data,
				 struct qt602240_message *message)
{
	struct qt602240_object *object;
	u16 reg;

	object = qt602240_get_object(data, QT602240_GEN_MESSAGE);
	if (!object)
		return -EINVAL;

	reg = object->start_address;
	return __qt602240_read_reg(data->client, reg,
			sizeof(struct qt602240_message), message);
}

static int qt602240_read_object(struct qt602240_data *data,
				u8 type, u8 offset, u8 *val)
{
	struct qt602240_object *object;
	u16 reg;

	object = qt602240_get_object(data, type);

	if (!object)
		return -EINVAL;

	reg = object->start_address;

	return __qt602240_read_reg(data->client, reg + offset, 1, val);
}

static int qt602240_read_diagnostic_object(struct qt602240_data *data,
				u8 type, u16 len, u8 *val)
{
	struct qt602240_object *object;
	u16 reg;

	object = qt602240_get_object(data, type);

	if (!object)
		return -EINVAL;

	reg = object->start_address;

	return __qt602240_read_reg(data->client, reg, len, val);
}


static int qt602240_write_object(struct qt602240_data *data,
				 u8 type, u8 offset, u8 val)
{
	struct qt602240_object *object;
	u16 reg;

	object = qt602240_get_object(data, type);
	if (!object)
		return -EINVAL;

	reg = object->start_address;

	return qt602240_write_reg(data->client, reg + offset, val);
}

#ifdef CONFIG_MACH_MSM8X55_VICTOR
static void reset_chip(struct qt602240_data *data)
{
	qt602240_write_object(data, QT602240_GEN_COMMAND,
			QT602240_COMMAND_RESET, 1);

	msleep(QT602240_RESET_TIME);
}

static void qt602240_calibrate_chip(struct qt602240_data *data)
{
	printk(KERN_INFO"qt602240_calibrate_chip");

#if 0
	if (qt602240_cal_check == 0) {
		/* change calibration suspend settings to zero until calibration confirmed good */
		qt602240_write_object(data, QT602240_GEN_ACQUIRE, QT602240_ACQUIRE_ATCHCALST, 0);
		qt602240_write_object(data, QT602240_GEN_ACQUIRE, QT602240_ACQUIRE_ATCHCALSTHR, 0);				
	}
#endif

	/* send calibration command to the chip */
	qt602240_write_object(data, QT602240_GEN_COMMAND, QT602240_COMMAND_CALIBRATE, 1);
	qt602240_cal_check = 1;
}

static unsigned int qt_time_point;
static unsigned int qt_time_diff;
static unsigned int qt_timer_state;
static unsigned int pre_atch_ch = 0, count = 0, first_good = 1;

static int qt602240_check_abs_time(void)
{
	qt_time_diff = 0;

	if (!qt_time_point)
		return 0;

	qt_time_diff = jiffies_to_msecs(jiffies) - qt_time_point;
	if (qt_time_diff > 0)
		return 1;
	else
		return 0;
}

void qt602240_check_chip_calibration(struct qt602240_data *data)
{
	uint8_t data_buffer[100] = { 0 };
	uint8_t try_ctr = 0;
	uint8_t data_byte = 0xF3; /* dianostic command to get touch flags */
	uint8_t tch_ch = 0, atch_ch = 0;
	uint8_t check_mask;
	uint8_t i;
	uint8_t j;
	uint8_t x_line_limit;

	/* we have had the first touchscreen or face suppression message
	 * after a calibration - check the sensor state and try to confirm if
	 * cal was good or bad */

	/* get touch flags from the chip using the diagnostic object */
	/* write command to command processor to get touch flags - 0xF3 Command required to do this */
	qt602240_write_object(data, QT602240_GEN_COMMAND, QT602240_COMMAND_DIAGNOSTIC, data_byte);
	qt602240_write_object(data, QT602240_TOUCH_MULTI, QT602240_TOUCH_TCHTHR, 30);
	msleep(10);

	/* read touch flags from the diagnostic object - clear buffer so the while loop can run first time */
	memset(data_buffer , 0xFF, sizeof(data_buffer));

	/* wait for diagnostic object to update */
	while (!((data_buffer[0] == 0xF3) && (data_buffer[1] == 0x00))) {
		/* wait for data to be valid  */
		if (try_ctr > 10) {
			/* Failed! */
			printk(KERN_INFO"[QT602240] Diagnostic Data did not update!!\n");
			qt_timer_state = 0;
			break;
		}
		msleep(2);
		try_ctr++; /* timeout counter */
		qt602240_read_diagnostic_object(data, QT602240_DEBUG_DIAGNOSTIC, 2, data_buffer);
	}

	/* data is ready - read the detection flags */
	/* data array is 20 x 16 bits for each set of flags, 2 byte header, 40 bytes for touch flags 40 bytes for antitouch flags*/
	qt602240_read_diagnostic_object(data, QT602240_DEBUG_DIAGNOSTIC, 82, data_buffer);

	/* count up the channels/bits if we recived the data properly */
	if ((data_buffer[0] == 0xF3) && (data_buffer[1] == 0x00)) {
		/* mode 0 : 16 x line, mode 1 : 17 etc etc upto mode 4.*/
		x_line_limit = data->pdata->x_line;  /* Victor(Mode 3) : 19(x) * 11(y) */

		if (x_line_limit > 20) {
			/* hard limit at 20 so we don't over-index the array */
			x_line_limit = 20;
		}

		/* double the limit as the array is in bytes not words */
		x_line_limit = x_line_limit << 1;

		/* count the channels and print the flags to the log */
		/* check X lines - data is in words so increment 2 at a time */
		for (i = 0; i < x_line_limit; i += 2) {
			/* print the flags to the log - only really needed for debugging */

			/* count how many bits set for this row */
			for (j = 0; j < 8; j++) {
				/* create a bit mask to check against */
				check_mask = 1 << j;

				/* check detect flags */
				if (data_buffer[2+i] & check_mask)
					tch_ch++;
				if (data_buffer[3+i] & check_mask)
					tch_ch++;

				/* check anti-detect flags */
				if (data_buffer[42+i] & check_mask)
					atch_ch++;
				if (data_buffer[43+i] & check_mask)
					atch_ch++;
			}
		}


		/* print how many channels we counted */
		/*
			printk(KERN_INFO"[QT602240] Flags Counted channels: t:%d a:%d \n", tch_ch, atch_ch);
		*/

		/* send page up command so we can detect when data updates next time,
		 * page byte will sit at 1 until we next send F3 command */
		data_byte = 0x01;
		qt602240_write_object(data, QT602240_GEN_COMMAND, QT602240_COMMAND_DIAGNOSTIC, data_byte);

		/* process counters and decide if we must re-calibrate or if cal was good */
		if ((tch_ch > 0) && (atch_ch == 0)) {
			/* cal was good - don't need to check any more */
			if (!qt602240_check_abs_time())
				qt_time_diff = 501;

			if (qt_timer_state == 1) {
				if (qt_time_diff > 500 && qt_time_diff <= 2000) {
					printk(KERN_INFO"[QT602240] calibration maybe good\n");
					qt602240_cal_check = 1;
					if (first_good) {
						qt602240_write_object(data, QT602240_GEN_ACQUIRE, QT602240_ACQUIRE_TCHAUTOCAL, 0x01);
						qt602240_write_object(data, QT602240_TOUCH_MULTI, QT602240_TOUCH_TCHTHR, 55);
					}
					first_good=0;
#if 0
					/* Write normal acquisition config back to the chip. */
					qt602240_write_object(data, QT602240_GEN_ACQUIRE, QT602240_ACQUIRE_ATCHCALST, 0x09);
					qt602240_write_object(data, QT602240_GEN_ACQUIRE, QT602240_ACQUIRE_ATCHCALSTHR, 0x23);
#endif
				} else if (qt_time_diff > 2000) {
					printk(KERN_INFO"[QT602240] calibration was good\n");
					qt_timer_state = 0;
					qt_time_point = jiffies_to_msecs(jiffies);
					qt602240_write_object(data, QT602240_GEN_ACQUIRE, QT602240_ACQUIRE_TCHAUTOCAL, 0x00);

					qt602240_cal_check = 0;
			    } else
				   qt602240_cal_check = 1;
			} else {
				qt_timer_state = 1;
				qt_time_point = jiffies_to_msecs(jiffies);
				qt602240_cal_check = 1;
			}
		} else if (atch_ch >= 6) {
			printk(KERN_DEBUG "[TSP] calibration was bad\n");
			/* cal was bad - must recalibrate and check afterwards */
			qt602240_calibrate_chip(data);
			qt_timer_state = 0;
			qt_time_point = jiffies_to_msecs(jiffies);
		} else if (atch_ch == pre_atch_ch) {
			count++;
			if (count == 5) {
				printk(KERN_DEBUG "[TSP] calibration was bad\n");
				/* cal was bad - must recalibrate and check afterwards */
				qt602240_calibrate_chip(data);
				qt_timer_state = 0;
				qt_time_point = jiffies_to_msecs(jiffies);
				count = 0;
			}
		} else {
			/*
				printk(KERN_INFO"[QT602240] calibration was not decided yet\n");
			*/
			/* we cannot confirm if good or bad - we must wait for next touch  message to confirm */
			qt602240_cal_check = 1;
			/* Reset the 100ms timer */
			qt_timer_state = 0;
			qt_time_point = jiffies_to_msecs(jiffies);
		}
		pre_atch_ch = atch_ch;
	}
}
#endif

static void qt602240_input_report(struct qt602240_data *data, int single_id)
{
	struct qt602240_finger *finger = data->finger;
	struct input_dev *input_dev = data->input_dev;
#ifndef CONFIG_MACH_MSM8X55_VICTOR
	int status = finger[single_id].status;
#endif
	int finger_num;
	int id;

	finger_num = 0;

	if (finger[single_id].status == QT602240_RELEASE) {
		for (id = 0; id < QT602240_MAX_FINGER; id++) {
			if(id != single_id && finger[id].status != 0)
				finger_num++;
		}

		if(finger_num == 0) {
			finger[single_id].status = 0;
			input_mt_sync(input_dev);
			input_sync(input_dev);
			return;
		}
	}

	
	for (id = 0; id < QT602240_MAX_FINGER; id++) {
		if (!finger[id].status)
			continue;
		if (finger[id].status == QT602240_RELEASE) {
			finger[id].status = 0;
			continue;
		}

		if(finger[id].area > QT602240_MAX_WIDTH)
			finger[id].area = QT602240_MAX_WIDTH;

		input_report_abs(input_dev, ABS_MT_POSITION_X, finger[id].x);
		input_report_abs(input_dev, ABS_MT_POSITION_Y, finger[id].y);
		input_report_abs(input_dev, ABS_MT_TOUCH_MAJOR, finger[id].area);
		input_report_abs(input_dev, ABS_MT_WIDTH_MAJOR, QT602240_MAX_WIDTH);
		input_report_abs(input_dev, ABS_MT_TRACKING_ID, id);

		input_mt_sync(input_dev);
	}
#ifndef CONFIG_MACH_MSM8X55_VICTOR
	input_report_key(input_dev, BTN_TOUCH, finger_num > 0);

	if (status != QT602240_RELEASE) {
		input_report_abs(input_dev, ABS_X, finger[single_id].x);
		input_report_abs(input_dev, ABS_Y, finger[single_id].y);
	}
#endif
	input_sync(input_dev);
}

static void qt602240_input_touchevent(struct qt602240_data *data,
				      struct qt602240_message *message, int id)
{
	struct qt602240_finger *finger = data->finger;
	struct device *dev = &data->client->dev;
	u8 status = message->message[0];
	int x;
	int y;
	int area;

	if (status & QT602240_RELEASE) {
		dev_dbg(dev, "[%d] released\n", id);
		
		finger[id].status = QT602240_RELEASE;
		qt602240_input_report(data, id);
		return;
	}

	if (status & QT602240_DETECT) {
		/* Check only AMP detection */
		if (!(status & (QT602240_PRESS | QT602240_MOVE)))
			return;

		x = (message->message[1] << 2) | ((message->message[3] & ~0x3f) >> 6);
		y = (message->message[2] << 2) | ((message->message[3] & ~0xf3) >> 2);
		area = message->message[4];

#ifndef CONFIG_MACH_MSM8X55_VICTOR
		dev_dbg(dev, "[%d] %s x: %d, y: %d, area: %d\n", id,
				status & QT602240_MOVE ? "moved" : "pressed",
				x, y, area);
#endif
		finger[id].status = status & QT602240_MOVE ?
			QT602240_MOVE : QT602240_PRESS;
		finger[id].x = x;
		finger[id].y = y;
		finger[id].area = area;

		qt602240_input_report(data, id);
	}
}

static irqreturn_t qt602240_interrupt(int irq, void *dev_id)
{
	struct qt602240_data *data = dev_id;
	struct qt602240_message message;
	struct qt602240_object *object;
	struct device *dev = &data->client->dev;
	int id;
	u8 reportid;
	u8 max_reportid;
	u8 min_reportid;
	uint8_t touch_message_flag = 0;

	do {
		if (qt602240_read_message(data, &message)) {
			dev_err(dev, "Failed to read message\n");
			goto end;
		}

		reportid = message.reportid;
		/* whether reportid is thing of QT602240_TOUCH_MULTI */
		object = qt602240_get_object(data, QT602240_TOUCH_MULTI);
		if (!object)
			goto end;

		max_reportid = object->max_reportid;
		min_reportid = max_reportid - object->num_report_ids + 1;
		id = reportid - min_reportid;

		if (reportid != 0xff)
			esd_check = 0;

		if (reportid >= min_reportid && reportid <= max_reportid) {
			qt602240_input_touchevent(data, &message, id);

		#ifdef CONFIG_MACH_MSM8X55_VICTOR
			/* PRESS or MOVE */
			if ((message.message[0] & QT602240_DETECT))
				touch_message_flag = 1;
		#endif
		}
		else if (reportid != 0xff) {
			qt602240_dump_message(dev, &message);
		}
	} while (!gpio_get_value(data->pdata->gpio_int));

	esd_check++;
end:
	if (esd_check >= 5) {
		printk(KERN_INFO"ESD!! Touch Reset!!!\n");
		reset_chip(data);
	}

	if (touch_message_flag && (qt602240_cal_check))
		qt602240_check_chip_calibration(data);

	return IRQ_HANDLED;
}

static int qt602240_check_reg_init(struct qt602240_data *data)
{
	struct qt602240_object *object;
	struct device *dev = &data->client->dev;
	int index = 0;
	int i, j;
	u8 version = data->info.version;
	u8 *init_vals;

	switch (version) {
	case QT602240_VER_20:
		init_vals = (u8 *)init_vals_ver_20;
		break;
	case QT602240_VER_21:
		init_vals = (u8 *)init_vals_ver_21;
		break;
	case QT602240_VER_22:
		init_vals = (u8 *)init_vals_ver_22;
		break;
#ifdef CONFIG_MACH_MSM8X55_VICTOR
	case QT602240_VER_32:
		init_vals = (u8 *)init_vals_ver_32;
		break;
#endif
	default:
		dev_err(dev, "Firmware version %d doesn't support\n", version);
		return -EINVAL;
	}

	for (i = 0; i < data->info.object_num; i++) {
		object = data->object_table + i;

		if (!qt602240_object_writable(object->type))
			continue;

		for (j = 0; j < object->size + 1; j++)
			qt602240_write_object(data, object->type, j,
					init_vals[index + j]);

		index += object->size + 1;
	}

	return 0;
}

static int qt602240_check_matrix_size(struct qt602240_data *data)
{
#ifndef CONFIG_MACH_MSM8X55_VICTOR
	const struct qt602240_platform_data *pdata = data->pdata;
	struct device *dev = &data->client->dev;
	int mode = -1;
	int error;
	u8 val;

	dev_dbg(dev, "Number of X lines: %d\n", pdata->x_line);
	dev_dbg(dev, "Number of Y lines: %d\n", pdata->y_line);

	switch (pdata->x_line) {
	case 0 ... 15:
		if (pdata->y_line <= 14)
			mode = 0;
		break;
	case 16:
		if (pdata->y_line <= 12)
			mode = 1;
		if (pdata->y_line == 13 || pdata->y_line == 14)
			mode = 0;
		break;
	case 17:
		if (pdata->y_line <= 11)
			mode = 2;
		if (pdata->y_line == 12 || pdata->y_line == 13)
			mode = 1;
		break;
	case 18:
		if (pdata->y_line <= 10)
			mode = 3;
		if (pdata->y_line == 11 || pdata->y_line == 12)
			mode = 2;
		break;
	case 19:
		if (pdata->y_line <= 9)
			mode = 4;
		if (pdata->y_line == 10 || pdata->y_line == 11)
			mode = 3;
		break;
	case 20:
		mode = 4;
	}

	if (mode < 0) {
		dev_err(dev, "Invalid X/Y lines\n");
		return -EINVAL;
	}

	error = qt602240_read_object(data, QT602240_SPT_CTECONFIG,
				QT602240_CTE_MODE, &val);
	if (error)
		return error;

	if (mode == val)
		return 0;

	/* Change the CTE configuration */
	qt602240_write_object(data, QT602240_SPT_CTECONFIG,
			QT602240_CTE_CTRL, 1);
	qt602240_write_object(data, QT602240_SPT_CTECONFIG,
			QT602240_CTE_MODE, mode);
	qt602240_write_object(data, QT602240_SPT_CTECONFIG,
			QT602240_CTE_CTRL, 0);
#endif
	return 0;
}

static int qt602240_make_highchg(struct qt602240_data *data)
{
#ifndef CONFIG_MACH_MSM8X55_VICTOR
	struct device *dev = &data->client->dev;
	int count = 10;
	int error;
	u8 val;

	/* Read dummy message to make high CHG pin */
	do {
		error = qt602240_read_object(data, QT602240_GEN_MESSAGE, 0, &val);

		if (error)
			return error;
	} while ((val != 0xff) && --count);

	if (!count) {
		dev_err(dev, "CHG pin isn't cleared\n");
		return -EBUSY;
	}
#endif
	return 0;
}

static void qt602240_handle_pdata(struct qt602240_data *data)
{
	const struct qt602240_platform_data *pdata = data->pdata;
	u8 voltage;

#ifndef CONFIG_MACH_MSM8X55_VICTOR
	/* Set touchscreen lines */
	qt602240_write_object(data, QT602240_TOUCH_MULTI, QT602240_TOUCH_XSIZE,
			pdata->x_line);
	qt602240_write_object(data, QT602240_TOUCH_MULTI, QT602240_TOUCH_YSIZE,
			pdata->y_line);

	/* Set touchscreen orient */
	qt602240_write_object(data, QT602240_TOUCH_MULTI, QT602240_TOUCH_ORIENT,
			pdata->orient);

	/* Set touchscreen burst length */
	qt602240_write_object(data, QT602240_TOUCH_MULTI,
			QT602240_TOUCH_BLEN, pdata->blen);

	/* Set touchscreen threshold */
	qt602240_write_object(data, QT602240_TOUCH_MULTI,
			QT602240_TOUCH_TCHTHR, pdata->threshold);

	/* Set touchscreen resolution */
	qt602240_write_object(data, QT602240_TOUCH_MULTI,
			QT602240_TOUCH_XRANGE_LSB, (pdata->x_size - 1) & 0xff);
	qt602240_write_object(data, QT602240_TOUCH_MULTI,
			QT602240_TOUCH_XRANGE_MSB, (pdata->x_size - 1) >> 8);
	qt602240_write_object(data, QT602240_TOUCH_MULTI,
			QT602240_TOUCH_YRANGE_LSB, (pdata->y_size - 1) & 0xff);
	qt602240_write_object(data, QT602240_TOUCH_MULTI,
			QT602240_TOUCH_YRANGE_MSB, (pdata->y_size - 1) >> 8);
#endif

	/* Set touchscreen voltage */
	if (data->info.version >= QT602240_VER_21 && pdata->voltage) {
		if (pdata->voltage < QT602240_VOLTAGE_DEFAULT) {
			voltage = (QT602240_VOLTAGE_DEFAULT - pdata->voltage) /
				QT602240_VOLTAGE_STEP;
			voltage = 0xff - voltage + 1;
		} else
			voltage = (pdata->voltage - QT602240_VOLTAGE_DEFAULT) /
				QT602240_VOLTAGE_STEP;

		qt602240_write_object(data, QT602240_SPT_CTECONFIG,
				QT602240_CTE_VOLTAGE, voltage);
	}
}

static int qt602240_get_info(struct qt602240_data *data)
{
	struct i2c_client *client = data->client;
	struct qt602240_info *info = &data->info;
	int error;
	u8 val;

	error = qt602240_read_reg(client, QT602240_FAMILY_ID, &val);
	if (error)
		return error;
	info->family_id = val;

	error = qt602240_read_reg(client, QT602240_VARIANT_ID, &val);
	if (error)
		return error;
	info->variant_id = val;

	error = qt602240_read_reg(client, QT602240_VERSION, &val);
	if (error)
		return error;
	info->version = val;

	error = qt602240_read_reg(client, QT602240_BUILD, &val);
	if (error)
		return error;
	info->build = val;

	error = qt602240_read_reg(client, QT602240_OBJECT_NUM, &val);
	if (error)
		return error;
	info->object_num = val;

	return 0;
}

static int qt602240_get_object_table(struct qt602240_data *data)
{
	int error;
	int i;
	u16 reg;
	u8 reportid = 0;
	u8 buf[QT602240_OBJECT_SIZE];

	for (i = 0; i < data->info.object_num; i++) {
		struct qt602240_object *object = data->object_table + i;

		reg = QT602240_OBJECT_START + QT602240_OBJECT_SIZE * i;
		error = qt602240_read_object_table(data->client, reg, buf);
		if (error)
			return error;

		object->type = buf[0];
		object->start_address = (buf[2] << 8) | buf[1];
		object->size = buf[3];
		object->instances = buf[4];
		object->num_report_ids = buf[5];

		if (object->num_report_ids) {
			reportid += object->num_report_ids *
					(object->instances + 1);
			object->max_reportid = reportid;
		}
	}

	return 0;
}

static int qt602240_initialize(struct qt602240_data *data)
{
	struct i2c_client *client = data->client;
	struct qt602240_info *info = &data->info;
	int error;
	u8 val;

	error = qt602240_get_info(data);
	if (error)
		return error;

	data->object_table = kcalloc(info->object_num,
				     sizeof(struct qt602240_data),
				     GFP_KERNEL);
	if (!data->object_table) {
		dev_err(&client->dev, "Failed to allocate memory\n");
		return -ENOMEM;
	}

	/* Get object table information */
	error = qt602240_get_object_table(data);
	if (error)
		return error;

	/* Check register init values */
	error = qt602240_check_reg_init(data);
	if (error)
		return error;

	/* Check X/Y matrix size */
	error = qt602240_check_matrix_size(data);
	if (error)
		return error;

	error = qt602240_make_highchg(data);

	if (error)
		return error;

	qt602240_handle_pdata(data);

	/* Backup to memory */
	qt602240_write_object(data, QT602240_GEN_COMMAND,
			QT602240_COMMAND_BACKUPNV,
			QT602240_BACKUP_VALUE);
	msleep(QT602240_BACKUP_TIME);

	/* Soft reset */
	qt602240_write_object(data, QT602240_GEN_COMMAND,
			QT602240_COMMAND_RESET, 1);
	msleep(QT602240_RESET_TIME);

	/* Update matrix size at info struct */
	error = qt602240_read_reg(client, QT602240_MATRIX_X_SIZE, &val);
	if (error)
		return error;
	info->matrix_xsize = val;

	error = qt602240_read_reg(client, QT602240_MATRIX_Y_SIZE, &val);
	if (error)
		return error;
	info->matrix_ysize = val;

	dev_info(&client->dev,
			"Family ID: %d Variant ID: %d Version: %d Build: %d\n",
			info->family_id, info->variant_id, info->version,
			info->build);

	dev_info(&client->dev,
			"Matrix X Size: %d Matrix Y Size: %d Object Num: %d\n",
			info->matrix_xsize, info->matrix_ysize,
			info->object_num);

	return 0;
}

static ssize_t qt602240_object_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct qt602240_data *data = dev_get_drvdata(dev);
	struct qt602240_object *object;
	int count = 0;
	int i, j;
	int error;
	u8 val;

	for (i = 0; i < data->info.object_num; i++) {
		object = data->object_table + i;

		count += sprintf(buf + count,
				"Object Table Element %d(Type %d)\n",
				i + 1, object->type);

		if (!qt602240_object_readable(object->type)) {
			count += sprintf(buf + count, "\n");
			continue;
		}

		for (j = 0; j < object->size + 1; j++) {
			error = qt602240_read_object(data,
						object->type, j, &val);
			if (error)
				return error;

			count += sprintf(buf + count,
					"  Byte %d: 0x%x (%d)\n", j, val, val);
		}

		count += sprintf(buf + count, "\n");
	}

	return count;
}

#ifdef CONFIG_MACH_MSM8X55_VICTOR
int qt602240_boot_read_mem(struct i2c_client *client, unsigned char *mem)
{
	struct i2c_msg rmsg;
	int ret;

	rmsg.addr = client->addr;
	rmsg.flags = I2C_M_RD;
	rmsg.len = 1;
	rmsg.buf = mem;
	ret = i2c_transfer(client->adapter, &rmsg, 1);

	return ret;
}
#endif

static int qt602240_load_fw(struct device *dev, const char *fn)
{
	struct qt602240_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
#ifdef QT602240_USE_FIRMWARE_CLASS
	const struct firmware *fw = NULL;
#endif
	unsigned int frame_size = 0;
	unsigned int pos = 0;
	int ret;

#ifdef QT602240_USE_FIRMWARE_CLASS
	ret = request_firmware(&fw, fn, dev);
	if (ret) {
		dev_err(dev, "Unable to open firmware %s\n", fn);
		return ret;
	}
#else
	unsigned char  *firmware_data;
	unsigned long int fw_size = 0;

	firmware_data = QT602240_firmware;
	fw_size = sizeof(QT602240_firmware);
#endif

	/* Change to the bootloader mode */
	qt602240_write_object(data, QT602240_GEN_COMMAND,
			QT602240_COMMAND_RESET, QT602240_BOOT_VALUE);
	msleep(QT602240_RESET_TIME);

	/* Change to slave address of bootloader */
	if (client->addr == QT602240_APP_LOW)
		client->addr = QT602240_BOOT_LOW;
	else
		client->addr = QT602240_BOOT_HIGH;

#ifdef CONFIG_MACH_MSM8X55_VICTOR
	{
		unsigned char boot_status;
		unsigned char boot_ver;
		unsigned int  crc_error_count;
		unsigned int next_frame;
		unsigned int j, read_status_flag;

		crc_error_count = 0;
		next_frame = 0;
		
		if ((qt602240_boot_read_mem(client, &boot_status) == 1) &&
			(boot_status & QT602240_WAITING_BOOTLOAD_CMD) == QT602240_WAITING_BOOTLOAD_CMD) {

			boot_ver = boot_status & QT602240_BOOT_STATUS_MASK;
			crc_error_count = 0;
			next_frame = 0;

#ifdef QT602240_USE_FIRMWARE_CLASS
				while (pos < fw->size) {
#else
				while (pos < fw_size) {
#endif
				for (j = 0; j < 5; j++) {
					if (qt602240_boot_read_mem(client, &boot_status) == 1)	{
						read_status_flag = 1;
						break;
					} else {
						mdelay(60);
						read_status_flag = 0;
					}
				}

				if (read_status_flag == 1) {
					if ((boot_status & QT602240_WAITING_BOOTLOAD_CMD) == QT602240_WAITING_BOOTLOAD_CMD) {

						if (qt602240_unlock_bootloader(client) == 0) {
							mdelay(10);
							printk(KERN_INFO"Unlock OK\n");
						} else {
							printk(KERN_INFO"Unlock fail\n");
						}
					} else if ((boot_status & QT602240_WAITING_BOOTLOAD_CMD) == QT602240_WAITING_FRAME_DATA) {

						#ifdef QT602240_USE_FIRMWARE_CLASS
							frame_size = ((*(fw->data + pos) << 8) | *(fw->data + pos + 1));
						#else
							frame_size = ((*(firmware_data + pos) << 8) | *(firmware_data + pos + 1));
						#endif

						/* We should add 2 at frame size as the the firmware data is not
						 * included the CRC bytes.
						 */
						frame_size += 2;

						/* Exit if frame data size is zero */
						if (0 == frame_size) {
							printk(KERN_INFO"0 == frame_size\n");
							ret = 0;
							goto out;
						}
						next_frame = 1;
						/* Write one frame to device */
						#ifdef QT602240_USE_FIRMWARE_CLASS
							qt602240_fw_write(client, fw->data + pos, frame_size);
						#else
							qt602240_fw_write(client, firmware_data + pos, frame_size);
						#endif

						mdelay(10);
					} else if (boot_status == QT602240_FRAME_CRC_CHECK) {
						#ifdef TOUCH_LOG_ENABLE
							printk(KERN_INFO"CRC Check\n");
						#endif
					} else if (boot_status == QT602240_FRAME_CRC_PASS) {
						if (next_frame == 1) {
							#ifdef TOUCH_LOG_ENABLE
								printk(KERN_INFO"CRC Ok\n");
							#endif
							pos += frame_size;
							next_frame = 0;

							#ifdef QT602240_USE_FIRMWARE_CLASS
								printk(KERN_INFO"Updated %d bytes / %zd bytes\n", pos, fw->size);
							#else
								printk(KERN_INFO"Updated %d bytes / %lu bytes\n", pos, fw_size);
							#endif
						} else
							printk(KERN_INFO"next_frame != 1\n");
					} else if (boot_status	== QT602240_FRAME_CRC_FAIL) {
						printk(KERN_INFO"CRC Fail\n");
						crc_error_count++;
					}
					if (crc_error_count > 10) {
						ret = 1;
						goto out;
					}
				} else {
					ret = 1;
					goto out;
				}
			}
		} else {
			printk(KERN_INFO"[TSP] read_boot_state() or (boot_status & 0xC0) == 0xC0) is fail!!!\n");
			ret = 1;
		}
	}
#else
	ret = qt602240_check_bootloader(client, QT602240_WAITING_BOOTLOAD_CMD);
	if (ret)
		goto out;

	/* Unlock bootloader */
	qt602240_unlock_bootloader(client);

#ifdef QT602240_USE_FIRMWARE_CLASS
	while (pos < fw->size) {
#else
	while (pos < fw_size) {
#endif
		ret = qt602240_check_bootloader(client,
						QT602240_WAITING_FRAME_DATA);
		if (ret)
			goto out;

#ifdef QT602240_USE_FIRMWARE_CLASS
		frame_size = ((*(fw->data + pos) << 8) | *(fw->data + pos + 1));
#else
		frame_size = ((*(firmware_data + pos) << 8) | *(firmware_data + pos + 1));
#endif

		/* We should add 2 at frame size as the the firmware data is not
		 * included the CRC bytes.
		 */
		frame_size += 2;

		/* Write one frame to device */
#ifdef QT602240_USE_FIRMWARE_CLASS
		qt602240_fw_write(client, fw->data + pos, frame_size);
#else
		qt602240_fw_write(client, firmware_data + pos, frame_size);
#endif

		ret = qt602240_check_bootloader(client,
						QT602240_FRAME_CRC_PASS);
		if (ret)
			goto out;

		pos += frame_size;
#ifdef QT602240_USE_FIRMWARE_CLASS
		printk(KERN_INFO"Updated %d bytes / %zd bytes\n", pos, fw->size);
#else
		printk(KERN_INFO"Updated %d bytes / %lu bytes\n", pos, fw_size);
#endif
	}
#endif /* CONFIG_MACH_MSM8X55_VICTOR */
out:
#ifdef QT602240_USE_FIRMWARE_CLASS
	release_firmware(fw);
#endif

	/* Change to slave address of application */
	if (client->addr == QT602240_BOOT_LOW)
		client->addr = QT602240_APP_LOW;
	else
		client->addr = QT602240_APP_HIGH;

	return ret;
}

#ifdef CONFIG_MACH_MSM8X55_VICTOR
static ssize_t qt602240_update_fw_show(struct device *dev,
					struct device_attribute *attr, char *buf)
{
	char a, b;
	struct qt602240_data *data = dev_get_drvdata(dev);

	a = data->info.version & 0xf0;
	a = a >> 4;
	b = data->info.version & 0x0f;
	return sprintf(buf, "%d%d\n", a, b);
}
#endif

static ssize_t qt602240_update_fw_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct qt602240_data *data = dev_get_drvdata(dev);
	unsigned int version;
	int error;

	if (sscanf(buf, "%u", &version) != 1) {
		dev_err(dev, "Invalid values\n");
		return -EINVAL;
	}

	if (data->info.version < QT602240_VER_32 || version < QT602240_VER_32) {
		dev_err(dev, "FW update supported starting with version 21\n");
		return -EINVAL;
	}

#ifdef CONFIG_MACH_MSM8X55_VICTOR
	printk(KERN_INFO"The firmware update Start!!\n");
	firmware_status = UPDATE_FIRM_UP;
#endif

	disable_irq(data->irq);

	error = qt602240_load_fw(dev, QT602240_FW_NAME);
	if (error) {
#ifdef CONFIG_MACH_MSM8X55_VICTOR
		firmware_status = FAIL_FIRM_UP;
#endif
		dev_err(dev, "The firmware update failed(%d)\n", error);
		count = error;
	} else {
#ifdef CONFIG_MACH_MSM8X55_VICTOR
		firmware_status = SUCCESS_FIRM_UP;

		printk(KERN_INFO"The firmware update succeeded!!\n");
#else
		dev_dbg(dev, "The firmware update succeeded\n");
#endif

		/* Wait for reset */
		msleep(QT602240_FWRESET_TIME);

		kfree(data->object_table);
		data->object_table = NULL;

		qt602240_initialize(data);
	}

	enable_irq(data->irq);

	return count;
}

#ifdef CONFIG_MACH_MSM8X55_VICTOR
static ssize_t qt602240_firm_status_show(struct device *dev,
					struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", firmware_status);
}
#endif

static DEVICE_ATTR(object, 0444, qt602240_object_show, NULL);
#ifdef CONFIG_MACH_MSM8X55_VICTOR
	static DEVICE_ATTR(firm_status, S_IRUGO | S_IWUSR | S_IXOTH, qt602240_firm_status_show, NULL);
	static DEVICE_ATTR(update_fw, 0664, qt602240_update_fw_show, qt602240_update_fw_store);
#else
	static DEVICE_ATTR(update_fw, 0664, NULL, qt602240_update_fw_store);
#endif

static struct attribute *qt602240_attrs[] = {
	&dev_attr_object.attr,
	&dev_attr_update_fw.attr,
	&dev_attr_firm_status.attr,
	NULL
};

static const struct attribute_group qt602240_attr_group = {
	.attrs = qt602240_attrs,
};

static void qt602240_start(struct qt602240_data *data)
{
	/* Touch enable */
	qt602240_write_object(data,
#ifdef CONFIG_MACH_MSM8X55_VICTOR
			QT602240_TOUCH_MULTI, QT602240_TOUCH_CTRL, 0x8F);
#else
			QT602240_TOUCH_MULTI, QT602240_TOUCH_CTRL, 0x83);
#endif

#ifdef CONFIG_MACH_MSM8X55_VICTOR
	qt602240_write_object(data,
			QT602240_GEN_POWER, QT602240_POWER_IDLEACQINT, 0x40);

	qt602240_write_object(data,
			QT602240_GEN_POWER, QT602240_POWER_ACTVACQINT, 0xFF);

	qt602240_write_object(data,
			QT602240_GEN_POWER, QT602240_POWER_ACTV2IDLETO, 0x32);
#endif
}

static void qt602240_stop(struct qt602240_data *data)
{
#ifdef CONFIG_MACH_MSM8X55_VICTOR
	qt602240_write_object(data,
			QT602240_GEN_POWER, QT602240_POWER_IDLEACQINT, 0);

	qt602240_write_object(data,
			QT602240_GEN_POWER, QT602240_POWER_ACTVACQINT, 0);

	qt602240_write_object(data,
			QT602240_GEN_POWER, QT602240_POWER_ACTV2IDLETO, 0);
#endif

	qt602240_write_object(data,
			QT602240_TOUCH_MULTI, QT602240_TOUCH_CTRL, 0);
}

static int qt602240_input_open(struct input_dev *dev)
{
	struct qt602240_data *data = input_get_drvdata(dev);

	qt602240_start(data);

	return 0;
}

static void qt602240_input_close(struct input_dev *dev)
{
	struct qt602240_data *data = input_get_drvdata(dev);

	qt602240_stop(data);
}

static int __devinit qt602240_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct qt602240_data *data;
	struct input_dev *input_dev;
	int error;

	if (!client->dev.platform_data)
		return -EINVAL;

	data = kzalloc(sizeof(struct qt602240_data), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!data || !input_dev) {
		dev_err(&client->dev, "Failed to allocate memory\n");
		error = -ENOMEM;
		goto err_free_mem;
	}

#ifdef CONFIG_MACH_MSM8X55_VICTOR
	qt_time_point = 0;
#endif

	input_dev->name = "AT42QT602240/ATMXT224 Touchscreen";
	input_dev->id.bustype = BUS_I2C;
	input_dev->dev.parent = &client->dev;
	input_dev->open = qt602240_input_open;
	input_dev->close = qt602240_input_close;

	__set_bit(EV_ABS, input_dev->evbit);
	__set_bit(EV_KEY, input_dev->evbit);
	__set_bit(BTN_TOUCH, input_dev->keybit);

	/* For single touch */
	input_set_abs_params(input_dev, ABS_X,
			     0, QT602240_MAX_XC, 0, 0);
	input_set_abs_params(input_dev, ABS_Y,
			     0, QT602240_MAX_YC, 0, 0);

	/* For multi touch */
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR,
			     0, QT602240_MAX_AREA, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_X,
			     0, QT602240_MAX_XC, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y,
			     0, QT602240_MAX_YC, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_WIDTH_MAJOR,
			     0, QT602240_MAX_WIDTH, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_TRACKING_ID, 0,
			QT602240_MAX_FINGER - 1, 0, 0);

	input_set_drvdata(input_dev, data);

	data->client = client;
	data->input_dev = input_dev;
	data->pdata = client->dev.platform_data;
	data->irq = client->irq;

#ifdef CONFIG_MACH_MSM8X55_VICTOR
	/* int GPIO Config */
	gpio_tlmm_config(GPIO_CFG(data->pdata->gpio_int, 0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	msleep(100);
#endif

	i2c_set_clientdata(client, data);

	error = qt602240_initialize(data);
	if (error)
		goto err_free_object;


#ifdef CONFIG_MACH_MSM8X55_VICTOR
	/* Set int GPIO Direction */
	gpio_request(data->pdata->gpio_int, "qt602240_interrupt");
	error = gpio_direction_input(data->pdata->gpio_int);

	if (error < 0)
		return error;
#endif

	error = request_threaded_irq(client->irq, NULL, qt602240_interrupt,
			IRQF_TRIGGER_FALLING, client->dev.driver->name, data);
	if (error) {
		dev_err(&client->dev, "Failed to register interrupt\n");
		goto err_free_object;
	}

	error = input_register_device(input_dev);
	if (error)
		goto err_free_irq;

	error = sysfs_create_group(&client->dev.kobj, &qt602240_attr_group);
	if (error)
		goto err_unregister_device;

#ifdef CONFIG_MACH_MSM8X55_VICTOR
	firmware_status = NO_FIRM_UP;
	esd_check = 0;
	qt_time_point = jiffies_to_msecs(jiffies);
#endif

#ifdef CONFIG_MACH_MSM8X55_VICTOR
#ifdef CONFIG_HAS_EARLYSUSPEND
	data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	data->early_suspend.suspend = qt602240_early_suspend;
	data->early_suspend.resume = qt602240_late_resume;
	register_early_suspend(&data->early_suspend);
#endif	/* CONFIG_HAS_EARLYSUSPEND */
#endif

	return 0;

err_unregister_device:
	input_unregister_device(input_dev);
	input_dev = NULL;
err_free_irq:
	free_irq(client->irq, data);
err_free_object:
	kfree(data->object_table);
err_free_mem:
	input_free_device(input_dev);
	kfree(data);
	return error;
}

static int __devexit qt602240_remove(struct i2c_client *client)
{
	struct qt602240_data *data = i2c_get_clientdata(client);

#ifdef CONFIG_MACH_MSM8X55_VICTOR
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&data->early_suspend);
#endif	/* CONFIG_HAS_EARLYSUSPEND */
#endif

	sysfs_remove_group(&client->dev.kobj, &qt602240_attr_group);
	free_irq(data->irq, data);
	input_unregister_device(data->input_dev);
	kfree(data->object_table);
	kfree(data);

	return 0;
}

#ifdef CONFIG_MACH_MSM8X55_VICTOR
static void qt602240_power_down(struct qt602240_data *data)
{
	disable_irq(data->client->irq);

	gpio_tlmm_config(GPIO_CFG(data->pdata->sda, 1, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_DISABLE);
	gpio_tlmm_config(GPIO_CFG(data->pdata->scl, 1, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_DISABLE);
	gpio_tlmm_config(GPIO_CFG(data->pdata->gpio_int, 0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_DISABLE);

	data->pdata->power(0);
}

static void qt602240_power_up(struct qt602240_data *data)
{
	data->pdata->power(1);

	gpio_tlmm_config(GPIO_CFG(data->pdata->scl, 1, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(data->pdata->sda, 1, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(data->pdata->gpio_int, 0, GPIO_CFG_INPUT,
				GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	msleep(80);
	enable_irq(data->client->irq);
	msleep(10);
}
#endif

#ifdef CONFIG_PM
#ifdef CONFIG_MACH_MSM8X55_VICTOR
static int qt602240_suspend(struct i2c_client *client)
#else
static int qt602240_suspend(struct i2c_client *client, pm_message_t mesg)
#endif
{
	struct qt602240_data *data = i2c_get_clientdata(client);
	struct input_dev *input_dev = data->input_dev;

	mutex_lock(&input_dev->mutex);

	if (input_dev->users)
		qt602240_stop(data);

	mutex_unlock(&input_dev->mutex);

	return 0;
}

static int qt602240_resume(struct i2c_client *client)
{
	struct qt602240_data *data = i2c_get_clientdata(client);
	struct input_dev *input_dev = data->input_dev;
	struct qt602240_finger *finger = data->finger;
	int id;

	for (id = 0; id < QT602240_MAX_FINGER; id++) {
		if (finger[id].status != 0)
			finger[id].status = 0;
	}

#ifndef CONFIG_MACH_MSM8X55_VICTOR
	qt602240_write_object(data, QT602240_GEN_COMMAND,
			QT602240_COMMAND_RESET, 1);

	msleep(QT602240_RESET_TIME);
#endif

	mutex_lock(&input_dev->mutex);

	if (input_dev->users)
		qt602240_start(data);

	mutex_unlock(&input_dev->mutex);
	return 0;
}
#else
#define qt602240_suspend	NULL
#define qt602240_resume		NULL
#endif


#ifdef CONFIG_HAS_EARLYSUSPEND
static void qt602240_early_suspend(struct early_suspend *h)
{
	struct qt602240_data *data = container_of(h, struct qt602240_data, early_suspend);

#ifdef CONFIG_MACH_MSM8X55_VICTOR
	if(firmware_status != UPDATE_FIRM_UP)
#endif
	{
		qt602240_suspend(data->client);
#ifdef CONFIG_MACH_MSM8X55_VICTOR
		qt602240_power_down(data);
		qt_timer_state = 0;
#endif
	}
}

static void qt602240_late_resume(struct early_suspend *h)
{
	struct qt602240_data *data = container_of(h, struct qt602240_data, early_suspend);

	
#ifdef CONFIG_MACH_MSM8X55_VICTOR
	if(firmware_status != UPDATE_FIRM_UP)
#endif
	{
#ifdef CONFIG_MACH_MSM8X55_VICTOR
		qt602240_power_up(data);
		qt602240_cal_check = 1;
#endif
		qt602240_resume(data->client);
	}
}
#endif

static const struct i2c_device_id qt602240_id[] = {
	{ "qt602240_ts", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, qt602240_id);

static struct i2c_driver qt602240_driver = {
	.driver = {
		.name	= "qt602240_ts",
		.owner	= THIS_MODULE,
	},
	.probe		= qt602240_probe,
	.remove		= __devexit_p(qt602240_remove),
#ifdef CONFIG_MACH_MSM8X55_VICTOR
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend	= qt602240_suspend,
	.resume		= qt602240_resume,
#endif
#else
	.suspend	= qt602240_suspend,
	.resume		= qt602240_resume,
#endif
	.id_table	= qt602240_id,
};

static int __init qt602240_init(void)
{
	return i2c_add_driver(&qt602240_driver);
}

static void __exit qt602240_exit(void)
{
	i2c_del_driver(&qt602240_driver);
}

module_init(qt602240_init);
module_exit(qt602240_exit);

/* Module information */
MODULE_AUTHOR("Joonyoung Shim <jy0922.shim@samsung.com>");
MODULE_DESCRIPTION("AT42QT602240/ATMXT224 Touchscreen driver");
MODULE_LICENSE("GPL");
