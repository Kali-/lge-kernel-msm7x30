/* drivers/media/video/msm/tcm9000md.h
*
* This software is for APTINA 1.3M sensor 
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

#ifndef TCM9000MD_H
#define TCM9000MD_H

#include <linux/types.h>
#include <mach/camera.h>

#define LGE_SUPPORT_FRONTCAM_FPSRANGE

#if 0//def CONFIG_MSM_CAMERA_DEBUG
#define CAM_MSG(fmt, args...)	printk(KERN_ERR "CAM_MSG [%-18s:%5d] " fmt, __FUNCTION__, __LINE__, ## args)
#define CAM_ERR(fmt, args...)	printk(KERN_ERR "CAM_ERR [%-18s:%5d] " fmt, __FUNCTION__, __LINE__, ## args)
#else
#define CAM_MSG(fmt, args...)
#define CAM_ERR(fmt, args...)
#endif

struct tcm9000md_address_value_pair {
	uint8_t register_address;
	uint8_t register_value;
};

enum tcm9000md_width {
	BYTE_1_LEN = 0,
	BYTE_2_LEN,
	BYTE_3_LEN,
	BYTE_4_LEN,
	WORD_LEN = BYTE_2_LEN,
	DWORD_LEN = BYTE_4_LEN
};

struct tcm9000md_reg {
	/* VGA 22fps for normal mode */
	const struct tcm9000md_address_value_pair *init_reg_settings; 
	uint16_t init_reg_settings_size;

	/* VGA 30fps for VT mode */
	const struct tcm9000md_address_value_pair *init_reg_vt_settings;  
	uint16_t init_reg_vt_settings_size;

	const struct tcm9000md_address_value_pair *sequencer_reg_settings;
	uint16_t sequencer_reg_settings_size;

	/*register for effect*/
	const struct tcm9000md_address_value_pair *effect_off_reg_settings;
	uint16_t effect_off_reg_settings_size;
	const struct tcm9000md_address_value_pair *effect_mono_reg_settings;
	uint16_t effect_mono_reg_settings_size;
	const struct tcm9000md_address_value_pair *effect_sepia_reg_settings;
	uint16_t effect_sepia_reg_settings_size;
	const struct tcm9000md_address_value_pair *effect_negative_reg_settings;
	uint16_t effect_negative_reg_settings_size;
	const struct tcm9000md_address_value_pair *effect_solarize_reg_settings;
	uint16_t effect_solarize_reg_settings_size;
	const struct tcm9000md_address_value_pair *effect_blue_reg_settings;
	uint16_t effect_blue_reg_settings_size;

	/*register for wb*/
	const struct tcm9000md_address_value_pair *wb_auto_reg_settings;
	uint16_t wb_auto_reg_settings_size;
	const struct tcm9000md_address_value_pair *wb_incandescent_reg_settings;
	uint16_t wb_incandescent_reg_settings_size;
	const struct tcm9000md_address_value_pair *wb_fluorescent_reg_settings;
	uint16_t wb_fluorescent_reg_settings_size;
	const struct tcm9000md_address_value_pair *wb_sunny_reg_settings;
	uint16_t wb_sunny_reg_settings_size;
	const struct tcm9000md_address_value_pair *wb_cloudy_reg_settings;
	uint16_t wb_cloudy_reg_settings_size;

	// donghyun.kwon(20110505) : add exposure compensation for VGA - start
	/*register for brightness*/
	const struct tcm9000md_address_value_pair *brightness_lv0_reg_settings;
	uint16_t brightness_lv0_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv1_reg_settings;
	uint16_t brightness_lv1_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv2_reg_settings;
	uint16_t brightness_lv2_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv3_reg_settings;
	uint16_t brightness_lv3_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv4_reg_settings;
	uint16_t brightness_lv4_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv5_reg_settings;
	uint16_t brightness_lv5_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv6_reg_settings;
	uint16_t brightness_lv6_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv7_reg_settings;
	uint16_t brightness_lv7_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv8_reg_settings;
	uint16_t brightness_lv8_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv9_reg_settings;
	uint16_t brightness_lv9_reg_settings_size;

	const struct tcm9000md_address_value_pair *brightness_lv10_reg_settings;
	uint16_t brightness_lv10_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv11_reg_settings;
	uint16_t brightness_lv11_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv12_reg_settings;
	uint16_t brightness_lv12_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv13_reg_settings;
	uint16_t brightness_lv13_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv14_reg_settings;
	uint16_t brightness_lv14_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv15_reg_settings;
	uint16_t brightness_lv15_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv16_reg_settings;
	uint16_t brightness_lv16_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv17_reg_settings;
	uint16_t brightness_lv17_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv18_reg_settings;
	uint16_t brightness_lv18_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv19_reg_settings;
	uint16_t brightness_lv19_reg_settings_size;

	const struct tcm9000md_address_value_pair *brightness_lv20_reg_settings;
	uint16_t brightness_lv20_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv21_reg_settings;
	uint16_t brightness_lv21_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv22_reg_settings;
	uint16_t brightness_lv22_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv23_reg_settings;
	uint16_t brightness_lv23_reg_settings_size;
	const struct tcm9000md_address_value_pair *brightness_lv24_reg_settings;
	uint16_t brightness_lv24_reg_settings_size;
	// donghyun.kwon(20110505) : add exposure compensation for VGA - end

	// donghyun.kwon (20110421) : add night mode -start
	const struct tcm9000md_address_value_pair *nightmode_on_reg_settings;
	uint16_t nightmode_on_reg_settings_size;
	const struct tcm9000md_address_value_pair *nightmode_off_reg_settings;
	uint16_t nightmode_off_reg_settings_size;
	// donghyun.kwon (20110421) : add night mode -end

#ifdef LGE_SUPPORT_FRONTCAM_FPSRANGE
	const struct tcm9000md_address_value_pair *framerate_auto_reg_settings;
	uint16_t framerate_auto_reg_settings_size;
	const struct tcm9000md_address_value_pair *framerate_fixed_7p5_reg_settings;
	uint16_t framerate_fixed_7p5_reg_settings_size;
	const struct tcm9000md_address_value_pair *framerate_fixed_10_reg_settings;
	uint16_t framerate_fixed_10_reg_settings_size;
	const struct tcm9000md_address_value_pair *framerate_fixed_15_reg_settings;
	uint16_t framerate_fixed_15_reg_settings_size;
#endif

};

#endif //TCM9000MD_H

