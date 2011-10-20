/* drivers/media/video/msm/tcm9000md.c 
*
* This software is for APTINA 1.3M sensor 
*  
* Copyright (C) 2010-2011 LGE Inc.  
*
* This software is licensed under the terms of the GNU General Public  
* License version 2, as published by the Free Software Foundation, and  
* may be copied, distributed, and modified under those terms.  
*  
* This program is distributed in the hope that it will be useful,  
* but WITHOUT ANY WARRANTY; without even the implied warranty of  
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
* GNU General Public License for more details. 
*/ 

#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <media/msm_camera.h>
#include <mach/gpio.h>
#include <mach/vreg.h>
#include <linux/byteorder/little_endian.h>

#include "tcm9000md.h"

#undef CAM_MSG
#undef CAM_ERR
#undef CDBG

#define CAM_MSG(fmt, args...)   printk(KERN_INFO "msm_camera: " fmt, ##args)
#define CAM_ERR(fmt, args...)   printk(KERN_INFO "msm_camera: " fmt, ##args)
#define CDBG(fmt, args...)      printk(KERN_INFO "msm_camera: " fmt, ##args)

/* SYSCTL Registers*/
#define TCM9000MD_REG_MODEL_ID      0x00
#define TCM9000MD_MODEL_ID          0x1048

/* chanhee.park@lge.com 
   temp : we define the delay time on MSM as 0xFFFE address 
*/
#define MT9M113_REG_REGFLAG_DELAY  		0xFFFE

DEFINE_MUTEX(tcm9000md_mutex);

enum{
  MT9M113_FPS_NORMAL = 0,
  MT9M113_FPS_5 = 5,
  MT9M113_FPS_7 = 7,
  MT9M113_FPS_10 = 10,
  MT9M113_FPS_12 = 12,
  MT9M113_FPS_15 = 15,
  MT9M113_FPS_20 = 20,
  MT9M113_FPS_24 = 24,
  MT9M113_FPS_25 = 25,
  MT9M113_FPS_30 = 30
};

struct tcm9000md_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
	int8_t  previous_mode;

   /* for Video Camera */
	int8_t effect;
	int8_t wb;
	unsigned char brightness;
	int8_t	scenemode;  // donghyun.kwon(20110502) ; add scene mode for VGA
	int8_t  exposure_lv;   // donghyun.kwon(20110505) : add exposure compensation for VGA

   /* for register write */
	int16_t write_byte;
	int16_t write_word;
};

static struct i2c_client  *tcm9000md_client;

static struct tcm9000md_ctrl_t *tcm9000md_ctrl = NULL;
static atomic_t init_reg_mode;


/*=============================================================
	EXTERNAL DECLARATIONS
==============================================================*/
extern struct tcm9000md_reg tcm9000md_regs;

static int32_t tcm9000md_i2c_txdata(unsigned short saddr,
	unsigned char *txdata, int length)
{
	struct i2c_msg msg[] = {
		{
			.addr = saddr,
			.flags = 0,
			.len = length,
			.buf = txdata,
		},
	};

	if (i2c_transfer(tcm9000md_client->adapter, msg, 1) < 0 ) {
		CAM_ERR("tcm9000md_i2c_txdata failed\n");
		return -EIO;
	}

	return 0;
}
static int32_t tcm9000md_i2c_write( uint16_t saddr, uint8_t addr, uint8_t data )
{
    int32_t rc = -EIO;
    unsigned char buf[2];

    memset(buf, 0, sizeof(buf));
    buf[0] = addr;
    buf[1] = data;
    rc = tcm9000md_i2c_txdata(saddr, buf, 2);

    if (rc < 0){
        CAM_ERR("i2c_write failed, addr = 0x%x, val = 0x%x!\n", addr, data);
    }

    return rc;
}

static int32_t tcm9000md_i2c_write_table(
	struct tcm9000md_address_value_pair const *reg_conf_tbl,
	int num_of_items_in_table)
{
    int32_t retry;
    int32_t i;
    int32_t rc = 0;

    for (i = 0; i < num_of_items_in_table; i++) {

    if(reg_conf_tbl->register_address == MT9M113_REG_REGFLAG_DELAY){
        mdelay(reg_conf_tbl->register_value);
    }
    else{
        rc = tcm9000md_i2c_write(tcm9000md_client->addr,
                                    reg_conf_tbl->register_address,
                                    reg_conf_tbl->register_value );
    }

    if(rc < 0){
    for(retry = 0; retry < 3; retry++){
        rc = tcm9000md_i2c_write(tcm9000md_client->addr,
                                    reg_conf_tbl->register_address,
                                    reg_conf_tbl->register_value );
        if(rc >= 0)
            retry = 3;        
        }
        reg_conf_tbl++;
    }else
        reg_conf_tbl++;
    }

    return rc;
}
#if 0
static int tcm9000md_i2c_rxdata( uint16_t saddr, uint8_t *rxdata, int length)
{
	struct i2c_msg msgs[] = {
	{
		.addr   = saddr,
		.flags = 0,
		.len   = 1,
		.buf   = rxdata,
	},
	{
		.addr   = saddr,
		.flags = I2C_M_RD,
		.len   = length,
		.buf   = rxdata,
	},
	};

	if (i2c_transfer(tcm9000md_client->adapter, msgs, 2) < 0) {
		CAM_ERR("tcm9000md_i2c_rxdata failed!\n");
		return -EIO;
	}

	return 0;
	
}
static int32_t tcm9000md_i2c_read( uint16_t saddr,
                                   uint8_t raddr, 
	                               uint8_t *rdata,
	                               enum tcm9000md_width width )
{
	int32_t rc = 0;
	unsigned char buf[4];

	if ((!rdata) || (width > DWORD_LEN))
		return -EIO;

	memset(buf, 0, sizeof(buf));

    buf[0] = raddr;
	rc = tcm9000md_i2c_rxdata(saddr, buf, width+1);
	if (rc < 0) {
        CAM_ERR("tcm9000md_i2c_read failed!\n");
        return rc;
	}

    switch (width) {
#if 1   // Little Endian
        case DWORD_LEN :
            *(rdata+3) = buf[3];
        case BYTE_3_LEN :
            *(rdata+2) = buf[2];
        case WORD_LEN :
            *(rdata+1) = buf[1];
        case BYTE_1_LEN :
            *rdata = buf[0];
            break;
#else
        case BYTE_1_LEN :
            *rdata = buf[0];
            break;
            
        case WORD_LEN :
            *rdata = buf[1];
            *(rdata+1) = buf[0];
            break;
            
        case BYTE_3_LEN :
            *rdata = buf[2];
            *(rdata+1) = buf[1];
            *(rdata+2) = buf[0];
            break;
            
        case DWORD_LEN :
            *rdata = buf[3];
            *(rdata+1) = buf[2];
            *(rdata+2) = buf[1];
            *(rdata+3) = buf[0];
            break;
#endif
	}

	if (rc < 0)
		CAM_ERR("tcm9000md_i2c_read failed!\n");
   
	return rc;
	
}
#endif

static long tcm9000md_set_effect(int effect)
{

    int32_t rc = 0; // donghyun.kwon(20110412) : add VGA camera parameters

    CAM_MSG("tcm9000md_set_effect: effect is %d\n", effect);

	// donghyun.kwon(20110412) : add VGA camera parameters -start
    tcm9000md_ctrl->effect = effect;

	switch(effect)
	{
		case CAMERA_EFFECT_OFF:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.effect_off_reg_settings[0], tcm9000md_regs.effect_off_reg_settings_size);
			break;	
		case CAMERA_EFFECT_MONO:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.effect_mono_reg_settings[0], tcm9000md_regs.effect_mono_reg_settings_size);
			break;	
		case CAMERA_EFFECT_SEPIA:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.effect_sepia_reg_settings[0], tcm9000md_regs.effect_sepia_reg_settings_size);
			break;	
		case CAMERA_EFFECT_NEGATIVE:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.effect_negative_reg_settings[0], tcm9000md_regs.effect_negative_reg_settings_size);
			break;	
		case CAMERA_EFFECT_SOLARIZE:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.effect_solarize_reg_settings[0], tcm9000md_regs.effect_solarize_reg_settings_size);
			break;	
		case CAMERA_EFFECT_AQUA :
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.effect_blue_reg_settings[0], tcm9000md_regs.effect_blue_reg_settings_size);
			break;	
		//case CAMERA_EFFECT_PASTEL :			
		//case CAMERA_EFFECT_MOSAIC :
		//case CAMERA_EFFECT_RESIZE :
		case CAMERA_EFFECT_POSTERIZE :
		case CAMERA_EFFECT_WHITEBOARD :
		case CAMERA_EFFECT_BLACKBOARD :
		default :
			CAM_MSG("tcm9000md_set_effect : Not supported effect parameter : %d\n", effect);
			break;
			
	}
	
	if(rc < 0 )
	{
		CAM_ERR("tcm9000md_set_effect: tcm9000md writing fail\n");
		return rc;
	}
	// donghyun.kwon(20110412) : add VGA camera parameters -end

    return 0;
}

static long tcm9000md_set_wb(int8_t wb)
{
    int32_t rc = 0; // donghyun.kwon(20110412) : add VGA camera parameters

    CAM_MSG("tcm9000md_set_wb : called, new wb: %d\n", wb);

    tcm9000md_ctrl->wb = wb;

	// donghyun.kwon(20110412) : add VGA camera parameters -start
	switch(wb)
	{
		case CAMERA_WB_AUTO:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.wb_auto_reg_settings[0], tcm9000md_regs.wb_auto_reg_settings_size);
			break;	
		case CAMERA_WB_INCANDESCENT:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.wb_incandescent_reg_settings[0], tcm9000md_regs.wb_incandescent_reg_settings_size);
			break;	
		case CAMERA_WB_FLUORESCENT:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.wb_fluorescent_reg_settings[0], tcm9000md_regs.wb_fluorescent_reg_settings_size);
			break;	
		case CAMERA_WB_DAYLIGHT:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.wb_sunny_reg_settings[0], tcm9000md_regs.wb_sunny_reg_settings_size);
			break;	
		case CAMERA_WB_CLOUDY_DAYLIGHT:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.wb_cloudy_reg_settings[0], tcm9000md_regs.wb_cloudy_reg_settings_size);
			break;	
		case CAMERA_WB_CUSTOM :
		case CAMERA_WB_TWILIGHT :			
		case CAMERA_WB_SHADE :
		default :
			CAM_MSG("tcm9000md_set_wb : Not supported wb parameter : %d\n", wb);
			break;
			
	}
	
	if(rc < 0 )
	{
		CAM_ERR("tcm9000md_set_wb: tcm9000md writing fail\n");
		return rc;
	}
	// donghyun.kwon(20110412) : add VGA camera parameters -end

    return 0;
}

/* =====================================================================================*/
/* tcm9000md_set_brightness                                                                        								    */
/* =====================================================================================*/
static long tcm9000md_set_brightness(int8_t brightness)
{
    long rc = 0;

    CAM_MSG("tcm9000md_set_brightness: %d\n", brightness);	

    tcm9000md_ctrl->brightness = brightness;
			
    return rc;
}


/* =====================================================================================*/
/* tcm9000md_set_scenemode                                                                        								    */
/* donghyun.kwon(20110502) ; add scene mode for VGA                                     */
/* =====================================================================================*/
static long tcm9000md_set_scenemode(int8_t scenemode)
{
    long rc = 0;

    CAM_MSG("tcm9000md_set_scenemode: %d\n", scenemode);	

    tcm9000md_ctrl->scenemode = scenemode;

	switch(scenemode)
	{
		case CAMERA_BESTSHOT_OFF: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.nightmode_off_reg_settings[0], tcm9000md_regs.nightmode_off_reg_settings_size);
			break;	
		case CAMERA_BESTSHOT_NIGHT: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.nightmode_on_reg_settings[0], tcm9000md_regs.nightmode_on_reg_settings_size);
			break;	
		default :
			CAM_MSG("tcm9000md_set_scenemode : Not supported scenemode parameter : %d\n", scenemode);
			break;
	}

	if(rc < 0 )
	{
		CAM_ERR("tcm9000md_set_scenemode: tcm9000md writing fail\n");
		return rc;
	}

    return rc;
}


/* =====================================================================================*/
/* tcm9000md_set_exposure_compensation                                                                        								    */
/* donghyun.kwon(20110505) : add exposure compensation for VGA                                     */
/* =====================================================================================*/
static long tcm9000md_set_exposure_compensation(int8_t exposure)
{
	long rc = 0;

	CAM_MSG("tcm9000md_set_exposure_compensation: %d\n", exposure);	

	tcm9000md_ctrl->exposure_lv = exposure;

	switch(exposure)
	{
		case -12:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv0_reg_settings[0], tcm9000md_regs.brightness_lv0_reg_settings_size);
			break;	
		case -11:	
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv1_reg_settings[0], tcm9000md_regs.brightness_lv1_reg_settings_size);
			break;	
		case -10:
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv2_reg_settings[0], tcm9000md_regs.brightness_lv2_reg_settings_size);
			break;	
		case -9:	
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv3_reg_settings[0], tcm9000md_regs.brightness_lv3_reg_settings_size);
			break;	
		case -8: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv4_reg_settings[0], tcm9000md_regs.brightness_lv4_reg_settings_size);
			break;	
		case -7: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv5_reg_settings[0], tcm9000md_regs.brightness_lv5_reg_settings_size);
			break;	
		case -6: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv6_reg_settings[0], tcm9000md_regs.brightness_lv6_reg_settings_size);
			break;	
		case -5: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv7_reg_settings[0], tcm9000md_regs.brightness_lv7_reg_settings_size);
			break;	
		case -4: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv8_reg_settings[0], tcm9000md_regs.brightness_lv8_reg_settings_size);
			break;	
		case -3: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv9_reg_settings[0], tcm9000md_regs.brightness_lv9_reg_settings_size);
			break;	
		case -2: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv10_reg_settings[0], tcm9000md_regs.brightness_lv10_reg_settings_size);
			break;	
		case -1: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv11_reg_settings[0], tcm9000md_regs.brightness_lv11_reg_settings_size);
			break;	
		case 0: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv12_reg_settings[0], tcm9000md_regs.brightness_lv12_reg_settings_size);
			break;	
		case 1: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv13_reg_settings[0], tcm9000md_regs.brightness_lv13_reg_settings_size);
			break;	
		case 2: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv14_reg_settings[0], tcm9000md_regs.brightness_lv14_reg_settings_size);
			break;	
		case 3: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv15_reg_settings[0], tcm9000md_regs.brightness_lv15_reg_settings_size);
			break;	
		case 4: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv16_reg_settings[0], tcm9000md_regs.brightness_lv16_reg_settings_size);
			break;	
		case 5: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv17_reg_settings[0], tcm9000md_regs.brightness_lv17_reg_settings_size);
			break;	
		case 6: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv18_reg_settings[0], tcm9000md_regs.brightness_lv18_reg_settings_size);
			break;	
		case 7: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv19_reg_settings[0], tcm9000md_regs.brightness_lv19_reg_settings_size);
			break;	
		case 8: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv20_reg_settings[0], tcm9000md_regs.brightness_lv20_reg_settings_size);
			break;	
		case 9: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv21_reg_settings[0], tcm9000md_regs.brightness_lv21_reg_settings_size);
			break;	
		case 10: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv22_reg_settings[0], tcm9000md_regs.brightness_lv22_reg_settings_size);
			break;	
		case 11: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv23_reg_settings[0], tcm9000md_regs.brightness_lv23_reg_settings_size);
			break;	
		case 12: 
			rc = tcm9000md_i2c_write_table(&tcm9000md_regs.brightness_lv24_reg_settings[0], tcm9000md_regs.brightness_lv24_reg_settings_size);
			break;	
		default :
			CAM_MSG("tcm9000md_set_exposure_compensation : Not supported exposure parameter : %d\n", exposure);
			break;
	}

	if(rc < 0 )
	{
		CAM_ERR("tcm9000md_set_exposure_compensation: tcm9000md writing fail\n");
		return rc;
	}
	return rc;
}


static long tcm9000md_set_preview_fps(uint16_t fps)
{
    long rc = 0;

    CAM_MSG("[tcm9000md] fps[%d] \n",fps);

#ifdef LGE_SUPPORT_FRONTCAM_FPSRANGE
	if (fps<=0)
	{
		rc = tcm9000md_i2c_write_table(&tcm9000md_regs.framerate_auto_reg_settings[0], tcm9000md_regs.framerate_auto_reg_settings_size);
	}
	else if (0<fps && fps <=8)
	{
		rc = tcm9000md_i2c_write_table(&tcm9000md_regs.framerate_fixed_7p5_reg_settings[0], tcm9000md_regs.framerate_fixed_7p5_reg_settings_size);
	}
	else if (8<fps && fps <=10)
	{
		rc = tcm9000md_i2c_write_table(&tcm9000md_regs.framerate_fixed_10_reg_settings[0], tcm9000md_regs.framerate_fixed_10_reg_settings_size);
	}
	else
	{
		rc = tcm9000md_i2c_write_table(&tcm9000md_regs.framerate_fixed_15_reg_settings[0], tcm9000md_regs.framerate_fixed_15_reg_settings_size);
	}
#endif

    return rc;
}

static long tcm9000md_set_sensor_mode(int mode)
{
    CAM_MSG("tcm9000md_set_sensor_mode: sensor mode is PREVIEW : %d\n", mode);

	tcm9000md_ctrl->previous_mode = mode;
   
	return 0;
}

/* =====================================================================================*/
/*  tcm9000md_sensor_config                                                                        								    */
/* =====================================================================================*/
int tcm9000md_sensor_config(void __user *argp)
{
    struct sensor_cfg_data cfg_data;
    long   rc = 0;

    CAM_MSG("tcm9000md_sensor_config...................\n");	

    if (copy_from_user(&cfg_data,(void *)argp,sizeof(struct sensor_cfg_data)))
        return -EFAULT;

    CAM_MSG("tcm9000md_ioctl, cfgtype = %d, mode = %d\n", cfg_data.cfgtype, cfg_data.mode);

    mutex_lock(&tcm9000md_mutex);

    msleep(10);

    switch (cfg_data.cfgtype) {
    case CFG_SET_MODE:
        CAM_MSG("tcm9000md_sensor_config: command is CFG_SET_MODE\n");
        rc = tcm9000md_set_sensor_mode(cfg_data.mode); 
        break;

    case CFG_SET_EFFECT:
        CAM_MSG("tcm9000md_sensor_config: command is CFG_SET_EFFECT\n");
        rc = tcm9000md_set_effect(cfg_data.cfg.effect);
        break;

    case CFG_SET_WB:
        CAM_MSG("tcm9000md_sensor_config: command is CFG_SET_WB\n");
        //rc = tcm9000md_set_wb(cfg_data.wb);
        rc = tcm9000md_set_wb(cfg_data.cfg.effect);  // donghyun.kwon(20110413) : add WB setting
        break;

    case CFG_SET_BRIGHTNESS:
        CAM_MSG("tcm9000md_sensor_config: command is CFG_SET_BRIGHTNESS\n");
        //rc = tcm9000md_set_brightness(cfg_data.brightness);
        rc = tcm9000md_set_brightness(cfg_data.cfg.effect);  // donghyun.kwon(20110413) : add Brightness setting
        break;

    case CFG_SET_FPS:
        CAM_MSG("%s:CFG_SET_FPS : user fps[%d]\n",__func__, cfg_data.cfg.fps.fps_div);
        rc = tcm9000md_set_preview_fps(cfg_data.cfg.fps.fps_div);
        break;

    case CFG_SET_SCENE:   // donghyun.kwon(20110502) ; add scene mode for VGA
        CAM_MSG("tcm9000md_sensor_config: command is CFG_SET_SCENE\n");
        rc = tcm9000md_set_scenemode(cfg_data.cfg.effect);
        break;

    case CFG_SET_EXPOSURE_VALUE:   // donghyun.kwon(20110505) : add exposure compensation for VGA
        CAM_MSG("tcm9000md_sensor_config: command is CFG_SET_EXPOSURE_VALUE\n");
        rc = tcm9000md_set_exposure_compensation(cfg_data.cfg.effect);
        break;

    default:	 	
        CAM_MSG("tcm9000md_sensor_config:cfg_data.cfgtype[%d]\n",cfg_data.cfgtype);
        rc = 0;//-EINVAL; 
        break;
    }

    mutex_unlock(&tcm9000md_mutex);

    if (rc < 0){
  	    CAM_ERR("tcm9000md: ERROR in sensor_config, %ld\n", rc);
    }

    return rc;
}

/*======================================================================================*/
/*  end :  sysfs                                                                         										    */
/*======================================================================================*/


/* =====================================================================================*/
/*  tcm9000md_sensor_init                                                                        								    */
/* =====================================================================================*/
int tcm9000md_sensor_init(const struct msm_camera_sensor_info *data)
{
	int rc =0;

    CDBG("tcm9000md_sensor_init\n");

    msm_camio_camif_pad_reg_reset();
    
	tcm9000md_ctrl = (struct tcm9000md_ctrl_t *)kzalloc(sizeof(struct tcm9000md_ctrl_t), GFP_KERNEL);  
	if (!tcm9000md_ctrl) {
		CAM_ERR("tcm9000md_init failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}
	
	if (data)
		tcm9000md_ctrl->sensordata = data;
	else
		goto init_fail;

	printk("[tcm9000md_sensor_init] init_reg_mode NORMARL.....\n");
	rc = tcm9000md_i2c_write_table(&tcm9000md_regs.init_reg_settings[0],
		 tcm9000md_regs.init_reg_settings_size);
	
	if(rc < 0 ){
	   CAM_ERR("tcm9000md: tcm9000md writing fail\n");
	   goto init_fail; 
	}
	
	tcm9000md_ctrl->previous_mode = SENSOR_PREVIEW_MODE;

	return rc;
init_fail:
    kfree(tcm9000md_ctrl);
    tcm9000md_ctrl = NULL;
init_done:
	CAM_ERR("tcm9000md:tcm9000md_sensor_init failed\n");
	return rc;

}
/* =====================================================================================*/
/*  tcm9000md_sensor_release                                                                        								    */
/* =====================================================================================*/
int tcm9000md_sensor_release(void)
{
    CDBG("tcm9000md_sensor_release\n");

    if(tcm9000md_ctrl)
    {
        kfree(tcm9000md_ctrl);
        tcm9000md_ctrl = NULL;
    }
	return 0;	
}

/* =====================================================================================*/
/*  tcm9000md_sensor_prove function                                                                        								    */
/* =====================================================================================*/
#if 0
static int tcm9000md_probe_init_done(const struct msm_camera_sensor_info *data)
{
	gpio_direction_output(data->sensor_reset, 0);
	gpio_free(data->sensor_reset);
	return 0;
}

static int tcm9000md_probe_init_sensor(const struct msm_camera_sensor_info *data)
{
	int32_t rc = 0;
	unsigned short chipid;
	CDBG("%s: %d\n", __func__, __LINE__);
	rc = gpio_request(data->sensor_reset, "tcm9000md");
	CDBG(" tcm9000md_probe_init_sensor \n");

	if (rc){
		CDBG(" tcm9000md_probe_init_sensor 2\n");
		goto init_probe_done;
	}
	
	CDBG(" tcm9000md_probe_init_sensor TCM9000MD_REG_MODEL_ID is read  \n");

    CDBG("tcm9000md_probe_init_sensor Power On\n");
	data->pdata->camera_power_on();
	    
	/* 3. Read sensor Model ID: */
	rc = tcm9000md_i2c_read(tcm9000md_client->addr, TCM9000MD_REG_MODEL_ID,
	                        (uint8_t*)&chipid, WORD_LEN );
	
    CDBG("tcm9000md_probe_init_sensor Power Off\n");
	data->pdata->camera_power_off();
	    
	if (rc < 0)
		goto init_probe_fail;
	CDBG("tcm9000md model_id = 0x%x\n", chipid);

	/* 4. Compare sensor ID to TCM9000MD ID: */
	if (chipid != TCM9000MD_MODEL_ID) {
		rc = -ENODEV;
		goto init_probe_fail;
	}

	goto init_probe_done;
init_probe_fail:
	CDBG(" tcm9000md_probe_init_sensor fails\n");
init_probe_done:
	CDBG(" tcm9000md_probe_init_sensor finishes\n");
	tcm9000md_probe_init_done(data);
	return rc;
}
#endif

static int tcm9000md_i2c_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	int rc = 0;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	tcm9000md_client = client;

	return rc;

probe_failure:	
	CAM_ERR("tcm9000md_i2c_probe.......rc[%d]....................\n",rc);
	return rc;		
	
}

static int tcm9000md_i2c_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id tcm9000md_i2c_ids[] = {
	{"tcm9000md", 0},
	{ /* end of list */},
};

static struct i2c_driver tcm9000md_i2c_driver = {
	.probe  = tcm9000md_i2c_probe,
	.remove = tcm9000md_i2c_remove, 
	.id_table = tcm9000md_i2c_ids,	
	.driver   = {
		.name =  "tcm9000md",
		.owner= THIS_MODULE,	
    },
};

static int tcm9000md_sensor_probe(const struct msm_camera_sensor_info *info,
	                                 	struct msm_sensor_ctrl *s)
{
	int rc = 0;

	CAM_MSG("tcm9000md_sensor_probe...................\n");

	rc = i2c_add_driver(&tcm9000md_i2c_driver);
	if (rc < 0) {
		rc = -ENOTSUPP;
		CAM_ERR("[tcm9000md_sensor_probe] return value :ENOTSUPP\n");
		goto probe_fail;
	}

	atomic_set(&init_reg_mode, 0);

#if 0   // Check Sensor ID
    rc = tcm9000md_probe_init_sensor(info);
    if (rc < 0)
        goto probe_fail;
#endif

	s->s_init    = tcm9000md_sensor_init;
	s->s_release = tcm9000md_sensor_release;
	s->s_config  = tcm9000md_sensor_config;
    s->s_camera_type = FRONT_CAMERA_2D;
	s->s_mount_angle = 180;
	
    CAM_MSG("tcm9000md.c : tcm9000md_sensor_probe - complete : %d \n", rc);
    return 0;

probe_fail:
    CAM_MSG("tcm9000md.c : tcm9000md_sensor_probe - Fail : %d \n", rc);
	return rc;
}
static int __tcm9000md_probe(struct platform_device *pdev)
{	
	return msm_camera_drv_start(pdev,tcm9000md_sensor_probe);	
}

static struct platform_driver msm_camera_driver = {
	.probe = __tcm9000md_probe,
	.driver = {
		.name = "msm_camera_tcm9000md",
		.owner = THIS_MODULE,
	},
};
static int __init tcm9000md_init(void)
{
	CAM_MSG("tcm9000md_init...................\n");
		
	return platform_driver_register(&msm_camera_driver);  
}

module_init(tcm9000md_init);

