/* arch/arm/mach-msm/rpc_server_testmode_sound.c
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
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/interrupt.h>
#include <mach/msm_rpcrouter.h>
#include "rpc_server_testmode_sound.h"
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include "testmode_input.h"
#include <linux/delay.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include "rpc_server_testmode.h" //jungjr_compile_error




#define TESTMODE_SUCCESS		0
#define TESTMODE_FAIL	1
#define TESTMODE_NOT_SUPPORTED 2
#define LCD_STATUS "/sys/class/leds/lcd-backlight/brightness"
static int need_to_wait_for_sound = 0;

int check_lcd_status(void)
{
    int fp, value;
	int l_size=20;
	unsigned char *buffer;

	fp = sys_open(LCD_STATUS, O_RDONLY, 0);

	if (fp < 0) {
		printk(KERN_ERR "%s: Can not open %s\n",
		__func__, "check_lcd_status");
		return 1;
	}else{

		sys_lseek(fp, (off_t)0, 0);
						
		buffer = kmalloc(l_size, GFP_KERNEL);

		if ((unsigned)sys_read(fp, (char *)buffer, l_size) != l_size) {
			printk(KERN_WARNING "%s: Can not open %s\n",
						__func__, LCD_STATUS);
		}

		sscanf(buffer,"%d", &value);			

		kfree(buffer);
		sys_close(fp);

		printk(KERN_ERR "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX : check_lcd_status %d\n",value);
		return value;
	}

}

/* MP3 Test (250-27-X)  Test mode 7.8 */
void *testmode_mp3_test(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
	struct testmode_relay_result *relay_result;
	//int cnt =0;
	
	relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

	relay_result->ret_value = TESTMODE_SUCCESS;

	if(check_lcd_status() <= 0)
		need_to_wait_for_sound = true;
		

	testmode_input_report_evt(TESTMODE_INPUT_KEY_INIT);
	
	if(need_to_wait_for_sound==true)
		msleep(10); //msleep(1000);
	testmode_input_report_evt(TESTMODE_INPUT_KEY_UNLOCK);

	if(need_to_wait_for_sound==true)
		msleep(20); //msleep(1500);

	//for(cnt=0;cnt<6;cnt++)
		//testmode_input_report_evt(TESTMODE_INPUT_KEY_BACK);
	
	switch (sub2_cmd) 
	{
		case MP3_PLAY_1KHZ_0DB_LR:
			testmode_input_report_evt(TESTMODE_1kHz_0dB_LR_128k);
			break;
			
		case MP3_PLAY_1KHZ_0DB_L:
		 	testmode_input_report_evt(TESTMODE_1kHz_0dB_L_128k);
			break;

		case MP3_PLAY_1KHZ_0DB_R:
		 	testmode_input_report_evt(TESTMODE_1kHz_0dB_R_128k);
			break;

		case MP3_PLAY_MULTISINE_20KHZ:
		 	testmode_input_report_evt(TESTMODE_MultiSine_20_20kHz_0dBp);
			break;

		case MP3_PLAY_TEST_STOP:
		 	testmode_input_report_evt(TESTMODE_MP3_Play_Mode_OFF);
			break;

		case MP3_PLAY_SAMPLE_FILE_COMPARE:
		 	testmode_input_report_evt(TESTMODE_MP3_Sample_File_Compare);
			break;

		case MP3_PLAY_NOSIGNAL_LR_128K:
		 	testmode_input_report_evt(TESTMODE_NoSignal_LR_128k);
			break;

		default:
			return testmode_reponse_not_supported();
	}

	need_to_wait_for_sound =false;
	return relay_result;
}


// START [sangki.hyun@lge.com] 20100615 LAB1_FW LGE_TEST_MODE {
//extern void akm_test_mode_set_audio(bool test_on);
#define akm_test_mode_set_audio(test_on)  
// END [sangki.hyun@lge.com] 20100615 LAB1_FW }

/* Speaker Phone Test (250-43-X) Test mode 7.8 */
void *testmode_speaker_phone_test(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
	struct testmode_relay_result *relay_result;
	
	relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

	relay_result->ret_value = TESTMODE_SUCCESS;

	if(check_lcd_status() <= 0)
		need_to_wait_for_sound = true;

	testmode_input_report_evt(TESTMODE_INPUT_KEY_INIT);
	if(need_to_wait_for_sound==true)
		msleep(1000);
	testmode_input_report_evt(TESTMODE_INPUT_KEY_UNLOCK);

	if(need_to_wait_for_sound==true)
		msleep(1500);

	switch (sub2_cmd) 
	{
	   case SPEAKER_PHONE_MODE_OFF:
	   		akm_test_mode_set_audio(false);
		 	testmode_input_report_evt(TESTMODE_Speaker_Phone_OFF);
			break;
			
		case SPEAKER_PHONE_MODE_ON:
			akm_test_mode_set_audio(true);
			testmode_input_report_evt(TESTMODE_Speaker_Phone_ON);
			break;

		default:
			return testmode_reponse_not_supported();
	}
		
	need_to_wait_for_sound =false;
	return relay_result;
}



/* Volume Level Test (250-51-X) Test mode 7.8 */
void *testmode_volume_level_test(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
	struct testmode_relay_result *relay_result;
	
	relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

	relay_result->ret_value = TESTMODE_SUCCESS;

	if(check_lcd_status() <= 0)
		need_to_wait_for_sound = true;

	testmode_input_report_evt(TESTMODE_INPUT_KEY_INIT);
	if(need_to_wait_for_sound==true)
		msleep(1000);
	testmode_input_report_evt(TESTMODE_INPUT_KEY_UNLOCK);

	if(need_to_wait_for_sound==true)
		msleep(1500);

	switch (sub2_cmd) 
	{
	   case TEST_VOLUME_LEVEL_0:
		 	testmode_input_report_evt(TESTMODE_Volume_Level_0);
			break;
			
		case TEST_VOLUME_LEVEL_MINIMUM:
			testmode_input_report_evt(TESTMODE_Minimum_Volume_Level);
			break;

		case TEST_VOLUME_LEVEL_MEDIUM:
			testmode_input_report_evt(TESTMODE_Medium_Volume_Level);
			break;

		case TEST_VOLUME_LEVEL_MAXIMUM:
			testmode_input_report_evt(TESTMODE_Maximum_Volume_Level);
			break;

		default:
			return testmode_reponse_not_supported();
	}

	need_to_wait_for_sound =false;
	return relay_result;
}



/* Acoustic Test (250-4-x) Test mode 7.8  */
void *testmode_acoustic_test(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
	struct testmode_relay_result *relay_result;
	
	relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

	relay_result->ret_value = TESTMODE_SUCCESS;

	if(check_lcd_status() <= 0)
		need_to_wait_for_sound = true;

	testmode_input_report_evt(TESTMODE_INPUT_KEY_INIT);
	if(need_to_wait_for_sound==true)
		msleep(1000);
	testmode_input_report_evt(TESTMODE_INPUT_KEY_UNLOCK);

	if(need_to_wait_for_sound==true)
		msleep(1500);

	akm_test_mode_set_audio(false);	//Protect Code
	switch (sub2_cmd) 
	{
	   case ACOUSTIC_OFF:
		 	testmode_input_report_evt(TESTMODE_Acoustic_OFF);
			//mvs_pkt_loopback_stop();
			break;
			
		case ACOUSTIC_ON:
			//mvs_pkt_loopback_start();
			testmode_input_report_evt(TESTMODE_Acoustic_ON);
			break;

		case HEADSET_PATH_ON:
			testmode_input_report_evt(TESTMODE_Headset_Path_Open);
			break;

		case HANDSET_PATH_ON:
			testmode_input_report_evt(TESTMODE_Handset_Path_Open);
			break;

		case ACOUSTIC_LOOPBACK_ON:
			testmode_input_report_evt(TESTMODE_ACOUSTIC_Loopback_ON);
			break;

		case ACOUSTIC_LOOPBACK_OFF:
			testmode_input_report_evt(TESTMODE_Acoustic_Loopback_OFF);
			break;

		case AUDIO_PATH_CHANGE_TO_MAIN_EARJACK://3.5PI
			testmode_input_report_evt(TESTMODE_PATH_CHANGE_TO_MAIN_EARJACK);
			break;
			
		case AUDIO_PATH_CHANGE_TO_SUB_EARJACK://20pin
			testmode_input_report_evt(TESTMODE_PATH_CHANGE_TO_SUB_EARJACK);
			break;
			
		case AUDIO_PATH_CHANGE_TO_DEFAULT:			
			testmode_input_report_evt(TESTMODE_PATH_CHANGE_TO_DEFAULT);
			break;

		default:
			return testmode_reponse_not_supported();
	}

	need_to_wait_for_sound =false;
	return relay_result;
}
// LGE_DOM_UPDATE_E jin333.kim@lge.com 2010/01/19 }

