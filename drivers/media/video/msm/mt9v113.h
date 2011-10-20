/* drivers/media/video/msm/mt9v113.h
*
* This software is for APTINA VGA sensor 
*  
* Copyright (C) 2010-2011 LGE Inc.  
*
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

#ifndef MT9V113_H
#define MT9V113_H

#include <linux/types.h>
#include <mach/camera.h>


#if 0//def CONFIG_MSM_CAMERA_DEBUG
#define CAM_MSG(fmt, args...)	printk(KERN_ERR "CAM_MSG [%-18s:%5d] " fmt, __FUNCTION__, __LINE__, ## args)
#define CAM_ERR(fmt, args...)	printk(KERN_ERR "CAM_ERR [%-18s:%5d] " fmt, __FUNCTION__, __LINE__, ## args)
#else
#define CAM_MSG(fmt, args...)
#define CAM_ERR(fmt, args...)
#endif

struct mt9v113_address_value_pair {
	unsigned short register_address;
	unsigned short register_value;
};


enum mt9v113_width {
	BYTE_1_LEN = 0,
	BYTE_2_LEN,
	BYTE_3_LEN,
	BYTE_4_LEN,
	WORD_LEN = BYTE_2_LEN,
	DWORD_LEN = BYTE_4_LEN
};

struct mt9v113_reg {
	/* VGA 22fps for normal mode */
	const struct mt9v113_address_value_pair *init_reg_settings; 
	uint16_t init_reg_settings_size;

	/* VGA 30fps for VT mode */
	const struct mt9v113_address_value_pair *init_reg_vt_settings;  
	uint16_t init_reg_vt_settings_size;

	const struct mt9v113_address_value_pair *sequencer_reg_settings;
	uint16_t sequencer_reg_settings_size;

	/*register for effect*/
	const struct mt9v113_address_value_pair *effect_off_reg_settings;
	uint16_t effect_off_reg_settings_size;
	const struct mt9v113_address_value_pair *effect_mono_reg_settings;
	uint16_t effect_mono_reg_settings_size;
	const struct mt9v113_address_value_pair *effect_sepia_reg_settings;
	uint16_t effect_sepia_reg_settings_size;
	const struct mt9v113_address_value_pair *effect_negative_reg_settings;
	uint16_t effect_negative_reg_settings_size;
	const struct mt9v113_address_value_pair *effect_solarize_reg_settings;
	uint16_t effect_solarize_reg_settings_size;
	const struct mt9v113_address_value_pair *effect_blue_reg_settings;
	uint16_t effect_blue_reg_settings_size;

	/*register for wb*/
	const struct mt9v113_address_value_pair *wb_auto_reg_settings;
	uint16_t wb_auto_reg_settings_size;
	const struct mt9v113_address_value_pair *wb_incandescent_reg_settings;
	uint16_t wb_incandescent_reg_settings_size;
	const struct mt9v113_address_value_pair *wb_fluorescent_reg_settings;
	uint16_t wb_fluorescent_reg_settings_size;
	const struct mt9v113_address_value_pair *wb_sunny_reg_settings;
	uint16_t wb_sunny_reg_settings_size;
	const struct mt9v113_address_value_pair *wb_cloudy_reg_settings;
	uint16_t wb_cloudy_reg_settings_size;

	/*register for brightness */
	const struct mt9v113_address_value_pair *brightness_lv_12_reg_settings;
	uint16_t brightness_lv_12_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv_10_reg_settings;
	uint16_t brightness_lv_10_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv_8_reg_settings;
	uint16_t brightness_lv_8_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv_6_reg_settings;
	uint16_t brightness_lv_6_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv_4_reg_settings;
	uint16_t brightness_lv_4_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv_2_reg_settings;
	uint16_t brightness_lv_2_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv0_reg_settings;
	uint16_t brightness_lv0_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv2_reg_settings;
	uint16_t brightness_lv2_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv4_reg_settings;
	uint16_t brightness_lv4_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv6_reg_settings;
	uint16_t brightness_lv6_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv8_reg_settings;
	uint16_t brightness_lv8_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv10_reg_settings;
	uint16_t brightness_lv10_reg_settings_size;
	const struct mt9v113_address_value_pair *brightness_lv12_reg_settings;
	uint16_t brightness_lv12_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv_12_reg_settings;
	uint16_t exposure_lv_12_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv_10_reg_settings;
	uint16_t exposure_lv_10_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv_8_reg_settings;
	uint16_t exposure_lv_8_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv_6_reg_settings;
	uint16_t exposure_lv_6_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv_4_reg_settings;
	uint16_t exposure_lv_4_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv_2_reg_settings;
	uint16_t exposure_lv_2_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv0_reg_settings;
	uint16_t exposure_lv0_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv2_reg_settings;
	uint16_t exposure_lv2_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv4_reg_settings;
	uint16_t exposure_lv4_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv6_reg_settings;
	uint16_t exposure_lv6_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv8_reg_settings;
	uint16_t exposure_lv8_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv10_reg_settings;
	uint16_t exposure_lv10_reg_settings_size;
	const struct mt9v113_address_value_pair *exposure_lv12_reg_settings;
	uint16_t exposure_lv12_reg_settings_size;

	/* register for scenemode */
	const struct mt9v113_address_value_pair *nightmode_off_reg_settings;
	uint16_t nightmode_off_reg_settings_size;
	const struct mt9v113_address_value_pair *nightmode_on_reg_settings;
	uint16_t nightmode_on_reg_settings_size;
	const struct mt9v113_address_value_pair *fps_preview_reg_settings;
	uint16_t fps_preview_reg_settings_size;
	const struct mt9v113_address_value_pair *fps_camcord_reg_settings;
	uint16_t fps_camcord_reg_settings_size;
	const struct mt9v113_address_value_pair *fps_vt_reg_settings;
	uint16_t fps_vt_reg_settings_size;
	const struct mt9v113_address_value_pair *fps_vt_night_reg_settings;
	uint16_t fps_vt_night_reg_settings_size;
};

#endif //MT9V113_H

