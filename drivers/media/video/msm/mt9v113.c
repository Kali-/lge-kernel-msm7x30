/* drivers/media/video/msm/mt9v113.c 
*
* This software is for APTINA VGA sensor 
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

#include "mt9v113.h"
  
#undef CAM_MSG
#undef CAM_ERR
#undef CDBG

#define CAM_MSG(fmt, args...)   printk(KERN_INFO "msm_camera: " fmt, ##args)
#define CAM_ERR(fmt, args...)   printk(KERN_INFO "msm_camera: " fmt, ##args)
#define CDBG(fmt, args...)      printk(KERN_INFO "msm_camera: " fmt, ##args)

/* SYSCTL Registers*/
#define MT9V113_REG_MODEL_ID      0x00
#define MT9V113_MODEL_ID          0x1048

/* chaehee.lim@lge.com 
   temp : we define the delay time on MSM as 0xFFFF address 
*/
#define MT9M113_REG_REGFLAG_DELAY  		0xFFFF

DEFINE_MUTEX(mt9v113_mutex);

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

struct mt9v113_ctrl_t {
	const struct msm_camera_sensor_info *sensordata;
	int8_t  previous_mode;

   /* for Video Camera */
	int8_t effect;
	int8_t wb;
	unsigned char exposurevalue;
  	 int8_t  scenemode;  
	 int16_t framerate;

   /* for register write */
	int16_t write_byte;
	int16_t write_word;
};

static struct i2c_client  *mt9v113_client;

static struct mt9v113_ctrl_t *mt9v113_ctrl = NULL;
static atomic_t init_reg_mode;

static int prev_effect_mode;
static int prev_balance_mode;
static int prev_exposurecomp_mode;
static int prev_scene_mode;
static int prev_fps_mode;

/*=============================================================
	EXTERNAL DECLARATIONS
==============================================================*/
extern struct mt9v113_reg mt9v113_regs;

static int32_t mt9v113_i2c_txdata(unsigned short saddr,
	unsigned char *txdata, int length)
{
	struct i2c_msg msg[] = {
		{
//			.addr = saddr << 1,
			.addr = saddr,
			.flags = 0,
			.len = length,
			.buf = txdata,
		},
	};

#if 0
		if (length == 2)
			CDBG("msm_io_i2c_w: 0x%04x 0x%04x\n",
				*(u16 *) txdata, *(u16 *) (txdata + 2));
		else if (length == 4)
			CDBG("msm_io_i2c_w: 0x%04x\n", *(u16 *) txdata);
		else
			CDBG("msm_io_i2c_w: length = %d\n", length);
#endif

	if (i2c_transfer(mt9v113_client->adapter, msg, 1) < 0 ) {
		CAM_ERR("mt9v113_i2c_txdata failed\n");
		return -EIO;
	}

	return 0;
}
#if 0
static int32_t mt9v113_i2c_write( uint16_t saddr, uint8_t addr, uint8_t data )
{
    int32_t rc = -EIO;
    unsigned char buf[2];

    memset(buf, 0, sizeof(buf));
    buf[0] = addr;
    buf[1] = data;
    rc = mt9v113_i2c_txdata(saddr, buf, 2);

    if (rc < 0){
        CAM_ERR("i2c_write failed, addr = 0x%x, val = 0x%x!\n", addr, data);
    }

    return rc;
}
#endif
static int32_t mt9v113_i2c_write_w_sensor(unsigned saddr, unsigned short waddr, unsigned short wdata)
{
	int32_t rc = -EIO;
	unsigned char buf[4];
	memset(buf, 0, sizeof(buf));

	buf[0] = (waddr & 0xFF00) >> 8;
	buf[1] = (waddr & 0x00FF);
	buf[2] = (wdata & 0xFF00) >> 8;
	buf[3] = (wdata & 0x00FF);
//	rc = mt9v113_i2c_txdata(mt9v113_client->addr, buf, 4);
        rc = mt9v113_i2c_txdata(saddr, buf, 4);

//       CDBG("[### WORD_LEN check] i2c_write , addr = 0x%04x, val = 0x%04x!\n", waddr, wdata);            

	if (rc < 0)
		CDBG("i2c_write_w failed, addr = 0x%x, val = 0x%x!\n", waddr, wdata);

	return rc;
}

static int32_t mt9v113_i2c_write_table(
	struct mt9v113_address_value_pair const *reg_conf_tbl,
	int num_of_items_in_table)

{
	int i;
	int32_t rc = -EIO;

	for (i = 0; i < num_of_items_in_table; i++) {
		if(reg_conf_tbl->register_address == 0xFFFF)
		{
			msleep(reg_conf_tbl->register_value);
			rc = 0;
		}
		else
			rc = mt9v113_i2c_write_w_sensor(mt9v113_client->addr, reg_conf_tbl->register_address, reg_conf_tbl->register_value);
//			rc = mt9m114_i2c_write(mt9m114_client->addr, reg_conf_tbl->waddr, reg_conf_tbl->wdata, reg_conf_tbl->width);

		if (rc < 0)
			break;

		reg_conf_tbl++;
	}

	return rc;
}


/*
static int32_t mt9v113_i2c_write_table(
	struct mt9v113_address_value_pair const *reg_conf_tbl,
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
        rc = mt9v113_i2c_write(mt9v113_client->addr,
                                    reg_conf_tbl->register_address,
                                    reg_conf_tbl->register_value );
    }

    if(rc < 0){
    for(retry = 0; retry < 3; retry++){
        rc = mt9v113_i2c_write(mt9v113_client->addr,
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
*/
#if 0
static int mt9v113_i2c_rxdata( uint16_t saddr, uint8_t *rxdata, int length)
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

	if (i2c_transfer(mt9v113_client->adapter, msgs, 2) < 0) {
		CAM_ERR("mt9v113_i2c_rxdata failed!\n");
		return -EIO;
	}

	return 0;
	
}
static int32_t mt9v113_i2c_read( uint16_t saddr,
                                   uint8_t raddr, 
	                               uint8_t *rdata,
	                               enum mt9v113_width width )
{
	int32_t rc = 0;
	unsigned char buf[4];

	if ((!rdata) || (width > DWORD_LEN))
		return -EIO;

	memset(buf, 0, sizeof(buf));

    buf[0] = raddr;
	rc = mt9v113_i2c_rxdata(saddr, buf, width+1);
	if (rc < 0) {
        CAM_ERR("mt9v113_i2c_read failed!\n");
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
		CAM_ERR("mt9v113_i2c_read failed!\n");
   
	return rc;
	
}
#endif


#if 1
static int mt9v113_i2c_rxdata(unsigned short saddr,
	unsigned char *rxdata, int length)
{
	struct i2c_msg msgs[] = {
	{
//		.addr   = saddr << 1,
		.addr   = saddr,
		.flags = 0,
		.len   = 2,
		.buf   = rxdata,
	},
	{
//		.addr   = saddr << 1,
		.addr   = saddr,
		.flags = I2C_M_RD,
		.len   = 2,
		.buf   = rxdata,
	},
	};

#if 0
	if (length == 2)
		CDBG("msm_io_i2c_r: 0x%04x 0x%04x\n",
			*(u16 *) rxdata, *(u16 *) (rxdata + 2));
	else if (length == 4)
		CDBG("msm_io_i2c_r: 0x%04x\n", *(u16 *) rxdata);
	else
		CDBG("msm_io_i2c_r: length = %d\n", length);
#endif

	if (i2c_transfer(mt9v113_client->adapter, msgs, 2) < 0) {
		CDBG("mt9v113_i2c_rxdata failed!\n");
		return -EIO;
	}

	return 0;
}


static int32_t mt9v113_i2c_read_w(unsigned short   saddr,
	unsigned short raddr, unsigned short *rdata)
{
	int32_t rc = 0;
	unsigned char buf[4];

	if (!rdata)
		return -EIO;

	memset(buf, 0, sizeof(buf));

	buf[0] = (raddr & 0xFF00)>>8;
	buf[1] = (raddr & 0x00FF);
	
	rc = mt9v113_i2c_rxdata(saddr, buf, 2);
	if (rc < 0)
		return rc;
	
	*rdata = buf[0] << 8 | buf[1];

	if (rc < 0)
		CDBG("mt9v113_i2c_read failed!\n");

//	CDBG("[### WORD_LEN check] i2c_read , addr = 0x%04x, val = 0x%04x!\n", raddr, *rdata);			  
	
	return rc;
}


#endif


static int32_t mt9v113_check_modechange(unsigned short checkaddr)
{
    unsigned short mode_mon = 0xff;	
    int i = 0;
    int rc = 0;
    int32_t local_val = checkaddr;
	
    for (i=0; i<50; i++)
    {
//        msleep(10);
	mdelay(10);
        mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, local_val);
        rc = mt9v113_i2c_read_w(mt9v113_client->addr, 0x0990, &mode_mon);
	
        if(mode_mon == 0)
        {
	    CAM_MSG("MT9V113 Mode Change SUCCESS");  
	    break;
        }	
    }

    if( rc < 0 )
    {
        CAM_ERR("mt9v113_check_modechange fail \n");
	return rc;
    }	
	
    return rc;

}

static long mt9v113_set_effect(int effect)
{
    int32_t rc = 0;

    if(prev_effect_mode == effect)
    {
        CAM_MSG("mt9v113_set_effect : Already set");
	return rc;
    }
	
    CAM_MSG("mt9v113_set_effect: effect is %d\n", effect);

    mt9v113_ctrl->effect = effect;

    switch(effect)
    {
        case CAMERA_EFFECT_OFF:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.effect_off_reg_settings[0], mt9v113_regs.effect_off_reg_settings_size);
	    break;
        case CAMERA_EFFECT_MONO:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.effect_mono_reg_settings[0], mt9v113_regs.effect_mono_reg_settings_size);			
	    break;
	case CAMERA_EFFECT_SEPIA:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.effect_sepia_reg_settings[0], mt9v113_regs.effect_sepia_reg_settings_size);					
	    break;
	case CAMERA_EFFECT_NEGATIVE:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.effect_negative_reg_settings[0], mt9v113_regs.effect_negative_reg_settings_size);							
	    break;
	case CAMERA_EFFECT_SOLARIZE:
	case CAMERA_EFFECT_AQUA:
	case CAMERA_EFFECT_POSTERIZE:
	case CAMERA_EFFECT_WHITEBOARD:
	case CAMERA_EFFECT_BLACKBOARD:
	default:
	    CAM_MSG("mt9v113_set_effect: Not supported effect : %d\n", effect);		
	    break;		
    }

#if 0
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0006);
    rc = mt9v113_check_modechange(0xA103);

    msleep(10);

    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0005);
    rc = mt9v113_check_modechange(0xA103);   
#else 
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0005);
//    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103); //recommend Aptina
//    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0006);

    rc = mt9v113_check_modechange(0xA103);	 

#endif	
    if( rc < 0 )
    {
        CAM_ERR("mt9v113_set_effect: effect write fail \n");
	return rc;
    }

    prev_effect_mode = effect;
	
    return 0;
}

static long mt9v113_set_wb(int8_t wb)
{
    int32_t rc = 0;

    if(prev_balance_mode == wb)
    {
        CAM_MSG("mt9v113_set_wb : Already set");
	return rc;
    }

    CAM_MSG("mt9v113_set_wb : called, new wb: %d\n", wb);

    mt9v113_ctrl->wb = wb;

    switch(wb)
    {
        case CAMERA_WB_AUTO:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.wb_auto_reg_settings[0], mt9v113_regs.wb_auto_reg_settings_size);					
	    break;
	case CAMERA_WB_INCANDESCENT:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.wb_incandescent_reg_settings[0], mt9v113_regs.wb_incandescent_reg_settings_size);							
	    break;
	case CAMERA_WB_FLUORESCENT:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.wb_fluorescent_reg_settings[0], mt9v113_regs.wb_fluorescent_reg_settings_size);									
	    break;
	case CAMERA_WB_DAYLIGHT:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.wb_sunny_reg_settings[0], mt9v113_regs.wb_sunny_reg_settings_size);											
	    break;
	case CAMERA_WB_CLOUDY_DAYLIGHT:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.wb_cloudy_reg_settings[0], mt9v113_regs.wb_cloudy_reg_settings_size);													
	    break;
        case CAMERA_WB_CUSTOM:
	case CAMERA_WB_TWILIGHT:
	case CAMERA_WB_SHADE:
	default:
	    CAM_MSG("mt9v113_set_wb: Not supported wb: %d\n", wb);		
	    break;
    }

    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0005);

    rc = mt9v113_check_modechange(0xA103);

    if( rc < 0 )
    {
        CAM_ERR("mt9v113_set_wb: wb write fail \n");
	return rc;
    }

    prev_balance_mode = wb;
	
    return 0;
}

/* =====================================================================================*/
/* mt9v113_set_exposure_compensation                                                                       								    */
/* =====================================================================================*/
static long mt9v113_set_exposure_compensation(int8_t exposurevalue)
{
    long rc = 0;

    if(prev_exposurecomp_mode == exposurevalue)
    {
        CAM_MSG("mt9v113_set_exposure_compensation : Already set");
	return rc;
    }

    CAM_MSG("mt9v113_set_exposure_compensation: %d\n", exposurevalue);	

    mt9v113_ctrl->exposurevalue = exposurevalue;
#if 0	
#if 0
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0005);
    mt9v113_check_modechange(0xA103);

//    msleep(10);
	
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0006);
    mt9v113_check_modechange(0xA103);
#endif
    switch(exposurevalue)
    {
	case -12:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv_12_reg_settings[0], mt9v113_regs.brightness_lv_12_reg_settings_size);		
	    break;
	case -10:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv_10_reg_settings[0], mt9v113_regs.brightness_lv_10_reg_settings_size);				
	    break;
	case -8:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv_8_reg_settings[0], mt9v113_regs.brightness_lv_8_reg_settings_size);		
	    break;
	case -6:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv_6_reg_settings[0], mt9v113_regs.brightness_lv_6_reg_settings_size);				
	    break;
        case -4:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv_4_reg_settings[0], mt9v113_regs.brightness_lv_4_reg_settings_size);					
	    break;
	case -2:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv_2_reg_settings[0], mt9v113_regs.brightness_lv_2_reg_settings_size);				
	    break;
	case 0:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv0_reg_settings[0], mt9v113_regs.brightness_lv0_reg_settings_size);				
	    break;
	case 2:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv2_reg_settings[0], mt9v113_regs.brightness_lv2_reg_settings_size);				
	    break;
	case 4:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv4_reg_settings[0], mt9v113_regs.brightness_lv4_reg_settings_size);				
	    break;
	case 6:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv6_reg_settings[0], mt9v113_regs.brightness_lv6_reg_settings_size);				
	    break;
	case 8:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv8_reg_settings[0], mt9v113_regs.brightness_lv8_reg_settings_size);				
	    break;
	case 10:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv10_reg_settings[0], mt9v113_regs.brightness_lv10_reg_settings_size);				
	    break;
	case 12:
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.brightness_lv12_reg_settings[0], mt9v113_regs.brightness_lv12_reg_settings_size);				
	    break;
	default:
	    CAM_MSG("mt9v113_set_exposure_compensation	: Not supported exposure value: %d\n", exposurevalue);		
	    break;
    }

	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0005);
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0006);
	
	rc = mt9v113_check_modechange(0xA103);

    if( rc < 0 )
    {
        CAM_ERR("mt9v113_set_exposure_compensation: exposure value write fail \n");
	return rc;
    }

    return rc;
#else
    switch(exposurevalue)
    {
	case -12:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv_12_reg_settings[0], mt9v113_regs.exposure_lv_12_reg_settings_size);		
		break;
	case -10:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv_10_reg_settings[0], mt9v113_regs.exposure_lv_10_reg_settings_size);				
		break;
	case -8:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv_8_reg_settings[0], mt9v113_regs.exposure_lv_8_reg_settings_size);		
		break;
	case -6:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv_6_reg_settings[0], mt9v113_regs.exposure_lv_6_reg_settings_size);				
		break;
	case -4:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv_4_reg_settings[0], mt9v113_regs.exposure_lv_4_reg_settings_size);					
		break;
	case -2:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv_2_reg_settings[0], mt9v113_regs.exposure_lv_2_reg_settings_size);				
		break;
	case 0:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv0_reg_settings[0], mt9v113_regs.exposure_lv0_reg_settings_size);				
		break;
	case 2:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv2_reg_settings[0], mt9v113_regs.exposure_lv2_reg_settings_size);				
		break;
	case 4:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv4_reg_settings[0], mt9v113_regs.exposure_lv4_reg_settings_size);				
		break;
	case 6:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv6_reg_settings[0], mt9v113_regs.exposure_lv6_reg_settings_size);				
		break;
	case 8:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv8_reg_settings[0], mt9v113_regs.exposure_lv8_reg_settings_size);				
		break;
	case 10:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv10_reg_settings[0], mt9v113_regs.exposure_lv10_reg_settings_size);				
		break;
	case 12:
		rc = mt9v113_i2c_write_table(&mt9v113_regs.exposure_lv12_reg_settings[0], mt9v113_regs.exposure_lv12_reg_settings_size);				
		break;
	default:
		CAM_MSG("mt9v113_set_exposure_compensation	: Not supported exposure value: %d\n", exposurevalue);		
		break;
    }

    if( rc < 0 )
    {
	CAM_ERR("mt9v113_set_exposure_compensation: exposure value write fail \n");
	return rc;
    }

    prev_exposurecomp_mode = exposurevalue;

    return rc;

#endif
	
}


static long mt9v113_set_scenemode(int8_t scenemode)
{
    long rc = 0;
	
    if(prev_scene_mode == scenemode)
    {
        CAM_MSG("mt9v113_set_scenemode : Already set");
        return rc;
    }

    CAM_MSG("mt9v113_set_scenemode: %d\n", scenemode);	

    mt9v113_ctrl->scenemode = scenemode;

    switch(scenemode)
    {
        case CAMERA_BESTSHOT_OFF: 
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.nightmode_off_reg_settings[0], mt9v113_regs.nightmode_off_reg_settings_size);
	    break;	
	case CAMERA_BESTSHOT_NIGHT: 
	    rc = mt9v113_i2c_write_table(&mt9v113_regs.nightmode_on_reg_settings[0], mt9v113_regs.nightmode_on_reg_settings_size);
	    break;	
	default :
	    CAM_MSG("mt9v113_set_scenemode : Not supported scenemode parameter : %d\n", scenemode);
	    break;
    }

//recommend Aptina
//    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
//    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0006);
//    rc = mt9v113_check_modechange(0xA103);

//    msleep(10);
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0005);
    rc = mt9v113_check_modechange(0xA103);

    if(rc < 0 )
    {
	CAM_ERR("mt9v113_set_scenemode: mt9v113 writing fail\n");
	return rc;
    }

    prev_scene_mode = scenemode;

    return rc;
}
static long mt9v113_set_preview_fps(uint16_t fps)
{
    long rc = 0;

    if(prev_fps_mode == fps)
    {
        CAM_MSG("mt9v113_set_preview_fps : Already set");
        return rc;
    }

    CAM_MSG("[mt9v113] fps[%d]\n",fps);

    if ( fps <= 0 ) // normal mode : support 10~22fps    
    {
	CAM_MSG("[mt9v113] Camera mode setting \n");
        rc = mt9v113_i2c_write_table(&mt9v113_regs.fps_preview_reg_settings[0], mt9v113_regs.fps_preview_reg_settings_size);    
    }
    else if (fps>0 && fps<10)  //VT night mode - 7.5fps mode
    {
        CAM_MSG("[mt9v113] VT Nightmode setting \n");
        rc = mt9v113_i2c_write_table(&mt9v113_regs.fps_vt_night_reg_settings[0], mt9v113_regs.fps_vt_night_reg_settings_size);    
    }
    else if (fps>=10 && fps<15)  //pclk:24.54MHz->fixed 10fps
    {
        CAM_MSG("[mt9v113] VT Normalmode setting \n");
        rc = mt9v113_i2c_write_table(&mt9v113_regs.fps_vt_reg_settings[0], mt9v113_regs.fps_vt_reg_settings_size);        
    }
    else  //Video normal mode : support 15fps
    {
        CAM_MSG("[mt9v113] Camcorder mode setting \n");
        rc = mt9v113_i2c_write_table(&mt9v113_regs.fps_camcord_reg_settings[0], mt9v113_regs.fps_camcord_reg_settings_size);    		
    }

    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0006);
    rc = mt9v113_check_modechange(0xA103);
    
    //	  msleep(10);
//recommend Aptina    
//    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
//    mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0005);
//    rc = mt9v113_check_modechange(0xA103);
    
    if(rc < 0 )
    {
    CAM_ERR("mt9v113_set_preview_fps: mt9v113 writing fail\n");
    return rc;
    }
    
    prev_fps_mode = fps;
    return rc;

}

static int mt9v113_reg_preview(void)
{
//	int rc = 0;
	unsigned short mode_mon = 0xff;	
	int32_t i;

 	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
 	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0001);	

	for (i=0; i<10; i++)
	{
		msleep(10);
		mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
		mt9v113_i2c_read_w(mt9v113_client->addr, 0x0990, &mode_mon);
		if(mode_mon == 0)
		{
			CAM_MSG("MT9V113 PREVIEW MODE SUCCESS");  
			break;
		}
		
		CAM_MSG("MT9V113 PREVIEW MODE : %d", mode_mon);
	}

	return 0;
}


static int mt9v113_reg_snapshot(void)
{
	unsigned short mode_mon = 0xff;	
	int32_t i;

 	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
 	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0002);	

	for (i=0; i<10; i++)
	{
		msleep(10);
		mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA103);
		mt9v113_i2c_read_w(mt9v113_client->addr, 0x0990, &mode_mon);
		if(mode_mon == 0)
		{
			CAM_MSG("MT9V113 SNAPSHOT MODE SUCCESS");  
			break;
		}
		
		CAM_MSG("MT9V113 SNAPSHOT MODE : %d", mode_mon);
	}

	return 0;
}

static long mt9v113_set_sensor_mode(int mode)
{
	int32_t rc = 0;
	

    	CAM_MSG("mt9v113_set_sensor_mode: sensor mode is %d\n", mode);

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
		rc = mt9v113_reg_preview();  
		if (rc < 0)
			CDBG("[ERROR]%s:Sensor Preview Mode Fail\n", __func__);
		else
			break;		
		break;
	case SENSOR_SNAPSHOT_MODE:
		rc = mt9v113_reg_snapshot();
		if (rc < 0)
			CDBG("[ERROR]%s:Sensor Snapshot Mode Fail\n", __func__);
		else
			break;		
		break;
	case SENSOR_RAW_SNAPSHOT_MODE:
		rc = 0;
		break;
	default:
		rc = -EINVAL;
		break;
	}
	CDBG("mt9v113_set_sensor_mode exit.... \n");

	mt9v113_ctrl->previous_mode = mode;

   	return rc;
//	return 0;
}

/* =====================================================================================*/
/*  mt9v113_sensor_config                                                                        								    */
/* =====================================================================================*/
int mt9v113_sensor_config(void __user *argp)
{
    struct sensor_cfg_data cfg_data;
    long   rc = 0;

    CAM_MSG("mt9v113_sensor_config...................\n");	

    if (copy_from_user(&cfg_data,(void *)argp,sizeof(struct sensor_cfg_data)))
        return -EFAULT;

    CAM_MSG("mt9v113_ioctl, cfgtype = %d, mode = %d\n", cfg_data.cfgtype, cfg_data.mode);

    mutex_lock(&mt9v113_mutex);

    msleep(10);

    switch (cfg_data.cfgtype) {
    case CFG_SET_MODE:
        CAM_MSG("mt9v113_sensor_config: command is CFG_SET_MODE\n");
        rc = mt9v113_set_sensor_mode(cfg_data.mode); 
        break;

    case CFG_SET_EFFECT:
        CAM_MSG("mt9v113_sensor_config: command is CFG_SET_EFFECT\n");
        rc = mt9v113_set_effect(cfg_data.cfg.effect);
        break;

    case CFG_SET_WB:
        CAM_MSG("mt9v113_sensor_config: command is CFG_SET_WB\n");
        //rc = mt9v113_set_wb(cfg_data.wb);
        rc = mt9v113_set_wb(cfg_data.mode);
        break;

    case CFG_SET_EXPOSURE_VALUE:
        CAM_MSG("mt9v113_sensor_config: command is CFG_SET_EXPOSURE_VALUE\n");
        //rc = mt9v113_set_brightness(cfg_data.brightness);
        rc = mt9v113_set_exposure_compensation(cfg_data.mode);
        break;

    case CFG_SET_FPS:
        CAM_MSG("%s:CFG_SET_FPS : user fps[%d]\n",__func__, cfg_data.cfg.fps.fps_div);
        rc = mt9v113_set_preview_fps(cfg_data.cfg.fps.fps_div);
        break;

    case CFG_SET_SCENE:  
        CAM_MSG("mt9v113_sensor_config: command is CFG_SET_SCENE\n");
        rc = mt9v113_set_scenemode(cfg_data.mode);
        break;
    default:	 	
        CAM_MSG("mt9v113_sensor_config:cfg_data.cfgtype[%d]\n",cfg_data.cfgtype);
        rc = 0;//-EINVAL; 
        break;
    }

    mutex_unlock(&mt9v113_mutex);

    if (rc < 0){
  	    CAM_ERR("mt9v113: ERROR in sensor_config, %ld\n", rc);
    }

    return rc;
}

/*======================================================================================*/
/*  end :  sysfs                                                                         										    */
/*======================================================================================*/

/* =====================================================================================*/
/*  mt9v113_precheck_reg                                                                        								    */
/* =====================================================================================*/
int mt9v113_precheck_reg(void)
{
	int rc = 0;
	unsigned short mode_mon = 0xff;	
	int32_t i;

	CDBG("mt9v113_precheck_reg\n");

// recommend Aptina
//	rc = mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x301A, 0x1218); 
//	if (rc < 0) return rc;
//	msleep(100);
//	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x301A, 0x121C);
//	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0018, 0x4028); // duplicated
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0018, 0x4028); // STANDBY_CONTROL

	mode_mon = 0xff;
	for (i=0; i<100; i++)
	{
		msleep(10);
		mt9v113_i2c_read_w(mt9v113_client->addr, 0x0018, &mode_mon);
		if(!(mode_mon & 0x4000))
		{
			CAM_MSG("MT9V113 MODE CHANGE SUCCESS");  
			break;
		}
		
		CAM_MSG("MT9V113 MODE CHANGE : %d", mode_mon);
	}

	mode_mon = 0xff;
	for (i=0; i<20; i++)
	{
		msleep(10);
		mt9v113_i2c_read_w(mt9v113_client->addr, 0x301A, &mode_mon);
		if((mode_mon & 0x0004))
		{
			CAM_MSG("MT9V113 MODE CHANGE SUCCESS");  
			break;
		}
		
		CAM_MSG("MT9V113 MODE CHANGE : %d", mode_mon);
	}
	
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x001A, 0x0013); // RESET_AND_MISC_CONTROL
	msleep(10);
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x001A, 0x0010); // RESET_AND_MISC_CONTROL
	msleep(10);
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0018, 0x4028); // STANDBY_CONTROL	

	mode_mon = 0xff;
	for (i=0; i<100; i++)
	{
		msleep(10);
		mt9v113_i2c_read_w(mt9v113_client->addr, 0x0018, &mode_mon);
		if(!(mode_mon & 0x4000))
		{
			CAM_MSG("MT9V113 MODE CHANGE SUCCESS");  
			break;
		}
		
		CAM_MSG("MT9V113 MODE CHANGE : %d", mode_mon);
	}

	mode_mon = 0xff;
	for (i=0; i<20; i++)
	{
		msleep(10);
		mt9v113_i2c_read_w(mt9v113_client->addr, 0x301A, &mode_mon);
		if((mode_mon & 0x0004))
		{
			CAM_MSG("MT9V113 MODE CHANGE SUCCESS");  
			break;
		}
		
		CAM_MSG("MT9V113 MODE CHANGE : %d", mode_mon);
	}

	// reduce_IO_current
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0x02F0); 
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0000); 
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0x02F2); 
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0210); 	
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0x02F4); 
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x001A); 
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0x2145); 
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x02F4);	
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x098C, 0xA134); 
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0990, 0x0001); 
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x31E0, 0x0001); 
//	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x31E0, 0x0001);	//duplicated
	mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x001A, 0x0210);	
	rc = mt9v113_i2c_write_w_sensor(mt9v113_client->addr, 0x0016, 0x42DF);		

	if(rc < 0 ) return rc;

	return 0;	
}




/* =====================================================================================*/
/*  mt9v113_sensor_init                                                                        								    */
/* =====================================================================================*/
int mt9v113_sensor_init(const struct msm_camera_sensor_info *data)
{
    int rc =0;

    CDBG("mt9v113_sensor_init\n");

    msm_camio_camif_pad_reg_reset();
	
    mt9v113_ctrl = (struct mt9v113_ctrl_t *)kzalloc(sizeof(struct mt9v113_ctrl_t), GFP_KERNEL);  
    if (!mt9v113_ctrl) {
        CAM_ERR("mt9v113_init failed!\n");
	rc = -ENOMEM;
	goto init_done;
    }
	
    if (data)
	mt9v113_ctrl->sensordata = data;
    else
	goto init_fail;

    CDBG("[mt9v113_sensor_init] init_reg_mode NORMAL.....\n");

    rc = mt9v113_precheck_reg();
    if (rc < 0 ) {
	CAM_ERR("mt9v113: mt9v113 precheck fail\n");
	goto init_fail; 
    }

    rc = mt9v113_i2c_write_table(&mt9v113_regs.init_reg_settings[0],
	 mt9v113_regs.init_reg_settings_size);
	
    if(rc < 0 ){
	CAM_ERR("mt9v113: mt9v113 writing fail\n");
	goto init_fail; 
    }
	
    mt9v113_ctrl->previous_mode = SENSOR_PREVIEW_MODE;

    prev_effect_mode = -1;
    prev_balance_mode = -1;
    prev_exposurecomp_mode = -1;
    prev_scene_mode = -1;
    prev_fps_mode = -1;	

    return rc;

init_fail:
    kfree(mt9v113_ctrl);
    mt9v113_ctrl = NULL;
init_done:
	CAM_ERR("mt9v113:mt9v113_sensor_init failed\n");
	return rc;

}
/* =====================================================================================*/
/*  mt9v113_sensor_release                                                                        								    */
/* =====================================================================================*/
int mt9v113_sensor_release(void)
{
    CDBG("mt9v113_sensor_release\n");
    
    if(mt9v113_ctrl)
    {
	kfree(mt9v113_ctrl);
	mt9v113_ctrl = NULL;
    }

	return 0;	
}

/* =====================================================================================*/
/*  mt9v113_sensor_prove function                                                                        								    */
/* =====================================================================================*/
#if 0
static int mt9v113_probe_init_done(const struct msm_camera_sensor_info *data)
{
	gpio_direction_output(data->sensor_reset, 0);
	gpio_free(data->sensor_reset);
	return 0;
}

static int mt9v113_probe_init_sensor(const struct msm_camera_sensor_info *data)
{
	int32_t rc = 0;
	unsigned short chipid;
	CDBG("%s: %d\n", __func__, __LINE__);
	rc = gpio_request(data->sensor_reset, "mt9v113");
	CDBG(" mt9v113_probe_init_sensor \n");

	if (rc){
		CDBG(" mt9v113_probe_init_sensor 2\n");
		goto init_probe_done;
	}
	
	CDBG(" mt9v113_probe_init_sensor MT9V113_REG_MODEL_ID is read  \n");

    CDBG("mt9v113_probe_init_sensor Power On\n");
	data->pdata->camera_power_on();
	    
	/* 3. Read sensor Model ID: */
	rc = mt9v113_i2c_read(mt9v113_client->addr, MT9V113_REG_MODEL_ID,
	                        (uint8_t*)&chipid, WORD_LEN );
	
    CDBG("mt9v113_probe_init_sensor Power Off\n");
	data->pdata->camera_power_off();
	    
	if (rc < 0)
		goto init_probe_fail;
	CDBG("mt9v113 model_id = 0x%x\n", chipid);

	/* 4. Compare sensor ID to MT9V113 ID: */
	if (chipid != MT9V113_MODEL_ID) {
		rc = -ENODEV;
		goto init_probe_fail;
	}

	goto init_probe_done;
init_probe_fail:
	CDBG(" mt9v113_probe_init_sensor fails\n");
init_probe_done:
	CDBG(" mt9v113_probe_init_sensor finishes\n");
	mt9v113_probe_init_done(data);
	return rc;
}
#endif

static int mt9v113_i2c_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	int rc = 0;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	mt9v113_client = client;

	return rc;

probe_failure:	
	CAM_ERR("mt9v113_i2c_probe.......rc[%d]....................\n",rc);
	return rc;		
	
}

static int mt9v113_i2c_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id mt9v113_i2c_ids[] = {
	{"mt9v113", 0},
	{ /* end of list */},
};

static struct i2c_driver mt9v113_i2c_driver = {
	.probe  = mt9v113_i2c_probe,
	.remove = mt9v113_i2c_remove, 
	.id_table = mt9v113_i2c_ids,	
	.driver   = {
		.name =  "mt9v113",
		.owner= THIS_MODULE,	
    },
};

static int mt9v113_sensor_probe(const struct msm_camera_sensor_info *info,
	                                 	struct msm_sensor_ctrl *s)
{
	int rc = 0;

	CAM_MSG("mt9v113_sensor_probe...................\n");

	rc = i2c_add_driver(&mt9v113_i2c_driver);
	if (rc < 0) {
		rc = -ENOTSUPP;
		CAM_ERR("[mt9v113_sensor_probe] return value :ENOTSUPP\n");
		goto probe_fail;
	}

	atomic_set(&init_reg_mode, 0);

#if 0   // Check Sensor ID
    rc = mt9v113_probe_init_sensor(info);
    if (rc < 0)
        goto probe_fail;
#endif

	s->s_init    = mt9v113_sensor_init;
	s->s_release = mt9v113_sensor_release;
	s->s_config  = mt9v113_sensor_config;
    s->s_camera_type = FRONT_CAMERA_2D;
	s->s_mount_angle = 180; //chaehee.lim@lge.com 2011.06.27 : Set Camera orientation for camcording
	
    CAM_MSG("mt9v113.c : mt9v113_sensor_probe - complete : %d \n", rc);
    CAM_MSG("mt9v113.c : s_mount_angle : %d \n", s->s_mount_angle);	
    return 0;

probe_fail:
    CAM_MSG("mt9v113.c : mt9v113_sensor_probe - Fail : %d \n", rc);
	return rc;
}
static int __mt9v113_probe(struct platform_device *pdev)
{	
	return msm_camera_drv_start(pdev,mt9v113_sensor_probe);	
}

static struct platform_driver msm_camera_driver = {
	.probe = __mt9v113_probe,
	.driver = {
		.name = "msm_camera_mt9v113",
		.owner = THIS_MODULE,
	},
};
static int __init mt9v113_init(void)
{
	CAM_MSG("mt9v113_init...................\n");
		
	return platform_driver_register(&msm_camera_driver); 
}

module_init(mt9v113_init);

