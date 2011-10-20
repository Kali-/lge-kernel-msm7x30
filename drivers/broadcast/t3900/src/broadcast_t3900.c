#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <linux/i2c.h>
#include <linux/pm.h>

#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/wakelock.h> 		/* wake_lock, unlock */

#include <mach/board_lge.h>

#include "../../broadcast_tdmb_drv_ifdef.h"
#include "../inc/broadcast_t3900.h"


//#define DELAY_USING_WAIT_EVENT_TIMEOUT  /* wait_event_timeout instead of msleep */
#ifdef DELAY_USING_WAIT_EVENT_TIMEOUT
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#endif

struct broadcast_t3900_ctrl_data
{
	int								pwr_state;
	struct wake_lock					wakelock;
	struct i2c_client*					pclient;
	struct broadcast_device_platform_data*	pctrl_fun;		/* defined in board_lge.h */
};

static struct broadcast_t3900_ctrl_data  TdmbCtrlInfo;

#ifdef DELAY_USING_WAIT_EVENT_TIMEOUT
#define SLEEP_WAIT_UNTIL_MS 0
#define SLEEP_EXIT_USER_STOP 1
static DECLARE_WAIT_QUEUE_HEAD(msleep_wait_queue);  /*wait_event_timeout queue */
static uint32 msleep_exit_condition = SLEEP_WAIT_UNTIL_MS;		/* sleep exit condition not timeout(sleep) */
static uint32 msleep_wait_queue_init = 0;
#else
static uint32 user_stop_flg = 0;
//static uint32 mdelay_in_flg = 0;
#endif  /* DELAY_USING_WAIT_EVENT_TIMEOUT */

struct i2c_client*	INC_GET_I2C_DRIVER(void)
{
	return TdmbCtrlInfo.pclient;
}


#ifdef DELAY_USING_WAIT_EVENT_TIMEOUT
void tdmb_t3900_set_userstop(void)
{
	if(msleep_exit_condition == SLEEP_WAIT_UNTIL_MS)
	{
		msleep_exit_condition = SLEEP_EXIT_USER_STOP;
		wake_up(&msleep_wait_queue);
	}
}

int tdmb_t3900_mdelay(int32 ms)
{
	int rc = 1;
	int wait_rc = OK;

	if(msleep_wait_queue_init == 0)
	{
		init_waitqueue_head(&msleep_wait_queue);
		msleep_wait_queue_init = 1;
	}

	msleep_exit_condition = SLEEP_WAIT_UNTIL_MS;
	/* sleep during msec set or msleep_exit_condition meet */
	wait_rc = wait_event_timeout(msleep_wait_queue, 
		(msleep_exit_condition == SLEEP_EXIT_USER_STOP), msecs_to_jiffies(ms));

	/* wait exit becaus of user stop not timeout */
	if(msleep_exit_condition == SLEEP_EXIT_USER_STOP)
	{
		rc = 0;
	}
	
	msleep_exit_condition = SLEEP_WAIT_UNTIL_MS;
	return rc;
}
#else
void tdmb_t3900_set_userstop(int mode)
{
	user_stop_flg = mode;
}

int tdmb_t3900_mdelay(int32 ms)
{
	int		rc = 1;  /* 0 : false, 1 : ture */
	int32	wait_loop =0;
	int32	wait_ms = ms;

	//mdelay_in_flg = 1;
	if(ms > 100)
	{
		wait_loop = (ms /100);   /* 100, 200, 300 more only , Otherwise this must be modified e.g (ms + 40)/50 */
		wait_ms = 100;
	}

	do
	{
		msleep(wait_ms);
		if(user_stop_flg == 1)
		{
			//printk("~~~~~~~~ Ustop flag is set so return false ms =(%d)~~~~~~~\n", ms);
			rc = 0;
			break;
		}
	}while((--wait_loop) > 0);

	//mdelay_in_flg = 0;
	//user_stop_flg = 0;

	//if(rc == 0)
	//{
		//printk("tdmb_t3900_delay return abnormal\n");
	//}
	return rc;
}
#endif  /* DELAY_USING_WAIT_EVENT_TIMEOUT */

void tdmb_t3900_must_mdelay(int32 ms)
{
	msleep(ms);
}

int tdmb_t3900_power_on(void)
{
	/*
	if(TdmbCtrlInfo.pwr_state == 1)
	{
		printk("tdmb_t3900_power is immediately on\n");
		return TRUE;
	}
	*/
	if((TdmbCtrlInfo.pctrl_fun == NULL) ||(TdmbCtrlInfo.pctrl_fun->dmb_power_on == NULL))
	{
		printk("tdmb_t3900_power_on function NULL\n");
		return FALSE;
	}

	wake_lock(&TdmbCtrlInfo.wakelock);
	TdmbCtrlInfo.pctrl_fun->dmb_power_on( );
	TdmbCtrlInfo.pwr_state = 1;

	return TRUE;
}

int tdmb_t3900_power_off(void)
{
	if(TdmbCtrlInfo.pwr_state == 0)
	{
		printk("tdmb_t3900_power is immediately off\n");
		return TRUE;
	}

	if((TdmbCtrlInfo.pctrl_fun == NULL) ||(TdmbCtrlInfo.pctrl_fun->dmb_power_off == NULL))
	{
		printk("tdmb_t3900_power_off function NULL\n");
		return FALSE;
	}

	TdmbCtrlInfo.pwr_state = 0;
	TdmbCtrlInfo.pctrl_fun->dmb_power_off( );
	wake_unlock(&TdmbCtrlInfo.wakelock);
	return TRUE;
}

int tdmb_t3900_select_antenna(unsigned int sel)
{
	return FALSE;
}


static int tdmb_t3900_i2c_write(uint8* txdata, int length)
{
	int rc;
	struct i2c_msg msg = 
	{
		TdmbCtrlInfo.pclient->addr,
		0,
		length,
		txdata
	};

	//if(i2c_transfer(TdmbCtrlInfo.pclient->adapter, &msg, 1) < 0)
	rc = i2c_transfer(TdmbCtrlInfo.pclient->adapter, &msg, 1);
	if(rc < 0)
	{
		printk("tdmb_t3900_i2c_write fail rc = (%d) addr =(0x%X)\n", rc, TdmbCtrlInfo.pclient->addr);
		return FALSE;
	}
	return TRUE;
}

int tdmb_t3900_i2c_write_burst(uint16 waddr, uint8* wdata, int length)
{
	uint8* buf;
	int	wlen;

	int rc;	/* success : 1 , fail : 0 */

	wlen = length + 2;

	buf = (uint8*)kmalloc(wlen, GFP_KERNEL);

	if((buf == NULL) || (length <= 0))
	{
		printk("tdmb_t3900_i2c_write_burst buf alloc fail\n");
		return FALSE;
	}

	buf[0] = (waddr>>8)&0xFF;
	buf[1] = (waddr&0xFF);

	memcpy(&buf[2], wdata, length);

	rc = tdmb_t3900_i2c_write(buf, wlen);

	kfree(buf);

	return rc;	
}


static int tdmb_t3900_i2c_read( uint16 raddr, uint8 *rxdata, int length)
{
	int rc;
	uint8 r_addr[2] = {raddr>>8, raddr&0xFF};

	struct i2c_msg msgs[2] = 
	{
		{
			.addr	= TdmbCtrlInfo.pclient->addr,
			.flags = 0,
			.len   = 2,
			.buf   = &r_addr[0],
		},
		{
			.addr	= TdmbCtrlInfo.pclient->addr,
			.flags = I2C_M_RD,
			.len   = length,
			.buf   = rxdata,
		},
	};

	//if (i2c_transfer(TdmbCtrlInfo.pclient->adapter, msgs, 2) < 0) 
	rc = i2c_transfer(TdmbCtrlInfo.pclient->adapter, msgs, 2);
	if(rc < 0)
	{
		printk("tdmb_t3900_i2c_read failed! rc =(%d),%x \n", rc, TdmbCtrlInfo.pclient->addr);
		return FALSE;
	}
	return TRUE;		
};

int tdmb_t3900_i2c_read_burst(uint16 raddr, uint8* rdata, int length)
{
	int rc;
 
	rc = tdmb_t3900_i2c_read(raddr, rdata, length);

	return rc;
}

int tdmb_t3900_i2c_write16(unsigned short reg, unsigned short val)
{
	int err;
	unsigned char buf[4] = { reg>>8, reg&0xff, val>>8, val&0xff };
	struct i2c_msg msg = 
	{	
		TdmbCtrlInfo.pclient->addr,
		0,
		4,
		buf 
	};
	
	if ((err = i2c_transfer( TdmbCtrlInfo.pclient->adapter, &msg, 1)) < 0) 
	{
		dev_err(&TdmbCtrlInfo.pclient->dev, "i2c write error\n");
		err = FALSE;
	}
	else
	{
		//printk(KERN_INFO "tdmb : i2c write ok:addr = %x data = %x\n", reg, val);
		err = TRUE;
	}

	return err;
}

int tdmb_t3900_i2c_read16(uint16 reg, uint16 *ret)
{
	int err;
	uint8 w_buf[2] = {reg>>8, reg&0xff};	
	uint8 r_buf[2] = {0,0};

	struct i2c_msg msgs[2] =
	{
		{
			.addr	= TdmbCtrlInfo.pclient->addr,
			.flags = 0,
			.len   = 2,
			.buf   = &w_buf[0],
		},
		{
			.addr	= TdmbCtrlInfo.pclient->addr,
			.flags = I2C_M_RD,
			.len   = 2,
			.buf   = &r_buf[0],
		},
	};
	
	if ((err = i2c_transfer(TdmbCtrlInfo.pclient->adapter, msgs, 2)) < 0) 
	{
		dev_err(&TdmbCtrlInfo.pclient->dev, "i2c read error\n");
		err = FALSE;
	}
	else
	{
		//printk( "tdmb addr = %x : i2c read ok: data[0] = %x data[1] = %x \n", TdmbCtrlInfo.pClient->addr, r_buf[0], r_buf[1]);
		*ret = r_buf[0]<<8 | r_buf[1];
		//printk( "tdmb : i2c read ok: data = %x\n", *ret);
		err = TRUE;
	}

	return err;
}

void	tdmb_rw_test(void)
{
	unsigned short i = 0;
	unsigned short w_val = 0;
	unsigned short r_val = 0;
	unsigned short err_cnt = 0;

	err_cnt = 0;
	for(i=1;i<11;i++)
	{
		w_val = (i%0xFF);
		tdmb_t3900_i2c_write16( 0x0a00+ 0x05, w_val);
		tdmb_t3900_i2c_read16(0x0a00+ 0x05, &r_val );
		if(r_val != w_val)
		{
			err_cnt++;
			printk("w_val:%x, r_val:%x\n", w_val,r_val);
		}
	}
}


static int  broadcast_t3900_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int rc;
	struct broadcast_device_platform_data *pdata;

	memset((void*)&TdmbCtrlInfo, 0x00, sizeof(struct broadcast_t3900_ctrl_data));

	printk("broadcast_t3900_i2c_probe( )\n");

	if(!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk(KERN_ERR "tdmb_lg2102_i2c_probe: need I2C_FUNC_I2C\n");
		rc = -ENODEV;
		return rc;
	}
	
	TdmbCtrlInfo.pclient = client;

	i2c_set_clientdata(client, (void*)&TdmbCtrlInfo);

	/* Register power control function */
	TdmbCtrlInfo.pwr_state = 0;
	pdata = (struct broadcast_device_platform_data*)client->dev.platform_data;
	TdmbCtrlInfo.pctrl_fun = pdata;

	if(TdmbCtrlInfo.pctrl_fun && TdmbCtrlInfo.pctrl_fun->dmb_gpio_init)
	{
		TdmbCtrlInfo.pctrl_fun->dmb_gpio_init( );
	}
	else
	{
		printk("broadcast_t3900_i2c_probe dmb_gpio_init is not called\n");
	}

	wake_lock_init(&TdmbCtrlInfo.wakelock, WAKE_LOCK_SUSPEND, dev_name(&client->dev));

	return OK;
}

static int broadcast_t3900_i2c_remove(struct i2c_client* client)
{
	printk("broadcast_t3900_i2c_remove is called\n");
	wake_lock_destroy(&TdmbCtrlInfo.wakelock);
	return OK;
}




static const struct i2c_device_id tdmb_t3900_id[] = {
	{"tdmb_t3900",	0},
	{},
};

MODULE_DEVICE_TABLE(i2c, tdmb_t3900_id);

static struct i2c_driver t3900_i2c_driver = {
	.probe = broadcast_t3900_i2c_probe,
	.remove = broadcast_t3900_i2c_remove,
	.id_table = tdmb_t3900_id,
	.driver = {
		.name = "tdmb_t3900",
		.owner = THIS_MODULE,
	},
};

int __devinit broadcast_tdmb_drv_init(void)
{
	int rc;
	printk("broadcast_tdmb_drv_init\n");

	rc = broadcast_tdmb_drv_start();
	
	if (rc) {
		printk("broadcast_tdmb_drv_start %s failed to load\n", __func__);
		return rc;
	}
	rc = i2c_add_driver(&t3900_i2c_driver);
	printk("broadcast_i2c_add_driver rc = (%d)\n", rc);

	return rc;
}

static void __exit broadcast_tdmb_drv_exit(void)
{
	i2c_del_driver(&t3900_i2c_driver);
}


/* EXPORT_SYMBOL() : when we use external symbol 
which is not included in current module - over kernel 2.6 */
//EXPORT_SYMBOL(broadcast_tdmb_is_on);

module_init(broadcast_tdmb_drv_init);
module_exit(broadcast_tdmb_drv_exit);
MODULE_DESCRIPTION("broadcast_tdmb_drv_init");
MODULE_LICENSE("INC");
