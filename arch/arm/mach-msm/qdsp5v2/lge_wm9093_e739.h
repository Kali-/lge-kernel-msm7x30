/* arch/arm/mach-msm/qdsp5v2/lge_tpa2055-amp.h
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

enum {
  HEADSET_VOICE_INPUT = 0,
  HEADSET_VOICE_OUTPUT,
  SPEAKER_VOICE_INPUT,
  SPEAKER_VOICE_OUTPUT,
  TTY_INPUT,
  TTY_OUTPUT,
  HEADSET_AUDIO_INPUT,
  HEADSET_AUDIO_OUTPUT,
  HEADSET_SPEAKER_INPUT,
  HEADSET_SPEAKER_OUTPUT_HEADSET,
  HEADSET_SPEAKER_OUTPUT_SPEAKER,
  SPEAKER_AUDIO_INPUT,
  SPEAKER_AUDIO_OUTPUT,
  AMP_CAL_MAX
};

void set_amp_PowerDown(void);
void set_amp_headset_stereo_voice(void);
void set_amp_speaker_stereo_voice(void);
void set_amp_tty(void);
void set_amp_headset_stereo_audio(void);
void set_amp_headset_speaker_audio(void);
void set_amp_speaker_stereo_audio(void);

