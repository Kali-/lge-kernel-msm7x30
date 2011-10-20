// [sangki.hyun@lge.com] 20100615 LAB1_FW LGE_TEST_MODE
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/delay.h>

#include "testmode_input.h"


#define KEY_INPUT_DELAY 10

#define DRIVER_NAME "testmode_input"

static struct input_dev *testmode_input_dev;

static char testmode_input_key[MAX_KEYS] = {
		TESTMODE_INPUT_KEY_HOME,
		TESTMODE_INPUT_KEY_BACK,
		TESTMODE_INPUT_KEY_INIT,
		TESTMODE_INPUT_KEY_UNLOCK,
		TESTMODE_INPUT_KEY_TDMB,
// 250-27 MP3 Test		
		TESTMODE_1kHz_0dB_LR_128k,
		TESTMODE_1kHz_0dB_L_128k,
		TESTMODE_1kHz_0dB_R_128k,
		TESTMODE_MultiSine_20_20kHz_0dBp,
		TESTMODE_MP3_Play_Mode_OFF,
		TESTMODE_MP3_Sample_File_Compare,
		TESTMODE_NoSignal_LR_128k,
// 250-43 Speaker Phone Test
		TESTMODE_Speaker_Phone_ON,
		TESTMODE_Speaker_Phone_OFF,
		TESTMODE_Normal_Mic1,
// 250-51 Volume Level Test
		TESTMODE_Volume_Level_0,
		TESTMODE_Minimum_Volume_Level,
		TESTMODE_Medium_Volume_Level,
		TESTMODE_Maximum_Volume_Level,
// 250-4 Acoustic Test
		TESTMODE_Acoustic_ON,
		TESTMODE_Headset_Path_Open,
		TESTMODE_Handset_Path_Open,
		TESTMODE_ACOUSTIC_Loopback_ON,
		TESTMODE_Acoustic_OFF,
		TESTMODE_Acoustic_Loopback_OFF,
// 250-3 Motor Test
		TESTMODE_Motor_ON,
		TESTMODE_Moter_OFF,
// 250-7 Camera Test
		TESTMODE_CAMERA_MODE_ON,
		TESTMODE_CAMERA_SHOT,
		TESTMODE_CAMERA_SAVE_IMAGE,
		TESTMODE_CAMERA_CALL_IMAGE,
		TESTMODE_CAMERA_ERASE_IMAGE,
		TESTMODE_CAMERA_MODE_OFF,
		TESTMODE_CAMCORDER_MODE_ON,
		TESTMODE_CAMCORDER_SHOT_RECORD_START,
		TESTMODE_CAMCORDER_RECORD_STOP_AND_SAVE,
		TESTMODE_CAMCORDER_PLAY_MOVING_FILE,
		TESTMODE_CAMCORDER_ERASE_MOVING_FILE,
		TESTMODE_CAMERA_SELECT_SENSOR,
		TESTMODE_CAMCORDER_CAMCORDER_MODE_OFF,
// 250-1 LCD Test		
		TESTMODE_LCD_INITIAL,
		TESTMODE_LCD_TILT,
		TESTMODE_LCD_COLOR_DISPLAY,
// 250-1 TDMB Test			
		TESTMODE_INPUT_KEY_TDMB_ON,
		TESTMODE_INPUT_KEY_TDMB_OFF,
		TESTMODE_INPUT_KEY_TDMB_RM_CH,
		TESTMODE_INPUT_KEY_TDMB_CHK,
// 250-50 Factory Reset Test		
		TESTMODE_INPUT_FACTORY_RESET,
		
//250-4 Acoustic Test 7.9  update
		TESTMODE_PATH_CHANGE_TO_MAIN_EARJACK,
		TESTMODE_PATH_CHANGE_TO_SUB_EARJACK,
		TESTMODE_PATH_CHANGE_TO_DEFAULT,
		TESTMODE_PATH_CHANGE_TO_BT_SCO_CALL_PATH,
    TESTMODE_PATH_CHANGE_TO_BT_SCO_CALL_PATH_OFF,
		
// 250-40 Key Code Input    
		TESTMODE_POUND, 	
		TESTMODE_STAR, 		
		TESTMODE_NUM_0, 	
		TESTMODE_NUM_1,	
		TESTMODE_NUM_2, 	
		TESTMODE_NUM_3, 	
		TESTMODE_NUM_4, 	
		TESTMODE_NUM_5, 	
		TESTMODE_NUM_6, 	
		TESTMODE_NUM_7, 	
		TESTMODE_NUM_8, 	
		TESTMODE_NUM_9, 	
		TESTMODE_CALL, 		
		TESTMODE_ENDCALL,
		TESTMODE_CLEAR, 		
		TESTMODE_DPAD_CENTER,
		TESTMODE_DPAD_UP, 			
		TESTMODE_DPAD_DOWN, 
		TESTMODE_DPAD_LEFT, 		
		TESTMODE_DPAD_RIGHT,
		TESTMODE_LCDOFF_AND_LOCK,
// START sungchae.koo@lge.com 2010/10/14 LAB1_FW : FACTORY_DEBUGGING {
		TESTMODE_ENABLE_ADB,
// END sungchae.koo@lge.com 2010/10/14 LAB1_FW }		
// START [jongrok.jung@lge.com] 20110309 Testmode 8.5  - db_integrity_check{		
		TESTMODE_DB_INTEGRITY_CHECK,
		TESTMODE_FPRI_CRC_CHECK,
		TESTMODE_FILE_CRC_CHECK,
		TESTMODE_DB_DUMP,
		TESTMODE_DB_COPY,
// END [jongrok.jung@lge.com] 20110309 Testmode 8.5 } 		
// START LGE_BROADCAST andrew74.kim@lge.com 2011-03-18 : TDMB BLT Test {
// Requested by Production Tech.
		TESTMODE_INPUT_KEY_TDMB_PLAY_ON,
		TESTMODE_INPUT_KEY_TDMB_PLAY_OFF, 
// END LGE_BROADCAST}
};

void testmode_input_report_evt(int evtcode)
{
	printk(KERN_INFO "[LGE] testmode_input_report_evt starts ..\n");

	if (testmode_input_dev != 0x0) {
		
		printk(KERN_INFO "[LGE] testmode  input_report_key starts  evtcode[%d]\n",evtcode);
		
		input_report_key(testmode_input_dev, evtcode, 1);
   		mdelay(KEY_INPUT_DELAY);
   		input_report_key(testmode_input_dev, evtcode, 0);
	}
	printk(KERN_INFO "[LGE] testmode_input_report_evt end ..\n");

	return;
}
EXPORT_SYMBOL(testmode_input_report_evt);

static int testmode_input_probe(struct platform_device *pdev)
{
	int i, rc;
	struct input_dev  *input_dev;

	input_dev = input_allocate_device();
	if (!input_dev) {
		rc = -ENOMEM;
		goto err_alloc_input_dev;
	}

	input_dev->name	= DRIVER_NAME;

	input_dev->id.vendor	= 0x0001;
	input_dev->id.product	= 1;
	input_dev->id.version	= 1;
	input_dev->dev.parent = &pdev->dev;

	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
  	input_dev->evbit[0] = BIT(EV_KEY);
  	input_dev->mscbit[0] = 0;

  	input_dev->keycodesize = sizeof(testmode_input_key);
  	input_dev->keycodemax = MAX_KEYS;
	input_dev->keycode = (void *)testmode_input_key;	

	for (i = 0 ; i < MAX_KEYS ; i++)
		input_set_capability(input_dev, EV_KEY, testmode_input_key[i]);

	rc = input_register_device(input_dev);
	if (rc) {
		goto err_reg_input_dev;
	}

	testmode_input_dev = input_dev;

	return 0;

err_alloc_input_dev:
err_reg_input_dev:
	testmode_input_dev = 0x0;
	input_free_device(input_dev);

	return rc;
}

static struct platform_driver testmode_input_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.probe	 = testmode_input_probe,
};

static int __init testmode_input_init(void)
{
	printk("[LGE] testmode_input %s\n",__FUNCTION__);
	return platform_driver_register(&testmode_input_driver);
}

static void __exit testmode_input_exit(void)
{
	printk("[LGE] testmode_input %s\n",__FUNCTION__);
	platform_driver_unregister(&testmode_input_driver);
}

module_init(testmode_input_init);
module_exit(testmode_input_exit);
