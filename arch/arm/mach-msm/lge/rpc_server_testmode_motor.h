/* arch/arm/mach-msm/rpc_server_testmode_motor.h
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

/* Motor Test (250-3-x) Test mode 7.8 */
typedef enum 
{
  MOTOR_TURN_OFF,
  MOTOR_TURN_ON  
} test_mode_req_motor_type;

extern void *testmode_motor_test(uint32_t sub1_cmd, uint32_t sub2_cmd);
extern void *testmode_reponse_not_supported(void);


// LGE_DOM_UPDATE_E jin333.kim@lge.com 2010/01/19 }

