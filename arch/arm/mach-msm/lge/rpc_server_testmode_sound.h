/* arch/arm/mach-msm/rpc_server_testmode_sound.h
 *
 * Copyright (c) 2008-2009, LG Electronics. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can find it at http://www.fsf.org.
 */
// [sangki.hyun@lge.com] 20100615 LAB1_FW LGE_TEST_MODE
// LGE_DOM_UPDATE_S jin333.kim@lge.com 2010/01/19 {

/* MP3 Test (250-27-X)  Test mode 7.8 */
typedef enum 
{
	MP3_PLAY_1KHZ_0DB_LR=0,
	MP3_PLAY_1KHZ_0DB_L,
	MP3_PLAY_1KHZ_0DB_R,
	MP3_PLAY_MULTISINE_20KHZ,
	MP3_PLAY_TEST_STOP,
	MP3_PLAY_SAMPLE_FILE_COMPARE,
	MP3_PLAY_NOSIGNAL_LR_128K
}test_mode_req_mp3_play_type;


/* Speaker Phone Test (250-43-X) Test mode 7.8 */
typedef enum
{
	SPEAKER_PHONE_MODE_OFF=0,
	SPEAKER_PHONE_MODE_ON,
	NOMAL_MIC1_MODE_ON,
	NC_MODE_ON,	
	ONLY_MIC2_ON_NC_MODE,		
	ONLY_MIC1_ON_NC_MODE,
}test_mode_req_speaker_phone_type;


/* Volume Level Test (250-51-X) Test mode 7.8 */
typedef enum
{
	TEST_VOLUME_LEVEL_0=0,
	TEST_VOLUME_LEVEL_MINIMUM,
	TEST_VOLUME_LEVEL_MEDIUM,
	TEST_VOLUME_LEVEL_MAXIMUM
}test_mode_req_test_volume_level_test_type;


/* Acoustic Test (250-4-x) Test mode 7.9  */
typedef enum 
{
  ACOUSTIC_OFF=0,
  ACOUSTIC_ON,
  HEADSET_PATH_ON,
  HANDSET_PATH_ON,
  ACOUSTIC_LOOPBACK_ON,
  ACOUSTIC_LOOPBACK_OFF,
  AUDIO_PATH_CHANGE_TO_MAIN_EARJACK,//3.5PI
  AUDIO_PATH_CHANGE_TO_SUB_EARJACK,//20pin
  AUDIO_PATH_CHANGE_TO_DEFAULT
}test_mode_req_acoustic_type;


extern void *testmode_mp3_test(uint32_t sub1_cmd, uint32_t sub2_cmd);
extern void *testmode_speaker_phone_test(uint32_t sub1_cmd, uint32_t sub2_cmd);
extern void *testmode_volume_level_test(uint32_t sub1_cmd, uint32_t sub2_cmd);
extern void *testmode_acoustic_test(uint32_t sub1_cmd, uint32_t sub2_cmd);
extern void *testmode_reponse_not_supported(void);

//extern void mvs_pkt_loopback_start();
//extern void mvs_pkt_loopback_stop();
extern int check_lcd_status(void);
// LGE_DOM_UPDATE_E jin333.kim@lge.com 2010/01/19 }


