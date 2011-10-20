#ifndef TESTMODE_H
#define TESTMODE_H

#include <linux/slab.h>	//jungjr_compile_error

// LGE_DOM_UPDATE_S itsuki@lge.com 2009/10/07 {
// [sangki.hyun@lge.com] 20100615 LAB1_FW LGE_TEST_MODE

/* arch/arm/mach-msm/rpc_server_testmode.c
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

#define TESTMODE_SUCCESS		0
#define TESTMODE_FAIL	1
#define TESTMODE_NOT_SUPPORTED 2

typedef enum
{
	TEST_MODE_SW_REV 									= 0,  /* 250 - 0 - X */
	TEST_MODE_LCD,												    /* 250 - 1 - X */
	TEST_MODE_FOLDER,
	TEST_MODE_MOTOR,	
	TEST_MODE_ACOUSTIC, 
	TEST_MODE_MIDI, 	
	TEST_MODE_VOD,	
	TEST_MODE_CAM,	
	TEST_MODE_BUZZER,
    TEST_MODE_NOT_DEFINED_9,	
	TEST_MODE_FACTORY_INIT 						= 10,	
	TEST_MODE_EFS_INTEGRITY,
	TEST_MODE_TX_POWER ,	
	TEST_MODE_IRDA,
	TEST_MODE_TRAFFIC_CHIP,
	TEST_MODE_MFORMAT,
	TEST_MODE_PHONE_CLEAR,
	TEST_MODE_NOT_DEFINED_17,
	TEST_MODE_LIGHTSENSOR,
	TEST_MODE_WIPI, 
	TEST_MODE_BREW_CNT 								= 20,
	TEST_MODE_BREW_SIZE,	
	TEST_MODE_KEY_TEST,
 	TEST_MODE_EXTERNAL_MEMORY, 
	TEST_MODE_BLUETOOTH,
	TEST_MODE_BATT_BAR_LEVEL,
	TEST_MODE_ANT_BAR_SIZE,
	TEST_MODE_MP3_TEST,
	TEST_MODE_FM_TX,
	TEST_MODE_SGPS, 
	TEST_MODE_GEOMETRIC_SENSOR 				= 30,
	TEST_MODE_ACCELATOR,
	TEST_MODE_ALCHOL_SENSOR,
	TEST_MODE_WLAN_TDMB_SDMB,
    TEST_MODE_TV_OUT,
    TEST_MODE_NOT_DEFINED_35,
	TEST_MODE_MANUAL_TEST,
	TEST_MODE_REMOVABLE_DISK_TEST ,	
	TEST_MODE_UV_SENSOR_TEST =38,
	TEST_MODE_3D_ACCELATOR_TEST,
	TEST_MODE_KEYDATA_INPUT 					= 40,
	TEST_MODE_MEMORY_CAPACITY_CHECK,
	TEST_MODE_SLEEP_MODE_TEST,
	TEST_MODE_SPEAKER_PHONE_MODE,
    TEST_MODE_VIRTUAL_SIM_TEST,
    TEST_MODE_PHOTO_SENSOR,	
	TEST_MODE_VCO_SELF_TUNING_TEST,
	TEST_MODE_MRD_USB_TEST,				
	TEST_MODE_TEST_SCR_CAL_BACKUP_TEST,				
    TEST_MODE_PROXIMITY_SENSOR_TEST,		
    TEST_MODE_FACTORY_RESET_TEST 			= 50,
    TEST_MODE_VOLUME_LEVEL_TEST,
        // START [sangki.hyun@lge.com] 20100801 LAB1_FW : Testmode 8.1 First Booting Check {
        TEST_MODE_MEMORY_BAD_BLOCK_CHECK = 57, 
        TEST_MODE_FIRST_BOOTING_COMPLETE_CHECK = 58,
        
        TEST_MODE_PID_CMD = 70,
        TEST_MODE_SW_VERSION_CMD = 71,
        TEST_MODE_IMEI_CMD = 72,
        TEST_MODE_CAL_CHECK_CMD = 82,
        TEST_MODE_BT_ADDRESS_CMD = 83,    
        // END [sangki.hyun@lge.com] 20100801 LAB1_FW : Testmode 8.1 First Booting Check }
	// START [jongrok.jung@lge.com] 20110316 Testmode db_integrity_check {
	TEST_MODE_DB_INTEGRITY_CHECK_TEST = 91,
	// END [jongrok.jung@lge.com] 20110316 Testmode db_integrity_check }

    TEST_MODE_MAX
} testmode_sub1_cmd_type;

typedef  enum
{
	TEST_MODE_APP_PROC = 0,
	TEST_MODE_MODEM_PROC = 1,
} testmode_func_ownership;

typedef  void* (*testmode_func_type)(uint32_t sub1_cmd, uint32_t sub2_cmd);

typedef  struct 
{
	testmode_sub1_cmd_type sub1_cmd;
	testmode_func_type testmode_func;
	testmode_func_ownership owner;
} testmode_func_table_entry_type;

void *testmode_reponse_not_supported(void);

// LGE_DOM_UPDATE_E itsuki@lge.com 2009/10/07 }
#endif
