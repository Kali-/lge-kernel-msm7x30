/* arch/arm/mach-msm/qdsp5v2/lge_wm9093-e739.h
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

/*Revision 0.1 -- THY -- 4/23/11 -- Original Version */


//Register Address
#define AMP_RESET	0x00

#define OUTPUT_ENA	0x01
#define INPUT_ENA	0x02
#define IN1_CONF	0x16
#define IN2_CONF	0x17
#define IN1A_VOL	0x18
#define IN1B_VOL	0x19
#define IN2A_VOL	0x1A
#define IN2B_VOL	0x1B
#define HPOUT_L_VOL	0x1C
#define HPOUT_R_VOL	0x1D
#define SPK_OUT_VOL	0x26
#define SPK_MIX_ATTEN	0x22
#define HPH_L_MIX_ATTEN 0x2F
#define HPH_R_MIX_ATTEN 0x30
#define SPK_OUT		0x24
#define SPK_BOOST	0x25
#define MIX_OUT_L	0x2D
#define MIX_OUT_R	0x2E
#define MIX_OUT_SPK	0x36

#define ANTI_POP	0x39

#define MIX_EN 0x03




//AMP_RESET	(0x00)
#define RESET_DEFAULT	0x9093

//OUTPUT_ENA	(0x01)
#define OUT_SPK		0x100B
#define OUT_HPH		0x030B

//INPUT_ENA(0x02)

#define TSHUT	0x60	//0110 0000   15:8 fixed


#define IN1A_EN	0x80	//1000 0000
#define IN1B_EN	0x40	//0100 0000
#define IN2A_EN	0x20	//0010 0000
#define IN2B_EN 0x10	//0001 0000

#define IN1_EN	0xC0	//1100 0000
#define IN2_EN	0x30	//0011 0000



//IN1_CONF(0x16), IN2_CONF(0x17)

#define CONF_SE		0x00	//0000 0001
#define CONF_DIFF	0x02	//0000 0011



//IN1A_VOL,IN1B_VOL,IN2A_VOL,IN2B_VOL (0x18, x019, 0x1A, 0x1B)

#define IN_VOL_UPDATE	0x01	//0000 0001 	15:8 fixed

#define IN_VOL_MUTE	0x80	//1000 0000	7:0
#define IN_VOL_N6	0x00	//0000 0000
#define IN_VOL_N3D5	0x01	//0000 0001
#define IN_VOL_0	0x02	//0000 0010
#define IN_VOL_3D5	0x03	//0000 0011
#define IN_VOL_6	0x04	//0000 0100
#define IN_VOL_12	0x05	//0000 0101
#define IN_VOL_18	0x06	//0000 0110


//HPOUT_VOL(0x1C, 0x1D) SPK_OUT_VOL(0x26)

#define OUT_VOL_UPDATE	0x01	//0000 0001 	15:8 fixed
#define OUT_VOL_DEFAULT	0x3C	//0011 1101	for -57db, 1dB for 1 plus


//SPK_MIX_ATTEN	(0x22) HPH_MIX_ATTEN	(0x2F,0x30)

#define MIX_MODE	0x00	//0000 0000	unmute

#define MIX_VOL_0	0x00	//00		shift needed - IN1A(7:6) IN1B(5:4) IN2A(3:2) IN2B(1:0)
#define MIX_VOL_N06	0x01	//01
#define MIX_VOL_N09	0x02	//02
#define MIX_VOL_N12	0x03	//03



//SPK_OUT	(0x24)

#define SPK_OUT_EN	0x01	//0000 0001
#define SPK_OUT_DIS	0x00	//0000 0000



//SPK_BOOST	(0x25)

#define SPK_BOOST_0	0x40	//0100 0000
#define SPK_BOOST_1D5	0x48	//0100 1000
#define SPK_BOOST_3	0x50	//0101 0000
#define SPK_BOOST_4D5	0x58	//0101 1000
#define SPK_BOOST_6	  0x60	  //0110 0000
#define SPK_BOOST_7D5	0x68	//0110 1000
#define SPK_BOOST_9	0x70	//0111 0000
#define SPK_BOOST_12	0x78	//0111 1000



//MIX_OUT_L, MIX_OUT_R, MIX_OUT_SPK (0x2D, 0x2E, 0x36)

#define MIX_IN1A	0x40	//0100 0000
#define MIX_IN1B	0x10	//0001 0000
#define MIX_IN2A	0x04	//0000 0100
#define MIX_IN2B	0x01	//0000 0001

#define MIX_IN1		0x50	//0101 0000
#define MIX_IN2		0x05	//0000 0101

// ANTI_POP	(0x39)
#define ANTI_POP_VMID	0x09	//0000 1001


// MIX_EN (0x03)

#define AGC_EN 0x40   //15:8
#define AGC_DIS 0x00

#define SPK_VOL_EN 0x01 //15:8
#define SPK_VOL_DIS 0x00

#define MIXOUT_HPH_L  0x20  //7:0
#define MIXOUT_HPH_R  0x10
#define MIXOUT_HPH  0x30
#define MIXOUT_SPK  0x08

#define AMP_8BIT_USE	0x00FF
#define AMP_16BIT_USE	0xFFFF



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
void set_amp_headset_mono_voice(void);
void set_amp_speaker_stereo_voice(void);
void set_amp_tty(void);
void set_amp_headset_stereo_audio(void);
void set_amp_headset_speaker_audio(void);
void set_amp_speaker_stereo_audio(void);

