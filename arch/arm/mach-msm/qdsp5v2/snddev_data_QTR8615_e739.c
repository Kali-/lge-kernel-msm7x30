/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/mfd/msm-adie-codec.h>
#include <linux/uaccess.h>
#include <mach/qdsp5v2/snddev_icodec.h>
//#include <mach/qdsp5v2/marimba_profile.h>
#include <mach/qdsp5v2/aux_pcm.h>
#include <mach/qdsp5v2/snddev_ecodec.h>
#include <mach/qdsp5v2/audio_dev_ctl.h>
#include <mach/qdsp5v2/snddev_virtual.h>
#include <mach/board.h>
#include <asm/mach-types.h>
#include <mach/gpio.h>
#include <mach/qdsp5v2/snddev_mi2s.h>
#include <mach/qdsp5v2/mi2s.h>
#include <mach/qdsp5v2/audio_acdb_def.h>

#include <mach/board_lge.h>

#include "mach/qdsp5v2/lge_wm9093_e739.h"

#include <mach/debug_mm.h>

#include "timpani_profile_7x30.h"


/* define the value for BT_SCO */
#define BT_SCO_PCM_CTL_VAL (PCM_CTL__RPCM_WIDTH__LINEAR_V |\
				PCM_CTL__TPCM_WIDTH__LINEAR_V)
#define BT_SCO_DATA_FORMAT_PADDING (DATA_FORMAT_PADDING_INFO__RPCM_FORMAT_V |\
				DATA_FORMAT_PADDING_INFO__TPCM_FORMAT_V)
#define BT_SCO_AUX_CODEC_INTF   AUX_CODEC_INTF_CTL__PCMINTF_DATA_EN_V

#define HANDSET_PCM_CTL_VAL (PCM_CTL__RPCM_WIDTH__LINEAR_V |\
				PCM_CTL__TPCM_WIDTH__LINEAR_V)
#define HANDSET_DATA_FORMAT_PADDING (DATA_FORMAT_PADDING_INFO__RPCM_FORMAT_V |\
				DATA_FORMAT_PADDING_INFO__TPCM_FORMAT_V)
#define HANDSET_AUX_CODEC_INTF   AUX_CODEC_INTF_CTL__PCMINTF_DATA_EN_V

#define AUX_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE},  \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x05, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0xC1)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xFF, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xFF, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xFF, 0x65)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x1E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0xA3, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x93, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x94, 0xFF, 0x1B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x99, 0x0F, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x9F, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x0C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xE6)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xFF, 0x07)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xFF, 0x07)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xC0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0xAB, 0x09, 0x09)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define SPEAKER_TX_48000_OSR_256 \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE},  \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x05, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0xD0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xFF, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xFF, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xFF, 0x65)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x1E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0xA3, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x93, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x94, 0xFF, 0x1B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x99, 0x0F, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x9F, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x0C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xE6)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xC0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0xAB, 0x09, 0x09)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }

#define SPEAKER_TX_48000_OSR_256_REC \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE},  \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x05, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0xD0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xFF, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xFF, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xFF, 0x65)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x1E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0xA3, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x93, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x94, 0xFF, 0x1B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x99, 0x0F, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x9F, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x0C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xE6)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xFF, 0x0D)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xFF, 0x0D)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xC0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0xAB, 0x09, 0x09)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
  
#define SPEAKER_TX_48000_OSR_256_CAM_REC \
	{{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_FLASH_IMAGE},  \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x05, 0x05)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x80, 0x05, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0x30)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xFF, 0xAC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0xD0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xFF, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xFF, 0xBC)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xFF, 0x65)}, \
	{ADIE_CODEC_ACTION_DELAY_WAIT, 0xbb8}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x82, 0xFF, 0x1E)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0xA3, 0x01, 0x01)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x93, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x94, 0xFF, 0x1B)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x99, 0x0F, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x9F, 0x03, 0x03)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x0C)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8b, 0xff, 0xE6)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8c, 0x03, 0x02)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x86, 0xFF, 0x0D)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x87, 0xFF, 0x0D)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xC0)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_ANALOG_READY}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x8A, 0xF0, 0xF0)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x83, 0x0C, 0x00)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_ANALOG_OFF}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x0D, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x14, 0xFF, 0x64)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x11, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0x12, 0xFF, 0x00)}, \
	{ADIE_CODEC_ACTION_ENTRY, ADIE_CODEC_PACK_ENTRY(0xAB, 0x09, 0x09)}, \
	{ADIE_CODEC_ACTION_STAGE_REACHED, ADIE_CODEC_DIGITAL_OFF} }
#define CHANNEL_MODE_MONO 1
#define CHANNEL_MODE_STEREO 2

//***************************************************************************************
//      RX VOICE PATH
//***************************************************************************************

//=======================================================================================
// Handset mono RX (voice call through receiver)    DEVICE_HANDSET_VOICE_RX 
//=======================================================================================
struct adie_codec_action_unit earpiece_voice_rx_actions[] =
	EAR_PRI_MONO_8000_OSR_256;

static struct adie_codec_hwsetting_entry earpiece_voice_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = earpiece_voice_rx_actions,
		.action_sz = ARRAY_SIZE(earpiece_voice_rx_actions),
	}
};

static struct adie_codec_dev_profile earpiece_voice_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = earpiece_voice_rx_settings,
	.setting_sz = ARRAY_SIZE(earpiece_voice_rx_settings),
};

struct snddev_icodec_data earpiece_voice_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "handset_voice_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_SPKR,
	.profile = &earpiece_voice_rx_profile,
	.channel_mode = CHANNEL_MODE_MONO,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -2000,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = 2000,
};

static struct platform_device lge_device_earpiece_voice_rx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_HANDSET_VOICE_RX,
	.dev = { .platform_data = &earpiece_voice_rx_data },
};


//=======================================================================================
// Headset stereo RX : (voice call through headset)    DEVICE_HEADSET_STEREO_VOICE_RX
//=======================================================================================
struct adie_codec_action_unit headset_mono_voice_rx_actions[] =
  HEADSET_RX_CAPLESS_48000_OSR_256_MONO;

static struct adie_codec_hwsetting_entry headset_mono_voice_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = headset_mono_voice_rx_actions,
		.action_sz = ARRAY_SIZE(headset_mono_voice_rx_actions),
	}
};

static struct adie_codec_dev_profile headset_mono_voice_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = headset_mono_voice_rx_settings,
	.setting_sz = ARRAY_SIZE(headset_mono_voice_rx_settings),
};

struct snddev_icodec_data headset_mono_voice_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_stereo_voice_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_SPKR_MONO,
	.profile = &headset_mono_voice_rx_profile,
	.channel_mode = CHANNEL_MODE_MONO,
	.default_sample_rate = 48000,
	.pamp_on = &set_amp_headset_mono_voice,
	.pamp_off = &set_amp_PowerDown,
	.max_voice_rx_vol[VOC_NB_INDEX] = -900,
	.min_voice_rx_vol[VOC_NB_INDEX] = -2400,
	.max_voice_rx_vol[VOC_WB_INDEX] = -900,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2400
};

static struct platform_device lge_device_headset_mono_voice_rx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_HEADSET_VOICE_RX,
	.dev = { .platform_data = &headset_mono_voice_rx_data },
};

//=======================================================================================
// Speaker stereo RX : (voice call through speaker)    DEVICE_SPEAKER_STEREO_VOICE_RX
//=======================================================================================
struct adie_codec_action_unit speaker_stereo_voice_rx_actions[] =
   SPEAKER_PRI_STEREO_48000_OSR_256;

static struct adie_codec_hwsetting_entry speaker_stereo_voice_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = speaker_stereo_voice_rx_actions,
		.action_sz = ARRAY_SIZE(speaker_stereo_voice_rx_actions),
	}
};

static struct adie_codec_dev_profile speaker_stereo_voice_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = speaker_stereo_voice_rx_settings,
	.setting_sz = ARRAY_SIZE(speaker_stereo_voice_rx_settings),
};

struct snddev_icodec_data speaker_stereo_voice_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "speaker_stereo_voice_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_LGE_SPEAKER_STEREO_VOICE_RX,
	.profile = &speaker_stereo_voice_rx_profile,
	.channel_mode = CHANNEL_MODE_STEREO,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = &set_amp_speaker_stereo_voice,
	.pamp_off = &set_amp_PowerDown,
	.max_voice_rx_vol[VOC_NB_INDEX] = 500,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1500,
	.max_voice_rx_vol[VOC_WB_INDEX] = 500,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1500,
};

static struct platform_device lge_device_speaker_stereo_voice_rx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_SPEAKER_VOICE_RX,
	.dev = { .platform_data = &speaker_stereo_voice_rx_data },
};

//=======================================================================================
// TTY RX : (TTY call)    DEVICE_TTY_RX
//=======================================================================================
struct adie_codec_action_unit tty_rx_actions[] =
	TTY_HEADSET_MONO_RX;

static struct adie_codec_hwsetting_entry tty_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = tty_rx_actions,
		.action_sz = ARRAY_SIZE(tty_rx_actions),
	}
};

static struct adie_codec_dev_profile tty_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = tty_rx_settings,
	.setting_sz = ARRAY_SIZE(tty_rx_settings),
};

static struct snddev_icodec_data tty_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE | SNDDEV_CAP_TTY),
	.name = "tty_headset_mono_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_SPKR_MONO,
	.profile = &tty_rx_profile,
	.channel_mode = CHANNEL_MODE_MONO,
	.default_sample_rate = 48000,
	.pamp_on = &set_amp_tty,
	.pamp_off = &set_amp_PowerDown,
	.max_voice_rx_vol[VOC_NB_INDEX] = 0,
	.min_voice_rx_vol[VOC_NB_INDEX] = -100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 0,
	.min_voice_rx_vol[VOC_WB_INDEX] = -100,
};

static struct platform_device lge_device_tty_rx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_TTY_RX,
	.dev = { .platform_data = &tty_rx_data },
};
//=======================================================================================
// BT sco RX : (voice call through BT)    DEVICE_BT_SCO_VOICE_RX
//=======================================================================================
struct snddev_ecodec_data bt_sco_voice_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_voice_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_SPKR,
	.channel_mode = CHANNEL_MODE_MONO,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
};

struct platform_device lge_device_bt_sco_voice_rx = {
	.name = "msm_snddev_ecodec",
	.id = DEVICE_ID_EXTERNAL_BT_SCO_VOICE_RX,
	.dev = { .platform_data = &bt_sco_voice_rx_data },
};



//***************************************************************************************
//      RX AUDIO PATH
//***************************************************************************************

//=======================================================================================
// Handset mono RX (audio through receiver)    DEVICE_HANDSET_AUDIO_RX 
//=======================================================================================
struct adie_codec_action_unit earpiece_audio_rx_actions[] =
	EAR_PRI_MONO_8000_OSR_256_AUDIO;

static struct adie_codec_hwsetting_entry earpiece_audio_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = earpiece_audio_rx_actions,
		.action_sz = ARRAY_SIZE(earpiece_audio_rx_actions),
	}
};

static struct adie_codec_dev_profile earpiece_audio_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = earpiece_audio_rx_settings,
	.setting_sz = ARRAY_SIZE(earpiece_audio_rx_settings),
};

static struct snddev_icodec_data earpiece_audio_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "handset_audio_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_LGE_HANDSET_AUDIO_RX,
	.profile = &earpiece_audio_rx_profile,
	.channel_mode = CHANNEL_MODE_MONO,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
	.max_voice_rx_vol[VOC_NB_INDEX] = -200,
	.min_voice_rx_vol[VOC_NB_INDEX] = -200,
	.max_voice_rx_vol[VOC_WB_INDEX] = -200,
	.min_voice_rx_vol[VOC_WB_INDEX] = -200,
};

static struct platform_device lge_device_earpiece_audio_rx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_HANDSET_AUDIO_RX,
	.dev = { .platform_data = &earpiece_audio_rx_data },
};

//=======================================================================================
// Headset stereo RX : (audio through headset)    DEVICE_HEADSET_STEREO_AUDIO_RX
//=======================================================================================
struct adie_codec_action_unit headset_stereo_audio_rx_actions[] =
	HPH_PRI_AB_LEG_STEREO_AUDIO;
 
static struct adie_codec_hwsetting_entry headset_stereo_audio_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = headset_stereo_audio_rx_actions,
		.action_sz = ARRAY_SIZE(headset_stereo_audio_rx_actions),
	}
};

static struct adie_codec_dev_profile headset_stereo_audio_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = headset_stereo_audio_rx_settings,
	.setting_sz = ARRAY_SIZE(headset_stereo_audio_rx_settings),
};

static struct snddev_icodec_data headset_stereo_audio_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_stereo_audio_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_SPKR_STEREO,
	.profile = &headset_stereo_audio_rx_profile,
	.channel_mode = CHANNEL_MODE_STEREO,
	.default_sample_rate = 48000,
	.pamp_on = &set_amp_headset_stereo_audio,
	.pamp_off = &set_amp_PowerDown,
	.max_voice_rx_vol[VOC_NB_INDEX] = -700,
	.min_voice_rx_vol[VOC_NB_INDEX] = -700,
	.max_voice_rx_vol[VOC_WB_INDEX] = -700,
	.min_voice_rx_vol[VOC_WB_INDEX] = -700
};

static struct platform_device lge_device_headset_stereo_audio_rx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_HEADSET_AUDIO_RX,
	.dev = { .platform_data = &headset_stereo_audio_rx_data },
};

//=======================================================================================
// Speaker stereo RX : (audio call through speaker)    DEVICE_SPEAKER_STEREO_AUDIO_RX
//=======================================================================================
struct adie_codec_action_unit speaker_stereo_audio_rx_actions[] =
   SPEAKER_PRI_STEREO_48000_OSR_256_AUDIO;

static struct adie_codec_hwsetting_entry speaker_stereo_audio_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = speaker_stereo_audio_rx_actions,
		.action_sz = ARRAY_SIZE(speaker_stereo_audio_rx_actions),
	}
};

static struct adie_codec_dev_profile speaker_stereo_audio_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = speaker_stereo_audio_rx_settings,
	.setting_sz = ARRAY_SIZE(speaker_stereo_audio_rx_settings),
};

static struct snddev_icodec_data speaker_stereo_audio_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "speaker_stereo_audio_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_STEREO,
	.profile = &speaker_stereo_audio_rx_profile,
	.channel_mode = CHANNEL_MODE_STEREO,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = &set_amp_speaker_stereo_audio,
	.pamp_off = &set_amp_PowerDown,
	.max_voice_rx_vol[VOC_NB_INDEX] = 500,
	.min_voice_rx_vol[VOC_NB_INDEX] = 500,
	.max_voice_rx_vol[VOC_WB_INDEX] = 500,
	.min_voice_rx_vol[VOC_WB_INDEX] =  500,
};

static struct platform_device lge_device_speaker_stereo_audio_rx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_SPEAKER_AUDIO_RX,
	.dev = { .platform_data = &speaker_stereo_audio_rx_data },
};

//=========================================================================================================
// Headset / Speaker dual RX : (audio through headset and speaker)    DEVICE_HEADSET_SPEAKER_AUDIO_RX
//=========================================================================================================
struct adie_codec_action_unit headset_speaker_audio_rx_actions[] =
	HPH_SPEAKER_PRI_AB_LEG_STEREO_AUDIO;

static struct adie_codec_hwsetting_entry headset_speaker_audio_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = headset_speaker_audio_rx_actions,
		.action_sz =
		ARRAY_SIZE(headset_speaker_audio_rx_actions),
	}
};

static struct adie_codec_dev_profile headset_speaker_audio_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = headset_speaker_audio_rx_settings,
	.setting_sz = ARRAY_SIZE(headset_speaker_audio_rx_settings),
};

static struct snddev_icodec_data headset_speaker_audio_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_speaker_stereo_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_LGE_HEADSET_SPEAKER_AUDIO_RX,
	.profile = &headset_speaker_audio_rx_profile,
	.channel_mode = CHANNEL_MODE_STEREO,
	.default_sample_rate = 48000,
	.pamp_on = &set_amp_headset_speaker_audio,
	.pamp_off = &set_amp_PowerDown,
	.voltage_on = NULL,
	.voltage_off = NULL,
	.max_voice_rx_vol[VOC_NB_INDEX] = -200,
	.min_voice_rx_vol[VOC_NB_INDEX] = -200,
	.max_voice_rx_vol[VOC_WB_INDEX] = -200,
	.min_voice_rx_vol[VOC_WB_INDEX] = -200,
};

static struct platform_device lge_device_headset_speaker_audio_rx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_HEADSET_SPEAKER_RX,
	.dev = { .platform_data = &headset_speaker_audio_rx_data },
};

//=======================================================================================
// BT sco RX : (audio through BT)    DEVICE_BT_SCO_AUDIO_RX
//=======================================================================================
static struct snddev_ecodec_data bt_sco_audio_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_audio_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_LGE_BT_SCO_AUDIO_RX,
	.channel_mode = CHANNEL_MODE_MONO,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
};

struct platform_device lge_device_bt_sco_audio_rx = {
	.name = "msm_snddev_ecodec",
	.id = DEVICE_ID_EXTERNAL_BT_SCO_AUDIO_RX,
	.dev = { .platform_data = &bt_sco_audio_rx_data },
};

//=======================================================================================
// BT A2DP RX : (audio through A2DP)    DEVICE_BT_A2DP_RX
//=======================================================================================
static struct snddev_virtual_data bt_a2dp_rx_data = {
	.capability = SNDDEV_CAP_RX,
	.name = "a2dp_rx",
	.copp_id = 2,
	.acdb_id = PSEUDO_ACDB_ID,
};

static struct platform_device lge_device_bt_a2dp_rx = {
	.name = "snddev_virtual",
	.id = 0,
	.dev = { .platform_data = &bt_a2dp_rx_data },
};

//***************************************************************************************
//      TX VOICE PATH
//***************************************************************************************

static enum hsed_controller handset_mic1_pmctl_id[] = {PM_HSED_CONTROLLER_0};
static enum hsed_controller handset_mic_aux_pmctl_id[] = {PM_HSED_CONTROLLER_1};

//=======================================================================================
// Handset voice TX (voice call through main mic)    DEVICE_HANDSET_VOICE_TX 
//=======================================================================================
struct adie_codec_action_unit handset_voice_tx_actions[] =
	AMIC_PRI_MONO_8000_OSR_256;

static struct adie_codec_hwsetting_entry handset_voice_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = handset_voice_tx_actions,
		.action_sz = ARRAY_SIZE(handset_voice_tx_actions),
	}
};

static struct adie_codec_dev_profile handset_voice_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = handset_voice_tx_settings,
	.setting_sz = ARRAY_SIZE(handset_voice_tx_settings),
};

static struct snddev_icodec_data handset_voice_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_voice_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_MIC,  //ACDB_ID_HANDSET_MIC
	.profile = &handset_voice_tx_profile,
	.channel_mode = CHANNEL_MODE_MONO,
	.pmctl_id = handset_mic1_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(handset_mic1_pmctl_id),
	.default_sample_rate = 48000,
};

static struct platform_device lge_device_handset_voice_tx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_HANDSET_VOICE_TX,
	.dev = { .platform_data = &handset_voice_tx_data },
};

//=======================================================================================
// Headset voice TX (voice call through headset mic)    DEVICE_HEADSET_VOICE_TX 
//=======================================================================================
struct adie_codec_action_unit headset_voice_tx_actions[] =
	AMIC1_HEADSET_TX_MONO_PRIMARY_OSR256;

static struct adie_codec_hwsetting_entry headset_voice_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = headset_voice_tx_actions,
		.action_sz = ARRAY_SIZE(headset_voice_tx_actions),
	}
};

static struct adie_codec_dev_profile headset_voice_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = headset_voice_tx_settings,
	.setting_sz = ARRAY_SIZE(headset_voice_tx_settings),
};

static struct snddev_icodec_data headset_voice_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "headset_voice_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_MIC,
	.profile = &headset_voice_tx_profile,
	.channel_mode = CHANNEL_MODE_MONO,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_tx_route_config,
	.pamp_off = msm_snddev_tx_route_deconfig,
};

static struct platform_device lge_device_headset_voice_tx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_HEADSET_VOICE_TX,
	.dev = { .platform_data = &headset_voice_tx_data },
};

//=======================================================================================
// Speaker voice TX (voice call through aux mic)    DEVICE_SPEAKER_VOICE_TX 
//=======================================================================================
struct adie_codec_action_unit speaker_voice_tx_actions[] =
	AUX_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry speaker_voice_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = speaker_voice_tx_actions,
		.action_sz = ARRAY_SIZE(speaker_voice_tx_actions),
	}
};

static struct adie_codec_dev_profile speaker_voice_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = speaker_voice_tx_settings,
	.setting_sz = ARRAY_SIZE(speaker_voice_tx_settings),
};

static struct snddev_icodec_data speaker_voice_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_voice_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC,
	.profile = &speaker_voice_tx_profile,
	.channel_mode = CHANNEL_MODE_MONO,
	.pmctl_id = handset_mic_aux_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(handset_mic_aux_pmctl_id),
	.default_sample_rate = 48000,
};

static struct platform_device lge_device_speaker_tx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_SPEAKER_VOICE_TX,
	.dev = { .platform_data = &speaker_voice_tx_data },
};

//=======================================================================================
// TTY TX : (TTY call)    DEVICE_TTY_TX
//=======================================================================================
struct adie_codec_action_unit tty_tx_actions[] =
	TTY_HEADSET_MONO_TX;

static struct adie_codec_hwsetting_entry tty_tx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = tty_tx_actions,
		.action_sz = ARRAY_SIZE(tty_tx_actions),
	}
};

static struct adie_codec_dev_profile tty_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = tty_tx_settings,
	.setting_sz = ARRAY_SIZE(tty_tx_settings),
};

static struct snddev_icodec_data tty_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE | SNDDEV_CAP_TTY),
	.name = "tty_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_MIC,
	.profile = &tty_tx_profile,
	.channel_mode = CHANNEL_MODE_MONO,
	.default_sample_rate = 8000,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device lge_device_tty_tx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_TTY_TX,
	.dev = { .platform_data = &tty_tx_data },
};

//=======================================================================================
// BT sco TX : (voice call through bt sco)    DEVICE_BT_SCO_VOICE_TX
//=======================================================================================
static struct snddev_ecodec_data bt_sco_voice_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_MIC,
	.channel_mode = CHANNEL_MODE_MONO,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
};

struct platform_device lge_device_bt_sco_voice_tx = {
	.name = "msm_snddev_ecodec",
	.id = DEVICE_ID_EXTERNAL_BT_SCO_VOICE_TX,
	.dev = { .platform_data = &bt_sco_voice_tx_data },
};



//***************************************************************************************
//      TX AUDIO PATH
//***************************************************************************************

//=======================================================================================
// Handset rec TX (recording through main mic)    DEVICE_MIC1_REC_TX 
//=======================================================================================
struct adie_codec_action_unit mic1_rec_tx_actions[] =
	SPEAKER_TX_48000_OSR_256_REC;

static struct adie_codec_hwsetting_entry mic1_rec_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = mic1_rec_tx_actions,
		.action_sz = ARRAY_SIZE(mic1_rec_tx_actions),
	}
};

static struct adie_codec_dev_profile mic1_rec_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = mic1_rec_tx_settings,
	.setting_sz = ARRAY_SIZE(mic1_rec_tx_settings),
};

static struct snddev_icodec_data mic1_rec_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "mic1_rec_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_LGE_MIC1_REC_TX,
	.profile = &mic1_rec_tx_profile,
	.channel_mode = CHANNEL_MODE_MONO,
	.pmctl_id = handset_mic1_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(handset_mic1_pmctl_id),
	.default_sample_rate = 48000,
};

static struct platform_device lge_device_mic1_rec_tx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_MIC1_REC,
	.dev = { .platform_data = &mic1_rec_tx_data },
};

//===========================================================================

static struct adie_codec_action_unit cam_rec_tx_actions[] =
	SPEAKER_TX_48000_OSR_256_CAM_REC;

static struct adie_codec_hwsetting_entry cam_rec_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = cam_rec_tx_actions,
		.action_sz = ARRAY_SIZE(cam_rec_tx_actions),
	}
};

static struct adie_codec_dev_profile cam_rec_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = cam_rec_tx_settings,
	.setting_sz = ARRAY_SIZE(cam_rec_tx_settings),
};

static struct snddev_icodec_data cam_rec_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "cam_rec_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_LGE_MIC1_REC_TX,
	.profile = &cam_rec_tx_profile,
	.channel_mode = CHANNEL_MODE_MONO,
	.pmctl_id = handset_mic1_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(handset_mic1_pmctl_id),
	.default_sample_rate = 48000,
};

static struct platform_device lge_device_cam_rec_tx = {
	.name = "snddev_icodec",
	.id = DEVICE_ID_INTERNAL_CAM_REC,
	.dev = { .platform_data = &cam_rec_tx_data },
};



/////////////////////////////////////////////////////////////////////////////////////////
/* Configurations list */
/////////////////////////////////////////////////////////////////////////////////////////
// Bryce uses this config
static struct platform_device *lge_snd_devices[] __initdata = {
  &lge_device_earpiece_voice_rx,
  &lge_device_headset_mono_voice_rx,
  &lge_device_speaker_stereo_voice_rx,
  &lge_device_tty_rx,
  &lge_device_bt_sco_voice_rx,
  &lge_device_earpiece_audio_rx,
  &lge_device_headset_stereo_audio_rx,
  &lge_device_speaker_stereo_audio_rx,
  &lge_device_headset_speaker_audio_rx,
  &lge_device_bt_sco_audio_rx,
  &lge_device_bt_a2dp_rx,
  &lge_device_handset_voice_tx,
  &lge_device_headset_voice_tx,
  &lge_device_speaker_tx,
  &lge_device_tty_tx,
  &lge_device_bt_sco_voice_tx,
  &lge_device_mic1_rec_tx,
  &lge_device_cam_rec_tx,
}; 
 
struct adie_codec_action_unit *codec_cal[] = {
  earpiece_voice_rx_actions,
  headset_mono_voice_rx_actions,
  speaker_stereo_voice_rx_actions,
  tty_rx_actions,
  headset_stereo_audio_rx_actions,
  headset_speaker_audio_rx_actions,
  speaker_stereo_audio_rx_actions,
  handset_voice_tx_actions,
  headset_voice_tx_actions,
  speaker_voice_tx_actions,
  tty_tx_actions,
  mic1_rec_tx_actions
};

void __init msm_snddev_init(void)
{
		platform_add_devices(lge_snd_devices, ARRAY_SIZE(lge_snd_devices));
}
