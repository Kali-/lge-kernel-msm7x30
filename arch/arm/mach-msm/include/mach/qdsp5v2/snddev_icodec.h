/* Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __MACH_QDSP5_V2_SNDDEV_ICODEC_H
#define __MACH_QDSP5_V2_SNDDEV_ICODEC_H
#include <linux/mfd/msm-adie-codec.h>
#include <mach/qdsp5v2/audio_def.h>
#include <mach/pmic.h>


#ifdef CONFIG_LGE_MODEL_E739
void set_ampcal_data(void);
void set_QTRcal_data(void);
void set_voicecal_data(void);

#define DEVICE_ID_INTERNAL_HANDSET_VOICE_RX 0
#define DEVICE_ID_INTERNAL_HEADSET_VOICE_RX   DEVICE_ID_INTERNAL_HANDSET_VOICE_RX+1
#define DEVICE_ID_INTERNAL_SPEAKER_VOICE_RX   DEVICE_ID_INTERNAL_HEADSET_VOICE_RX+1
#define DEVICE_ID_INTERNAL_TTY_RX                         DEVICE_ID_INTERNAL_SPEAKER_VOICE_RX+1
#define DEVICE_ID_INTERNAL_HANDSET_AUDIO_RX  DEVICE_ID_INTERNAL_TTY_RX+1
#define DEVICE_ID_INTERNAL_HEADSET_AUDIO_RX   DEVICE_ID_INTERNAL_HANDSET_AUDIO_RX+1
#define DEVICE_ID_INTERNAL_SPEAKER_AUDIO_RX   DEVICE_ID_INTERNAL_HEADSET_AUDIO_RX+1
#define DEVICE_ID_INTERNAL_HEADSET_SPEAKER_RX   DEVICE_ID_INTERNAL_SPEAKER_AUDIO_RX+1

#define DEVICE_ID_INTERNAL_HANDSET_VOICE_TX   DEVICE_ID_INTERNAL_HEADSET_SPEAKER_RX+1
#define DEVICE_ID_INTERNAL_HEADSET_VOICE_TX   DEVICE_ID_INTERNAL_HANDSET_VOICE_TX+1
#define DEVICE_ID_INTERNAL_SPEAKER_VOICE_TX   DEVICE_ID_INTERNAL_HEADSET_VOICE_TX+1
#define DEVICE_ID_INTERNAL_TTY_TX                         DEVICE_ID_INTERNAL_SPEAKER_VOICE_TX+1
#define DEVICE_ID_INTERNAL_MIC1_REC                    DEVICE_ID_INTERNAL_TTY_TX+1
#define DEVICE_ID_INTERNAL_CAM_REC                    DEVICE_ID_INTERNAL_MIC1_REC+1
#define DEVICE_ID_INTERNAL_VOICE_REC                    DEVICE_ID_INTERNAL_CAM_REC+1



#define DEVICE_ID_EXTERNAL_BT_SCO_VOICE_RX  DEVICE_ID_INTERNAL_VOICE_REC + 1
#define DEVICE_ID_EXTERNAL_BT_SCO_AUDIO_RX  DEVICE_ID_EXTERNAL_BT_SCO_VOICE_RX +1
#define DEVICE_ID_EXTERNAL_BT_SCO_VOICE_TX  DEVICE_ID_EXTERNAL_BT_SCO_AUDIO_RX +1

#endif
//LGE_UPDATE_E
struct snddev_icodec_data {
	u32 capability; /* RX or TX */
	const char *name;
	u32 copp_id; /* audpp routing */
	u32 acdb_id; /* Audio Cal purpose */
	/* Adie profile */
	struct adie_codec_dev_profile *profile;
	/* Afe setting */
	u8 channel_mode;
	enum hsed_controller *pmctl_id; /* tx only enable mic bias */
	u32 pmctl_id_sz;
	u32 default_sample_rate;
	void (*pamp_on) (void);
	void (*pamp_off) (void);
	void (*voltage_on) (void);
	void (*voltage_off) (void);
	s32 max_voice_rx_vol[VOC_RX_VOL_ARRAY_NUM]; /* [0]: NB,[1]: WB */
	s32 min_voice_rx_vol[VOC_RX_VOL_ARRAY_NUM];
	u32 dev_vol_type;
	u32 property; /*variable used to hold the properties
				internal to the device*/
};
#endif
