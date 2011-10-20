/* arch/arm/mach-msm/qdsp5v2/lge_audio_amp.h
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

/*Revision 0.1 -- BJB -- 3/18/10 -- Original Version */
/*Revision 0.2 -- BJB -- 3/19/10 -- Corrected dB to DB in Register 0x01, Added function prototypes.*/
/*Revision 0.3 -- AME -- 5/17/10 -- No changes to TPA2055D3_FDK.h file.*/

#include <linux/i2c.h>

/*  Audio Devices for Subsystem */
#define ICODEC_HEADSET_ST_RX	0
#define ICODEC_HEADSET_SPK_RX	1
#define ICODEC_SPEAKER_RX		2
#define ICODEC_HEADSET_PHONE_RX	3
#define ICODEC_SPEAKER_PHONE_RX	4
#define ICODEC_FM_HEADSET_ST_RX	5
#define ICODEC_FM_SPEAKER_RX	6
#define ICODEC_FM_RADIO_VOL_CTL	7
#define ICODEC_TURN_AMP_OFF		8
#define ICODEC_FM_HEADSET_ON		9
#define ICODEC_FM_SPEAKER_ON		10

#define	DEBUG_AMP_CTL	1
#define AMP_IOCTL_MAGIC 't'

#define AMP_SET_DATA	_IOW(AMP_IOCTL_MAGIC, 0, struct amp_cal *)
#define AMP_GET_DATA	_IOW(AMP_IOCTL_MAGIC, 1, struct amp_cal *)

/*  Call Mode  */
#define IN1_GAIN 0
#define IN2_GAIN 1
#define SPK_VOL  2
#define HP_LVOL  3
#define HP_RVOL	 4
#define SPK_LIM 5
#define HP_LIM 6

/* -------- LGE AMP control API  ---------*/
void lge_amp_spk_stereo_on(void);
void lge_amp_spk_phone_on(void);
void lge_amp_hp_stereo_on(void);
void lge_amp_hp_stereo_spk_stereo_on(void);
void lge_amp_hp_phone_on(void);
void lge_amp_earpiece_on(void);
void lge_amp_spk_off(void);
void lge_amp_off_all(void);
void lge_amp_hs_call_mode(void);
void lge_amp_hs_lpm_mode(void);
void lge_amp_fm_spk_stereo_on(void);
void lge_amp_fm_hp_stereo_on(void);

struct amp_data {
	struct i2c_client *client;
	void (*amp_spk_stereo_on)(void);
	void (*amp_spk_phone_on)(void);
	void (*amp_hp_stereo_on)(void);
	void (*amp_hp_stereo_spk_stereo_on)(void);
	void (*amp_hp_phone_on)(void);
	void (*amp_earpiece_on)(void);
	void (*amp_spk_off)(void);
	void (*amp_off_all)(void);
	void (*amp_fm_spk_stereo_on)(void);
	void (*amp_fm_hp_stereo_on)(void);
	long (*amp_calibration)(unsigned int , unsigned long);
};

struct amp_cal {
	int dev_type;
	int gain_type;
	int data;
};

struct fm_radio_vol {
	u8 hpvol;
	u8 spkvol;
};
	
void tpa2055_amp_init(struct amp_data *pdata);
void wm9093_amp_init (struct amp_data *pdata);

