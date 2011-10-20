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
#include "rpc_server_testmode_motor.h"
#include "rpc_server_testmode_sound.h"
#include <linux/syscalls.h>
#include <linux/fcntl.h> 
#include "testmode_input.h"
#include <linux/delay.h>
#include "rpc_server_testmode.h" //jungjr_compile_error

#define TESTMODE_SUCCESS		0
#define TESTMODE_FAIL	1
#define TESTMODE_NOT_SUPPORTED 2

static int need_to_wait_for_motor = 0;

/* Motor Test (250-3-x) Test mode 7.8 */
void *testmode_motor_test(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
	struct testmode_relay_result *relay_result;
	//int cnt =0;
	relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

	relay_result->ret_value = TESTMODE_SUCCESS;

	if(check_lcd_status() <= 0)
		need_to_wait_for_motor = true;

	printk(KERN_INFO "[LGE] testmode_motor_test starts ..\n");

	testmode_input_report_evt(TESTMODE_INPUT_KEY_INIT);
	if(need_to_wait_for_motor==true)
		msleep(10);
	testmode_input_report_evt(TESTMODE_INPUT_KEY_UNLOCK);

	if(need_to_wait_for_motor==true)
		msleep(20);
	
	//for(cnt=0;cnt<6;cnt++)
		//testmode_input_report_evt(TESTMODE_INPUT_KEY_BACK);

	switch (sub2_cmd) 
	{
		case MOTOR_TURN_ON:
			testmode_input_report_evt(TESTMODE_Motor_ON);
			break;
			
		case MOTOR_TURN_OFF:
		 	testmode_input_report_evt(TESTMODE_Moter_OFF);
			break;

		default:
			return testmode_reponse_not_supported();
	}

	need_to_wait_for_motor=false;
	return relay_result;
}
// LGE_DOM_UPDATE_E jin333.kim@lge.com 2010/01/19 }

