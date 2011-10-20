// [sangki.hyun@lge.com] 20100615 LAB1_FW LGE_TEST_MODE
#include <linux/module.h>
#include <mach/msm_rpcrouter.h>

#include <linux/delay.h>

#include "rpc_server_testmode.h"
#include "testmode_input.h"

/* Handler for the camera test mode on (250-7-X) */
void *testmode_cam(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
	struct testmode_relay_result *relay_result;
	
	relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

	relay_result->ret_value = TESTMODE_SUCCESS;

	testmode_input_report_evt(TESTMODE_INPUT_KEY_INIT);
	msleep(1000);
	testmode_input_report_evt(TESTMODE_INPUT_KEY_UNLOCK);	

	switch (sub2_cmd) 
	{
	   case 0:
		 	testmode_input_report_evt(TESTMODE_CAMERA_MODE_OFF);
			break;
			
		case 1:
			testmode_input_report_evt(TESTMODE_INPUT_KEY_INIT);
			testmode_input_report_evt(TESTMODE_CAMERA_MODE_ON);
			mdelay(1500);
			break;

		case 2:
			testmode_input_report_evt(TESTMODE_CAMERA_SHOT);
			break;

		case 4:
			testmode_input_report_evt(TESTMODE_CAMERA_CALL_IMAGE);
			break;	

		case 5:
			testmode_input_report_evt(TESTMODE_CAMERA_ERASE_IMAGE);
			mdelay(1500);
			break;		

		case 9:
			testmode_input_report_evt(TESTMODE_CAMERA_CALL_IMAGE);
			break;	

		case 10:
			testmode_input_report_evt(TESTMODE_CAMCORDER_CAMCORDER_MODE_OFF);
			break;	

		case 11:
			testmode_input_report_evt(TESTMODE_INPUT_KEY_INIT);
			testmode_input_report_evt(TESTMODE_CAMCORDER_MODE_ON);
			mdelay(1500);
		break;	

		case 12:
			testmode_input_report_evt(TESTMODE_CAMCORDER_SHOT_RECORD_START);
			break;	

		case 13:
			testmode_input_report_evt(TESTMODE_CAMCORDER_RECORD_STOP_AND_SAVE);
			break;	
			
		case 14:
			testmode_input_report_evt(TESTMODE_CAMCORDER_PLAY_MOVING_FILE);
			break;	

		case 15:
			testmode_input_report_evt(TESTMODE_CAMCORDER_ERASE_MOVING_FILE);
			mdelay(1500);
			break;				

		default:
			kfree(relay_result);
			return testmode_reponse_not_supported();
	}

	return relay_result;
}
