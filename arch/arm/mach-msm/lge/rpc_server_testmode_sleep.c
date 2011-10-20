/* arch/arm/mach-msm/rpc_server_testmode_sleep.c
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
#include "rpc_server_testmode_sleep.h"
#include <linux/syscalls.h>
#include <linux/fcntl.h> 
#include "testmode_input.h"
#include <linux/delay.h>
#include "rpc_server_testmode.h" //jungjr_compile_error

#define TESTMODE_SUCCESS		0
#define TESTMODE_FAIL	1
#define TESTMODE_NOT_SUPPORTED 2

extern void wirte_flight_mode(int mode);

/* Sleep Test (250-42-0) Test mode */
// START sangki.hyun@lge.com 20100914 LAB1_FW : Flight_mode {
#if 0  // below function already exist in lge_ats_cmd.c 
void wirte_flight_mode(int mode)
{
	char buf[10];

	int fd = sys_open("/sys/devices/platform/autoall/flight", O_WRONLY, 0);
	
	if (fd != -1) 
	{
		sprintf(buf, "%d", mode);

		sys_write(fd, buf, strlen(buf));

		sys_close(fd);
	}
}
#endif 
// END sangki.hyun@lge.com 20100914 LAB1_FW : Flight_mode }

#ifdef LGE_TEST_MODE_TBD
extern int check_lcd_status(void);  // sangki.hyun@lge.com 20100901 LAB1_FW : sleep mode 
#endif /* LGE_TEST_MODE_TBD */

void* testmode_sleep_mode_test(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
 //jungjr_compile_error   int value = 0 ;
    
    struct testmode_relay_result *relay_result;
    
    relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

    // START sangki.hyun@lge.com 20100901 LAB1_FW : sleep mode {
    switch( sub2_cmd )
    {
        case 0:
        {
#ifdef LGE_TEST_MODE_TBD			
            struct input_dev* idev = NULL;
            extern struct input_dev* get_ats_input_dev(void);
            
            idev = get_ats_input_dev();
            
            if(idev == NULL)
                printk("%s: input device addr is NULL\n",__func__);
            
            if(check_lcd_status() > 0)
            {
                /* LCD ON STATS ÀÏ ¶§¸¸ */
                input_report_key(idev,(unsigned int)KEY_POWER, 1);
                input_report_key(idev,(unsigned int)KEY_POWER, 0);
            }
#endif /* LGE_TEST_MODE_TBD */            
            relay_result->ret_value = TESTMODE_SUCCESS;
            
            break;
        }

        // START sangki.hyun@lge.com 20100914 LAB1_FW : Flight_mode {
        case 1:
            wirte_flight_mode(1);   
            break;
        // END sangki.hyun@lge.com 20100914 LAB1_FW : Flight_mode }
        
        default:        
            break;
        
    }
    // END sangki.hyun@lge.com 20100901 LAB1_FW : sleep mode }
    return relay_result;
}
// LGE_TEST_MODE end 
// END [sangki.hyun@lge.com] 20100615 LAB1_FW }

