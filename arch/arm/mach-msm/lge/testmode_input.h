// [sangki.hyun@lge.com] 20100615 LAB1_FW LGE_TEST_MODE
/* list of events as a input key */
#define TESTMODE_INPUT_KEY_HOME				1
#define TESTMODE_INPUT_KEY_BACK				2

#define TESTMODE_INPUT_KEY_INIT					3
#define TESTMODE_INPUT_KEY_UNLOCK			4
#define TESTMODE_INPUT_KEY_TDMB				5
// 250-27 MP3 Test
#define TESTMODE_1kHz_0dB_LR_128k					6
#define TESTMODE_1kHz_0dB_L_128k					7
#define TESTMODE_1kHz_0dB_R_128k					8 
#define TESTMODE_MultiSine_20_20kHz_0dBp		9
#define TESTMODE_MP3_Play_Mode_OFF				10
#define TESTMODE_MP3_Sample_File_Compare	11
#define TESTMODE_NoSignal_LR_128k					12
// 250-43 Speaker Phone Test
#define TESTMODE_Speaker_Phone_ON					13
#define TESTMODE_Speaker_Phone_OFF				14
#define TESTMODE_Normal_Mic1								15
// 250-51 Volume Level Test
#define TESTMODE_Volume_Level_0						16
#define TESTMODE_Minimum_Volume_Level 			17
#define TESTMODE_Medium_Volume_Level			18
#define TESTMODE_Maximum_Volume_Level			19
// 250-4 Acoustic Test
#define TESTMODE_Acoustic_ON								20
#define TESTMODE_Headset_Path_Open				21
#define TESTMODE_Handset_Path_Open				22
#define TESTMODE_ACOUSTIC_Loopback_ON								23
#define TESTMODE_Acoustic_OFF								24
#define TESTMODE_Acoustic_Loopback_OFF			25
// 250-3 Motor Test
#define TESTMODE_Motor_ON									26
#define TESTMODE_Moter_OFF									27
// 250-7 Camera Test
#define TESTMODE_CAMERA_MODE_ON					28
#define TESTMODE_CAMERA_SHOT							29
#define TESTMODE_CAMERA_SAVE_IMAGE			30
#define TESTMODE_CAMERA_CALL_IMAGE			31
#define TESTMODE_CAMERA_ERASE_IMAGE			32
#define TESTMODE_CAMERA_MODE_OFF				33
#define TESTMODE_CAMCORDER_MODE_ON			34
#define TESTMODE_CAMCORDER_SHOT_RECORD_START			35
#define TESTMODE_CAMCORDER_RECORD_STOP_AND_SAVE	36
#define TESTMODE_CAMCORDER_PLAY_MOVING_FILE				37
#define TESTMODE_CAMCORDER_ERASE_MOVING_FILE			38
#define TESTMODE_CAMERA_SELECT_SENSOR			39
#define TESTMODE_CAMCORDER_CAMCORDER_MODE_OFF		40	
// 250-1 LCD Test
#define TESTMODE_LCD_INITIAL								41
#define TESTMODE_LCD_TILT									42
#define TESTMODE_LCD_COLOR_DISPLAY				43

// 250-1 TDMB Test
#define TESTMODE_INPUT_KEY_TDMB_ON				44
#define TESTMODE_INPUT_KEY_TDMB_OFF				45
#define TESTMODE_INPUT_KEY_TDMB_RM_CH			46
#define TESTMODE_INPUT_KEY_TDMB_CHK				47

// 250-50 Factory Reset Test
#define TESTMODE_INPUT_FACTORY_RESET			48

//250-4 Acoustic Test 7.9  update
#define TESTMODE_PATH_CHANGE_TO_MAIN_EARJACK	49
#define TESTMODE_PATH_CHANGE_TO_SUB_EARJACK		50
#define TESTMODE_PATH_CHANGE_TO_DEFAULT			51
#define TESTMODE_PATH_CHANGE_TO_BT_SCO_CALL_PATH 52
#define TESTMODE_PATH_CHANGE_TO_BT_SCO_CALL_PATH_OFF 53

#define TESTMODE_POUND 	54
#define TESTMODE_STAR	55
#define TESTMODE_NUM_0 	56
#define TESTMODE_NUM_1 	57
#define TESTMODE_NUM_2 	58
#define TESTMODE_NUM_3 	59
#define TESTMODE_NUM_4 	60
#define TESTMODE_NUM_5 	61
#define TESTMODE_NUM_6 	62 
#define TESTMODE_NUM_7 	63 
#define TESTMODE_NUM_8 	64 
#define TESTMODE_NUM_9 	65
#define TESTMODE_CALL 		66
#define TESTMODE_ENDCALL 	67 
#define TESTMODE_CLEAR 		68
#define TESTMODE_DPAD_CENTER 69 
#define TESTMODE_DPAD_UP	70 
#define TESTMODE_DPAD_DOWN 	71
#define TESTMODE_DPAD_LEFT 		72 
#define TESTMODE_DPAD_RIGHT 	73
#define TESTMODE_LCDOFF_AND_LOCK 	74

// START sungchae.koo@lge.com 2010/10/14 LAB1_FW : FACTORY_DEBUGGING {
#define TESTMODE_ENABLE_ADB 75
// END sungchae.koo@lge.com 2010/10/14 LAB1_FW }		

// START [jongrok.jung@lge.com] 20110309 Testmode 8.5  - db_integrity_check{
#define TESTMODE_DB_INTEGRITY_CHECK 76
#define TESTMODE_FPRI_CRC_CHECK 77
#define TESTMODE_FILE_CRC_CHECK 78
#define TESTMODE_DB_DUMP 79
#define TESTMODE_DB_COPY 80
//#define MAX_KEYS	80
//#define MAX_KEYS	73
// END [jongrok.jung@lge.com] 20110309 Testmode 8.5 }  

// START LGE_BROADCAST andrew74.kim@lge.com 2011-03-18 : TDMB BLT Test {
// Requested by Production Tech.
#define TESTMODE_INPUT_KEY_TDMB_PLAY_ON  81
#define TESTMODE_INPUT_KEY_TDMB_PLAY_OFF 82
#define MAX_KEYS 82
// END LGE_BROADCAST}

/* exported function to deliver an event to the android event hub */
void testmode_input_report_evt(int evtcode);
