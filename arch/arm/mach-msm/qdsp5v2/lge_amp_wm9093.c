/* arch/arm/mach-msm/qdsp5v2/lge_amp_wm9093.c
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
#include <mach/board_lge.h>
#include "lge_audio_amp.h"

#define WM9093_REMOVE_POPUP_NOISE	/* HP 1.8V always enable */
#define WM9093_DEBUG

#define FM_HP_VOL 0
#define FM_SPK_VOL 1
#define FM_OFF 2

#define FM_MAXIUM_VOL 15

enum {
	WM9093_CAL_INPUTA =0,
	WM9093_CAL_INPUTB,
	WM9093_CAL_SPK_AMP_GAIN,
	WM9039_CAL_SPK_BOOST,
	WM9093_CAL_SPK_VOL,
	WM9093_CAL_HP_VOL,
};

struct wm9093_reg_type
{
  u8  reg;
  u8  val[2];
};

struct wm9093_param_type
{
	unsigned char in1_inputa;	// Speaker Amp Gain
	unsigned char in1_inputb;	// Speaker Amp Gain

	unsigned char sp_amp_gain;	// Speaker Amp Gain
	unsigned char sp_boost;			// Speaker boost
	unsigned char sp_vol;
	unsigned char hp_vol;
	unsigned char in1a_spkmix_vol;  //IN1A_SPKMIX_VOL
	unsigned char in1b_spkmix_vol;  //IN1B_SPKMIX_VOL
	unsigned char in2a_spkmix_vol;  //IN2A_SPKMIX_VOL
	unsigned char in2b_spkmix_vol;  //IN2B_SPKMIX_VOL
	unsigned char agc_enable;		//AGC_ENABLE
	unsigned char agc_clip_thr;  	//AGC Clip Threshold
	unsigned char agc_clip_atk;  	//AGC Clip attack
	unsigned char agc_clip_dcy;  	//AGC Clip dcy
	unsigned char agc_ramp;  	//AGC Clip dcy
	unsigned int  agc_min_gain;  	//Min_gain
};


/* Volume control. for Register 1Ch.1Dh.26h */
#define WM9093_AMP_VOLUME_UPDATE						0x0100

#define WM9093_AMP_ZC_GAIN_IMMEDIATE				0x0000
#define WM9093_AMP_ZC_GAIN_ZERO_CROSS_ONLY	0x0080

#define WM9093_AMP_MUTE_MUTE								0x0040
#define WM9093_AMP_MUTE_UNMUTE							0x0000

/* Speaker Mode and Boost Control.for Register 25h */
#define WM90903_AMP_SPEAKER_AMP_GAIN_0DB				0x00
#define WM90903_AMP_SPEAKER_AMP_GAIN_M12DB			0x80
#define WM90903_AMP_SPEAKER_BOOST_CONTROL_M12DB			0x38
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_00	0x00	/* 0 db */
/*#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_19	0x08
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_41	0x10
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_68	0x18
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_00	0x20
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_37	0x28
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_81	0x30
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_3_98	0x38 */
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_19	0x01	/* 1.5 db */
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_41	0x02	/* 3.0 db */
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_68	0x03	/* 4.5 db */
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_00	0x04	/* 6.0 db */
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_37	0x05	/* 7.5 db */
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_81	0x06	/* 9.0 db */
#define WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_3_98	0x07	/* 12.0 db */

/*====================================================================*/
/*				struct declaration
	In this structure, I considered that we need delay sometimes.
	So I added delay.
*/
typedef enum {
	WM9093_CMD,
	WM9093_DELAY,
	WM9093_SPEAKER_BOOST,
	WM9093_SPEAKER_AMP_CONTROL,
	WM9093_HEADSET_AMP_CONTROL,
	WM9093_IN1_INPUTA,
	WM9093_IN1_INPUTB,
	WM9093_SPK_MIX_VOL,
	WM9093_AGC_ENABLE,
	WM9093_AGC_CONTROL0,
	WM9093_AGC_CONTROL2,
	WM9093_END_SEQ
} wm9093_control_type;


struct wm9093_data
{
	wm9093_control_type wm_type;
	u8			  addr;
	u8			  data1;
	u8			  data2;
};

static u8 CurFmRadioVol = 11;
static int CurCodecType = ICODEC_TURN_AMP_OFF;
static u8 fm_radio_enabled = FM_OFF;

/*########################################################################
**########################################################################
**
**  Local function prototypes
**
**########################################################################
**########################################################################*/

/*	IN1 Line Input A Volume
	IN1 Line Input B Volume
	Speaker Amp Gain
	Speaker boost,
	SP Vol
	HP Vol
	in1a_spkmix_vol
	in1b_spkmix_vol
	in1b_spkmix_vol
	in2b_spkmix_vol
	agc_enable
	agc_clip_threshold
	agc_clip_attack
	agc_clip_dcy*/
struct wm9093_param_type wm9093_param_table[] = {
#if defined (CONFIG_LGE_DOMESTIC)
/* ICODEC_HEADSET_ST_RX */
	{	0x00,	//0x02
		0x00,	//0x02
		WM90903_AMP_SPEAKER_AMP_GAIN_0DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_00,
		0x00,
		56,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_HEADSET_SPK_RX */
	{	0x03,//0x02
		0x03,	//0x02
		WM90903_AMP_SPEAKER_AMP_GAIN_0DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_81, //WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_3_98,
		57,
		43,//51,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_SPEAKER_RX */
	{	0x03,	//0x02	//0x03,
		0x03,	//0x02
		WM90903_AMP_SPEAKER_AMP_GAIN_0DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_81,	//WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_37 ,
		57,	//61,
		0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_HEADSET_PHONE_RX */
	{	0x02, //0x00,
		0x02,	//0x00
		WM90903_AMP_SPEAKER_AMP_GAIN_M12DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_00,
		0x00,
		49,//59,//57
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_SPEAKER_PHONE_RX */
	{	0x05,	//0x05,
		0x05,	//0x02, //0x05,
		WM90903_AMP_SPEAKER_AMP_GAIN_0DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_00,//WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_3_98
		56,//57, //63,
		0x00,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_FM_HEADSET_ST_RX */
	{	0x07,//07
		0x07,
		WM90903_AMP_SPEAKER_AMP_GAIN_M12DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_00,
		0x00, 
		0x39,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_FM_SPEAKER_RX */
	{	0x07,
		0x07,
		WM90903_AMP_SPEAKER_AMP_GAIN_0DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_3_98,
		0x00,
		0x39,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#else /*~CONFIG_LGE_DOMESTIC*/
/* ICODEC_HEADSET_ST_RX */
	{	0x02,
		0x02,
		WM90903_AMP_SPEAKER_AMP_GAIN_0DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_00,
		0x00,
		54,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_HEADSET_SPK_RX */
	{	0x04,
		0x04,
		WM90903_AMP_SPEAKER_AMP_GAIN_0DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_3_98,
		60,
		47,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_SPEAKER_RX */
	{	0x01,
		0x01,
		WM90903_AMP_SPEAKER_AMP_GAIN_0DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_3_98 ,
		59,
		0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_HEADSET_PHONE_RX */
	{	0x01,
		0x01,
		WM90903_AMP_SPEAKER_AMP_GAIN_M12DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_00,
		0x00,
		59,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_SPEAKER_PHONE_RX */
	{	0x03,
		0x03,
		WM90903_AMP_SPEAKER_AMP_GAIN_0DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_2_37,
		61,
		0x00,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_FM_HEADSET_ST_RX */
	{	0x02,//07
		0x02,
		WM90903_AMP_SPEAKER_AMP_GAIN_M12DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_1_00,
		0x00, 
		0x39,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/* ICODEC_FM_SPEAKER_RX */
	{	0x01,
		0x01,
		WM90903_AMP_SPEAKER_AMP_GAIN_0DB,
		WM9093_AMP_SPEAKER_OUT_GAIN_BOOSTT_3_98,
		0x00,
		0x39,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#endif /*CONFIG_LGE_DOMESTIC*/
};

struct wm9093_data wm9093_handset[] =
{
		/* control_type,  address,   data1(delay),	data2 */
		{WM9093_CMD,	0x00, 	0x90, 	0x93},
		{WM9093_CMD, 						0x39, 	0x00, 		0x0D},
		{WM9093_DELAY,  				0x00,   50, 		0x00},
		{WM9093_CMD, 						0x01, 	0x00, 		0x0B},
		{WM9093_CMD, 						0x22, 	0x11, 		0x00},
		{WM9093_CMD, 						0x24, 	0x00, 		0x20},
		{WM9093_CMD, 						0x15, 	0x00, 		0x00},
		{WM9093_SPEAKER_BOOST, 	0x25, 	0x01, 		0xC0},
		{WM9093_CMD, 						0x01, 	0x10, 		0x0B},
		{WM9093_END_SEQ,				0x00,	0x00,		0x00}
};

struct wm9093_data wm9093_headset_speaker[] =
{
		{WM9093_CMD,	0x00, 	0x90, 	0x93},
		{WM9093_CMD,	0x39,	0x00,		0x0D},
		{WM9093_DELAY,	0x00,	50, 		0x00},
		{WM9093_CMD,	0x01,	0x00,		0x0B},
		{WM9093_CMD,	0x02,	0x60,		0x20},
		{WM9093_CMD,	0x17,	0x00,		0x03},
		{WM9093_IN1_INPUTA,	0x1A,	0x01,		0x02},
		{WM9093_CMD,	0x36,	0x00,		0x04},
		{WM9093_CMD,	0x2D,	0x00,		0x04},
		{WM9093_CMD,	0x2E,	0x00,		0x04},
		{WM9093_AGC_ENABLE,	0x03,	0x00,		0x38},
		{WM9093_AGC_CONTROL0,	0x62,	0x80,		0x00},
		{WM9093_AGC_CONTROL2,	0x64,	0x02,		0x00},
		{WM9093_SPK_MIX_VOL,	0x22,	0x00, 		0x05},
		{WM9093_SPEAKER_BOOST,	0x25,	0x01,		0x60},
		{WM9093_SPEAKER_AMP_CONTROL,	0x26,	0x01,		0x39},
		{WM9093_CMD,	0x03,	0x01,		0x38},
		{WM9093_CMD,	0x2F,	0x00,		0x00},
		{WM9093_CMD,	0x30,	0x00,		0x00},
		{WM9093_CMD,	0x25,	0x01,		0x38},
		{WM9093_HEADSET_AMP_CONTROL,	0x1C,	0x00,		0x39},
		{WM9093_HEADSET_AMP_CONTROL,	0x1D,	0x01,		0x39},
		{WM9093_CMD,	0x17,	0x00,		0x02},
		{WM9093_CMD,	0x01,	0x10,		0x0B},
		{WM9093_CMD,	0x46,	0x01,		0x00},
		{WM9093_CMD,	0x49,	0x01,		0x00},
		{WM9093_END_SEQ,0x00,	0x00,		0x00}
};

struct wm9093_data *wm9093_fm_radio_speaker_media;
struct wm9093_data wm9093_fm_radio_speaker_rev_d[] =
{
	/* control_type,  address,	data1(delay),	data2 */
	{WM9093_CMD,	0x00,	0x90,	0x93},
	{WM9093_CMD,	0x39,	0x00,		0x0D},
	{WM9093_DELAY,	0x00,	50, 		0x00},
	{WM9093_CMD,	0x01,	0x00,		0x0B},
	{WM9093_CMD,	0x02,	0x60,		0x20},
	{WM9093_IN1_INPUTA, 0x1A,	0x01,		0x02},
	{WM9093_CMD,	0x36,	0x00,		0x04},
	{WM9093_CMD,	0x22,	0x00,		0x50},
	{WM9093_CMD,	0x03,	0x01,		0x08},
	{WM9093_SPEAKER_BOOST,	0x25,	0x01,		0x60},
	{WM9093_SPEAKER_AMP_CONTROL,	0x26,	0x01,		0x3f},
	{WM9093_CMD,	0x17,	0x00,		0x02},
	{WM9093_CMD,	0x01,	0x10,		0x0B},
	{WM9093_END_SEQ,0x00,	0x00,		0x00}
};

struct wm9093_data wm9093_fm_radio_speaker_rev1_0[] =
{
		/* control_type,  address,	data1(delay),	data2 */
/*	{WM9093_CMD,	0x00, 	0x90, 	0x93},
	{WM9093_CMD,	0x39,	0x00,		0x0D},
	{WM9093_DELAY,	0x00,	50, 		0x00},
	{WM9093_CMD,	0x01,	0x00,		0x0B},
	{WM9093_CMD,	0x02,	0x60,		0x80},
	{WM9093_IN1_INPUTA,	0x18,	0x01,	0x02},
	{WM9093_CMD,	0x36,	0x00,		0x40},
	{WM9093_CMD,	0x03,	0x00,		0x08},
	{WM9093_CMD,	0x22,	0x00,		0x00},
	{WM9093_CMD,	0x03,	0x01,		0x08},
	{WM9093_SPEAKER_BOOST,	0x25,	0x01,		0x60},
	{WM9093_SPEAKER_AMP_CONTROL,	0x26,	0x01,		0x3f},
	{WM9093_CMD,	0x16,	0x00,		0x02},
	{WM9093_CMD,	0x01,	0x10,		0x0B},
	{WM9093_DELAY,	0x00,	56, 			0x00},
	{WM9093_END_SEQ,0x00,	0x00,		0x00}	*/	
	{WM9093_CMD,	0x00, 	0x90, 	0x93},
	{WM9093_CMD,	0x39,	0x00,		0x0D},
	{WM9093_DELAY,	0x00,	50, 		0x00},
	{WM9093_CMD,	0x01,	0x00,		0x0B},
	{WM9093_CMD,	0x02,	0x60,		0xC0},
	{WM9093_CMD,	0x16,	0x00,		0x01},	
	{WM9093_IN1_INPUTA,	0x18,	0x01,	0x00},
	{WM9093_IN1_INPUTB,	0x19,	0x01,	0x00},
	{WM9093_CMD,	0x36,	0x00,		0x50},
	{WM9093_CMD,	0x03,	0x00,		0x08},
	{WM9093_CMD,	0x22,	0x00,		0x50},
	{WM9093_CMD,	0x03,	0x01,		0x08},
	{WM9093_SPEAKER_BOOST,	0x25,	0x01,		0x60},
	{WM9093_SPEAKER_AMP_CONTROL,	0x26,	0x01,		0x3f},
	{WM9093_CMD,	0x16,	0x00,		0x00},
	{WM9093_CMD,	0x01,	0x10,		0x0B},
	{WM9093_DELAY,	0x00,	56, 			0x00},
	{WM9093_END_SEQ,0x00,	0x00,		0x00}		

};

struct wm9093_data wm9093_fm_radio_headset_media[] =
{
		/* control_type,  address,	data1(delay),	data2 */
		{WM9093_CMD,	0x00, 	0x90, 	0x93},
		{WM9093_CMD,	0x39,	0x00,		0x0D},
		{WM9093_DELAY,	0x00,	50, 		0x00},
		{WM9093_CMD,	0x01,	0x00,		0x0B},	// 0000 0000 0000 1011	// from wolfson
		{WM9093_CMD,	0x02,	0x60,		0xC0},	// 0110 0000 1100 0000		// from wolfson
		{WM9093_CMD,	0x16,	0x00,		0x01},	// 0000 0000 0000 0001		// from wolfson
		{WM9093_IN1_INPUTA,	0x18,	0x00,	0x03},	// 0000 0000 0000 0010	// from wolfson
		{WM9093_IN1_INPUTB,	0x19,	0x01,	0x03},	// 0000 0001 0000 0010	// from wolfson
		{WM9093_CMD,	0x2D,	0x00,		0x40},	// 0000 0000 0100 0000	// from wolfson
		{WM9093_CMD,	0x2E,	0x00,		0x10},	// 0000 0000 0001 0000	// from wolfson
		{WM9093_CMD,	0x03,	0x00,		0x30},	// 0000 0000 0011 0000	// from wolfson
		{WM9093_CMD,	0x2F,	0x00,		0x00},
		{WM9093_CMD,	0x30,	0x00,		0x00},
		{WM9093_CMD,	0x16,	0x00,		0x00},
		{WM9093_HEADSET_AMP_CONTROL,	0x1C,	0x00,		0x2d},
		{WM9093_HEADSET_AMP_CONTROL,	0x1D,	0x01,		0x2d},
		{WM9093_CMD,	0x46,	0x01,		0x00},
		{WM9093_CMD,	0x49,	0x01,		0x00},
		{WM9093_DELAY,	0x00,	56, 			0x00},	// wait 56ms until finising sequencer.
		{WM9093_END_SEQ,0x00,	0x00,		0x00}
};

struct wm9093_data wm9093_headset_mute[] =
{
		{WM9093_CMD,	0x1c,	0x00,		0x40},
		{WM9093_CMD,	0x1d,	0x00,		0x40}
};

struct wm9093_data wm9093_speaker_mute[] =
{
		{WM9093_CMD,	0x26,	0x00,		0x40}
};

struct wm9093_data wm9093_headset_speaker_mute[] =
{
		{WM9093_CMD,	0x1c,	0x00,		0x40},
		{WM9093_CMD,	0x1d,	0x00,		0x40},
		{WM9093_CMD,	0x26,	0x00,		0x40}

};


struct wm9093_data wm9093_stereo_headset[] =
{
		/* control_type,  address,	data1(delay),	data2 */
		{WM9093_CMD,	0x00, 	0x90, 	0x93},
		{WM9093_CMD,	0x39,	0x00,		0x0D},
		{WM9093_DELAY,	0x00,	50, 		0x00},
		{WM9093_CMD,	0x01,	0x00,		0x0B},	// 0000 0000 0000 1011	// from wolfson
		{WM9093_CMD,	0x02,	0x60,		0xC0},	// 0110 0000 1100 0000		// from wolfson
		{WM9093_CMD,	0x16,	0x00,		0x01},	// 0000 0000 0000 0001		// from wolfson
		{WM9093_IN1_INPUTA,	0x18,	0x01,	0x02},	// 0000 0000 0000 0010	// from wolfson
		{WM9093_IN1_INPUTB,	0x19,	0x01,	0x02},	// 0000 0001 0000 0010	// from wolfson
		{WM9093_CMD,	0x2D,	0x00,		0x40},	// 0000 0000 0100 0000	// from wolfson
		{WM9093_CMD,	0x2E,	0x00,		0x10},	// 0000 0000 0001 0000	// from wolfson
		{WM9093_AGC_ENABLE,	0x03,	0x00,		0x30},	// 0000 0000 0011 0000	// from wolfson
		{WM9093_AGC_CONTROL0,	0x62,	0x80,		0x00},
		{WM9093_AGC_CONTROL2,	0x64,	0x02,		0x00},
		{WM9093_CMD,	0x2F,	0x00,		0x00},
		{WM9093_CMD,	0x30,	0x00,		0x00},
		{WM9093_CMD,	0x16,	0x00,		0x00},
		{WM9093_HEADSET_AMP_CONTROL,	0x1C,	0x00,		0x2d},
		{WM9093_HEADSET_AMP_CONTROL,	0x1D,	0x01,		0x2d},
		{WM9093_CMD,	0x46,	0x01,		0x00},
		{WM9093_CMD,	0x49,	0x01,		0x00},
		{WM9093_DELAY,	0x00,	56, 			0x00},	// wait 56ms until finising sequencer.
		{WM9093_END_SEQ,0x00,	0x00,		0x00}
};

#if 0
struct wm9093_data wm9093_mono_headset[] =
{
		/* control_type,  address,	data1(delay),	data2 */
		{WM9093_CMD,	0x00, 	0x90, 	0x93},
		{WM9093_CMD,	0x39,	0x00,		0x0D},
		{WM9093_DELAY,	0x00,	50, 		0x00},
		{WM9093_CMD,	0x01,	0x00,		0x0B},	// 0000 0000 0000 1011	// from wolfson
		{WM9093_CMD,	0x02,	0x60,		0xC0},	// 0110 0000 1100 0000		// from wolfson
		{WM9093_CMD,	0x16,	0x00,		0x01},	// 0000 0000 0000 0001		// from wolfson
		{WM9093_IN1_INPUTA,	0x18,	0x01,	0x02},	// 0000 0000 0000 0010	// from wolfson
		{WM9093_IN1_INPUTB,	0x19,	0x01,	0x02},	// 0000 0001 0000 0010	// from wolfson
		{WM9093_CMD,	0x2D,	0x00,		0x40},	// 0000 0000 0100 0000	// from wolfson
		{WM9093_CMD,	0x2E,	0x00,		0x40},	// 0000 0000 0100 0000	// from wolfson
		{WM9093_AGC_ENABLE,	0x03,	0x00,		0x30},	// 0000 0000 0011 0000	// from wolfson
		{WM9093_AGC_CONTROL0,	0x62,	0x80,		0x00},
		{WM9093_AGC_CONTROL2,	0x64,	0x02,		0x00},
		{WM9093_CMD,	0x2F,	0x00,		0x00},
		{WM9093_CMD,	0x30,	0x00,		0x00},
		{WM9093_CMD,	0x16,	0x00,		0x02},
		{WM9093_HEADSET_AMP_CONTROL,	0x1C,	0x00,		0x2d},
		{WM9093_HEADSET_AMP_CONTROL,	0x1D,	0x01,		0x2d},
		{WM9093_CMD,	0x46,	0x01,		0x00},
		{WM9093_CMD,	0x49,	0x01,		0x00},
		{WM9093_DELAY,	0x00,	56, 			0x00},	// wait 56ms until finising sequencer.
		{WM9093_END_SEQ,0x00,	0x00,		0x00}
};
#endif

struct wm9093_data wm9093_mono_speaker[] =
{
		/* control_type,  address,	data1(delay),	data2 */
		{WM9093_CMD,	0x00, 	0x90, 	0x93},
		{WM9093_CMD,	0x39,	0x00,		0x0D},
		{WM9093_DELAY,	0x00,	50, 		0x00},
		{WM9093_CMD,	0x01,	0x00,		0x0B},
		{WM9093_CMD,	0x02,	0x60,		0x20},
		{WM9093_IN1_INPUTA,	0x1A,	0x01,		0x02},
		{WM9093_CMD,	0x36,	0x00,		0x04},
		{WM9093_SPK_MIX_VOL,	0x22,	0x00,		0x00},
		{WM9093_AGC_ENABLE,	0x03,	0x01,		0x08},
		{WM9093_AGC_CONTROL0,	0x62,	0x80,		0x00},
		{WM9093_AGC_CONTROL2,	0x64,	0x02,		0x00},
		{WM9093_SPEAKER_BOOST,	0x25,	0x01,		0x60},
		{WM9093_SPEAKER_AMP_CONTROL,	0x26,	0x01,		0x3f},
		{WM9093_CMD,	0x17,	0x00,		0x02},
		{WM9093_CMD,	0x01,	0x10,		0x0B},
		{WM9093_END_SEQ,0x00,	0x00,		0x00}
};

struct wm9093_data wm9093_stereo_speaker[] =
{
		/* control_type,  address,	data1(delay),	data2 */
		{WM9093_CMD,	0x00, 	0x90, 	0x93},
		{WM9093_CMD,	0x39,	0x00,		0x0D},
		{WM9093_DELAY,	0x00,	50, 		0x00},
		{WM9093_CMD,	0x01,	0x00,		0x0B},	// 0000 0000 0000 1011	// from wolfson
		{WM9093_CMD,	0x02,	0x60,		0xC0},	// 0110 0000 1100 0000		// from wolfson
		{WM9093_IN1_INPUTA, 0x18,	0x01,	0x05},	// 0000 0000 0000 0010	// from wolfson
		{WM9093_CMD,	0x36,	0x00,		0x50},
		{WM9093_SPK_MIX_VOL,	0x22,	0x00,		0x00},
		{WM9093_AGC_ENABLE,	0x03,	0x01,		0x08},
		{WM9093_AGC_CONTROL0,	0x62,	0x80,		0x00},
		{WM9093_AGC_CONTROL2,	0x64,	0x02,		0x00},
		{WM9093_SPEAKER_BOOST,	0x25,	0x01,		0x60},
		{WM9093_SPEAKER_AMP_CONTROL,	0x26,	0x00,		0x9f},
		{WM9093_CMD,	0x16,	0x00,		0x01},
		{WM9093_CMD,	0x01,	0x10,		0x0B},
		{WM9093_END_SEQ,0x00,	0x00,		0x00}
};


struct wm9093_data wm9093_power_down[] =
{
		/* control_type,  address,	data1(delay),	data2 */
		{WM9093_CMD,	0x00,	0x00,		0x00},
		{WM9093_CMD,	0x39,	0x00,		0x00},
		{WM9093_END_SEQ,0x00,	0x00,		0x00}
};

struct wm9093_data wm9093_headset_power_down[] =
{
		{WM9093_CMD,	0x01,	0x00,	0x0B},
		{WM9093_DELAY,	0x00,	50, 		0x00},
		{WM9093_CMD,	0x39,	0x00,	0x0D},
		{WM9093_CMD,	0x49,	0x01,	0x10},
		{WM9093_DELAY,	0x00,	3, 			0x00},	// wait 56ms until finising sequencer.
		{WM9093_END_SEQ,0x00,	0x00,		0x00}
};

struct wm9093_data wm9093_headset_power_on[] =
{
		{WM9093_CMD,	0x01,	0x00,	0x0B},
		{WM9093_DELAY,	0x00,	50, 		0x00},
		{WM9093_CMD,	0x39,	0x00,	0x0D},
		{WM9093_CMD,	0x49,	0x01,	0x00},
		{WM9093_DELAY,	0x00,	3, 			0x00},	// wait 56ms until finising sequencer.
		{WM9093_END_SEQ,0x00,	0x00,		0x00}
};

struct ST_fm_headset_volume_tbl	{
	u8 data1;
	u8 data2;
};

struct ST_fm_headset_volume_tbl fm_headset_volume_tbl[] =	{
#if 0
         {0x01, 0}       // 0
        ,{0x01, 4}      // 1
        ,{0x01, 8}      // 2
        ,{0x01, 12}      // 3
        ,{0x01, 16}      // 4
        ,{0x01, 20}      // 5
        ,{0x01, 23}      // 6
        ,{0x01, 26}      // 7
        ,{0x01, 29}      // 8
        ,{0x01, 32}      // 9
        ,{0x01, 35}      // 10
        ,{0x01, 38}      // 11 => default
        ,{0x01, 41}      // 12
        ,{0x01, 44}      // 13
        ,{0x01, 47}      //14
        ,{0x01, 50}      //15
        ,{0x01, 53}      //16
        ,{0x01, 55}      //17
        ,{0x01, 57}      //18
        ,{0x01, 59}      //19
        ,{0x01, 61}      //20
#else
		{0x01, 0}       // 0
        ,{0x01, 4}      // 1
        ,{0x01, 8}      // 2
        ,{0x01, 12}      // 3
        ,{0x01, 16}      // 4
        ,{0x01, 20}      // 5
        ,{0x01, 23}      // 6
        ,{0x01, 26}      // 7
        ,{0x01, 29}      // 8
        ,{0x01, 32}      // 9
        ,{0x01, 35}      // 10
        ,{0x01, 38}      // 11 => default
        ,{0x01, 41}      // 12
        ,{0x01, 44}      // 13
        ,{0x01, 47}      //14
        ,{0x01, 50}      //15
#endif
};

struct ST_fm_headset_volume_tbl fm_speaker_volume_tbl[] =	{
#if 0
	 {0x01, 0}		 // 0
	,{0x01, 7}		// 1
	,{0x01, 12}		// 2
	,{0x01, 17}		// 3
	,{0x01, 22}		// 4
	,{0x01, 27} 	 // 5
	,{0x01, 30} 	 // 6
	,{0x01, 33} 	 // 7
	,{0x01, 36} 	 // 8
	,{0x01, 39} 	 // 9
	,{0x01, 42} 	 // 10
	,{0x01, 45} 	 // 11 => default
	,{0x01, 47} 	 // 12
	,{0x01, 49} 	 // 13
	,{0x01, 51} 	 //14
	,{0x01, 53} 	 //15
	,{0x01, 55} 	 //16
	,{0x01, 57} 	 //17
	,{0x01, 59} 	 //18
	,{0x01, 61} 	 //19
	,{0x01, 63} 	 //20
#else
	{0x01, 0}		 // 0
	,{0x01, 7}		// 1
	,{0x01, 12}		// 2
	,{0x01, 17}		// 3
	,{0x01, 21}		// 4
	,{0x01, 24} 	 // 5
	,{0x01, 27} 	 // 6
	,{0x01, 31} 	 // 7
	,{0x01, 35} 	 // 8
	,{0x01, 39} 	 // 9
	,{0x01, 43} 	 // 10
	,{0x01, 47} 	 // 11 => default
	,{0x01, 51} 	 // 12
	,{0x01, 55} 	 // 13
	,{0x01, 59} 	 //14
	,{0x01, 63} 	 //15
#endif
};

static struct mutex amp_lock;
static struct amp_data *_data = NULL;

static int amp_read_register(u8 reg, int* ret)
{
	struct i2c_msg	xfer[2];
	u16				data = 0xffff;
	u16				retval;


	xfer[0].addr = _data->client->addr;
	xfer[1].flags = 0;
	xfer[0].len  = 1;
	xfer[0].buf = &reg;

	xfer[1].addr = _data->client->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = 2;
	xfer[1].buf = (u8*)&data;

	retval = i2c_transfer(_data->client->adapter, xfer, 2);

	*ret =  (data>>8) | ((data & 0xff) << 8);

	return retval;
}

static int amp_write_register(u8 reg, u8 *value)
{
	int				 err;
	unsigned char    buf[3];
	struct i2c_msg	msg = { _data->client->addr, 0, 3, &buf[0] };

	buf[0] = reg;
	buf[1] = value[0];
	buf[2] = value[1];

	if ((err = i2c_transfer(_data->client->adapter, &msg, 1)) < 0){
		return -EIO;
	}
	else {
		return 0;
	}
}

#if 0
static void wm9093_power_ctl(int on)
{
}
#endif

void wm9093_setConfig2(int codec,const struct wm9093_data* data_table)
{
	int i;
	int ret;
	u8 reg;
	unsigned char val[2];
	unsigned char codec_idx_wm9093 = 0;

	CurCodecType = codec;
	codec_idx_wm9093 = CurCodecType;

	MM_INFO("wm9093_setConfig2[%d] processing start!!!", codec);
	for (i = 0; data_table[i].wm_type != WM9093_END_SEQ; i++)
	{
		MM_INFO("wm9093_setConfig2[%d] processing idx[%d]", codec, i);
		switch(data_table[i].wm_type)
		{
			case  WM9093_DELAY:
				msleep(data_table[i].data1);
				break;

			case WM9093_SPEAKER_BOOST:
				reg = data_table[i].addr;
				val[0] = data_table[i].data1;

				if ( wm9093_param_table[codec_idx_wm9093].sp_amp_gain )
					val[1] = data_table[i].data2 | WM90903_AMP_SPEAKER_AMP_GAIN_M12DB;		// 0x80
				else
					val[1] = data_table[i].data2 & ~(WM90903_AMP_SPEAKER_AMP_GAIN_M12DB);	// 0x80

				val[1] &= ~(WM90903_AMP_SPEAKER_BOOST_CONTROL_M12DB);
				val[1] |=wm9093_param_table[codec_idx_wm9093].sp_boost<<3;
				ret = amp_write_register(reg, val);
				if (ret < 0)
					MM_ERR("wm9093_setConfig2\n");
				break;

			case WM9093_SPEAKER_AMP_CONTROL:
				reg = data_table[i].addr;
				val[0] = data_table[i].data1;
				val[1] = data_table[i].data2 & ~((unsigned char) 0x3f);			// 0011 1111
				if ((codec == ICODEC_FM_HEADSET_ST_RX ||
					codec == ICODEC_FM_SPEAKER_RX))
					val[1] |= fm_speaker_volume_tbl[CurFmRadioVol].data2;
				else
					val[1] |= wm9093_param_table[codec_idx_wm9093].sp_vol;

				ret = amp_write_register(reg, val);
				if (ret < 0)
					MM_ERR("wm9093_setConfig2\n");
				break;

			case WM9093_HEADSET_AMP_CONTROL:
				reg = data_table[i].addr;
				val[0] = data_table[i].data1;
				val[1] = data_table[i].data2 & ~((unsigned char) 0x3f);			// 0011 1111

				if ((codec == ICODEC_FM_HEADSET_ST_RX ||
					codec == ICODEC_FM_SPEAKER_RX))
					val[1] |= fm_headset_volume_tbl[CurFmRadioVol].data2;
				else
					val[1] |= wm9093_param_table[codec_idx_wm9093].hp_vol;

				ret = amp_write_register(reg, val);
				if (ret < 0)
					MM_ERR("wm9093_setConfig2\n");
				break;

			case WM9093_IN1_INPUTA:
				reg = data_table[i].addr;
				val[0] = data_table[i].data1;
				val[1] = data_table[i].data2 & ~((unsigned char) 0x1f);			// 0001 1111
				val[1]  |= wm9093_param_table[codec_idx_wm9093].in1_inputa;
				ret = amp_write_register(reg, val);
				if (ret < 0)
					MM_ERR("wm9093_setConfig2\n");
				break;

			case WM9093_IN1_INPUTB:
				reg = data_table[i].addr;
				val[0] = data_table[i].data1;
				val[1] = data_table[i].data2 & ~((unsigned char) 0x1f);			// 0001 1111
				val[1] |= wm9093_param_table[codec_idx_wm9093].in1_inputb;
				ret = amp_write_register(reg, val);
				if (ret < 0)
					MM_ERR("wm9093_setConfig2\n");
				break;

			case WM9093_SPK_MIX_VOL:
				reg = data_table[i].addr;
				val[0] = data_table[i].data1;
				val[1] = wm9093_param_table[codec_idx_wm9093].in1a_spkmix_vol << 6;
				val[1] |= wm9093_param_table[codec_idx_wm9093].in1b_spkmix_vol << 4;
				val[1] |= wm9093_param_table[codec_idx_wm9093].in2a_spkmix_vol << 2;
				val[1] |= wm9093_param_table[codec_idx_wm9093].in2b_spkmix_vol;
				ret = amp_write_register(reg, val);
				if (ret < 0)
					MM_ERR("wm9093_setConfig2\n");
				break;

			case WM9093_AGC_ENABLE:
				reg = data_table[i].addr;
				if(wm9093_param_table[codec_idx_wm9093].agc_enable)
				{
					val[0] = data_table[i].data1|1<<6;
				}
				else
				{
					val[0] = data_table[i].data1;
				}
				val[1] = data_table[i].data2;
				ret = amp_write_register(reg, val);
				if (ret < 0)
					MM_ERR("wm9093_setConfig2\n");
				break;

			case WM9093_AGC_CONTROL0:
				if(wm9093_param_table[codec_idx_wm9093].agc_enable)
				{
					reg = data_table[i].addr;
					val[0] = data_table[i].data1|wm9093_param_table[codec_idx_wm9093].agc_clip_thr;
					val[1] = wm9093_param_table[codec_idx_wm9093].agc_clip_atk << 4;
					val[1] |= wm9093_param_table[codec_idx_wm9093].agc_clip_dcy;
					ret = amp_write_register(reg, val);
					if (ret < 0)
						MM_ERR("wm9093_setConfig2\n");
				}
				break;

			case WM9093_AGC_CONTROL2:
				if(wm9093_param_table[codec_idx_wm9093].agc_enable)
				{
					reg = data_table[i].addr;
					if(wm9093_param_table[codec_idx_wm9093].agc_ramp){
						val[0] = data_table[i].data1 |1;
					}
					else{
						val[0] = data_table[i].data1;
					}
					val[1] = wm9093_param_table[codec_idx_wm9093].agc_min_gain;
					ret = amp_write_register(reg, val);
					if (ret < 0)
						MM_ERR("wm9093_setConfig2\n");
				}
				break;

			default:
				reg = data_table[i].addr;
				val[0] = data_table[i].data1;
				val[1] = data_table[i].data2;
				ret = amp_write_register(reg, val);
				if (ret < 0)
					MM_ERR("wm9093_setConfig2\n");
				break;
		}
	}
}

void wm9093_setConfig(const u8* data, u8 data_len)
{
	struct wm9093_reg_type cfg;
	u8	data_idx = 0;
	u8 i = 0;

	for ( i = 0 ; i < (u8) (data_len/3); i++ )
	{
		cfg.reg=data[data_idx];
		cfg.val[0]= data[data_idx+1];
		cfg.val[1]= data[data_idx+2];
		amp_write_register(cfg.reg, cfg.val);
		data_idx += 3;
	}
}

u8 wm9093_readConfig(int regAdd, u8 *regVal)
{
	int retval = 0;
	struct wm9093_reg_type cfg;
	int read_data;

	cfg.reg = regAdd;
	cfg.val[0] = *regVal;
	cfg.val[1] = *(regVal+1);
	retval |= amp_read_register(cfg.reg, &read_data);

	return read_data;
}

static void wm9093_amp_spk_stereo_on(void)
{
	fm_radio_enabled = FM_OFF;
	wm9093_setConfig2(ICODEC_SPEAKER_RX, wm9093_mono_speaker);
}

static void wm9093_amp_spk_phone_on(void)
{
	fm_radio_enabled = FM_OFF;
	wm9093_setConfig2(ICODEC_SPEAKER_PHONE_RX, wm9093_mono_speaker);	
}

static void wm9093_amp_hp_stereo_on(void)
{
	fm_radio_enabled = FM_OFF;
	wm9093_setConfig2(ICODEC_HEADSET_ST_RX, wm9093_headset_power_on);
	wm9093_setConfig2(ICODEC_HEADSET_ST_RX, wm9093_stereo_headset);
}

static void wm9093_amp_hp_stereo_spk_stereo_on(void)
{
	fm_radio_enabled = FM_OFF;
	wm9093_setConfig2(ICODEC_HEADSET_SPK_RX, wm9093_headset_speaker);
}

static void wm9093_amp_hp_phone_on(void)
{
	fm_radio_enabled = FM_OFF;
	wm9093_setConfig2(ICODEC_HEADSET_PHONE_RX, wm9093_headset_power_on);	//?
	wm9093_setConfig2(ICODEC_HEADSET_PHONE_RX, wm9093_stereo_headset);
}

static void wm9093_amp_earpiece_on(void)
{
	fm_radio_enabled = FM_OFF;
}

static void wm9093_amp_spk_off(void)
{
	fm_radio_enabled = FM_OFF;
	if (CurCodecType == ICODEC_TURN_AMP_OFF)
		return;
	if (CurCodecType == ICODEC_HEADSET_ST_RX ||
		CurCodecType == ICODEC_HEADSET_PHONE_RX)
		wm9093_setConfig2(ICODEC_TURN_AMP_OFF, wm9093_headset_power_down);
	wm9093_setConfig2(ICODEC_TURN_AMP_OFF, wm9093_power_down);
}

static void wm9093_amp_off_all(void)
{
	fm_radio_enabled = FM_OFF;
	if (CurCodecType == ICODEC_TURN_AMP_OFF)
		return;
	if (CurCodecType == ICODEC_HEADSET_ST_RX ||
		CurCodecType == ICODEC_HEADSET_PHONE_RX)
		wm9093_setConfig2(ICODEC_TURN_AMP_OFF, wm9093_headset_power_down);
	wm9093_setConfig2(ICODEC_TURN_AMP_OFF, wm9093_power_down);
}

static void wm9093_amp_fm_spk_stereo_on(void)
{
	fm_radio_enabled = FM_SPK_VOL;
	if(CurFmRadioVol != 0)
		wm9093_setConfig2(ICODEC_FM_SPEAKER_RX, wm9093_fm_radio_speaker_media);
}

static void wm9093_amp_fm_hp_stereo_on(void)
{
	fm_radio_enabled = FM_HP_VOL;
	if(CurFmRadioVol != 0)
		wm9093_setConfig2(ICODEC_FM_HEADSET_ST_RX, wm9093_fm_radio_headset_media);
}

static int wm9093_fm_radio_volume_ctl(int level)
{
	struct wm9093_reg_type cfg;

	if(level <= 0) {
		wm9093_param_table[ICODEC_FM_HEADSET_ST_RX].hp_vol = fm_headset_volume_tbl[0].data2;
		wm9093_param_table[ICODEC_FM_SPEAKER_RX].sp_vol= fm_speaker_volume_tbl[0].data2;
		if(fm_radio_enabled == FM_HP_VOL) {
			wm9093_setConfig2(ICODEC_TURN_AMP_OFF, wm9093_headset_power_down);
			wm9093_setConfig2(ICODEC_TURN_AMP_OFF, wm9093_power_down);
			CurCodecType = ICODEC_FM_HEADSET_ST_RX;
		}
		else if(fm_radio_enabled == FM_SPK_VOL) {
			wm9093_setConfig2(ICODEC_TURN_AMP_OFF, wm9093_power_down);
			CurCodecType = ICODEC_FM_SPEAKER_RX;
		}
		CurFmRadioVol = 0;
	} else if (level >= FM_MAXIUM_VOL) {
		wm9093_param_table[ICODEC_FM_HEADSET_ST_RX].hp_vol = fm_headset_volume_tbl[FM_MAXIUM_VOL].data2;
		wm9093_param_table[ICODEC_FM_SPEAKER_RX].sp_vol = fm_speaker_volume_tbl[FM_MAXIUM_VOL].data2;
		if(CurFmRadioVol == 0 && level != 0)
		{
			CurFmRadioVol = FM_MAXIUM_VOL;
			if(fm_radio_enabled == FM_HP_VOL) {
				wm9093_setConfig2(ICODEC_FM_HEADSET_ST_RX, wm9093_fm_radio_headset_media);
				CurCodecType = ICODEC_FM_HEADSET_ST_RX;
			}
			else if(fm_radio_enabled == FM_SPK_VOL) {
				wm9093_setConfig2(ICODEC_FM_SPEAKER_RX, wm9093_fm_radio_speaker_media);
				CurCodecType = ICODEC_FM_SPEAKER_RX;
			}
		}
		else if(fm_radio_enabled == FM_HP_VOL) {
			cfg.reg = 0x1C;
			cfg.val[0]= fm_headset_volume_tbl[level].data1;
			cfg.val[1]= fm_headset_volume_tbl[level].data2;
			amp_write_register(cfg.reg, cfg.val);

			cfg.reg = 0x1D;
			cfg.val[0]= fm_headset_volume_tbl[level].data1;
			cfg.val[1]= fm_headset_volume_tbl[level].data2;
			amp_write_register(cfg.reg, cfg.val);
		}
		else if(fm_radio_enabled == FM_SPK_VOL) {
			cfg.reg = 0x26;
			cfg.val[0]= fm_speaker_volume_tbl[level].data1;
			cfg.val[1]= fm_speaker_volume_tbl[level].data2;
			amp_write_register(cfg.reg, cfg.val);
		}
		CurFmRadioVol = FM_MAXIUM_VOL;
	} else {
		wm9093_param_table[ICODEC_FM_HEADSET_ST_RX].hp_vol = fm_headset_volume_tbl[level].data2;
		wm9093_param_table[ICODEC_FM_SPEAKER_RX].sp_vol = fm_speaker_volume_tbl[level].data2;
		if(CurFmRadioVol == 0 && level != 0)
		{
			CurFmRadioVol = level;
			if(fm_radio_enabled == FM_HP_VOL) {
				wm9093_setConfig2(ICODEC_FM_HEADSET_ST_RX, wm9093_fm_radio_headset_media);
				CurCodecType = ICODEC_FM_HEADSET_ST_RX;
			}
			else if(fm_radio_enabled == FM_SPK_VOL) {
				wm9093_setConfig2(ICODEC_FM_SPEAKER_RX, wm9093_fm_radio_speaker_media);
				CurCodecType = ICODEC_FM_SPEAKER_RX;
			}
		}
		else if(fm_radio_enabled == FM_HP_VOL) {
			cfg.reg = 0x1C;
			cfg.val[0]= fm_headset_volume_tbl[level].data1;
			cfg.val[1]= fm_headset_volume_tbl[level].data2;
			amp_write_register(cfg.reg, cfg.val);

			cfg.reg = 0x1D;
			cfg.val[0]= fm_headset_volume_tbl[level].data1;
			cfg.val[1]= fm_headset_volume_tbl[level].data2;
			amp_write_register(cfg.reg, cfg.val);
		}
		else if(fm_radio_enabled == FM_SPK_VOL) {
			cfg.reg = 0x26;
			cfg.val[0]= fm_speaker_volume_tbl[level].data1;
			cfg.val[1]= fm_speaker_volume_tbl[level].data2;
			amp_write_register(cfg.reg, cfg.val);
		}
		CurFmRadioVol = level;
	}

	return 0;
}

static long wm9093_amp_calibration(unsigned int cmd, unsigned long arg)
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
		case ICODEC_HEADSET_ST_RX:
			if (amp_cal.gain_type == WM9093_CAL_INPUTA)
				wm9093_param_table[ICODEC_HEADSET_ST_RX].in1_inputa= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_INPUTB)
				wm9093_param_table[ICODEC_HEADSET_ST_RX].in1_inputb= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_AMP_GAIN)
				wm9093_param_table[ICODEC_HEADSET_ST_RX].sp_amp_gain= amp_cal.data;
			else if (amp_cal.gain_type == WM9039_CAL_SPK_BOOST)
				wm9093_param_table[ICODEC_HEADSET_ST_RX].sp_boost= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_VOL)
				wm9093_param_table[ICODEC_HEADSET_ST_RX].sp_vol= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_HP_VOL)
				wm9093_param_table[ICODEC_HEADSET_ST_RX].hp_vol= amp_cal.data;
			else {
				MM_ERR("invalid set_gain_type for HP  %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_HEADSET_SPK_RX:
			if (amp_cal.gain_type == WM9093_CAL_INPUTA)
				wm9093_param_table[ICODEC_HEADSET_SPK_RX].in1_inputa= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_INPUTB)
				wm9093_param_table[ICODEC_HEADSET_SPK_RX].in1_inputb= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_AMP_GAIN)
				wm9093_param_table[ICODEC_HEADSET_SPK_RX].sp_amp_gain= amp_cal.data;
			else if (amp_cal.gain_type == WM9039_CAL_SPK_BOOST)
				wm9093_param_table[ICODEC_HEADSET_SPK_RX].sp_boost= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_VOL)
				wm9093_param_table[ICODEC_HEADSET_SPK_RX].sp_vol= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_HP_VOL)
				wm9093_param_table[ICODEC_HEADSET_SPK_RX].hp_vol= amp_cal.data;
			else {
				MM_ERR("invalid set_gain_type for HP & SPK  %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_SPEAKER_RX:
			if (amp_cal.gain_type == WM9093_CAL_INPUTA)
				wm9093_param_table[ICODEC_SPEAKER_RX].in1_inputa= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_INPUTB)
				wm9093_param_table[ICODEC_SPEAKER_RX].in1_inputb= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_AMP_GAIN)
				wm9093_param_table[ICODEC_SPEAKER_RX].sp_amp_gain= amp_cal.data;
			else if (amp_cal.gain_type == WM9039_CAL_SPK_BOOST)
				wm9093_param_table[ICODEC_SPEAKER_RX].sp_boost= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_VOL)
				wm9093_param_table[ICODEC_SPEAKER_RX].sp_vol= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_HP_VOL)
				wm9093_param_table[ICODEC_SPEAKER_RX].hp_vol= amp_cal.data;
			else {
				MM_ERR("invalid set_gain_type for SPK  %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_HEADSET_PHONE_RX:
			if (amp_cal.gain_type == WM9093_CAL_INPUTA)
				wm9093_param_table[ICODEC_HEADSET_PHONE_RX].in1_inputa= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_INPUTB)
				wm9093_param_table[ICODEC_HEADSET_PHONE_RX].in1_inputb= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_AMP_GAIN)
				wm9093_param_table[ICODEC_HEADSET_PHONE_RX].sp_amp_gain= amp_cal.data;
			else if (amp_cal.gain_type == WM9039_CAL_SPK_BOOST)
				wm9093_param_table[ICODEC_HEADSET_PHONE_RX].sp_boost= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_VOL)
				wm9093_param_table[ICODEC_HEADSET_PHONE_RX].sp_vol= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_HP_VOL)
				wm9093_param_table[ICODEC_HEADSET_PHONE_RX].hp_vol= amp_cal.data;
			else {
				MM_ERR("invalid set_gain_type for HP_PHONE %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_SPEAKER_PHONE_RX:
			if (amp_cal.gain_type == WM9093_CAL_INPUTA)
				wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].in1_inputa= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_INPUTB)
				wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].in1_inputb= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_AMP_GAIN)
				wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].sp_amp_gain= amp_cal.data;
			else if (amp_cal.gain_type == WM9039_CAL_SPK_BOOST)
				wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].sp_boost= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_VOL)
				wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].sp_vol= amp_cal.data;
			else if (amp_cal.gain_type == WM9093_CAL_HP_VOL)
				wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].hp_vol= amp_cal.data;

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
		case ICODEC_HEADSET_ST_RX:
			if (amp_cal.gain_type == WM9093_CAL_INPUTA)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_ST_RX].in1_inputa;
			else if (amp_cal.gain_type == WM9093_CAL_INPUTB)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_ST_RX].in1_inputb;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_AMP_GAIN)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_ST_RX].sp_amp_gain;
			else if (amp_cal.gain_type == WM9039_CAL_SPK_BOOST)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_ST_RX].sp_boost;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_VOL)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_ST_RX].sp_vol;
			else if (amp_cal.gain_type == WM9093_CAL_HP_VOL)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_ST_RX].hp_vol;
			else {
				MM_ERR("invalid get_gain_type for HP %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_HEADSET_SPK_RX:
			if (amp_cal.gain_type == WM9093_CAL_INPUTA)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_SPK_RX].in1_inputa;
			else if (amp_cal.gain_type == WM9093_CAL_INPUTB)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_SPK_RX].in1_inputb;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_AMP_GAIN)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_SPK_RX].sp_amp_gain;
			else if (amp_cal.gain_type == WM9039_CAL_SPK_BOOST)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_SPK_RX].sp_boost;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_VOL)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_SPK_RX].sp_vol;
			else if (amp_cal.gain_type == WM9093_CAL_HP_VOL)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_SPK_RX].hp_vol;
			else {
				MM_ERR("invalid get_gain_type for SPK %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_SPEAKER_RX:
			if (amp_cal.gain_type == WM9093_CAL_INPUTA)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_RX].in1_inputa;
			else if (amp_cal.gain_type == WM9093_CAL_INPUTB)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_RX].in1_inputb;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_AMP_GAIN)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_RX].sp_amp_gain;
			else if (amp_cal.gain_type == WM9039_CAL_SPK_BOOST)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_RX].sp_boost;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_VOL)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_RX].sp_vol;
			else if (amp_cal.gain_type == WM9093_CAL_HP_VOL)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_RX].hp_vol;
			else {
				MM_ERR("invalid get_gain_type for SPK %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_HEADSET_PHONE_RX:
			if (amp_cal.gain_type == WM9093_CAL_INPUTA)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_PHONE_RX].in1_inputa;
			else if (amp_cal.gain_type == WM9093_CAL_INPUTB)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_PHONE_RX].in1_inputb;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_AMP_GAIN)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_PHONE_RX].sp_amp_gain;
			else if (amp_cal.gain_type == WM9039_CAL_SPK_BOOST)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_PHONE_RX].sp_boost;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_VOL)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_PHONE_RX].sp_vol;
			else if (amp_cal.gain_type == WM9093_CAL_HP_VOL)
				amp_cal.data = wm9093_param_table[ICODEC_HEADSET_PHONE_RX].hp_vol;
			else {
				MM_ERR("invalid get_gain_type for HP_PHONE %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_SPEAKER_PHONE_RX:
			if (amp_cal.gain_type == WM9093_CAL_INPUTA)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].in1_inputa;
			else if (amp_cal.gain_type == WM9093_CAL_INPUTB)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].in1_inputb;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_AMP_GAIN)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].sp_amp_gain;
			else if (amp_cal.gain_type == WM9039_CAL_SPK_BOOST)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].sp_boost;
			else if (amp_cal.gain_type == WM9093_CAL_SPK_VOL)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].sp_vol;
			else if (amp_cal.gain_type == WM9093_CAL_HP_VOL)
				amp_cal.data = wm9093_param_table[ICODEC_SPEAKER_PHONE_RX].hp_vol;
			else {
				MM_ERR("invalid get_gain_type for SPK_PHONE %d\n", amp_cal.gain_type);
				rc = -EFAULT;
			}
			break;
		case ICODEC_FM_RADIO_VOL_CTL:
			wm9093_fm_radio_volume_ctl(amp_cal.data);
			break;
		case ICODEC_TURN_AMP_OFF:
			amp_cal.data = 1;
			if (lge_bd_rev >= LGE_REV_10)
				wm9093_amp_off_all();
			else
				amp_cal.data = 0;
			break;
		case ICODEC_FM_HEADSET_ON :
			amp_cal.data = 1;
			if (lge_bd_rev >= LGE_REV_10)
				wm9093_amp_fm_hp_stereo_on();
			else
				amp_cal.data = 0;
			break;
		case ICODEC_FM_SPEAKER_ON :
			amp_cal.data = 1;
			if (lge_bd_rev >= LGE_REV_10)
				wm9093_amp_fm_spk_stereo_on();
			else
				amp_cal.data = 0;
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

void wm9093_amp_init (struct amp_data *pdata)
{
	_data = pdata;
	pdata->amp_spk_stereo_on = wm9093_amp_spk_stereo_on;
	pdata->amp_spk_phone_on = wm9093_amp_spk_phone_on;
	pdata->amp_hp_stereo_on = wm9093_amp_hp_stereo_on;
	pdata->amp_hp_stereo_spk_stereo_on = wm9093_amp_hp_stereo_spk_stereo_on;
	pdata->amp_hp_phone_on = wm9093_amp_hp_phone_on;
	pdata->amp_earpiece_on = wm9093_amp_earpiece_on;
	pdata->amp_spk_off = wm9093_amp_spk_off;
	pdata->amp_off_all = wm9093_amp_off_all;
	pdata->amp_fm_spk_stereo_on = wm9093_amp_fm_spk_stereo_on;
	pdata->amp_fm_hp_stereo_on = wm9093_amp_fm_hp_stereo_on;
	pdata->amp_calibration = wm9093_amp_calibration;

	if (lge_bd_rev >= LGE_REV_10)
		wm9093_fm_radio_speaker_media = wm9093_fm_radio_speaker_rev1_0;
	else		{
		wm9093_fm_radio_speaker_media = wm9093_fm_radio_speaker_rev_d;
		wm9093_param_table[ICODEC_FM_HEADSET_ST_RX].in1_inputa = 0x07;
		wm9093_param_table[ICODEC_FM_HEADSET_ST_RX].in1_inputb = 0x07;
		wm9093_param_table[ICODEC_FM_SPEAKER_RX].in1_inputa = 0x06;
		wm9093_param_table[ICODEC_FM_SPEAKER_RX].in1_inputb = 0x06;
	}
	mutex_init(&amp_lock);
	wm9093_setConfig2(ICODEC_TURN_AMP_OFF, wm9093_power_down);
}

