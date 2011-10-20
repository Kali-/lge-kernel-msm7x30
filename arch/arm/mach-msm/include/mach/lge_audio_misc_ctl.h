/* arch/arm/mach-msm/include/mach/qdsp5v2/lge_audio_misc_ctrl.h
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

#ifndef __ASM_ARCH_MSM_MACH_QDSP5_V2_LGE_AUDIO_CTRL_H
#define __ASM_ARCH_MSM_MACH_QDSP5_V2_LGE_AUDIO_CTRL_H

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/android_pmem.h>
#include <linux/msm_audio.h>
#include <asm/atomic.h>
#include <asm/ioctls.h>
#include <mach/msm_adsp.h>
#include <mach/debug_mm.h>
#include <mach/qdsp5v2/audio_dev_ctl.h>
#include <mach/qdsp5v2/afe.h>
#include <mach/msm_rpcrouter.h>

#define INTERNAL_CODEC_TYPE "snddev_icodec"
#define EXTERNAL_CODEC_TYPE "msm_snddev_ecodec"
#define MI2S_TYPE "snddev_mi2s" 
#define VIRTUAL_CODEC  "snddev_virtual"

int __init audio_misc_init(void);

typedef enum{
	MSM_IEARPIECE_DEVICE = 0,
	MSM_IMIC_DEVICE = 1,
	MSM_IHS_STEREO_RX_DEVICE = 2,
	MSM_IHS_MONO_RX_DEVICE = 3,
	MSM_IHS_MONO_TX_DEVICE = 4,
	MSM_BT_SCO_EARPIECE_DEVICE = 5,
	MSM_BT_SCO_MIC_DEVICE = 6,
	MSM_IFMRADIO_HANDSET_DEVICE = 7,
	MSM_ISPEAKER_RX_DEVICE = 8,
	MSM_IFMRADIO_SPEAKER_DEVICE = 9,
	MSM_IFMRADIO_HEADSET_DEVICE = 10,
	MSM_ITTY_HS_MONO_TX_DEVICE = 11,
	MSM_ITTY_HS_MONO_RX_DEVICE =12,
	MSM_ISPEAKER_TX_DEVICE = 13,
	MSM_IHS_STEREO_SPEAKER_STEREO_RX_DEVICE = 14,
	MSM_A2DP_RX_DEVICE = 15,
	MSM_A2DP_TX_DEVICE = 16,
	MSM_SNDDEV_MI2S_STEREO_RX_DEVICE = 17,
	MSM_SNDDEV_MI2S_FM_TX_DEVICE = 18,
	MSM_UPLINK_RX_DEVICE = 19,
	MSM_IEARPIECE_AUDIENCE_DEVICE = 20,
	MSM_IMIC_AUDIENCE_DEVICE = 21,
	MSM_ISPEAKER_AUDIENCE_RX_DEVICE = 22,
	MSM_ISPEAKER_AUDIENCE_TX_DEVICE = 23,
	MSM_IDUAL_MIC_ENDFIRE_DEVICE = 24,
	MSM_SPK_IDUAL_MIC_ENDFIRE_DEVICE = 25,
	MAX_BRYCE_AUDIO_DEVICES
}LGE_BRYCE_AUDIO_DEVICES;

typedef enum{
	ICODEC = 0,
	ECODEC = 1,
	MI2S   = 2,
	VCODEC = 3,
	MAX_CDC_TYPE
}MSM7x30_CODEC_TYPE;

typedef enum{
	RX_VOL_MIN = 0,
	RX_VOL_MAX = 1,	
	MAX_PARAM
}audio_parameter_type;

//int max_devices;
//struct platform_device *bryce_audio_devices[MAX_BRYCE_AUDIO_DEVICES];


#define GET_DEV_POINTER(x,y) x = bryce_audio_devices[y]

#define GET_CODEC_TYPE(x,y)                         \
if ( 0 == strcmp(y->name,INTERNAL_CODEC_TYPE))      \
    x = ICODEC;                                     \
else if ( 0 == strcmp(y->name,EXTERNAL_CODEC_TYPE)) \
    x = ECODEC;                                     \
else if ( 0 == strcmp(y->name,MI2S_TYPE))           \
    x = MI2S;                                       \
else if ( 0 == strcmp(y->name,VIRTUAL_CODEC))       \
    x = VCODEC                                      \

#define GET_RX_MIN_VOL(x,y,z)					\
{												\
if( (ICODEC == z) || (ECODEC == z) )			\
{												\
	if( (y->capability) & SNDDEV_CAP_RX)		\
		x = (s32*)&(y->min_voice_rx_vol[0]);	\
	else										\
		x = NULL;								\
}												\
else											\
	x = NULL;									\
}												\

#define GET_RX_MAX_VOL(x,y,z)					\
{												\
if( (ICODEC == z) || (ECODEC == z) )			\
{												\
	if( (y->capability) & SNDDEV_CAP_RX)		\
		x = (s32*)&(y->max_voice_rx_vol[0]);	\
	else										\
		x = NULL;								\
}												\
else											\
	x = NULL;									\
}												\


#endif
