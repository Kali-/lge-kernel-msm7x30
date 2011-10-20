/* arch/arm/mach-msm/qdsp5v2/lge_wm9093-e739.c
 *
 * Copyright (C) 2010 LGE, Inc.
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

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/device.h>
#include <asm/gpio.h>
#include <asm/system.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <asm/ioctls.h>
#include <mach/debug_mm.h>
#include <linux/slab.h>
#include "mach/qdsp5v2/lge_wm9093_e739.h"

#define MODULE_NAME	"lge_amp"

char amp_cal_data[AMP_CAL_MAX] = {IN_VOL_0, OUT_VOL_DEFAULT-7,                    /* HEADSET_VOICE */
                                                                  IN_VOL_0, OUT_VOL_DEFAULT-3,            /* SPEAKER_VOICE */
                                                                  IN_VOL_0, OUT_VOL_DEFAULT,          /* TTY */
                                                                  IN_VOL_0, OUT_VOL_DEFAULT+1,          /* HEADSET_AUDIO */
                                                                  IN_VOL_12, OUT_VOL_DEFAULT-12, OUT_VOL_DEFAULT,  /* HEADSET_SPEAKER */
                                                                  IN_VOL_0, OUT_VOL_DEFAULT-1};        /* SPEAKER_AUDIO */

struct wm9093_reg_type
{
  u8  reg;
  u8  val[2];
};

struct amp_data {
	struct i2c_client *client;
};


static struct mutex amp_lock;
static struct amp_data *_data = NULL;
bool  bHeadset_OFF = false;

static int amp_read_register(u8 reg, int* ret)
{
	struct i2c_msg	xfer[2];
	u16				data = 0xffff;
	u16				retval;


	xfer[0].addr = _data->client->addr;
	xfer[0].flags = 0;
	xfer[0].len  = 1;
	xfer[0].buf = &reg;

	xfer[1].addr = _data->client->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = 2;
	xfer[1].buf = (u8*)&data;

	retval = i2c_transfer(_data->client->adapter, xfer, 2);

	*ret =  (data>>8) | ((data & 0xff) << 8);

	return retval;
}

int wm9093_readConfig(int regAdd, u8 *regVal)
{
	int retval = 0;
	struct wm9093_reg_type cfg;
	int read_data;

	cfg.reg = regAdd;
	cfg.val[0] = *regVal;
	cfg.val[1] = *(regVal+1);
	retval |= amp_read_register(cfg.reg, &read_data);

	return read_data;
}

static int amp_write_register(u8 reg, u16 val)
{
	int				 err;
	unsigned char    buf[3];
    unsigned char temp;
	struct i2c_msg	msg = { _data->client->addr, 0, 3, &buf[0] };

	buf[0] = reg;
	memcpy(&buf[1], &val, sizeof(u16));

    temp = buf[1];
    buf[1] = buf[2];
    buf[2] = temp;

	if ((err = i2c_transfer(_data->client->adapter, &msg, 1)) < 0){
		return -EIO;
	}
	else {
		return 0;
	}
}
/*
static uint32_t msm_snd_debug = 1;
module_param_named(debug_mask, msm_snd_debug, uint, 0664);

#if DEBUG_AMP_CTL
#define D(fmt, args...) printk(fmt, ##args)
#else
#define D(fmt, args...) do {} while(0)
#endif
*/
void set_amp_PowerDown(void)
{
	int ret=0;

    if (bHeadset_OFF)
    {
      ret |= amp_write_register(0x01, 0x000B);
      msleep(50);
      ret |= amp_write_register(0x39, 0x000D);
      ret |= amp_write_register(0x49, 0x0110);
      msleep(3);
    }

    ret |= amp_write_register(0x00, 0x0000);
    ret |= amp_write_register(0x39, 0x0000);

    MM_INFO("1 set_amp_PowerDown() %d\n", ret);
}
EXPORT_SYMBOL(set_amp_PowerDown);

void set_amp_headset_mono_voice(void)
{
	int ret=0;

    ret |= amp_write_register(AMP_RESET, RESET_DEFAULT);
    ret |= amp_write_register(ANTI_POP, (AMP_8BIT_USE)&(ANTI_POP_VMID) );

    msleep(50);

	ret |= amp_write_register(OUTPUT_ENA, (AMP_16BIT_USE)&(OUT_HPH) );
	ret |= amp_write_register(INPUT_ENA, (AMP_16BIT_USE)&((TSHUT<<8)| IN1_EN) );
	ret |= amp_write_register(IN1A_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[HEADSET_VOICE_INPUT]) );  ////////////////////////
	ret |= amp_write_register(IN1B_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[HEADSET_VOICE_INPUT]) );  ////////////////////////

        
	ret |= amp_write_register(MIX_OUT_L, (AMP_8BIT_USE)&(MIX_IN1A) );
	ret |= amp_write_register(MIX_OUT_R, (AMP_8BIT_USE)&(MIX_IN1A) );

	ret |= amp_write_register(HPH_L_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));  ///////////////////
	ret |= amp_write_register(HPH_R_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));  //////////////////
	ret |= amp_write_register(MIX_EN, (AMP_16BIT_USE)&((SPK_VOL_DIS<<8)|MIXOUT_HPH));

	ret |= amp_write_register(HPOUT_L_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[HEADSET_VOICE_OUTPUT]));  ////////////////////////////
	ret |= amp_write_register(HPOUT_R_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[HEADSET_VOICE_OUTPUT]));  ///////////////////////////


	ret |= amp_write_register(IN1_CONF, (AMP_16BIT_USE)&(CONF_DIFF));
  
	ret |= amp_write_register(0x46, 0x0100);
	ret |= amp_write_register(0x49, 0x0100);

    bHeadset_OFF = true;
    MM_INFO("2 set_amp_headset_mono_voice() %d\n", ret);
}
EXPORT_SYMBOL(set_amp_headset_mono_voice);

void set_amp_speaker_stereo_voice(void)
{
	int ret=0;

    ret |= amp_write_register(AMP_RESET, RESET_DEFAULT);
    ret |= amp_write_register(ANTI_POP, (AMP_8BIT_USE)&(ANTI_POP_VMID) );

				msleep(50);

	ret |= amp_write_register(OUTPUT_ENA, (AMP_16BIT_USE)&(OUT_SPK) );
	ret |= amp_write_register(INPUT_ENA, (AMP_16BIT_USE)&((TSHUT<<8)| IN2_EN) );
        
	ret |= amp_write_register(IN2A_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[SPEAKER_VOICE_INPUT]) );
	ret |= amp_write_register(IN2B_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[SPEAKER_VOICE_INPUT]) );
  
	ret |= amp_write_register(MIX_OUT_SPK, (AMP_8BIT_USE)&(MIX_IN2) );
	ret |= amp_write_register(SPK_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));
	ret |= amp_write_register(MIX_EN, (AMP_16BIT_USE)&((SPK_VOL_EN<<8)|MIXOUT_SPK));
        
//	ret |= amp_write_register(SPK_BOOST, (AMP_8BIT_USE)&(SPK_BOOST_0));
	ret |= amp_write_register(SPK_BOOST, (AMP_8BIT_USE)&SPK_BOOST_6);
	ret |= amp_write_register(SPK_OUT_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[SPEAKER_VOICE_OUTPUT]));

	ret |= amp_write_register(IN2_CONF, (AMP_16BIT_USE)&(CONF_SE));
    MM_INFO("3 set_amp_speaker_stereo_voice() %d\n", ret);
}
EXPORT_SYMBOL(set_amp_speaker_stereo_voice);

void set_amp_tty(void)
{
	int ret=0;

    ret |= amp_write_register(AMP_RESET, RESET_DEFAULT);
    ret |= amp_write_register(ANTI_POP, (AMP_8BIT_USE)&(ANTI_POP_VMID) );

    msleep(50);

	ret |= amp_write_register(OUTPUT_ENA, (AMP_16BIT_USE)&(OUT_HPH) );
	ret |= amp_write_register(INPUT_ENA, (AMP_16BIT_USE)&((TSHUT<<8)| IN1_EN) );
	ret |= amp_write_register(IN1A_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[TTY_INPUT]) );  ////////////////////////
	ret |= amp_write_register(IN1B_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[TTY_INPUT]) );  ////////////////////////

        
	ret |= amp_write_register(MIX_OUT_L, (AMP_8BIT_USE)&(MIX_IN1A) );
	ret |= amp_write_register(MIX_OUT_R, (AMP_8BIT_USE)&(MIX_IN1A) );

	ret |= amp_write_register(HPH_L_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));  ///////////////////
	ret |= amp_write_register(HPH_R_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));  //////////////////
	ret |= amp_write_register(MIX_EN, (AMP_16BIT_USE)&((SPK_VOL_DIS<<8)|MIXOUT_HPH));

	ret |= amp_write_register(HPOUT_L_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[TTY_OUTPUT]));  ////////////////////////////
	ret |= amp_write_register(HPOUT_R_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[TTY_OUTPUT]));  ///////////////////////////


	ret |= amp_write_register(IN1_CONF, (AMP_16BIT_USE)&(CONF_DIFF));
  
	ret |= amp_write_register(0x46, 0x0100);
	ret |= amp_write_register(0x49, 0x0100);
    bHeadset_OFF = true;
    MM_INFO("4 set_amp_tty() %d\n", ret);

}
EXPORT_SYMBOL(set_amp_tty);

void set_amp_headset_stereo_audio(void)
{
	int ret=0;
//	wm9093_setConfig2(ICODEC_HEADSET_ST_RX, wm9093_headset_power_on);


    ret |= amp_write_register(AMP_RESET, RESET_DEFAULT);
    ret |= amp_write_register(ANTI_POP, (AMP_8BIT_USE)&(ANTI_POP_VMID) );

    msleep(50);

	ret |= amp_write_register(OUTPUT_ENA, (AMP_16BIT_USE)&(OUT_HPH) );
	ret |= amp_write_register(INPUT_ENA, (AMP_16BIT_USE)&((TSHUT<<8)| IN1_EN) );
	ret |= amp_write_register(IN1A_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[HEADSET_AUDIO_INPUT]) );
	ret |= amp_write_register(IN1B_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[HEADSET_AUDIO_INPUT]) );

        
	ret |= amp_write_register(MIX_OUT_L, (AMP_8BIT_USE)&(MIX_IN1A) );
	ret |= amp_write_register(MIX_OUT_R, (AMP_8BIT_USE)&(MIX_IN1B) );
	ret |= amp_write_register(HPH_L_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));
	ret |= amp_write_register(HPH_R_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));
	ret |= amp_write_register(MIX_EN, (AMP_16BIT_USE)&((SPK_VOL_DIS<<8)|MIXOUT_HPH));

	ret |= amp_write_register(HPOUT_L_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[HEADSET_AUDIO_OUTPUT]));
	ret |= amp_write_register(HPOUT_R_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[HEADSET_AUDIO_OUTPUT]));

	ret |= amp_write_register(IN1_CONF, (AMP_16BIT_USE)&(CONF_SE));
  
	ret |= amp_write_register(0x46, 0x0100);
	ret |= amp_write_register(0x49, 0x0100);

    bHeadset_OFF = true;

    MM_INFO("5 set_amp_headset_stereo_audio() %d\n", ret);

}
EXPORT_SYMBOL(set_amp_headset_stereo_audio);

void set_amp_headset_speaker_audio(void)
{
	int ret =0;

    ret |= amp_write_register(AMP_RESET, RESET_DEFAULT);
    ret |= amp_write_register(ANTI_POP, (AMP_8BIT_USE)&(ANTI_POP_VMID) );

    msleep(50);

	ret |= amp_write_register(OUTPUT_ENA, (AMP_16BIT_USE)&(OUT_HPH|OUT_SPK) );
	ret |= amp_write_register(INPUT_ENA, (AMP_16BIT_USE)&((TSHUT<<8)| IN1_EN) );
	ret |= amp_write_register(IN1A_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[HEADSET_SPEAKER_INPUT]) );
	ret |= amp_write_register(IN1B_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[HEADSET_SPEAKER_INPUT]) );

        
	ret |= amp_write_register(MIX_OUT_L, (AMP_8BIT_USE)&(MIX_IN1A) );
	ret |= amp_write_register(MIX_OUT_R, (AMP_8BIT_USE)&(MIX_IN1B) );
  	ret |= amp_write_register(MIX_OUT_SPK, (AMP_8BIT_USE)&(MIX_IN1) );

	ret |= amp_write_register(HPH_L_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));
	ret |= amp_write_register(HPH_R_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));
	ret |= amp_write_register(SPK_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));
	ret |= amp_write_register(MIX_EN, (AMP_16BIT_USE)&((SPK_VOL_EN<<8)|MIXOUT_HPH|MIXOUT_SPK));

	ret |= amp_write_register(HPOUT_L_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[HEADSET_SPEAKER_OUTPUT_HEADSET]));
	ret |= amp_write_register(HPOUT_R_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[HEADSET_SPEAKER_OUTPUT_HEADSET]));
	ret |= amp_write_register(SPK_BOOST, (AMP_8BIT_USE)&(SPK_BOOST_0));
	ret |= amp_write_register(SPK_OUT_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[HEADSET_SPEAKER_OUTPUT_SPEAKER]));

	ret |= amp_write_register(IN1_CONF, (AMP_16BIT_USE)&(CONF_SE));
  
	ret |= amp_write_register(0x46, 0x0100);
	ret |= amp_write_register(0x49, 0x0100);

    bHeadset_OFF = true;
    MM_INFO(" 6 set_amp_headset_speaker_audio() %d\n", ret );
    
}
EXPORT_SYMBOL(set_amp_headset_speaker_audio);

void set_amp_speaker_stereo_audio(void)
{
	int ret=0;


//	ret |= amp_write_register(OUTPUT_ENA, (AMP_16BIT_USE)&(OUT_SPK) );		//???????????????

    ret |= amp_write_register(AMP_RESET, RESET_DEFAULT);
    ret |= amp_write_register(ANTI_POP, (AMP_8BIT_USE)&(ANTI_POP_VMID) );

				msleep(50);

	ret |= amp_write_register(OUTPUT_ENA, (AMP_16BIT_USE)&(OUT_SPK) );
	ret |= amp_write_register(INPUT_ENA, (AMP_16BIT_USE)&((TSHUT<<8)| IN2_EN) );
        
	ret |= amp_write_register(IN2A_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[SPEAKER_AUDIO_INPUT]) );
	ret |= amp_write_register(IN2B_VOL, (AMP_16BIT_USE)&((IN_VOL_UPDATE<<8)|amp_cal_data[SPEAKER_AUDIO_INPUT]) );
  
	ret |= amp_write_register(MIX_OUT_SPK, (AMP_8BIT_USE)&(MIX_IN2) );
	ret |= amp_write_register(SPK_MIX_ATTEN, (AMP_8BIT_USE)&((MIX_VOL_0<<6)|(MIX_VOL_0<<4)|(MIX_VOL_0<<2)|MIX_VOL_0));
	ret |= amp_write_register(MIX_EN, (AMP_16BIT_USE)&((SPK_VOL_EN<<8)|MIXOUT_SPK));
        
	ret |= amp_write_register(0x62, 0x8000);
	ret |= amp_write_register(0x64, 0x0200);
	ret |= amp_write_register(SPK_BOOST, (AMP_8BIT_USE)&SPK_BOOST_6);
	ret |= amp_write_register(SPK_OUT_VOL, (AMP_16BIT_USE)&((OUT_VOL_UPDATE<<8)|amp_cal_data[SPEAKER_AUDIO_OUTPUT]));

	ret |= amp_write_register(IN2_CONF, (AMP_16BIT_USE)&(CONF_SE));

//	wm9093_setConfig2(ICODEC_SPEAKER_RX, wm9093_stereo_speaker);
    MM_INFO("7 set_amp_speaker_stereo_voice() %d\n", ret);
}
EXPORT_SYMBOL(set_amp_speaker_stereo_audio);

static int wm9093_amp_ctl_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct amp_data *data;
	struct i2c_adapter* adapter = client->adapter;
	int err;
	
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_WORD_DATA)){
		err = -EOPNOTSUPP;
		return err;
	}
	
	data = kzalloc(sizeof (struct amp_data), GFP_KERNEL);
	if (NULL == data) {
			return -ENOMEM;
	}
	_data = data;
	data->client = client;
	i2c_set_clientdata(client, data);
		
	mutex_init(&amp_lock);
	set_amp_PowerDown();
	return 0;
}

static int wm9093_amp_ctl_remove(struct i2c_client *client)
{
	struct amp_data *data = i2c_get_clientdata(client);
	kfree (data);
	
	printk(KERN_INFO "%s()\n", __FUNCTION__);
	i2c_set_clientdata(client, NULL);
	return 0;
}


static struct i2c_device_id wm9093_amp_idtable[] = {
	{ "lge_amp", 1 },
};

static struct i2c_driver wm9093_amp_ctl_driver = {
	.probe = wm9093_amp_ctl_probe,
	.remove = wm9093_amp_ctl_remove,
	.id_table = wm9093_amp_idtable,
	.driver = {
		.name = MODULE_NAME,
	},
};


static int __init wm9093_amp_ctl_init(void)
{
	return i2c_add_driver(&wm9093_amp_ctl_driver);	
}

static void __exit wm9093_amp_ctl_exit(void)
{
	return i2c_del_driver(&wm9093_amp_ctl_driver);
}

module_init(wm9093_amp_ctl_init);
module_exit(wm9093_amp_ctl_exit);

MODULE_DESCRIPTION("WM9093 Amp Control");
MODULE_AUTHOR("Youn Tae Ho <taeho.youn@lge.com>");
MODULE_LICENSE("GPL");
