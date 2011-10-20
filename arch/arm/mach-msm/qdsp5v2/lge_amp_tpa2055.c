/* arch/arm/mach-msm/qdsp5v2/lge_amp_tpa2055.c
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
#include <linux/slab.h>
#include "lge_audio_amp.h"

/* ---- I2C ADDR -----*/
#define SUBSYSTEM_CONTROL	(char) 0x00
#define INPUT_CONTROL		(char) 0x01
#define LIMITER_CONTROL		(char) 0x02
#define SPEAKER_OUTPUT		(char) 0x03
#define HEADPHONE_OUTPUT	(char) 0x04
#define SPEAKER_VOLUME		(char) 0x05
#define HP_LEFT_VOLUME		(char) 0x06
#define HP_RIGHT_VOLUME		(char) 0x07

/* ------- Subsystem Control Register (0x00) -------*/
#define THERMAL				(char) 0x01
#define SPK_FAULT			(char) 0x02
#define SSM_EN				(char) 0x04
#define SWS					(char) 0x08
#define BYPASS				(char) 0x10
#define VERSION				(char) 0x20

/* -------- Input Control Register (0x01) ---------*/
#define IN2GAIN_0DB			(char) 0x00
#define IN2GAIN_6DB			(char) 0x01
#define IN2GAIN_12DB		(char) 0x02
#define IN2GAIN_20DB		(char) 0x03
#define IN1GAIN_0DB			(char) 0x00
#define IN1GAIN_6DB			(char) 0x04
#define IN1GAIN_12DB		(char) 0x08
#define IN1GAIN_20DB		(char) 0x0C
#define IN2_SE				(char) 0x00
#define IN2_DIFF			(char) 0x10
#define IN1_SE				(char) 0x00
#define IN1_DIFF			(char) 0x20

/* -------- Release and Attack Time Register (0x02) ---------*/
#define ATTACK_0P64MS		(char) 0x00
#define ATTACK_1P28MS		(char) 0x01
#define ATTACK_2P56MS		(char) 0x02
#define ATTACK_5P12MS		(char) 0x03
#define ATTACK_10P24MS		(char) 0x04
#define ATTACK_20P48MS		(char) 0x05
#define ATTACK_30P72MS		(char) 0x06
#define ATTACK_40P96MS		(char) 0x07
#define RELEASE_41MS		(char) 0x00
#define RELEASE_123MS		(char) 0x08
#define RELEASE_205MS		(char) 0x10
#define RELEASE_287MS		(char) 0x18
#define RELEASE_369MS		(char) 0x20
#define RELEASE_451MS		(char) 0x28
#define RELEASE_533MS		(char) 0x30
#define RELEASE_615MS		(char) 0x38
#define RELEASE_697MS		(char) 0x40
#define RELEASE_779MS		(char) 0x48
#define RELEASE_861MS		(char) 0x50
#define RELEASE_943MS		(char) 0x58
#define RELEASE_1025MS		(char) 0x60
#define RELEASE_1107MS		(char) 0x68
#define RELEASE_1189MS		(char) 0x70
#define RELEASE_1271MS		(char) 0x78

/* -------- Speaker MUX and Limiter Register (0x03) ---------*/
#define SLIMLVL_2P83V		(char) 0x00
#define SLIMLVL_2P97V		(char) 0x01
#define SLIMLVL_3P10V		(char) 0x02
#define SLIMLVL_3P22V		(char) 0x03
#define SLIMLVL_3P35V		(char) 0x04
#define SLIMLVL_3P46V		(char) 0x05
#define SLIMLVL_3P58V		(char) 0x06
#define SLIMLVL_3P69V		(char) 0x07
#define SLIMLVL_3P79V		(char) 0x08
#define SLIMLVL_3P90V		(char) 0x09
#define SLIMLVL_4P00V		(char) 0x0A
#define SLIMLVL_4P20V		(char) 0x0B
#define SLIMLVL_4P38V		(char) 0x0C
#define SLIMLVL_4P56V		(char) 0x0D
#define SLIMLVL_4P73V		(char) 0x0E
#define SLIMLVL_4P90V		(char) 0x0F
#define SPLIM_EN			(char) 0x10
#define SPKOUT_MUTE			(char) 0x00
#define SPKOUT_IN1			(char) 0x20
#define SPKOUT_IN2			(char) 0x40
#define SPKOUT_IN1IN2		(char) 0x60

/* -------- Headphone MUX and Limiter Register (0x04) ---------*/
#define HLIMLVL_0P65V		(char) 0x00
#define HLIMLVL_0P75V		(char) 0x01
#define HLIMLVL_0P85V		(char) 0x02
#define HLIMLVL_0P95V		(char) 0x03
#define HLIMLVL_1P05V		(char) 0x04
#define HLIMLVL_1P15V		(char) 0x05
#define HLIMLVL_1P25V		(char) 0x06
#define HLIMLVL_1P35V		(char) 0x07
#define HPLIM_EN			(char) 0x10
#define HPOUT_MUTE			(char) 0x00
#define HPOUT_IN1			(char) 0x20
#define HPOUT_IN2			(char) 0x40
#define HPOUT_IN1IN2		(char) 0x60

/* -------- Speaker Volume Register (0x05) ---------*/
#define SPK_VOL_M60DB		(char) 0x00
#define SPK_VOL_M50DB		(char) 0x01
#define SPK_VOL_M45DB		(char) 0x02
#define SPK_VOL_M42DB		(char) 0x03
#define SPK_VOL_M39DB		(char) 0x04
#define SPK_VOL_M36DB		(char) 0x05
#define SPK_VOL_M33DB		(char) 0x06
#define SPK_VOL_M30DB		(char) 0x07
#define SPK_VOL_M27DB		(char) 0x08
#define SPK_VOL_M24DB		(char) 0x09
#define SPK_VOL_M21DB		(char) 0x0A
#define SPK_VOL_M20DB		(char) 0x0B
#define SPK_VOL_M19DB		(char) 0x0C
#define SPK_VOL_M18DB		(char) 0x0D
#define SPK_VOL_M17DB		(char) 0x0E
#define SPK_VOL_M16DB		(char) 0x0F
#define SPK_VOL_M15DB		(char) 0x10
#define SPK_VOL_M14DB		(char) 0x11
#define SPK_VOL_M13DB		(char) 0x12
#define SPK_VOL_M12DB		(char) 0x13
#define SPK_VOL_M11DB		(char) 0x14
#define SPK_VOL_M10DB		(char) 0x15
#define SPK_VOL_M9DB		(char) 0x16
#define SPK_VOL_M8DB		(char) 0x17
#define SPK_VOL_M7DB		(char) 0x18
#define SPK_VOL_M6DB		(char) 0x19
#define SPK_VOL_M5DB		(char) 0x1A
#define SPK_VOL_M4DB		(char) 0x1B
#define SPK_VOL_M3DB		(char) 0x1C
#define SPK_VOL_M2DB		(char) 0x1D
#define SPK_VOL_M1DB		(char) 0x1E
#define SPK_VOL_0DB			(char) 0x1F
#define SPK_EN				(char) 0x20

/* -------- Headphone Left Channel Volume Register (0x06) ---------*/
#define HPL_VOL_M60DB		(char) 0x00
#define HPL_VOL_M50DB		(char) 0x01
#define HPL_VOL_M45DB		(char) 0x02
#define HPL_VOL_M42DB		(char) 0x03
#define HPL_VOL_M39DB		(char) 0x04
#define HPL_VOL_M36DB		(char) 0x05
#define HPL_VOL_M33DB		(char) 0x06
#define HPL_VOL_M30DB		(char) 0x07
#define HPL_VOL_M27DB		(char) 0x08
#define HPL_VOL_M24DB		(char) 0x09
#define HPL_VOL_M21DB		(char) 0x0A
#define HPL_VOL_M20DB		(char) 0x0B
#define HPL_VOL_M19DB		(char) 0x0C
#define HPL_VOL_M18DB		(char) 0x0D
#define HPL_VOL_M17DB		(char) 0x0E
#define HPL_VOL_M16DB		(char) 0x0F
#define HPL_VOL_M15DB		(char) 0x10
#define HPL_VOL_M14DB		(char) 0x11
#define HPL_VOL_M13DB		(char) 0x12
#define HPL_VOL_M12DB		(char) 0x13
#define HPL_VOL_M11DB		(char) 0x14
#define HPL_VOL_M10DB		(char) 0x15
#define HPL_VOL_M9DB		(char) 0x16
#define HPL_VOL_M8DB		(char) 0x17
#define HPL_VOL_M7DB		(char) 0x18
#define HPL_VOL_M6DB		(char) 0x19
#define HPL_VOL_M5DB		(char) 0x1A
#define HPL_VOL_M4DB		(char) 0x1B
#define HPL_VOL_M3DB		(char) 0x1C
#define HPL_VOL_M2DB		(char) 0x1D
#define HPL_VOL_M1DB		(char) 0x1E
#define HPL_VOL_0DB			(char) 0x1F
#define HPL_EN				(char) 0x20
#define HP_TRACK			(char) 0x40

/* -------- Headphone Right Channel Volume Register (0x07) ---------*/
#define HPR_VOL_M60DB		(char) 0x00
#define HPR_VOL_M50DB		(char) 0x01
#define HPR_VOL_M45DB		(char) 0x02
#define HPR_VOL_M42DB		(char) 0x03
#define HPR_VOL_M39DB		(char) 0x04
#define HPR_VOL_M36DB		(char) 0x05
#define HPR_VOL_M33DB		(char) 0x06
#define HPR_VOL_M30DB		(char) 0x07
#define HPR_VOL_M27DB		(char) 0x08
#define HPR_VOL_M24DB		(char) 0x09
#define HPR_VOL_M21DB		(char) 0x0A
#define HPR_VOL_M20DB		(char) 0x0B
#define HPR_VOL_M19DB		(char) 0x0C
#define HPR_VOL_M18DB		(char) 0x0D
#define HPR_VOL_M17DB		(char) 0x0E
#define HPR_VOL_M16DB		(char) 0x0F
#define HPR_VOL_M15DB		(char) 0x10
#define HPR_VOL_M14DB		(char) 0x11
#define HPR_VOL_M13DB		(char) 0x12
#define HPR_VOL_M12DB		(char) 0x13
#define HPR_VOL_M11DB		(char) 0x14
#define HPR_VOL_M10DB		(char) 0x15
#define HPR_VOL_M9DB		(char) 0x16
#define HPR_VOL_M8DB		(char) 0x17
#define HPR_VOL_M7DB		(char) 0x18
#define HPR_VOL_M6DB		(char) 0x19
#define HPR_VOL_M5DB		(char) 0x1A
#define HPR_VOL_M4DB		(char) 0x1B
#define HPR_VOL_M3DB		(char) 0x1C
#define HPR_VOL_M2DB		(char) 0x1D
#define HPR_VOL_M1DB		(char) 0x1E
#define HPR_VOL_0DB			(char) 0x1F
#define HPR_EN				(char) 0x20

#define FM_HP_VOL 0
#define FM_SPK_VOL 1
#define FM_OFF 2

#define FM_MAXIUM_VOL 15

struct amp_cal_type {
	u8 in1_gain;
	u8 in2_gain;
	u8 spk_vol;
	u8 hp_lvol;
	u8 hp_rvol;
	u8 spk_lim;
	u8 hp_lim;
};

struct amp_cal_type amp_cal_data[] = {
	/*HEADSET_ST_RX*/
	{IN1GAIN_6DB, IN2GAIN_12DB, SPK_VOL_M2DB, HPL_VOL_M13DB, HPR_VOL_M13DB,
	SLIMLVL_4P90V, HLIMLVL_1P15V},
	/*ICODEC_HEADSET_SPK_RX*/
	{IN1GAIN_6DB, IN2GAIN_12DB, SPK_VOL_M2DB, HPL_VOL_M21DB, HPR_VOL_M21DB,
	SLIMLVL_4P90V, HLIMLVL_1P15V},
	/*ICODEC_SPEAKER_RX*/
	{IN1GAIN_6DB, IN2GAIN_12DB, SPK_VOL_M2DB, HPL_VOL_M13DB, HPR_VOL_M13DB,
	SLIMLVL_4P90V, HLIMLVL_1P15V},
	/*ICODEC_HEADSET_PHONE_RX*/
	{IN1GAIN_6DB, IN2GAIN_12DB, SPK_VOL_M2DB, HPL_VOL_M10DB, HPR_VOL_M10DB,
	SLIMLVL_4P90V, HLIMLVL_1P15V},
	/*ICODEC_SPEAKER_PHONE_RX*/
	{IN1GAIN_6DB, IN2GAIN_12DB, SPK_VOL_M2DB, HPL_VOL_M10DB, HPR_VOL_M10DB,
	SLIMLVL_4P90V, HLIMLVL_1P15V},
	/*ICODEC_FM_HEADSET_ST_RX*/
	{IN1GAIN_6DB, IN2GAIN_12DB, SPK_VOL_M2DB, HPL_VOL_M13DB, HPR_VOL_M13DB,
	SLIMLVL_4P90V, HLIMLVL_1P15V},
	/*ICODEC_FM_SPEAKER_RX*/
	{IN1GAIN_6DB, IN2GAIN_12DB, SPK_VOL_M2DB, HPL_VOL_M13DB, HPR_VOL_M13DB,
	SLIMLVL_4P90V, HLIMLVL_1P15V},
};

struct fm_radio_vol fm_radio_volume_table[] = {
	{HPL_VOL_M60DB, SPK_VOL_M60DB}, /* Level 0 */
	{HPL_VOL_M45DB, SPK_VOL_M45DB}, /* Level 1 */
	{HPL_VOL_M39DB, SPK_VOL_M39DB}, /* Level 2 */
	{HPL_VOL_M33DB, SPK_VOL_M33DB}, /* Level 3 */
	{HPL_VOL_M30DB, SPK_VOL_M30DB}, /* Level 4 */
	{HPL_VOL_M24DB, SPK_VOL_M24DB}, /* Level 5 */
	{HPL_VOL_M20DB, SPK_VOL_M20DB}, /* Level 6 */
	{HPL_VOL_M17DB, SPK_VOL_M17DB}, /* Level 7 */
	{HPL_VOL_M18DB, SPK_VOL_M18DB}, /* Level 8 */
	{HPL_VOL_M13DB, SPK_VOL_M13DB}, /* Level 9 */
	{HPL_VOL_M10DB, SPK_VOL_M10DB}, /* Level 10 */
	{HPL_VOL_M7DB, SPK_VOL_M7DB}, /* Level 11 */
	{HPL_VOL_M5DB, SPK_VOL_M5DB}, /* Level 12 */
	{HPL_VOL_M3DB, SPK_VOL_M3DB}, /* Level 13 */
	{HPL_VOL_M1DB, SPK_VOL_M1DB}, /* Level 14 */
	{HPL_VOL_0DB, SPK_VOL_0DB}, /* Level 15 */
};

bool first_boot_flag = true;
static uint32_t fm_radio_enabled = FM_OFF;
static struct mutex amp_lock;
static struct amp_data *_data;
#if 0
static int ReadI2C(char reg, char *ret)
{

	unsigned int err;
	unsigned char buf = reg;

	struct i2c_msg msg[2] = {
		{ _data->client->addr, 0, 1, &buf },
		{ _data->client->addr, I2C_M_RD, 1, ret}
	};

	err = i2c_transfer(_data->client->adapter, msg, 2);
	return err;

}
#endif
static int WriteI2C(char reg, char val)
{
	int	err;
	unsigned char    buf[2];
	struct i2c_msg	msg = { _data->client->addr, 0, 2, buf };

	buf[0] = reg;
	buf[1] = val;

	err = i2c_transfer(_data->client->adapter, &msg, 1);
	return err;
}


static void tpa2055d3_reset_to_defaults(void)
{
	/* Can also use this function to switch to Bypass mode */
	int fail = 0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (BYPASS | SWS));
	fail |= WriteI2C(INPUT_CONTROL, (IN2GAIN_0DB | IN1GAIN_0DB | IN2_SE | IN1_SE));
	fail |= WriteI2C(LIMITER_CONTROL, (ATTACK_2P56MS | RELEASE_451MS));
	fail |= WriteI2C(SPEAKER_OUTPUT, (SLIMLVL_4P90V | SPKOUT_MUTE));
	fail |= WriteI2C(HEADPHONE_OUTPUT, (HLIMLVL_1P15V | HPOUT_MUTE));
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_VOL_M10DB));
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_0DB | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, HPR_VOL_0DB);
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_reset_to_defaults\n");
}
#if 0
static void tpa2055d3_power_up_class_d_in1(void)
{
	int fail = 0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (IN1GAIN_0DB | IN1_SE));
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | SPK_VOL_M10DB));
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_IN1);
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_class_d_in1\n");
}
#endif
static void tpa2055d3_power_up_class_d_in2(void)
{
	int fail = 0;
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[ICODEC_SPEAKER_RX].in2_gain | IN2_DIFF));
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | SPK_VOL_M60DB));
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_IN2 | SPLIM_EN | amp_cal_data[ICODEC_SPEAKER_RX].spk_lim);
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | amp_cal_data[ICODEC_SPEAKER_RX].spk_vol));

	MM_INFO("first_boot_flag %d\n", first_boot_flag);

	if (first_boot_flag == true)	{
		fail |= WriteI2C(SUBSYSTEM_CONTROL, (SWS | BYPASS | SSM_EN));
		msleep(100);
		first_boot_flag = false;
	}

	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_class_d_in2\n");
}

static void tpa2055d3_power_up_class_d_in2_fm(void)
{
	int fail = 0;
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[ICODEC_FM_SPEAKER_RX].in2_gain | IN2_DIFF));
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | SPK_VOL_M60DB));
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_IN2 | SPLIM_EN | amp_cal_data[ICODEC_FM_SPEAKER_RX].spk_lim);
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | amp_cal_data[ICODEC_FM_SPEAKER_RX].spk_vol));

	MM_INFO("first_boot_flag %d\n", first_boot_flag);

	if (first_boot_flag == true)	{
		fail |= WriteI2C(SUBSYSTEM_CONTROL, (SWS | BYPASS | SSM_EN));
		msleep(100);
		first_boot_flag = false;
	}

	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_class_d_in2_fm\n");
}

static void tpa2055d3_power_up_class_d_phone_in2(void)
{
	int fail = 0;
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[ICODEC_SPEAKER_PHONE_RX].in2_gain | IN2_DIFF));
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | SPK_VOL_M60DB));
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_IN2 | SPLIM_EN | amp_cal_data[ICODEC_SPEAKER_PHONE_RX].spk_lim);
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | amp_cal_data[ICODEC_SPEAKER_PHONE_RX].spk_vol));

	MM_INFO("first_boot_flag %d\n", first_boot_flag);

	if (first_boot_flag == true)	{
		fail |= WriteI2C(SUBSYSTEM_CONTROL, (SWS | BYPASS | SSM_EN));
		msleep(100);
		first_boot_flag = false;
	}

	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_class_d_phone_in2\n");
}
#if 0
static void tpa2055d3_power_up_class_d_in1in2(void)
{
	int fail = 0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (IN1GAIN_0DB | IN2GAIN_0DB | IN1_SE | IN2_SE));
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | SPK_VOL_M10DB));
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_IN1IN2);
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_class_d_in1in2\n");
}
#endif
static void tpa2055d3_power_down_class_d(void)
{
	int fail = 0;
	fail |= WriteI2C(SPEAKER_VOLUME, SPK_EN);
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_MUTE);
	fail |= WriteI2C(SPEAKER_VOLUME, ~SPK_EN & SPK_VOL_M60DB);
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_down_class_d\n");
}

static void tpa2055d3_power_up_hp_in1(void)
{
	int fail = 0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[ICODEC_HEADSET_ST_RX].in1_gain | IN1_SE));
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_M60DB | HPL_EN | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, (amp_cal_data[ICODEC_HEADSET_ST_RX].hp_rvol | HPR_EN));
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_IN1 | HPLIM_EN | amp_cal_data[ICODEC_HEADSET_ST_RX].hp_lim);
	fail |= WriteI2C(HP_LEFT_VOLUME, (amp_cal_data[ICODEC_HEADSET_ST_RX].hp_lvol | HPL_EN | HP_TRACK));
	MM_INFO("HP : gain %x, volR:%x, volL:%x\n", amp_cal_data[ICODEC_HEADSET_ST_RX].in1_gain, amp_cal_data[ICODEC_HEADSET_ST_RX].hp_rvol, amp_cal_data[ICODEC_HEADSET_ST_RX].hp_lvol);
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_hp_in1\n");
}

static void tpa2055d3_power_up_hp_in1_fm(void)
{
	int fail = 0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[ICODEC_FM_HEADSET_ST_RX].in1_gain | IN1_SE));
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_M60DB | HPL_EN | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, (amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_rvol | HPR_EN));
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_IN1 | HPLIM_EN | amp_cal_data[ICODEC_HEADSET_ST_RX].hp_lim);
	fail |= WriteI2C(HP_LEFT_VOLUME, (amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_lvol | HPL_EN | HP_TRACK));
	MM_INFO("HP : gain %x, volR:%x, volL:%x\n", amp_cal_data[ICODEC_FM_HEADSET_ST_RX].in1_gain, amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_rvol, amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_lvol);
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_hp_in1_fm\n");
}

static void tpa2055d3_power_up_hp_phone_in1(void)
{
	int fail = 0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[ICODEC_HEADSET_PHONE_RX].in1_gain | IN1_SE));
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_M60DB | HPL_EN | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, (amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_rvol | HPR_EN));
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_IN1 | HPLIM_EN | amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_lim);
	fail |= WriteI2C(HP_LEFT_VOLUME, (amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_lvol | HPL_EN | HP_TRACK));
	MM_INFO("HP : gain %x, volR:%x, volL:%x\n", amp_cal_data[ICODEC_HEADSET_PHONE_RX].in1_gain, amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_rvol, amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_lvol);
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_hp_phone_in1\n");
}
#if 0
static void tpa2055d3_power_up_in2(void)
{
	int fail = 0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (IN2GAIN_0DB | IN2_SE));
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_M60DB | HPL_EN | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, (HPR_VOL_M60DB | HPR_EN));
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_IN2);
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_0DB | HPL_EN | HP_TRACK));
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_in2\n");
}
#endif
#if 0
static void tpa2055d3_power_up_hp_in1in2(void)
{
	int fail = 0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (IN1GAIN_0DB | IN2GAIN_0DB | IN1_SE | IN2_SE));
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_M60DB | HPL_EN | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, (HPR_VOL_M60DB | HPR_EN));
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_IN1IN2);
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_0DB | HPL_EN | HP_TRACK));
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_hp_in1in2\n");
}
#endif
static void tpa2055d3_power_up_in1in2(void)
{
	int fail = 0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[ICODEC_HEADSET_SPK_RX].in1_gain | IN1_SE)|(amp_cal_data[ICODEC_HEADSET_SPK_RX].in2_gain | IN2_DIFF));
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_M60DB | HPL_EN | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, (amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_rvol | HPR_EN));
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_IN1 | HPLIM_EN | amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_lim);
	fail |= WriteI2C(HP_LEFT_VOLUME, (amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_lvol | HPL_EN | HP_TRACK));
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | amp_cal_data[ICODEC_HEADSET_SPK_RX].spk_vol));
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_IN2 | SPLIM_EN | amp_cal_data[ICODEC_HEADSET_SPK_RX].spk_lim);

	msleep(10);
	MM_INFO("MIX : HP - gain %x, volR %x, volL %x : SPK - gain %x, vol %x\n", amp_cal_data[ICODEC_HEADSET_SPK_RX].in1_gain, amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_rvol, amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_lvol, amp_cal_data[ICODEC_HEADSET_SPK_RX].in2_gain, amp_cal_data[ICODEC_HEADSET_SPK_RX].spk_vol);
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_up_in1in2\n");
}

static void tpa2055d3_power_down_hp(void)
{
	int fail = 0;
	fail |= WriteI2C(HP_LEFT_VOLUME, HPL_EN | HP_TRACK | HPL_VOL_M60DB);
	fail |= WriteI2C(HP_LEFT_VOLUME, HPL_VOL_M60DB);
	fail |= WriteI2C(HP_RIGHT_VOLUME, HPR_VOL_M60DB);
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_MUTE);
	if (fail != 0)
		MM_ERR("I2C error : tpa2055d3_power_down_hp\n");
}

/* -------- LGE AMP control API  ---------*/
static void tpa2055_amp_spk_stereo_on(void)
{
	fm_radio_enabled = FM_OFF;
	tpa2055d3_power_up_class_d_in2();
}

static void tpa2055_amp_spk_phone_on(void)
{
	fm_radio_enabled = FM_OFF;
	tpa2055d3_power_up_class_d_phone_in2();
}

static void tpa2055_amp_hp_stereo_on(void)
{
	fm_radio_enabled = FM_OFF;
	tpa2055d3_power_up_hp_in1();
}

static void tpa2055_amp_hp_stereo_spk_stereo_on(void)
{
	fm_radio_enabled = FM_OFF;
	tpa2055d3_power_up_in1in2();
}

static void tpa2055_amp_hp_phone_on(void)
{
	fm_radio_enabled = FM_OFF;
	tpa2055d3_power_up_hp_phone_in1();
}

static void tpa2055_amp_earpiece_on(void)
{
	fm_radio_enabled = FM_OFF;
	tpa2055d3_reset_to_defaults();
}

static void tpa2055_amp_spk_off(void)
{
	fm_radio_enabled = FM_OFF;
	tpa2055d3_power_down_class_d();
}

static void tpa2055_amp_off_all(void)
{
	fm_radio_enabled = FM_OFF;
	tpa2055d3_power_down_class_d();
	tpa2055d3_power_down_hp();
}

static void tpa2055_amp_fm_spk_stereo_on(void)
{
	fm_radio_enabled = FM_SPK_VOL;
	tpa2055d3_power_up_class_d_in2_fm();
}

static void tpa2055_amp_fm_hp_stereo_on(void)
{
	fm_radio_enabled = FM_HP_VOL;
	tpa2055d3_power_up_hp_in1_fm();
}

static void tpa2055_amp_set_fm_radio_volume(int index)
{
	int fail = 0;
	if(index <= 0) {
		amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_lvol = fm_radio_volume_table[0].hpvol;
		amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_rvol = fm_radio_volume_table[0].hpvol;
		amp_cal_data[ICODEC_FM_SPEAKER_RX].spk_vol = fm_radio_volume_table[0].spkvol;
	} else if (index >= FM_MAXIUM_VOL) {
		amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_lvol = fm_radio_volume_table[FM_MAXIUM_VOL].hpvol;
		amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_rvol = fm_radio_volume_table[FM_MAXIUM_VOL].hpvol;
		amp_cal_data[ICODEC_FM_SPEAKER_RX].spk_vol = fm_radio_volume_table[FM_MAXIUM_VOL].spkvol;
	} else {
		amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_lvol = fm_radio_volume_table[index].hpvol;
		amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_rvol = fm_radio_volume_table[index].hpvol;
		amp_cal_data[ICODEC_FM_SPEAKER_RX].spk_vol = fm_radio_volume_table[index].spkvol;
	}

	if (fm_radio_enabled == FM_SPK_VOL) {
		fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | amp_cal_data[ICODEC_FM_SPEAKER_RX].spk_vol));
	} else if (fm_radio_enabled == FM_HP_VOL) {
		fail |= WriteI2C(HP_RIGHT_VOLUME, (amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_rvol | HPR_EN));
		fail |= WriteI2C(HP_LEFT_VOLUME, (amp_cal_data[ICODEC_FM_HEADSET_ST_RX].hp_lvol | HPL_EN | HP_TRACK));
	}
	if (fail != 0)
		MM_ERR("I2C error : tpa2055_amp_set_fm_radio_volume\n");
}

static long tpa2055_amp_calibration(unsigned int cmd, unsigned long arg)
{
	int rc = 0;
	struct amp_cal amp_cal;
	mutex_lock(&amp_lock);

	switch (cmd) {
	case AMP_SET_DATA:
		if (copy_from_user(&amp_cal, (void __user *) arg, sizeof(amp_cal))) {
			MM_ERR("AMP_SET_DATA : invalid pointer\n");
			rc = -EFAULT;
			break;
		}
		switch (amp_cal.dev_type) {
		case ICODEC_HEADSET_SPK_RX:
			if (amp_cal.gain_type == IN1_GAIN)
				amp_cal_data[ICODEC_HEADSET_SPK_RX].in1_gain = amp_cal.data;
			else if (amp_cal.gain_type == IN2_GAIN)
				amp_cal_data[ICODEC_HEADSET_SPK_RX].in2_gain = amp_cal.data;
			else if (amp_cal.gain_type == SPK_VOL)
				amp_cal_data[ICODEC_HEADSET_SPK_RX].spk_vol = amp_cal.data;
			else if (amp_cal.gain_type == HP_LVOL)
				amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_lvol = amp_cal.data;
			else if (amp_cal.gain_type == HP_RVOL)
				amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_rvol = amp_cal.data;
			else if (amp_cal.gain_type == SPK_LIM)
				amp_cal_data[ICODEC_HEADSET_SPK_RX].spk_lim = amp_cal.data;
			else if (amp_cal.gain_type == HP_LIM)
				amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_lim = amp_cal.data;
			else {
				MM_ERR("invalid set_gain_type for HP & SPK  %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_HEADSET_ST_RX:
			if (amp_cal.gain_type == IN1_GAIN)
				amp_cal_data[ICODEC_HEADSET_ST_RX].in1_gain = amp_cal.data;
			else if (amp_cal.gain_type == HP_LVOL)
				amp_cal_data[ICODEC_HEADSET_ST_RX].hp_lvol = amp_cal.data;
			else if (amp_cal.gain_type == HP_RVOL)
				amp_cal_data[ICODEC_HEADSET_ST_RX].hp_rvol = amp_cal.data;
			else if (amp_cal.gain_type == HP_LIM)
				amp_cal_data[ICODEC_HEADSET_ST_RX].hp_lim = amp_cal.data;
			else {
				MM_ERR("invalid set_gain_type for HP %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_SPEAKER_RX:
			if (amp_cal.gain_type == IN2_GAIN)
				amp_cal_data[ICODEC_SPEAKER_RX].in2_gain = amp_cal.data;
			else if (amp_cal.gain_type == SPK_VOL)
				amp_cal_data[ICODEC_SPEAKER_RX].spk_vol = amp_cal.data;
			else if (amp_cal.gain_type == SPK_LIM)
				amp_cal_data[ICODEC_SPEAKER_RX].spk_lim = amp_cal.data;
			else {
				MM_ERR("invalid set_gain_type for SPK %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_HEADSET_PHONE_RX:
			if (amp_cal.gain_type == IN1_GAIN)
				amp_cal_data[ICODEC_HEADSET_PHONE_RX].in1_gain = amp_cal.data;
			else if (amp_cal.gain_type == HP_LVOL)
				amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_lvol = amp_cal.data;
			else if (amp_cal.gain_type == HP_RVOL)
				amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_rvol = amp_cal.data;
			else if (amp_cal.gain_type == HP_LIM)
				amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_lim = amp_cal.data;
			else {
				MM_ERR("invalid set_gain_type for HP_PHONE %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_SPEAKER_PHONE_RX:
			if (amp_cal.gain_type == IN2_GAIN)
				amp_cal_data[ICODEC_SPEAKER_PHONE_RX].in2_gain = amp_cal.data;
			else if (amp_cal.gain_type == SPK_VOL)
				amp_cal_data[ICODEC_SPEAKER_PHONE_RX].spk_vol = amp_cal.data;
			else if (amp_cal.gain_type == SPK_LIM)
				amp_cal_data[ICODEC_SPEAKER_PHONE_RX].spk_lim = amp_cal.data;
			else {
				MM_ERR("invalid set_gain_type for SPK_PHONE %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		default:
			MM_ERR("unknown dev type for setdata %d\n", amp_cal.dev_type);
			rc = -EFAULT;
			break;
		}
		break;
	case AMP_GET_DATA:
		if (copy_from_user(&amp_cal, (void __user *) arg, sizeof(amp_cal))) {
			MM_ERR("AMP_GET_DATA : invalid pointer\n");
			rc = -EFAULT;
			break;
		}
		switch (amp_cal.dev_type) {
		case ICODEC_HEADSET_SPK_RX:
			if (amp_cal.gain_type == IN1_GAIN)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_SPK_RX].in1_gain;
			else if (amp_cal.gain_type == IN2_GAIN)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_SPK_RX].in2_gain;
			else if (amp_cal.gain_type == SPK_VOL)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_SPK_RX].spk_vol;
			else if (amp_cal.gain_type == HP_LVOL)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_lvol;
			else if (amp_cal.gain_type == HP_RVOL)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_rvol;
			else if (amp_cal.gain_type == SPK_LIM)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_SPK_RX].spk_lim;
			else if (amp_cal.gain_type == HP_LIM)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_SPK_RX].hp_lim;
			else {
				MM_ERR("invalid get_gain_type for HP & SPK %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_HEADSET_ST_RX:
			if (amp_cal.gain_type == IN1_GAIN)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_ST_RX].in1_gain;
			else if (amp_cal.gain_type == HP_LVOL)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_ST_RX].hp_lvol;
			else if (amp_cal.gain_type == HP_RVOL)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_ST_RX].hp_rvol;
			else if (amp_cal.gain_type == HP_LIM)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_ST_RX].hp_lim;
			else {
				MM_ERR("invalid get_gain_type for HP %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_SPEAKER_RX:
			if (amp_cal.gain_type == IN2_GAIN)
				amp_cal.data = amp_cal_data[ICODEC_SPEAKER_RX].in2_gain;
			else if (amp_cal.gain_type == SPK_VOL)
				amp_cal.data = amp_cal_data[ICODEC_SPEAKER_RX].spk_vol;
			else if (amp_cal.gain_type == SPK_LIM)
				amp_cal.data = amp_cal_data[ICODEC_SPEAKER_RX].spk_lim;
			else {
				MM_ERR("invalid get_gain_type for SPK %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_HEADSET_PHONE_RX:
			if (amp_cal.gain_type == IN1_GAIN)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_PHONE_RX].in1_gain;
			else if (amp_cal.gain_type == HP_LVOL)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_lvol;
			else if (amp_cal.gain_type == HP_RVOL)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_rvol;
			else if (amp_cal.gain_type == HP_LIM)
				amp_cal.data = amp_cal_data[ICODEC_HEADSET_PHONE_RX].hp_lim;
			else {
				MM_ERR("invalid get_gain_type for HP_PHONE %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_SPEAKER_PHONE_RX:
			if (amp_cal.gain_type == IN2_GAIN)
				amp_cal.data = amp_cal_data[ICODEC_SPEAKER_PHONE_RX].in2_gain;
			else if (amp_cal.gain_type == SPK_VOL)
				amp_cal.data = amp_cal_data[ICODEC_SPEAKER_PHONE_RX].spk_vol;
			else if (amp_cal.gain_type == SPK_LIM)
				amp_cal.data = amp_cal_data[ICODEC_SPEAKER_PHONE_RX].spk_lim;
			else {
				MM_ERR("invalid get_gain_type for SPK_PHONE %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_FM_RADIO_VOL_CTL:
			tpa2055_amp_set_fm_radio_volume(amp_cal.data);
			break;
		case ICODEC_TURN_AMP_OFF:
			tpa2055_amp_off_all();
			break;
		default:
			MM_ERR("unknown dev type for getdata %d\n", amp_cal.dev_type);
			rc = -EFAULT;
			break;
		}
		MM_ERR("AMP_GET_DATA :dev %d, gain %d, data %d \n", amp_cal.dev_type, amp_cal.gain_type, amp_cal.data);
		if (copy_to_user((void __user *)arg, &amp_cal, sizeof(amp_cal))) {
			MM_ERR("AMP_GET_DATA : invalid pointer\n");
			rc = -EFAULT;
		}
		break;
	default:
		MM_ERR("unknown command\n");
		rc = -EINVAL;
		break;
	}
	mutex_unlock(&amp_lock);
	return rc;
}

void tpa2055_amp_init(struct amp_data *pdata)
{
	_data = pdata;
	pdata->amp_spk_stereo_on = tpa2055_amp_spk_stereo_on;
	pdata->amp_spk_phone_on = tpa2055_amp_spk_phone_on;
	pdata->amp_hp_stereo_on = tpa2055_amp_hp_stereo_on;
	pdata->amp_hp_stereo_spk_stereo_on = tpa2055_amp_hp_stereo_spk_stereo_on;
	pdata->amp_hp_phone_on = tpa2055_amp_hp_phone_on;
	pdata->amp_earpiece_on = tpa2055_amp_earpiece_on;
	pdata->amp_spk_off = tpa2055_amp_spk_off;
	pdata->amp_off_all = tpa2055_amp_off_all;
	pdata->amp_fm_spk_stereo_on = tpa2055_amp_fm_spk_stereo_on;
	pdata->amp_fm_hp_stereo_on = tpa2055_amp_fm_hp_stereo_on;
	pdata->amp_calibration = tpa2055_amp_calibration;

	mutex_init(&amp_lock);
}

