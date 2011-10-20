
// LGE_TEST_MODE
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

// created by KIMSUNGMIN @ LG Electronics.


#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/interrupt.h>



#include <mach/msm_rpcrouter.h>

#include "rpc_server_testmode_memory.h"
#include <linux/syscalls.h>
#include <linux/fcntl.h> 
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/div64.h>
#include <linux/mtd/mtd.h>

#include "testmode_input.h"

#include "rpc_server_testmode.h"

// for l_ret ( the result of real process )
enum {
	MEMORY_TM_SUCCESS = 0,
	MEMORY_TM_FAIL ,
	MEMORY_TM_NOT_SUPPORTED
};

static struct mtd_info *mtd;
static uint16_t arm11_bad_block_address[BAD_BLOCK_MAX_LENGTH];

static testmode_partition_table arm11_parti_table[PARTI_NAME_LAST] =
{
    /* Read Me --------------------------------------------
          init.c 파일의 board_part_list struct 에 정의 되어 있습니다. 
          Android Partition Table 이 변경되면 아래 부분도 같이 수정되어야 한다.
    ----------------------------------------------------*/
    { "boot" },
    { "system" },
    { "recovery" },
    { "lgdrm" },
    { "splash" },
#if defined (FEATURE_LGE_FOTA)
    { "FOTABIN" },
    { "FOTA" },
#endif /* FEATURE_LGE_FOTA */
    { "misc" },
    { "cache" },
    { "userdata" },
};

static int shell_cmd_in_kernel( char *cmd );
char external_memory_copy_test(void);

//static int real_format_sdcard(void);
extern void *testmode_dispath_command(uint32_t sub1_cmd, uint32_t sub2_cmd);

// START [sangki.hyun@lge.com] 20100710 LAB1_FW Testmode Memory Check {
/* ==========================================================================
===========================================================================*/
#if 0  // [sangki.hyun@lge.com] 20100821 LAB1_FW : SDCARD SIZE Check
struct statfs_local {
 __u32 f_type;
 __u32 f_bsize;
 __u32 f_blocks;
 __u32 f_bfree;
 __u32 f_bavail;
 __u32 f_files;
 __u32 f_ffree;
 __kernel_fsid_t f_fsid;
 __u32 f_namelen;
 __u32 f_frsize;
 __u32 f_spare[5];
};
#endif

struct statfs_local {
 __u32 f_type;
 __u32 f_bsize;
 __u64 f_blocks;
 __u64 f_bfree;
 __u64 f_bavail;
 __u64 f_files;
 __u64 f_ffree;
 __kernel_fsid_t f_fsid;
 __u32 f_namelen;
 __u32 f_frsize;
 __u32 f_spare[5];
} __attribute__ ((packed,aligned(4)));

/* ==========================================================================
===========================================================================*/
// END [sangki.hyun@lge.com] 20100710 LAB1_FW }
void *testmode_external_memory(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
    struct testmode_relay_result *relay_result;
    struct statfs_local  sf;
    extern char external_memory_copy_test(void);

//jungjr_compile_error        unsigned int status = 0;
    unsigned int rt_ext_mem_size = 0;
    unsigned char rt_ext_mem_chk = 0;
//jungjr_compile_error    unsigned char rt_ext_mem_fmt = 0;


    relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);
    relay_result->ret_value = MEMORY_TM_SUCCESS;

    switch(sub2_cmd)
    {
        case SUB2_EXTERNAL_SOCKET_MEMORY_CHECK:
            rt_ext_mem_chk = external_memory_copy_test();
            if( rt_ext_mem_chk )
            {
                relay_result->ret_value = MEMORY_TM_FAIL;
            }
            else
            {
                relay_result->ret_value = MEMORY_TM_SUCCESS;
            }
            memcpy(relay_result->ret_string, &rt_ext_mem_chk, 1);
            break;

        case SUB2_EXTERNAL_FLASH_MEMORY_SIZE:
            if (sys_statfs64("/sdcard", sizeof(sf), (struct statfs64 *)&sf) != 0)
            {
                relay_result->ret_value = MEMORY_TM_FAIL;
                break;
            }

            rt_ext_mem_size = (sf.f_blocks *  sf.f_bsize) >> 20; // needs Mb.
            memcpy(relay_result->ret_string, &rt_ext_mem_size, 4);
            break;


        case SUB2_EXTERNAL_FLASH_USED_SIZE: 
            // START sangki.hyun@lge.com 20100901 LAB1_FW : sdcard used size {
            kzfree(relay_result);
            relay_result = testmode_dispath_command(sub1_cmd, sub2_cmd);
            // END sangki.hyun@lge.com 20100901 LAB1_FW : sdcard used size }
            
            if (sys_statfs64("/sdcard", sizeof(sf), (struct statfs64 *)&sf) != 0)
            {
                relay_result->ret_value = MEMORY_TM_FAIL;
                break;
            }

            rt_ext_mem_size = ((sf.f_blocks - sf.f_bfree) * sf.f_bsize);
            memcpy(relay_result->ret_string, &rt_ext_mem_size, 4);						
            break;

        case SUB2_EXTERNAL_FLASH_FORMAT:		
            // START [sangki.hyun@lge.com] 20100801 LAB1_FW : Testmode 8.1 {
            kzfree(relay_result);
            relay_result = testmode_dispath_command(sub1_cmd, sub2_cmd);
            // END [sangki.hyun@lge.com] 20100801 LAB1_FW : Testmode 8.1 }
            break;

        case SUB2_EXTERNAL_FLASH_INTEGRITY:
            relay_result = testmode_reponse_not_supported();
            break;

    }

    return relay_result;

}


char external_memory_copy_test(void)
{
	char return_value = 1;
	char *src = (void *)0;
	char *dest = (void *)0;
	off_t fd_offset;
	int fd;
	mm_segment_t old_fs=get_fs();
    set_fs(get_ds());

	if ( (fd = sys_open((const char __user *) "/sdcard/SDTest.txt", O_CREAT | O_RDWR, 0) ) < 0 )
	{
		printk(KERN_ERR "[ATCMD_EMT] Can not access SD card\n");
		goto file_fail;
	}

	if ( (src = (char *)kmalloc(10, GFP_KERNEL)) )
	{
		sprintf(src,"TEST");
		if ((sys_write(fd, (const char __user *) src, 5)) < 0)
		{
			printk(KERN_ERR "[ATCMD_EMT] Can not write SD card \n");
			goto file_fail;
		}
		fd_offset = sys_lseek(fd, 0, 0);
	}
	if ( (dest = kmalloc(10, GFP_KERNEL)) )
	{
		if ((sys_read(fd, (char __user *) dest, 5)) < 0)
		{
			printk(KERN_ERR "[ATCMD_EMT]Can not read SD card \n");
			goto file_fail;
		}
		if ((memcmp(src, dest, 4)) == 0)
			return_value = 0;
		else
			return_value = 1;
	}

	kfree(src);
	kfree(dest);
file_fail:
	sys_close(fd);
    set_fs(old_fs);
	sys_unlink((const char __user *)"/sdcard/SDTest.txt");
	return return_value;
}

void *testmode_memory_volume_check(uint32_t sub1_cmd, uint32_t sub2_cmd)

{
// START [sangki.hyun@lge.com] 20100710 LAB1_FW Testmode Memory Check {
	struct testmode_relay_result *relay_result;
	struct statfs_local  sf;
	unsigned int rt_mem_size = 3;
	int l_ret = MEMORY_TM_FAIL;
	int total_ext_mem_size = 0;
	int used_ext_mem_size = 0;
	int available_ext_mem_size = 0;

	relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

        if (sys_statfs64("/data", sizeof(sf), (struct statfs64 *)&sf) != 0)
	{
	  printk(KERN_ERR "[Testmode]can not get sdcard infomation \n");
	  l_ret = MEMORY_TM_FAIL;
	}
	else
	{
		total_ext_mem_size = (sf.f_blocks * sf.f_bsize) >> 10;
		available_ext_mem_size = (sf.f_bavail * sf.f_bsize) >> 10;
		used_ext_mem_size = total_ext_mem_size - available_ext_mem_size;

		switch(sub2_cmd)
		{
			case SUB2_SIZE_OF_TOTAL_MEMORY_FOR_USER:
				// process!
				rt_mem_size = total_ext_mem_size;	// form Kilobyte UNIT
				l_ret = MEMORY_TM_SUCCESS;
				break;
				
			case SUB2_SIZE_OF_USED_MEMORY:
				// process!
				rt_mem_size = used_ext_mem_size;	// form Kilobyte UNIT
				l_ret = MEMORY_TM_SUCCESS;
				break;				

			case SUB2_SIZE_OF_USABLE_MEMORY:
				// process!
				rt_mem_size = available_ext_mem_size;	// form Kilobyte UNIT
				l_ret = MEMORY_TM_SUCCESS;
				break;	
		}
	}
	
	if (total_ext_mem_size==0)
		l_ret = MEMORY_TM_FAIL;

	// store the result of process
	relay_result->ret_value = l_ret;

	// put the result to rpc buffer!
	memcpy(relay_result->ret_string, &rt_mem_size, 4);

// END [sangki.hyun@lge.com] 20100710 LAB1_FW }

	return relay_result;

}


void *testmode_efs_integrity(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
	struct testmode_relay_result *relay_result;
	//unsigned int rt_efs_integrity = 2;
	unsigned char rt_efs_integrity = 2;
	int l_ret = MEMORY_TM_FAIL;
	int fp;
	int l_size;
	unsigned char *buffer;
	unsigned char file_name[50]={0,};	

	unsigned char l_attb[50]={0,};
	unsigned char l_own[50]={0,};
	unsigned char l_usr[50]={0,};	
	unsigned char l_data[50]={0,};
	unsigned char l_time[50]={0,};
	unsigned char l_f_name[50]={0,};
	int l_file_size = -1;

  	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);

	relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

	// to real specific test! and get rt_efs_integrity! and l_ret
	switch(sub2_cmd)
		{
#if 0		
			case SUB2_EFS_INTEGRITY_TEST:
				// process!
				break;
				
			case SUB2_EFS_INTEGRITY_CRC_TEST:
				// process!
				break;				
#endif
			case SUB2_INTERNAL_MEMORY_INTEGRITY_TEST:
				// process!
#if 1
				shell_cmd_in_kernel("/system/bin/ls /sdcard > /data/ls_sdcard");
				shell_cmd_in_kernel("/system/bin/ls /sdcard/DCIM > /data/ls_sdcard_DCIM");
				shell_cmd_in_kernel("/system/bin/ls /sdcard/DCIM/Camera > /data/ls_sdcard_Camera");
				shell_cmd_in_kernel("/system/bin/ls /sdcard/image > /data/ls_sdcard_image");
				shell_cmd_in_kernel("/system/bin/ls /sdcard/video > /data/ls_sdcard_video");
				shell_cmd_in_kernel("/system/bin/ls /sdcard/music > /data/ls_sdcard_music");
				shell_cmd_in_kernel("/system/bin/ls /sdcard/doc > /data/ls_sdcard_doc");
				//shell_cmd_in_kernel("cd /data");
				sys_chdir("/data");
				shell_cmd_in_kernel("/system/bin/cat ls_sdcard ls_sdcard_DCIM ls_sdcard_Camera ls_sdcard_image ls_sdcard_video ls_sdcard_music ls_sdcard_doc > for_integrity");
				shell_cmd_in_kernel("/system/bin/cmp /data/for_integrity /dic/for_integrity > /data/cmp_result");
				shell_cmd_in_kernel("/system/bin/ls -l /data/cmp_result  > /data/check_result");
#else
				//sys_chmod("/system/etc/integrity_check.sh",777);
				shell_cmd_in_kernel("chmod 777 /system/etc/integrity_check.sh");
				shell_cmd_in_kernel("/system/bin/sh /system/etc/integrity_check.sh ");
				
				//shell_cmd_in_kernel("/system/etc/integrity_check.sh ");

#endif

				l_size = 200;	// "test"
				memcpy(file_name,"/data/check_result",18);
				
				fp = sys_open(file_name, O_RDONLY, 0);
				if (fp < 0) {
					printk(KERN_WARNING "%s: Can not open %s\n",
						__func__, file_name);
				}
				
				sys_lseek(fp, (off_t)0, 0);
				
				buffer = kmalloc(l_size, GFP_KERNEL);
				
				if ((unsigned)sys_read(fp, (char *)buffer, l_size) != l_size) {
					printk(KERN_WARNING "\n(test mode)-(fail) testmode_efs_integrity (file=%s)-(filename=%s)\n",__func__,file_name);
				}
				
				sscanf(buffer,"%s %s %s %d %s %s %s ",l_attb,l_own,l_usr,&l_file_size,l_data,l_time,l_f_name);
				printk("(integrity test) %s %s %s %d %s %s %s \n",l_attb,l_own,l_usr,l_file_size,l_data,l_time,l_f_name);
					
				kfree(buffer);
				sys_close(fp);		

#if 1
				shell_cmd_in_kernel("/system/bin/rm /data/ls_sdcard");
				shell_cmd_in_kernel("/system/bin/rm /data/ls_sdcard_image");
				shell_cmd_in_kernel("/system/bin/rm /data/ls_sdcard_video");
				shell_cmd_in_kernel("/system/bin/rm /data/ls_sdcard_music");
				shell_cmd_in_kernel("/system/bin/rm /data/ls_sdcard_doc");
				
				shell_cmd_in_kernel("/system/bin/rm /data/for_integrity");
				shell_cmd_in_kernel("/system/bin/rm /data/cmp_result");
				shell_cmd_in_kernel("/system/bin/rm /data/check_result");
#endif
				sys_chdir("/");

				if (l_file_size == 0)
					{	// integrity OK!
						rt_efs_integrity = 1;
						l_ret = MEMORY_TM_SUCCESS;
						printk("=========> integrity OK");
					}
				else
					{	// there is something different
						rt_efs_integrity = 0;
						l_ret = MEMORY_TM_SUCCESS;//MEMORY_TM_FAIL;
						printk("=========> integrity Fail");
					}

				break;	
		}

	// store the result of process
	relay_result->ret_value = l_ret;

	// put the result to rpc buffer!
	#if 1
	memcpy(relay_result->ret_string, &rt_efs_integrity, 1);
	#else
	memcpy(relay_result->ret_string, &rt_efs_integrity, 4);
	#endif

	set_fs(old_fs);


	return relay_result;

}


// should  be rename from uv_sensor_test to gbnand_test
void *testmode_uv_sensor_test(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
	struct testmode_relay_result *relay_result;
	unsigned char rt_gbnand = 1;
	int l_ret = MEMORY_TM_FAIL;
	int fp;
	int l_size;
	unsigned char *buffer;
	unsigned char file_name[50]={0,};
	int l_loop=0;
//	int l_result1 = -1;
	int l_result2 = -1;
	int l_dev_num;
	char cmd_string[100]={0,};

	int total_ext_mem_size = 0;
	int used_ext_mem_size = 0;
	int available_ext_mem_size = 0;
	int l_blk_size = 0;


	relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);

	// to real specific test! and get rt_gbnand! and l_ret

	local_irq_disable();


//=================== < unmount >
//	shell_cmd_in_kernel("/system/bin/umount /lgtdata");
	shell_cmd_in_kernel("/system/bin/umount /sdcard");	
//=================== < format >

#if 0
	shell_cmd_in_kernel("/system/bin/getprop GBNAND.DEV.MINOR > /system/etc/for_gbdev.tst");

	for(l_loop=0;l_loop<15000;l_loop++);

	l_size = 10;	// "test"
	memcpy(file_name,"/system/etc/for_gbdev.tst",25);
#endif

	l_size = 20;

	fp = sys_open("/system/etc/gbnand-dev-minor", O_RDONLY, 0);
	if (fp < 0) {
		printk(KERN_WARNING "%s: Can not open %s\n",
			__func__, "/system/etc/gbnand-dev-minor");
	}
	
	sys_lseek(fp, (off_t)0, 0);
	
	buffer = kmalloc(l_size, GFP_KERNEL);
	memset(buffer,0x00,l_size);
	
	if ((unsigned)sys_read(fp, (char *)buffer, l_size) != l_size) {
		printk(KERN_WARNING "\n(test mode)-(fail) sys_read fail (file=%s)-(filename=%s)-(l_size=%d)\n",__func__,"/system/etc/gbnand-dev-minor",l_size);
	}

	printk("..................test............................>>  (%s)\n",buffer);
	sscanf(buffer,"MINOR=%d",&l_dev_num);
	printk("..................test............................>>  (%d)\n",l_dev_num);

#if 0	
	sprintf(cmd_string,"/system/bin/mke2fs -b 4096 -m 1 -L android -j -v /dev/block/vold/179:%d ",l_dev_num+1);
	l_result1 = shell_cmd_in_kernel(cmd_string);
	
	memset(cmd_string,0x00,100);
#endif
#if 1
	sprintf(cmd_string,"/system/bin/newfs_msdos -F 32 -c 16 -L sdcard -O android /dev/block/vold/179:%d ",l_dev_num+3);
	l_result2 = shell_cmd_in_kernel(cmd_string);
#endif

	kfree(buffer);
	sys_close(fp);		

//	shell_cmd_in_kernel("/system/bin/rm /system/etc/for_gbdev.tst");
//====================== < re-mount >
#if 0
	memset(cmd_string,0x00,100);

	sprintf(cmd_string, "/system/bin/mount -t ext3 /dev/block/vold/179:%d /lgtdata", l_dev_num+1);
	shell_cmd_in_kernel(cmd_string);
#endif
#if 1	
	for(l_loop=0;l_loop<15000;l_loop++);
	
	memset(cmd_string,0x00,100);

	sprintf(cmd_string, "/system/bin/mount -t vfat /dev/block/vold/179:%d /sdcard", l_dev_num+3);
	shell_cmd_in_kernel(cmd_string);
#endif		

	switch(sub2_cmd)
		{
			case SUB2_INTERNAL_MEMORY_TEST:
				// process!
				shell_cmd_in_kernel("/system/bin/df /sdcard > /system/etc/ext_form.tst");
				
				//do 
				{
					for(l_loop=0;l_loop<5000;l_loop++);
					//fp = sys_open("/system/etc/ext_test.tst",O_RDONLY, 0);
				}
				//while(fp < 0);
				
				l_size = 200;	// "test"
				memset(file_name,0x00,50);
				memcpy(file_name,"/system/etc/ext_form.tst",24);
				
				fp = sys_open(file_name, O_RDONLY, 0);
				if (fp < 0) {
					printk(KERN_WARNING "%s: Can not open %s\n",
						__func__, file_name);
				}
				
				sys_lseek(fp, (off_t)0, 0);
				
				buffer = kmalloc(l_size, GFP_KERNEL);
				
				if ((unsigned)sys_read(fp, (char *)buffer, l_size) != l_size) {
					printk(KERN_WARNING "\n(test mode)-(fail) testmode_external_memory for format verify (file=%s)-(filename=%s)\n",__func__,file_name);
				}
				
				sscanf(buffer,"/sdcard: %dK total, %dK used, %dK available <block size %d> ",&total_ext_mem_size,&used_ext_mem_size,&available_ext_mem_size,&l_blk_size);
					
				kfree(buffer);
				sys_close(fp);		
				
				shell_cmd_in_kernel("/system/bin/rm /system/etc/ext_form.tst");

				// verify the formatted media
				if ((used_ext_mem_size ==8)/*&&(total_ext_mem_size)*/)
					{
						rt_gbnand = 0;	
						l_ret = MEMORY_TM_SUCCESS;	
					}
				else
					{
						rt_gbnand = 1;						
						l_ret = MEMORY_TM_FAIL; 
					}
				

				break;
				
			case SUB2_INTERNAL_MOMORY_FORMAT:
				// process!
				if (/*(l_result1 ==0 )&&*/(l_result2 ==0 ))
					{
						rt_gbnand = 0;	
						l_ret = MEMORY_TM_SUCCESS;	
					}
				else
					{
						rt_gbnand = 1;						
						l_ret = MEMORY_TM_FAIL;	
					}
				

				break;				

		}

	// directory initialize in /sdcard @ gbnand 
	#if 1
	sys_mkdir("/sdcard/image", 0777);
	sys_mkdir("/sdcard/video", 0777);
	sys_mkdir("/sdcard/music", 0777);
	sys_mkdir("/sdcard/doc", 0777);
	#else
	shell_cmd_in_kernel("/system/bin/mkdir 777 /sdcard/image");
	shell_cmd_in_kernel("/system/bin/mkdir 777 /sdcard/video");
	shell_cmd_in_kernel("/system/bin/mkdir 777 /sdcard/music");
	shell_cmd_in_kernel("/system/bin/mkdir 777 /sdcard/doc");
	#endif

	local_irq_enable();


	// store the result of process
	relay_result->ret_value = l_ret;

	// put the result to rpc buffer!
	memcpy(relay_result->ret_string, &rt_gbnand, 1);


	return relay_result;

}

static int shell_cmd_in_kernel( char *cmd )
{
	char cmd_string[200]={0,};
	char *envp[] = {"HOME=/","TERM=linux",NULL,};
	char *argv[] = {"sh","-c",cmd_string,NULL,};
	int ret = 0;
	
	//sprintf(cmd_string, "/system/bin/%s", cmd);
	sprintf(cmd_string, "%s", cmd);
	
	if ((ret = call_usermodehelper("/system/bin/sh", argv, envp, UMH_WAIT_PROC)) != 0)
	 {
		printk(KERN_ERR "call usermodehelper function call fail %d\n",ret);
		printk(KERN_ERR "------------------------->cmd: %s\n",cmd_string);
		return ret;
	 }

	return ret;

}

void *testmode_memory_bad_block_check(uint32_t sub1_cmd, uint32_t sub2_cmd)
{
    struct testmode_relay_result *relay_result;
    int block_count = 0;
    int curblk = 0, nand_curblk = 0;
    int arm11_bad_block_total_num = 0;

    relay_result = (struct testmode_relay_result *)kzalloc(sizeof(struct testmode_relay_result), GFP_KERNEL);
    relay_result->ret_value = MEMORY_TM_SUCCESS;
    
    switch(sub2_cmd)
    {
        case SUB2_BAD_BLOCK_TOTAL_NUMBER:
        {
            testmode_nand_partition parti_index = BOOT_PARTI_INDEX;

            memset( arm11_bad_block_address, 0xFFFF, (sizeof(uint16_t) * BAD_BLOCK_MAX_LENGTH) );
            
            while( parti_index != PARTI_NAME_LAST )
            {
                
                mtd = get_mtd_device_nm( arm11_parti_table[parti_index].parti_name);
                
                if(mtd == NULL)
                {
                    relay_result->ret_value = MEMORY_TM_FAIL;
                    break;
                }
                
                block_count = mtd->size;
                do_div(block_count, mtd->erasesize);
                curblk = 0;
                
                while (curblk < block_count)
                {
                    loff_t addr = curblk * mtd->erasesize;
                    
                    if (mtd->block_isbad(mtd, addr) == IS_BLOCK_BAD)
                    {
                        if( arm11_bad_block_total_num < BAD_BLOCK_MAX_LENGTH )
                        {
                            /* 20 개 만 return 한다 */
                            arm11_bad_block_address[arm11_bad_block_total_num] = nand_curblk + curblk;
                        }
                        arm11_bad_block_total_num++;
                    }
                    curblk++;
                }
                nand_curblk += curblk;

                parti_index++;
            }

            relay_result->ret_value = MEMORY_TM_SUCCESS;
            memcpy(relay_result->ret_string, &arm11_bad_block_total_num, 4);  
            
            break;
        }

        case SUB2_BAD_BLOCK_ADDRESS:
            memcpy(relay_result->ret_string, arm11_bad_block_address, (sizeof(uint16_t) * BAD_BLOCK_MAX_LENGTH) );
            relay_result->ret_value = MEMORY_TM_SUCCESS;
            break;
        
        default:
            relay_result = testmode_reponse_not_supported();
            break;
    }
    
    return relay_result;
    
}
// END [sangki.hyun@lge.com] 20100721 LAB1_FW : Testmode 8.1 }


