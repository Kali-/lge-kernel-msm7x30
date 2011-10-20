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
#include <mach/qdsp5v2/marimba_profile.h>
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

#include <linux/fs.h>
#include <linux/fcntl.h> 
#include <mach/debug_mm.h>
#include <linux/fcntl.h> 
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/limits.h>

#include "lge_audio_amp.h"

/* define the value for BT_SCO */
#define BT_SCO_PCM_CTL_VAL (PCM_CTL__RPCM_WIDTH__LINEAR_V |\
				PCM_CTL__TPCM_WIDTH__LINEAR_V)
#define BT_SCO_DATA_FORMAT_PADDING (DATA_FORMAT_PADDING_INFO__RPCM_FORMAT_V |\
				DATA_FORMAT_PADDING_INFO__TPCM_FORMAT_V)
#define BT_SCO_AUX_CODEC_INTF   AUX_CODEC_INTF_CTL__PCMINTF_DATA_EN_V

static struct adie_codec_action_unit iearpiece_48KHz_osr256_actions[] =
	HANDSET_RX_48000_OSR_256;

static struct adie_codec_hwsetting_entry iearpiece_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = iearpiece_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(iearpiece_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile iearpiece_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = iearpiece_settings,
	.setting_sz = ARRAY_SIZE(iearpiece_settings),
};

static struct snddev_icodec_data snddev_iearpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "handset_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_SPKR,
	.profile = &iearpiece_profile,
	.channel_mode = 1,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
//20110524, sh80.choi@lge.com, Domestic WM Gain set [SOUND_DOMESTIC]_START
#if defined (CONFIG_LGE_DOMESTIC)
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -1600,
#else
//20110524, sh80.choi@lge.com, Domestic WM Gain set [SOUND_DOMESTIC]_END
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,
	.min_voice_rx_vol[VOC_NB_INDEX] = -2300,
#endif	//#if defined (CONFIG_LGE_TUNE_610)
	.max_voice_rx_vol[VOC_WB_INDEX] = -200,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1700
};

static struct platform_device msm_iearpiece_device = {
	.name = "snddev_icodec",
	.id = 0,
	.dev = { .platform_data = &snddev_iearpiece_data },
};

static struct adie_codec_action_unit imic_8KHz_osr256_actions[] =
	HANDSET_TX_8000_OSR_256;

static struct adie_codec_action_unit imic_16KHz_osr256_actions[] =
	HANDSET_TX_16000_OSR_256;

static struct adie_codec_action_unit imic_48KHz_osr256_actions[] =
	HANDSET_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry imic_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = imic_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(imic_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = imic_16KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(imic_16KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = imic_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(imic_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile imic_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = imic_settings,
	.setting_sz = ARRAY_SIZE(imic_settings),
};

static enum hsed_controller imic_pmctl_id[] = {PM_HSED_CONTROLLER_0};

static struct snddev_icodec_data snddev_imic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "handset_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_MIC,
	.profile = &imic_profile,
	.channel_mode = 1,
	.pmctl_id = imic_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(imic_pmctl_id),
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_imic_device = {
	.name = "snddev_icodec",
	.id = 1,
	.dev = { .platform_data = &snddev_imic_data },
};

static struct adie_codec_action_unit ihs_stereo_rx_48KHz_osr256_actions[] =
	HEADSET_STEREO_RX_LEGACY_48000_OSR_256;

static struct adie_codec_hwsetting_entry ihs_stereo_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_stereo_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_stereo_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_stereo_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ihs_stereo_rx_settings,
	.setting_sz = ARRAY_SIZE(ihs_stereo_rx_settings),
};

static struct snddev_icodec_data snddev_ihs_stereo_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_stereo_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_SPKR_STEREO,
	.profile = &ihs_stereo_rx_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pamp_on = lge_amp_hp_stereo_on,
	.pamp_off = lge_amp_off_all,
	.max_voice_rx_vol[VOC_NB_INDEX] = -700,		/* headset_audio be tunned */
	.min_voice_rx_vol[VOC_NB_INDEX] = -700,
	.max_voice_rx_vol[VOC_WB_INDEX] = -900,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2400
};

static struct platform_device msm_ihs_stereo_rx_device = {
	.name = "snddev_icodec",
	.id = 2,
	.dev = { .platform_data = &snddev_ihs_stereo_rx_data },
};

static struct adie_codec_action_unit ihs_mono_rx_48KHz_osr256_actions[] =
	HEADSET_RX_LEGACY_48000_OSR_256;

static struct adie_codec_hwsetting_entry ihs_mono_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_mono_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_mono_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_mono_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ihs_mono_rx_settings,
	.setting_sz = ARRAY_SIZE(ihs_mono_rx_settings),
};

static struct snddev_icodec_data snddev_ihs_mono_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_mono_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_SPKR_MONO,
	.profile = &ihs_mono_rx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = lge_amp_hp_phone_on,
	.pamp_off = lge_amp_off_all,
	.max_voice_rx_vol[VOC_NB_INDEX] = -300,		/* headset_voice be tunned */
	.min_voice_rx_vol[VOC_NB_INDEX] = -1800,
	.max_voice_rx_vol[VOC_WB_INDEX] = -900,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2400,

};

static struct platform_device msm_ihs_mono_rx_device = {
	.name = "snddev_icodec",
	.id = 3,
	.dev = { .platform_data = &snddev_ihs_mono_rx_data },
};

static struct adie_codec_action_unit ihs_mono_tx_8KHz_osr256_actions[] =
	HEADSET_MONO_TX_8000_OSR_256;

static struct adie_codec_action_unit ihs_mono_tx_16KHz_osr256_actions[] =
	HEADSET_MONO_TX_16000_OSR_256;

static struct adie_codec_action_unit ihs_mono_tx_48KHz_osr256_actions[] =
	HEADSET_MONO_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry ihs_mono_tx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ihs_mono_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_mono_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = ihs_mono_tx_16KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_mono_tx_16KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_mono_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ihs_mono_tx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_mono_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = ihs_mono_tx_settings,
	.setting_sz = ARRAY_SIZE(ihs_mono_tx_settings),
};

static struct snddev_icodec_data snddev_ihs_mono_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "headset_mono_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_MIC,
	.profile = &ihs_mono_tx_profile,
	.channel_mode = 1,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = msm_snddev_tx_route_config,
	.pamp_off = msm_snddev_tx_route_deconfig,
};

static struct platform_device msm_ihs_mono_tx_device = {
	.name = "snddev_icodec",
	.id = 6,
	.dev = { .platform_data = &snddev_ihs_mono_tx_data },
};

static struct adie_codec_action_unit ifmradio_handset_osr64_actions[] =
	FM_HANDSET_OSR_64;

static struct adie_codec_hwsetting_entry ifmradio_handset_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ifmradio_handset_osr64_actions,
		.action_sz = ARRAY_SIZE(ifmradio_handset_osr64_actions),
	}
};

static struct adie_codec_dev_profile ifmradio_handset_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ifmradio_handset_settings,
	.setting_sz = ARRAY_SIZE(ifmradio_handset_settings),
};

static struct snddev_icodec_data snddev_ifmradio_handset_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_FM),
	.name = "fmradio_handset_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_LP_FM_SPKR_PHONE_STEREO_RX,
	.profile = &ifmradio_handset_profile,
	.channel_mode = 1,
	.default_sample_rate = 8000,
	.pamp_on = NULL,
	.pamp_off = NULL,
	.dev_vol_type = SNDDEV_DEV_VOL_DIGITAL,
};

static struct platform_device msm_ifmradio_handset_device = {
	.name = "snddev_icodec",
	.id = 7,
	.dev = { .platform_data = &snddev_ifmradio_handset_data },
};


static struct adie_codec_action_unit ispeaker_rx_48KHz_osr256_actions[] =
   SPEAKER_RX_48000_OSR_256;

static struct adie_codec_hwsetting_entry ispeaker_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ispeaker_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ispeaker_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ispeaker_rx_settings,
	.setting_sz = ARRAY_SIZE(ispeaker_rx_settings),
};

static struct snddev_icodec_data snddev_ispeaker_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "speaker_stereo_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_STEREO,
	.profile = &ispeaker_rx_profile,
	.channel_mode = 1,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = lge_amp_spk_stereo_on,
	.pamp_off = lge_amp_spk_off,
	.max_voice_rx_vol[VOC_NB_INDEX] = 500,		/* speaker_audio be tunned */
	.min_voice_rx_vol[VOC_NB_INDEX] = 500,
	.max_voice_rx_vol[VOC_WB_INDEX] = 1000,
	.min_voice_rx_vol[VOC_WB_INDEX] = -500,
};

static struct platform_device msm_ispeaker_rx_device = {
	.name = "snddev_icodec",
	.id = 8,
	.dev = { .platform_data = &snddev_ispeaker_rx_data },

};

static struct adie_codec_action_unit ifmradio_speaker_osr64_actions[] =
	ADIE_AUXPGA_SPEAKER_RX;

static struct adie_codec_hwsetting_entry ifmradio_speaker_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ifmradio_speaker_osr64_actions,
		.action_sz = ARRAY_SIZE(ifmradio_speaker_osr64_actions),
	}
};

static struct adie_codec_dev_profile ifmradio_speaker_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ifmradio_speaker_settings,
	.setting_sz = ARRAY_SIZE(ifmradio_speaker_settings),
};

static struct snddev_icodec_data snddev_ifmradio_speaker_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "fmradio_speaker_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_STEREO,
	.profile = &ifmradio_speaker_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = lge_amp_fm_spk_stereo_on,
	.pamp_off = lge_amp_spk_off,
	.max_voice_rx_vol[VOC_NB_INDEX] = 500,
	.min_voice_rx_vol[VOC_NB_INDEX] = 500,
	.max_voice_rx_vol[VOC_WB_INDEX] = 1000,
	.min_voice_rx_vol[VOC_WB_INDEX] = -500,
};

static struct platform_device msm_ifmradio_speaker_device = {
	.name = "snddev_icodec",
	.id = 9,
	.dev = { .platform_data = &snddev_ifmradio_speaker_data },
};

static struct adie_codec_action_unit ifmradio_headset_osr64_actions[] =
	ADIE_AUXPGA_HEADSET_STEREO_RX;

static struct adie_codec_hwsetting_entry ifmradio_headset_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ifmradio_headset_osr64_actions,
		.action_sz = ARRAY_SIZE(ifmradio_headset_osr64_actions),
	}
};

static struct adie_codec_dev_profile ifmradio_headset_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ifmradio_headset_settings,
	.setting_sz = ARRAY_SIZE(ifmradio_headset_settings),
};

static struct snddev_icodec_data snddev_ifmradio_headset_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "fmradio_headset_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_SPKR_STEREO,
	.profile = &ifmradio_headset_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pamp_on = lge_amp_fm_hp_stereo_on,
	.pamp_off = lge_amp_off_all,
	.max_voice_rx_vol[VOC_NB_INDEX] = -700,
	.min_voice_rx_vol[VOC_NB_INDEX] = -700,
	.max_voice_rx_vol[VOC_WB_INDEX] = -900,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2400
};

static struct platform_device msm_ifmradio_headset_device = {
	.name = "snddev_icodec",
	.id = 10,
	.dev = { .platform_data = &snddev_ifmradio_headset_data },
};

static struct snddev_ecodec_data snddev_bt_sco_earpiece_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_rx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_SPKR,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
	.max_voice_rx_vol[VOC_NB_INDEX] = 400,		/* bt_sco be tunned */
//20110524, sh80.choi@lge.com, Domestic WM Gain set [SOUND_DOMESTIC]_START	
#if defined (CONFIG_LGE_DOMESTIC)	
	.min_voice_rx_vol[VOC_NB_INDEX] = -1600,
#else
//20110524, sh80.choi@lge.com, Domestic WM Gain set [SOUND_DOMESTIC]_END
	.min_voice_rx_vol[VOC_NB_INDEX] = -1100,
#endif	//#if defined (CONFIG_LGE_TUNE_610)
	.max_voice_rx_vol[VOC_WB_INDEX] = 400,
//20110524, sh80.choi@lge.com, Domestic WM Gain set [SOUND_DOMESTIC]_START
#if defined (CONFIG_LGE_DOMESTIC)	
	.min_voice_rx_vol[VOC_WB_INDEX] = -1600,
#else
//20110524, sh80.choi@lge.com, Domestic WM Gain set [SOUND_DOMESTIC]_END
	.min_voice_rx_vol[VOC_WB_INDEX] = -1100,
#endif	//#if defined (CONFIG_LGE_TUNE_610)
};

static struct snddev_ecodec_data snddev_bt_sco_mic_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "bt_sco_tx",
	.copp_id = 1,
	.acdb_id = ACDB_ID_BT_SCO_MIC,
	.channel_mode = 1,
	.conf_pcm_ctl_val = BT_SCO_PCM_CTL_VAL,
	.conf_aux_codec_intf = BT_SCO_AUX_CODEC_INTF,
	.conf_data_format_padding_val = BT_SCO_DATA_FORMAT_PADDING,
};

struct platform_device msm_bt_sco_earpiece_device = {
	.name = "msm_snddev_ecodec",
	.id = 0,
	.dev = { .platform_data = &snddev_bt_sco_earpiece_data },
};

struct platform_device msm_bt_sco_mic_device = {
	.name = "msm_snddev_ecodec",
	.id = 1,
	.dev = { .platform_data = &snddev_bt_sco_mic_data },
};

static struct adie_codec_action_unit itty_hs_mono_tx_8KHz_osr256_actions[] =
	TTY_HEADSET_MONO_TX_8000_OSR_256;

static struct adie_codec_hwsetting_entry itty_hs_mono_tx_settings[] = {
	/* 8KHz, 16KHz, 48KHz TTY Tx devices can shared same set of actions */
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = itty_hs_mono_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = itty_hs_mono_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = itty_hs_mono_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_tx_8KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile itty_hs_mono_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = itty_hs_mono_tx_settings,
	.setting_sz = ARRAY_SIZE(itty_hs_mono_tx_settings),
};

static struct snddev_icodec_data snddev_itty_hs_mono_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE | SNDDEV_CAP_TTY),
	.name = "tty_headset_mono_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_TTY_HEADSET_MIC,
	.profile = &itty_hs_mono_tx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_itty_hs_mono_tx_device = {
	.name = "snddev_icodec",
	.id = 16,
	.dev = { .platform_data = &snddev_itty_hs_mono_tx_data },
};

static struct adie_codec_action_unit itty_hs_mono_rx_8KHz_osr256_actions[] =
	TTY_HEADSET_MONO_RX_CLASS_D_8000_OSR_256;

static struct adie_codec_action_unit itty_hs_mono_rx_16KHz_osr256_actions[] =
	TTY_HEADSET_MONO_RX_CLASS_D_16000_OSR_256;

static struct adie_codec_action_unit itty_hs_mono_rx_48KHz_osr256_actions[] =
	TTY_HEADSET_MONO_RX_CLASS_D_48000_OSR_256;

static struct adie_codec_hwsetting_entry itty_hs_mono_rx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = itty_hs_mono_rx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_rx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 16000,
		.osr = 256,
		.actions = itty_hs_mono_rx_16KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_rx_16KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = itty_hs_mono_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(itty_hs_mono_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile itty_hs_mono_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = itty_hs_mono_rx_settings,
	.setting_sz = ARRAY_SIZE(itty_hs_mono_rx_settings),
};

static struct snddev_icodec_data snddev_itty_hs_mono_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE | SNDDEV_CAP_TTY),
	.name = "tty_headset_mono_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_TTY_HEADSET_SPKR,
	.profile = &itty_hs_mono_rx_profile,
	.channel_mode = 1,
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
	.max_voice_rx_vol[VOC_NB_INDEX] = 0,
	.min_voice_rx_vol[VOC_NB_INDEX] = 0,
	.max_voice_rx_vol[VOC_WB_INDEX] = 0,
	.min_voice_rx_vol[VOC_WB_INDEX] = 0,
};

static struct platform_device msm_itty_hs_mono_rx_device = {
	.name = "snddev_icodec",
	.id = 17,
	.dev = { .platform_data = &snddev_itty_hs_mono_rx_data },
};

static struct adie_codec_action_unit ispeaker_tx_8KHz_osr256_actions[] =
	SPEAKER_TX_8000_OSR_256;

static struct adie_codec_action_unit ispeaker_tx_48KHz_osr256_actions[] =
	SPEAKER_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry ispeaker_tx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ispeaker_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_tx_8KHz_osr256_actions),
	},
	{ /* 8KHz profile is good for 16KHz */
		.freq_plan = 16000,
		.osr = 256,
		.actions = ispeaker_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ispeaker_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_tx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ispeaker_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = ispeaker_tx_settings,
	.setting_sz = ARRAY_SIZE(ispeaker_tx_settings),
};

static enum hsed_controller ispk_pmctl_id[] = {PM_HSED_CONTROLLER_1};

static struct snddev_icodec_data snddev_ispeaker_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_mono_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC,
	.profile = &ispeaker_tx_profile,
	.channel_mode = 1,
	.pmctl_id = ispk_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(ispk_pmctl_id),
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_ispeaker_tx_device = {
	.name = "snddev_icodec",
	.id = 18,
	.dev = { .platform_data = &snddev_ispeaker_tx_data },
};


static struct adie_codec_action_unit
	ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions[] =
	HEADSET_STEREO_SPEAKER_STEREO_RX_CAPLESS_48000_OSR_256;


static struct adie_codec_hwsetting_entry
	ihs_stereo_speaker_stereo_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions,
		.action_sz =
		ARRAY_SIZE(ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ihs_stereo_speaker_stereo_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ihs_stereo_speaker_stereo_rx_settings,
	.setting_sz = ARRAY_SIZE(ihs_stereo_speaker_stereo_rx_settings),
};

static struct snddev_icodec_data snddev_ihs_stereo_speaker_stereo_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "headset_stereo_speaker_stereo_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HEADSET_STEREO_PLUS_SPKR_STEREO_RX,
	.profile = &ihs_stereo_speaker_stereo_rx_profile,
	.channel_mode = 2,
	.default_sample_rate = 48000,
	.pamp_on = lge_amp_hp_stereo_spk_stereo_on,
	.pamp_off = lge_amp_off_all,
#ifndef CONFIG_MACH_LGE
	.voltage_on = msm_snddev_hsed_voltage_on,
	.voltage_off = msm_snddev_hsed_voltage_off,
#endif
	.max_voice_rx_vol[VOC_NB_INDEX] = -200,		/* headset_speaker be tunned */
	.min_voice_rx_vol[VOC_NB_INDEX] = -200,
	.max_voice_rx_vol[VOC_WB_INDEX] = -500,
	.min_voice_rx_vol[VOC_WB_INDEX] = -2000,
};

static struct platform_device msm_ihs_stereo_speaker_stereo_rx_device = {
	.name = "snddev_icodec",
	.id = 21,
	.dev = { .platform_data = &snddev_ihs_stereo_speaker_stereo_rx_data },
};

static struct snddev_mi2s_data snddev_mi2s_stereo_rx_data = {
	.capability = SNDDEV_CAP_RX ,
	.name = "hdmi_stereo_rx",
	.copp_id = 3,
	.acdb_id = ACDB_ID_HDMI,
	.channel_mode = 2,
	.sd_lines = MI2S_SD_0,
	.route = msm_snddev_tx_route_config,
	.deroute = msm_snddev_tx_route_deconfig,
	.default_sample_rate = 48000,
};

static struct platform_device msm_snddev_mi2s_stereo_rx_device = {
	.name = "snddev_mi2s",
	.id = 0,
	.dev = { .platform_data = &snddev_mi2s_stereo_rx_data },
};


static struct snddev_mi2s_data snddev_mi2s_fm_tx_data = {
	.capability = SNDDEV_CAP_TX ,
	.name = "fmradio_stereo_tx",
	.copp_id = 2,
	.acdb_id = ACDB_ID_FM_TX,
	.channel_mode = 2,
	.sd_lines = MI2S_SD_3,
	.route = NULL,
	.deroute = NULL,
	.default_sample_rate = 48000,
};

static struct platform_device  msm_snddev_mi2s_fm_tx_device = {
	.name = "snddev_mi2s",
	.id = 1,
	.dev = { .platform_data = &snddev_mi2s_fm_tx_data},
};

static struct snddev_virtual_data snddev_a2dp_tx_data = {
	.capability = SNDDEV_CAP_TX,
	.name = "a2dp_tx",
	.copp_id = 5,
	.acdb_id = PSEUDO_ACDB_ID,
};

static struct snddev_virtual_data snddev_a2dp_rx_data = {
	.capability = SNDDEV_CAP_RX,
	.name = "a2dp_rx",
	.copp_id = 2,
	.acdb_id = PSEUDO_ACDB_ID,
};

static struct platform_device msm_a2dp_rx_device = {
	.name = "snddev_virtual",
	.id = 0,
	.dev = { .platform_data = &snddev_a2dp_rx_data },
};

static struct platform_device msm_a2dp_tx_device = {
	.name = "snddev_virtual",
	.id = 1,
	.dev = { .platform_data = &snddev_a2dp_tx_data },
};

static struct snddev_virtual_data snddev_uplink_rx_data = {
	.capability = SNDDEV_CAP_RX,
	.name = "uplink_rx",
	.copp_id = 5,
	.acdb_id = PSEUDO_ACDB_ID,
};

static struct platform_device msm_uplink_rx_device = {
	.name = "snddev_virtual",
	.id = 2,
	.dev = { .platform_data = &snddev_uplink_rx_data },
};

/* LGE Additional Audio devices */
static struct adie_codec_action_unit ispeaker_in_call_rx_48KHz_osr256_actions[] =
   SPEAKER_IN_CALL_RX_48000_OSR_256;

static struct adie_codec_hwsetting_entry ispeaker_in_call_rx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ispeaker_in_call_rx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_in_call_rx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ispeaker_in_call_rx_profile = {
	.path_type = ADIE_CODEC_RX,
	.settings = ispeaker_in_call_rx_settings,
	.setting_sz = ARRAY_SIZE(ispeaker_in_call_rx_settings),
};

static struct snddev_icodec_data snddev_ispeaker_in_call_rx_data = {
	.capability = (SNDDEV_CAP_RX | SNDDEV_CAP_VOICE),
	.name = "speaker_in_call_rx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_MONO,
	.profile = &ispeaker_in_call_rx_profile,
	.channel_mode = 1,
	.pmctl_id = NULL,
	.pmctl_id_sz = 0,
	.default_sample_rate = 48000,
	.pamp_on = lge_amp_spk_phone_on,
	.pamp_off = lge_amp_spk_off,
#if defined(CONFIG_LGE_DOMESTIC)
	.max_voice_rx_vol[VOC_NB_INDEX] = 1000,		/* speaker_voice be tunned */
	.min_voice_rx_vol[VOC_NB_INDEX] = -500,
	.max_voice_rx_vol[VOC_WB_INDEX] = 1000,
	.min_voice_rx_vol[VOC_WB_INDEX] = -500,
#else
	.max_voice_rx_vol[VOC_NB_INDEX] = 1000,		/* speaker_voice be tunned */
	.min_voice_rx_vol[VOC_NB_INDEX] = -1200,
	.max_voice_rx_vol[VOC_WB_INDEX] = 1000,
	.min_voice_rx_vol[VOC_WB_INDEX] = -1200,
#endif
};

static struct platform_device msm_ispeaker_in_call_rx_device = {
	.name = "snddev_icodec",
	.id = 31,
	.dev = { .platform_data = &snddev_ispeaker_in_call_rx_data },

};

static struct adie_codec_action_unit ispeaker_in_call_tx_8KHz_osr256_actions[] =
	SPEAKER_IN_CALL_TX_8000_OSR_256;

static struct adie_codec_action_unit ispeaker_in_call_tx_48KHz_osr256_actions[] =
	SPEAKER_IN_CALL_TX_48000_OSR_256;

static struct adie_codec_hwsetting_entry ispeaker_in_call_tx_settings[] = {
	{
		.freq_plan = 8000,
		.osr = 256,
		.actions = ispeaker_in_call_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_in_call_tx_8KHz_osr256_actions),
	},
	{ /* 8KHz profile is good for 16KHz */
		.freq_plan = 16000,
		.osr = 256,
		.actions = ispeaker_in_call_tx_8KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_in_call_tx_8KHz_osr256_actions),
	},
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = ispeaker_in_call_tx_48KHz_osr256_actions,
		.action_sz = ARRAY_SIZE(ispeaker_in_call_tx_48KHz_osr256_actions),
	}
};

static struct adie_codec_dev_profile ispeaker_in_call_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = ispeaker_in_call_tx_settings,
	.setting_sz = ARRAY_SIZE(ispeaker_in_call_tx_settings),
};

static struct snddev_icodec_data snddev_ispeaker_in_call_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "speaker_in_call_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC,
	.profile = &ispeaker_in_call_tx_profile,
	.channel_mode = 1,
	.pmctl_id = ispk_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(ispk_pmctl_id),
	.default_sample_rate = 48000,
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_ispeaker_in_call_tx_device = {
	.name = "snddev_icodec",
	.id = 32,
	.dev = { .platform_data = &snddev_ispeaker_in_call_tx_data },
};

#if defined(CONFIG_LGE_DOMESTIC)
//=======================================================================================
// Handset rec TX (recording through main mic)    DEVICE_REC_TX 
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
	.acdb_id = ACDB_ID_HANDSET_MIC,
	.profile = &mic1_rec_tx_profile,
	.channel_mode = 1,
	.pmctl_id = imic_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(imic_pmctl_id),
	.default_sample_rate = 48000,
};

static struct platform_device msm_mic1_rec_tx = {
	.name = "snddev_icodec",
	.id = 33,
	.dev = { .platform_data = &mic1_rec_tx_data },
};

//=======================================================================================
// Handset rec TX (recording through main mic)    DEVICE_CAM_TX 
//=======================================================================================
struct adie_codec_action_unit cam_rec_tx_actions[] =
	SPEAKER_TX_48000_OSR_256_REC;

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
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC_ENDFIRE,
	.profile = &cam_rec_tx_profile,
	.channel_mode = 1,
	.pmctl_id = imic_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(imic_pmctl_id),
	.default_sample_rate = 48000,
};

static struct platform_device msm_cam_rec_tx = {
	.name = "snddev_icodec",
	.id = 34,
	.dev = { .platform_data = &cam_rec_tx_data },
};

//=======================================================================================
// Handset rec TX (recording through main mic)    DEVICE_ASR_TX 
//=======================================================================================
struct adie_codec_action_unit mic1_asr_tx_actions[] =
	SPEAKER_TX_48000_OSR_256_ASR;

static struct adie_codec_hwsetting_entry mic1_asr_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = mic1_asr_tx_actions,
		.action_sz = ARRAY_SIZE(mic1_asr_tx_actions),
	}
};

static struct adie_codec_dev_profile mic1_asr_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = mic1_asr_tx_settings,
	.setting_sz = ARRAY_SIZE(mic1_asr_tx_settings),
};

static struct snddev_icodec_data mic1_asr_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "voice_recog_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_MIC,
	.profile = &mic1_asr_tx_profile,
	.channel_mode = 1,
	.pmctl_id = imic_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(imic_pmctl_id),
	.default_sample_rate = 48000,
};

static struct platform_device msm_mic1_asr_tx = {
	.name = "snddev_icodec",
	.id = 35,
	.dev = { .platform_data = &mic1_asr_tx_data },
};
#else /*Victor Global START*/

//=======================================================================================
// Handset rec TX (recording through main mic)    DEVICE_REC_TX 
//=======================================================================================
struct adie_codec_action_unit mic1_rec_tx_actions[] =
	SPEAKER_TX_8000_OSR_256_REC;

static struct adie_codec_hwsetting_entry mic1_rec_tx_settings[] = {
	{
		.freq_plan = 8000,
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
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC,
	.profile = &mic1_rec_tx_profile,
	.channel_mode = 1,
	.pmctl_id = ispk_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(ispk_pmctl_id),
	.default_sample_rate = 8000,
};

static struct platform_device msm_mic1_rec_tx = {
	.name = "snddev_icodec",
	.id = 33,
	.dev = { .platform_data = &mic1_rec_tx_data },
};

//=======================================================================================
// Handset rec TX (recording through main mic)    DEVICE_CAM_TX 
//=======================================================================================
struct adie_codec_action_unit cam_rec_tx_actions[] =
	SPEAKER_TX_8000_OSR_256_CAM;

static struct adie_codec_hwsetting_entry cam_rec_tx_settings[] = {
	{
		.freq_plan = 8000,
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
	.acdb_id = ACDB_ID_SPKR_PHONE_MIC,
	.profile = &cam_rec_tx_profile,
	.channel_mode = 1,
	.pmctl_id = ispk_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(ispk_pmctl_id),
	.default_sample_rate = 8000,
	.pamp_on = NULL,
	.pamp_off = NULL,
};

static struct platform_device msm_cam_rec_tx = {
	.name = "snddev_icodec",
	.id = 34,
	.dev = { .platform_data = &cam_rec_tx_data },
};

//=======================================================================================
// Handset rec TX (recording through main mic)    DEVICE_ASR_TX 
//=======================================================================================
struct adie_codec_action_unit mic1_asr_tx_actions[] =
	SPEAKER_TX_48000_OSR_256_ASR;

static struct adie_codec_hwsetting_entry mic1_asr_tx_settings[] = {
	{
		.freq_plan = 48000,
		.osr = 256,
		.actions = mic1_asr_tx_actions,
		.action_sz = ARRAY_SIZE(mic1_asr_tx_actions),
	}
};

static struct adie_codec_dev_profile mic1_asr_tx_profile = {
	.path_type = ADIE_CODEC_TX,
	.settings = mic1_asr_tx_settings,
	.setting_sz = ARRAY_SIZE(mic1_asr_tx_settings),
};

static struct snddev_icodec_data mic1_asr_tx_data = {
	.capability = (SNDDEV_CAP_TX | SNDDEV_CAP_VOICE),
	.name = "voice_recog_tx",
	.copp_id = 0,
	.acdb_id = ACDB_ID_HANDSET_MIC,
	.profile = &mic1_asr_tx_profile,
	.channel_mode = 1,
	.pmctl_id = imic_pmctl_id,
	.pmctl_id_sz = ARRAY_SIZE(imic_pmctl_id),
	.default_sample_rate = 48000,
};

static struct platform_device msm_mic1_asr_tx = {
	.name = "snddev_icodec",
	.id = 35,
	.dev = { .platform_data = &mic1_asr_tx_data },
};
#endif /*Victor Global END*/

static struct platform_device *snd_devices_surf[] __initdata = {
	&msm_iearpiece_device,
	&msm_imic_device,
	&msm_ihs_stereo_rx_device,
	&msm_ihs_mono_rx_device,
	&msm_ihs_mono_tx_device,
	&msm_bt_sco_earpiece_device,
	&msm_bt_sco_mic_device,
	&msm_ifmradio_handset_device,
	&msm_ispeaker_rx_device,
	&msm_ifmradio_speaker_device,
	&msm_ifmradio_headset_device,
	&msm_itty_hs_mono_tx_device,
	&msm_itty_hs_mono_rx_device,
	&msm_ispeaker_tx_device,
	&msm_ihs_stereo_speaker_stereo_rx_device,
	&msm_a2dp_rx_device,
	&msm_a2dp_tx_device,
	&msm_snddev_mi2s_stereo_rx_device,
	&msm_snddev_mi2s_fm_tx_device,
	&msm_uplink_rx_device,
	/* LGE Additional Audio devices */
	&msm_ispeaker_in_call_rx_device,
	&msm_ispeaker_in_call_tx_device,
	&msm_mic1_rec_tx,
	&msm_cam_rec_tx,
	&msm_mic1_asr_tx,
};

#ifdef CONFIG_LGE_DOMESTIC
static char * strtok(char *s, const char *delim);
static char *strtok_r(char *s, const char *delim, char **last);
static char strtol(const char *nptr, char **endptr, int base);
static int atoi(const char *str);

enum {
  QTR_EARPIECE = 0,
  QTR_HEADSET_VOICE,
  QTR_SPEAKER_VOICE,
  QTR_TTY,
  QTR_HEADSET_AUDIO,
  QTR_HEADSET_SPEAKER,
  QTR_SPEAKER_AUDIO,
  QTR_HANDSET_MIC,
  QTR_HEADSET_MIC,
  QTR_SPEAKER_MIC,
  QTR_TTY_MIC,
  QTR_REC_MIC,
//20110618 mikyoung.chang@lge.com add devices for FMC, SKT FMC, Camcording [START]
#if defined(CONFIG_LGE_DOMESTIC)
  QTR_CAM_REC_TX,
  QTR_ASR_TX, 
#endif
 //20110618 mikyoung.chang@lge.com add devices for FMC, SKT FMC, Camcording [END]
  QTR_CAL_MAX
};

enum {
  VOICE_EARPIECE = 0,
  VOICE_HEADSET,
  VOICE_SPEAKER,
  VOICE_BT
};

int atoi(const char *str)
{
  int nSign = 0, nSum = 0;

  if ((*str == '-') || (*str >= '0' && *str <= '9'))  {
    if (*str == '-')    {
      nSign = 1;
      str++;
    }

    while(*str >= '0' && *str <= '9')
    {
      nSum = (nSum * 10) + (*str - '0');
      str++;
    }

    if (nSign)
      return (-1) * nSum;
    else
      return nSum;
  }
  else
    return 0;
}
  
char *strtok(char *s, const char *delim)
{
	static char *last;

	return strtok_r(s, delim, &last);
}

char *strtok_r(char *s, const char *delim, char **last)
{
	char *spanp;
	int c, sc;
	char *tok;


	if (s == NULL && (s = *last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		*last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

char strtol(const char *nptr, char **endptr, int base)
{
	const char *s;
	long acc, cutoff;
	int c;
	int neg, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	s = nptr;
	do {
		c = (unsigned char) *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? LONG_MIN : LONG_MAX;
	cutlim = cutoff % base;
	cutoff /= base;
	if (neg) {
		if (cutlim > 0) {
			cutlim -= base;
			cutoff += 1;
		}
		cutlim = -cutlim;
	}
	for (acc = 0, any = 0;; c = (unsigned char) *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0)
			continue;
		if (neg) {
			if (acc < cutoff || (acc == cutoff && c > cutlim)) {
				any = -1;
				acc = LONG_MIN;
			} else {
				any = 1;
				acc *= base;
				acc -= c;
			}
		} else {
			if (acc > cutoff || (acc == cutoff && c > cutlim)) {
				any = -1;
				acc = LONG_MAX;
			} else {
				any = 1;
				acc *= base;
				acc += c;
			}
		}
	}
	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);
	return (char)acc;
}

static  s32 voice_level_data[4][2] = {
    {0,-1700},
    {-700,-2200},
    {500,-1500},
//20110524, sh80.choi@lge.com, Domestic WM Gain set [SOUND_DOMESTIC]_START
#if defined (CONFIG_LGE_DOMESTIC)
    {400,-1600}};
#else
//20110524, sh80.choi@lge.com, Domestic WM Gain set [SOUND_DOMESTIC]_END
	{400,-1100}};
#endif	//#if defined (CONFIG_LGE_TUNE_610) 

void set_voicecal_data(void)
{
  struct file *ampcal_Filp = NULL;
  ssize_t read_size = 0;
  char bmp_data_array[900];
  char *token = NULL;
  char *separator = "\n,:";
  int nIndex1 = 0, nIndex2 = 0;

 #define earpiece_voice_rx_data snddev_iearpiece_data
 #define headset_stereo_voice_rx_data snddev_ihs_mono_rx_data
 #define speaker_stereo_voice_rx_data snddev_ispeaker_in_call_rx_data
 #define bt_sco_voice_rx_data snddev_bt_sco_earpiece_data

  set_fs(get_ds());

  ampcal_Filp = filp_open("/system/sounds/tuning/voice.txt", O_RDONLY |O_LARGEFILE, 0);

  if(IS_ERR_OR_NULL(ampcal_Filp))
  {
    MM_INFO("open fail\n");
    return;
  }

  read_size = ampcal_Filp->f_op->read(ampcal_Filp, bmp_data_array, 900, &ampcal_Filp->f_pos);
  
  token = strtok(bmp_data_array, separator);


    MM_INFO("11voice READ :::: %d\n",  earpiece_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
    MM_INFO("11voice READ :::: %d\n",  headset_stereo_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
    MM_INFO("11voice READ :::: %d\n",  speaker_stereo_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
    MM_INFO("11voice READ :::: %d\n",  bt_sco_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);

  while ((token = strtok(NULL, separator)) != NULL)
  {
    token = strtok(NULL, separator);
//    voice_level_data[nIndex1][nIndex2] = (s32)strtol(token, NULL, 10);    
    voice_level_data[nIndex1][nIndex2] = atoi(token);    
    MM_INFO("voice READ :::: %s\n", token);
    MM_INFO("voice READ :::: %d\n", voice_level_data[nIndex1][nIndex2]);
    nIndex2++;
    if (nIndex2 == 2)
    {
      nIndex1++;
      nIndex2 = 0;
    }
  }

  earpiece_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX] = voice_level_data[VOICE_EARPIECE][0];
  earpiece_voice_rx_data.min_voice_rx_vol[VOC_NB_INDEX] = voice_level_data[VOICE_EARPIECE][1];
  earpiece_voice_rx_data.max_voice_rx_vol[VOC_WB_INDEX] = voice_level_data[VOICE_EARPIECE][0];
  earpiece_voice_rx_data.min_voice_rx_vol[VOC_WB_INDEX] = voice_level_data[VOICE_EARPIECE][0];

  headset_stereo_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX] = voice_level_data[VOICE_HEADSET][0];
  headset_stereo_voice_rx_data.min_voice_rx_vol[VOC_NB_INDEX] = voice_level_data[VOICE_HEADSET][1];
  headset_stereo_voice_rx_data.max_voice_rx_vol[VOC_WB_INDEX] = voice_level_data[VOICE_HEADSET][0];
  headset_stereo_voice_rx_data.min_voice_rx_vol[VOC_WB_INDEX] = voice_level_data[VOICE_HEADSET][0];

  speaker_stereo_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX] = voice_level_data[VOICE_SPEAKER][0];
  speaker_stereo_voice_rx_data.min_voice_rx_vol[VOC_NB_INDEX] = voice_level_data[VOICE_SPEAKER][1];
  speaker_stereo_voice_rx_data.max_voice_rx_vol[VOC_WB_INDEX] = voice_level_data[VOICE_SPEAKER][0];
  speaker_stereo_voice_rx_data.min_voice_rx_vol[VOC_WB_INDEX] = voice_level_data[VOICE_SPEAKER][0];

  bt_sco_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX] = voice_level_data[VOICE_BT][0];
  bt_sco_voice_rx_data.min_voice_rx_vol[VOC_NB_INDEX] = voice_level_data[VOICE_BT][1];
  bt_sco_voice_rx_data.max_voice_rx_vol[VOC_WB_INDEX] = voice_level_data[VOICE_BT][0];
  bt_sco_voice_rx_data.min_voice_rx_vol[VOC_WB_INDEX] = voice_level_data[VOICE_BT][0];

    MM_INFO("22voice READ :::: %d\n",  earpiece_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
    MM_INFO("22voice READ :::: %d\n",  headset_stereo_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
    MM_INFO("22voice READ :::: %d\n",  speaker_stereo_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
    MM_INFO("22voice READ :::: %d\n",  bt_sco_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
}
//LGE_UPDATE_E

static int QTR_index_data[QTR_CAL_MAX][3] = {
    {17,-1,-1},
    {48,46,47},
    {15,-1,-1},
    {61,59,60},
    {48,46,47},
    {41,39,40},
    {15,-1,-1},
    {16,11,-1},
    {16,11,-1},
    {15,11,-1},
 	//20110618 mikyoung.chang@lge.com add devices for FMC, SKT FMC, Camcording [START]
#if defined(CONFIG_LGE_DOMESTIC)
    {16,11,-1},
    {16,11,-1},
    {15,11,-1},
    {16,11,-1},
 #else
    {15,11,-1},
    {15,11,-1},
#endif
 //20110618 mikyoung.chang@lge.com add devices for FMC, SKT FMC, Camcording [END]
	};

static u8 QTR_cal_data[QTR_CAL_MAX][3] = {
    {0x04, 0xFF,0xFF},
    {0x02,0x0,0x0},
    {0x03,0xFF,0xFF},
    {0x0,0x1,0x1},
    {0x0,0x10,0x10},
    {0x0,0x10,0x10},
    {0x00,0xFF,0xFF},
    {0x08,0xD0,0xFF},
    {0x06,0xC8,0xFF},
    {0x07,0xC1,0xFF},
	//20110618 mikyoung.chang@lge.com add devices for FMC, SKT FMC, Camcording [START]
#if defined(CONFIG_LGE_DOMESTIC)
    {0x11,0xd0,0xFF},
    {0x1C,0xd0,0xFF},
    {0xf,0xd0,0xFF},
    {0x1C,0xd0,0xFF},
 #else
    {0x11,0xd0,0xFF},
    {0x1C,0xd0,0xFF},
#endif
	 //20110618 mikyoung.chang@lge.com add devices for FMC, SKT FMC, Camcording [END]
	};

static struct adie_codec_action_unit *codec_cal[QTR_CAL_MAX] = {
   iearpiece_48KHz_osr256_actions, //handset_rx,
   ihs_mono_rx_48KHz_osr256_actions, //headset_mono_rx,
   ispeaker_in_call_rx_48KHz_osr256_actions, //speaker_in_call_rx,
   itty_hs_mono_rx_48KHz_osr256_actions, //tty_headset_mono_rx,
   ihs_stereo_rx_48KHz_osr256_actions, //headset_stereo_rx,
   ihs_stereo_speaker_stereo_rx_48KHz_osr256_actions, //headset_stereo_speaker_stereo_rx,
   ispeaker_rx_48KHz_osr256_actions, //speaker_stereo_rx,
   imic_48KHz_osr256_actions, //handset_tx,
   ihs_mono_tx_48KHz_osr256_actions, //headset_mono_tx,
   ispeaker_in_call_tx_48KHz_osr256_actions, //speaker_in_call_tx,
   mic1_rec_tx_actions, //tty_headset_mono_tx,
   cam_rec_tx_actions, //speaker_mono_tx,
 //20110618 mikyoung.chang@lge.com add devices for FMC, SKT FMC, Camcording [START]
#if defined(CONFIG_LGE_DOMESTIC)
	ispeaker_tx_48KHz_osr256_actions, //speaker_mono_tx,
	mic1_asr_tx_actions,
#endif
 //20110618 mikyoung.chang@lge.com add devices for FMC, SKT FMC, Camcording [END]
};
	
void set_QTRcal_data(void)
{
  struct file *ampcal_Filp = NULL;
  ssize_t read_size = 0;
  char bmp_data_array[900];
  char *token = NULL;
  char *separator = "\n,:";
  mm_segment_t old_fs=get_fs();
  int nIndex1 = 0, nIndex2 = 0;
  u8 reg=0, mask=0, val=0;
  int i, j;
  struct adie_codec_action_unit *temp;
  bool bReadCalData = true;

  set_fs(get_ds());
  ampcal_Filp = filp_open("/system/sounds/tuning/QTR.txt", O_RDONLY |O_LARGEFILE, 0);

  if(IS_ERR_OR_NULL(ampcal_Filp))
  {
     MM_INFO("QTR.txt open fail\n");
     set_fs(old_fs);	
#if 0	 
	for (i = 0; i < QTR_CAL_MAX; i++)
	{
	 temp = codec_cal[i];
	 for (j = 0; j < 3; j++)
	 {
		 if (QTR_cal_data[i][j] != 0xFF && QTR_index_data[i][j] != -1)
		 {
			 ADIE_CODEC_UNPACK_ENTRY(temp[QTR_index_data[i][j]].action,reg, mask, val);
			 MM_INFO("QTR CHANGE %d -->> %d\n", val, QTR_cal_data[i][j]);
			 temp[QTR_index_data[i][j]].action = ADIE_CODEC_PACK_ENTRY(reg, 0xFF, QTR_cal_data[i][j]);
		 }
	 }
	}
#endif	
    return;
  }

  read_size = ampcal_Filp->f_op->read(ampcal_Filp, bmp_data_array, 900, &ampcal_Filp->f_pos);
  
  token = strtok(bmp_data_array, separator);
      MM_INFO("open fail4\n");

  if ((u8)strtol(token, NULL, 16) == 0)
  {
    bReadCalData = false;
  }
  else
  {
    bReadCalData = true;
  }
  if (bReadCalData == false)
  {
      filp_close(ampcal_Filp,NULL);
      set_fs(old_fs);
      return;
  }
      MM_INFO("open fail4\n");

  
  while ((token = strtok(NULL, separator)) != NULL)
  {
    token = strtok(NULL, separator);
    QTR_cal_data[nIndex1][nIndex2] = (u8)strtol(token, NULL, 16);    
    MM_INFO("QTR READ :::: %d\n", QTR_cal_data[nIndex1][nIndex2]);
    nIndex2++;
    if (nIndex2 == 3)
    {
      nIndex1++;
      nIndex2 = 0;
    }
  }
  
  for (i = 0; i < QTR_CAL_MAX; i++)
  {
     temp = codec_cal[i];
     for (j = 0; j < 3; j++)
     {
        if (QTR_cal_data[i][j] != 0xFF && QTR_index_data[i][j] != -1)
        {
            ADIE_CODEC_UNPACK_ENTRY(temp[QTR_index_data[i][j]].action,reg, mask, val);
            MM_INFO("QTR CHANGE %d -->> %d\n", val, QTR_cal_data[i][j]);
            temp[QTR_index_data[i][j]].action = ADIE_CODEC_PACK_ENTRY(reg, 0xFF, QTR_cal_data[i][j]);
        }
     }
  }

  filp_close(ampcal_Filp,NULL);

  set_fs(old_fs);

  if (bReadCalData == true)
        set_voicecal_data();

}
#endif


void __init msm_snddev_init(void)
{
	platform_add_devices(snd_devices_surf, ARRAY_SIZE(snd_devices_surf));
}
