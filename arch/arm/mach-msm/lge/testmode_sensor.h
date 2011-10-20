/* arch/arm/mach-msm/testmode_accelator.h
 *
 * Copyright (C) 2008 Google, Inc.
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 * Author: Brian Swetland <swetland@google.com>
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
//LGE jspark - sensor testmode
// [sangki.hyun@lge.com] 20100615 LAB1_FW LGE_TEST_MODE
 
#ifndef TESTMODE_ACCELATOR_H
#define TESTMODE_ACCELATOR_H

//LGE jspark - sensor testmode
void* testmode_accelator(uint32_t sub1_cmd, uint32_t sub2_cmd);
void* testmode_proximity(uint32_t sub1_cmd, uint32_t sub2_cmd);


#endif

