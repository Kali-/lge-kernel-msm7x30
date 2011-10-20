// [sangki.hyun@lge.com] 20100615 LAB1_FW LGE_TEST_MODE

#include <linux/module.h>
#include <mach/msm_rpcrouter.h>
#include "rpc_server_testmode.h"

#define OFF	0
#define ON		1

int testmode_key_p = -1;
int testmode_key_flag = OFF;

#define MAX_TESTKEY	20

static unsigned char testkey[MAX_TESTKEY] = {0x0, };

/* sample code for key test mode on (250-22) */
void testmode_input_scancode(unsigned char scancode)
{
	if (testmode_key_flag == OFF)
		return;
	else {
		testmode_key_p++;

		if (testmode_key_p == MAX_TESTKEY)
			testmode_key_p = 0;
		
		testkey[testmode_key_p] = scancode;
	}

	return;
}
EXPORT_SYMBOL(testmode_input_scancode);

void *testmode_key_test(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
	struct testmode_relay_result *relay_result;
	
	relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

	relay_result->ret_value = TESTMODE_SUCCESS;

	switch (sub2_cmd) 
	{
		case 1:
			testmode_key_flag = ON;
			testmode_key_p = -1;
			memset(testkey, 0x0, MAX_TESTKEY);
	
			break;
		case 0:
			testmode_key_flag = OFF;
			
			if (testmode_key_p != -1)
				memcpy(relay_result->ret_string, testkey, MAX_TESTKEY);
			
			break;

		default:
			kfree(relay_result);
			return testmode_reponse_not_supported();
	}

	return relay_result;
}
