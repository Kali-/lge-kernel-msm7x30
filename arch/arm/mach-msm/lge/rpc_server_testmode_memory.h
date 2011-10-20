
// LGE_DOM_UPDATE_S smtk.kim@lge.com 2009/12/31 {
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

// for external memory test
enum {
	SUB2_EXTERNAL_SOCKET_MEMORY_CHECK = 0,
	SUB2_EXTERNAL_FLASH_MEMORY_SIZE,
	SUB2_EXTERNAL_FLASH_FORMAT,
	SUB2_EXTERNAL_FLASH_INTEGRITY,
	SUB2_EXTERNAL_FLASH_USED_SIZE
};


// for memory volume check
enum {
	SUB2_SIZE_OF_TOTAL_MEMORY_FOR_USER = 0,
	SUB2_SIZE_OF_USED_MEMORY,
	SUB2_SIZE_OF_USABLE_MEMORY
};

// for efs integrity 
enum {
	SUB2_EFS_INTEGRITY_TEST = 0,
	SUB2_EFS_INTEGRITY_CRC_TEST,
	SUB2_INTERNAL_MEMORY_INTEGRITY_TEST
};

// for uv sensor test
enum {
	SUB2_INTERNAL_MEMORY_TEST = 0,
	SUB2_INTERNAL_MOMORY_FORMAT
};

#define BAD_BLOCK_MAX_LENGTH    20
#define IS_BLOCK_BAD    1

enum {
	SUB2_BAD_BLOCK_TOTAL_NUMBER = 0,
	SUB2_BAD_BLOCK_ADDRESS
};

typedef enum
{
    BOOT_PARTI_INDEX, 
    SYSTEM_PARTI_NAME_INDEX,
    RECOVERY_PARTI_NAME_INDEX,
    LGDRM_PARTI_NAME_INDEX,
    SPLASH_PARTI_NAME_INDEX,
#if defined (FEATURE_LGE_FOTA)
    FOTABIN_PARTI_NAME_INDEX,
    FOTA_PARTI_NAME_INDEX,
#endif /* FEATURE_LGE_FOTA */
    MISC_PARTI_NAME_INDEX,
    CACHE_PARTI_NAME_INDEX,
    USERDATA_PARTI_NAME_INDEX,
    PARTI_NAME_LAST,
} testmode_nand_partition;

typedef struct
{
    char parti_name[15];
} testmode_partition_table ;


// below functions should be called after booting completely 
extern void *testmode_efs_integrity(uint32_t sub1_cmd, uint32_t sub2_cmd);
extern void *testmode_memory_volume_check(uint32_t sub1_cmd, uint32_t sub2_cmd);
extern void *testmode_external_memory(uint32_t sub1_cmd, uint32_t sub2_cmd);
extern void *testmode_uv_sensor_test(uint32_t sub1_cmd, uint32_t sub2_cmd);
extern void *testmode_memory_bad_block_check(uint32_t sub1_cmd, uint32_t sub2_cmd);
// LGE_DOM_UPDATE_E smtk.kim@lge.com 2009/12/31 }


