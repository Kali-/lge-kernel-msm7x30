#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>

#include "../../broadcast_tdmb_typedef.h"
#include "../../broadcast_tdmb_drv_ifdef.h"
#include "../inc/tdmb_tunerbbdrv_t3900def.h"

static int g_ch_setting_done = ERROR;
int broadcast_drv_if_power_on(void)
{
	int8 rc = ERROR;
	boolean retval = FALSE;

	retval = tunerbb_drv_t3900_power_on( );

	if(retval == TRUE)
	{
		rc = OK;
	}
	tunerbb_drv_t3900_set_userstop( 1 );

	return rc;
}


int broadcast_drv_if_power_off(void)
{
	int8 rc = ERROR;
	boolean retval = FALSE;

	retval = tunerbb_drv_t3900_power_off( );

	if(retval == TRUE)
	{
		rc = OK;
	}
	tunerbb_drv_t3900_set_userstop( 1 );

	return rc;
}


int broadcast_drv_if_open(void) 
{
	int8 rc = ERROR;
	boolean retval = FALSE;

	printk("broadcast_drv_if_open\n");
	retval = tunerbb_drv_t3900_init( );

	if(retval == TRUE)
	{
		rc = OK;
	}

	return rc;	
}


int broadcast_drv_if_close(void)
{
	int8 rc = ERROR;
	boolean retval = FALSE;

	retval = tunerbb_drv_t3900_stop( );

	if(retval == TRUE)
	{
		rc = OK;
	}

	return rc;
}


int broadcast_drv_if_set_channel(unsigned int freq_num, unsigned int subch_id, unsigned int op_mode)
{
	int8 rc = ERROR;
	boolean retval = FALSE;

	//printk("broadcast_drv_if_set_channel IN( )\n");
	retval = tunerbb_drv_t3900_set_channel(freq_num, subch_id, op_mode);
	//printk("broadcast_drv_if_set_channel OUT( ) result = (%d)\n", retval);
	if(retval == TRUE)
	{
		rc = OK;
	}

	g_ch_setting_done = rc;

	return rc;	
}

int broadcast_drv_if_resync(void)
{
	return ERROR;
}

int broadcast_drv_if_detect_sync(int op_mode)
{
	int8 rc = ERROR;
	boolean retval = FALSE;

	if(g_ch_setting_done == OK)
	{
		printk("broadcast_drv_if_detect_sync. channel_set_ok = (%d)\n", g_ch_setting_done);
		return OK;
	}
	
	retval = tunerbb_drv_t3900_re_syncdetector(op_mode);

	if(retval == TRUE)
	{
		rc = OK;
	}

	g_ch_setting_done = rc;

	return rc;
}

int broadcast_drv_if_get_sig_info(struct broadcast_tdmb_sig_info *dmb_bb_info)
{
	int8 rc = ERROR;
	boolean retval = FALSE;

	retval = tunerbb_drv_t3900_get_ber(dmb_bb_info);

	if(retval == TRUE)
	{
		rc = OK;
	}

	if(g_ch_setting_done == ERROR)
	{
		dmb_bb_info->cir = 0;
		dmb_bb_info->msc_ber = 20000;
	}
	else
	{
		dmb_bb_info->cir = 1;
	}
	//printk("broadcast_drv_if_get_sig_info ber = (%d)\n", dmb_bb_info->msc_ber);
	return rc;
}


int broadcast_drv_if_get_ch_info(char* buffer, unsigned int* buffer_size)
{
	int8 rc = ERROR;
	boolean retval = FALSE;

	if(buffer == NULL || buffer_size == NULL)
	{
		printk("broadcast_drv_if_get_ch_info argument error\n");
		return rc;
	}

	retval = tunerbb_drv_t3900_get_fic(buffer, buffer_size);

	if(retval == TRUE)
	{
		rc = OK;
	}

	return rc;	
}


int broadcast_drv_if_get_dmb_data(char** buffer_ptr, unsigned int* buffer_size, unsigned int user_buffer_size)
{
	return ERROR;
}

int broadcast_drv_if_reset_ch(void)
{
	int8 rc = ERROR;
	boolean retval = FALSE;

	retval = tunerbb_drv_t3900_reset_ch( );

	if(retval == TRUE)
	{
		rc = OK;
	}

	return rc;
}

int broadcast_drv_if_user_stop(int mode)
{
	tunerbb_drv_t3900_set_userstop( mode );
	return OK ;
}

int broadcast_drv_if_select_antenna(unsigned int sel)
{
	tunerbb_drv_t3900_select_antenna(sel);
	return OK;
}

int broadcast_drv_if_isr(void)
{
	return ERROR;
}

