/* drivers/media/video/msm/tcm9000md_reg.h 
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

#include "tcm9000md.h"
#include <linux/kernel.h>

#define TCM9001MD_SENSOR  // donghyun.kwon(20110609) : change initial code for VGA CAM

/* chanhee.park@lge.com 
   temp : we define the delay time on MSM as 0xFFFE address 
*/
/************************************************************
; slave address 0x7A
;	MCK:31M	PCK:60M
;
; Preview : 640x480
; Capture : 1280x960
; Preview frame rate 22fps
; 
;************************************************************/
struct tcm9000md_address_value_pair const tcm9000md_reg_init_settings[] = {
#ifdef TCM9001MD_SENSOR
	{  0x0000,0x0048  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0001,0x0010  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0002,0x00DC  }  ,	//alcint_sekiout[7:0];
	{  0x0003,0x0080  }  ,	//alc_ac5060out/alc_les_modeout[2:0]/*/*/alcint_sekiout[9:;
	{  0x0004,0x0020  }  ,	//alc_agout[7:0];
	{  0x0005,0x0000  }  ,	//*/*/*/*/alc_agout[11:8];
	{  0x0006,0x0000  }  ,	//alc_dgout[7:0];
	{  0x0007,0x0010  }  ,	//alc_esout[7:0];
	{  0x0008,0x0080  }  ,	//alc_okout//alc_esout[13:8];
	{  0x0009,0x00BA  }  ,	//awb_uiout[7:0];
	{  0x000A,0x007B  }  ,	//awb_uiout[15:8];
	{  0x000B,0x00FD  }  ,	//awb_uiout[23:16];
	{  0x000C,0x003F  }  ,	//*/*/awb_uiout[29:24];
	{  0x000D,0x0033  }  ,	//awb_viout[7:0];
	{  0x000E,0x0039  }  ,	//awb_viout[15:8];
	{  0x000F,0x00FE  }  ,	//awb_viout[2316];
	{  0x0010,0x003F  }  ,	//*/*/awb_viout[29:24];
	{  0x0011,0x00CA  }  ,	//awb_pixout[7:0];
	{  0x0012,0x00AF  }  ,	//awb_pixout[15:8];
	{  0x0013,0x0004  }  ,	//*/*/*/*/*/awb_pixout[18:16];
	{  0x0014,0x0077  }  ,	//awb_rgout[7:0];
	{  0x0015,0x0080  }  ,	//awb_ggout[7:0];
	{  0x0016,0x0079  }  ,	//awb_bgout[7:0];
	{  0x0017,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0018,0x009C  }  ,	//LSTB/*/*/*/*/*/*/*;
	{  0x0019,0x0004  }  ,	//*/*/*/*/*/TSEL[2:0];
	{  0x001A,0x0090  }  ,	//*/*/*/*/*/*/*/*;
	{  0x001B,0x0000  }  ,	//*/*/*/CKREF_DIV[4:0];
	{  0x001C,0x0000  }  ,	//CKVAR_SS0DIV[7:0];
	{  0x001D,0x0000  }  ,	//*/SPCK_SEL/*/EXTCLK_THROUGH/*/*/*/CKVAR_SS0DIV[8];
	{  0x001E,0x008E  }  ,	//CKVAR_SS1DIV[7:0];
	{  0x001F,0x0000  }  ,	//MRCK_DIV[3:0]/*/*/*/CKVAR_SS1DIV[8];
	{  0x0020,0x0080  }  ,	//VCO_DIV[1:0]/*/CLK_SEL[1:0]/AMON0SEL[1:0]/*;
	{  0x0021,0x000B  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0022,0x0007  }  ,	//TBINV/RLINV//WIN_MODE//HV_INTERMIT[2:0];
	{  0x0023,0x0096  }  ,	//H_COUNT[7:0];
	{  0x0024,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0025,0x0042  }  ,	//V_COUNT[7:0];
	{  0x0026,0x0000  }  ,	//*/*/*/*/*/V_COUNT[10:8];
	{  0x0027,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0028,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0029,0x0083  }  ,	//*/*/*/*/*/*/*/*;
	{  0x002A,0x0084  }  ,	//*/*/*/*/*/*/*/*;
	{  0x002B,0x00AE  }  ,	//*/*/*/*/*/*/*/*;
	{  0x002C,0x0021  }  ,	//*/*/*/*/*/*/*/*;
	{  0x002D,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x002E,0x0004  }  ,	//*/*/*/*/*/*/*/*;
	{  0x002F,0x007D  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0030,0x0019  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0031,0x0088  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0032,0x0088  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0033,0x0009  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0034,0x006C  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0035,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0036,0x0090  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0037,0x0022  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0038,0x000B  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0039,0x00AA  }  ,	//*/*/*/*/*/*/*/*;
	{  0x003A,0x000A  }  ,	//*/*/*/*/*/*/*/*;
	{  0x003B,0x0084  }  ,	//*/*/*/*/*/*/*/*;
	{  0x003C,0x0003  }  ,	//*/*/*/*/*/*/*/*;
	{  0x003D,0x0010  }  ,	//*/*/*/*/*/*/*/*;
	{  0x003E,0x004C  }  ,	//*/*/*/*/*/*/*/*;
	{  0x003F,0x001D  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0040,0x0034  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0041,0x0005  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0042,0x0012  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0043,0x00B0  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0044,0x003F  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0045,0x00FF  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0046,0x0044  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0047,0x0044  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0048,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0049,0x00E8  }  ,	//*/*/*/*/*/*/*/*;
	{  0x004A,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x004B,0x009F  }  ,	//*/*/*/*/*/*/*/*;
	{  0x004C,0x009B  }  ,	//*/*/*/*/*/*/*/*;
	{  0x004D,0x002B  }  ,	//*/*/*/*/*/*/*/*;
	{  0x004E,0x0053  }  ,	//*/*/*/*/*/*/*/*;
	{  0x004F,0x0050  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0050,0x000E  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0051,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0052,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0053,0x0004  }  ,	//TP_MODE[4:0]/TPG_DR_SEL/TPG_CBLK_SW/TPG_LINE_SW;
	{  0x0054,0x0008  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0055,0x0014  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0056,0x0084  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0057,0x0030  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0058,0x0080  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0059,0x0080  }  ,	//*/*/*/*/*/*/*/*;
	{  0x005A,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x005B,0x0006  }  ,	//*/*/*/*/*/*/*/*;
	{  0x005C,0x00F0  }  ,	//*/*/*/*/*/*/*/*;
	{  0x005D,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x005E,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x005F,0x00B0  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0060,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0061,0x001B  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0062,0x004F  }  ,	//HYUKO_START[7:0];
	{  0x0063,0x0004  }  ,	//VYUKO_START[7:0];
	{  0x0064,0x0010  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0065,0x0020  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0066,0x0030  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0067,0x0028  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0068,0x0066  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0069,0x00C0  }  ,	//*/*/*/*/*/*/*/*;
	{  0x006A,0x0030  }  ,	//*/*/*/*/*/*/*/*;
	{  0x006B,0x0030  }  ,	//*/*/*/*/*/*/*/*;
	{  0x006C,0x003F  }  ,	//*/*/*/*/*/*/*/*;
	{  0x006D,0x00BF  }  ,	//*/*/*/*/*/*/*/*;
	{  0x006E,0x00AB  }  ,	//*/*/*/*/*/*/*/*;
	{  0x006F,0x0030  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0070,0x0080  }  ,	//AGMIN_BLACK_ADJ[7:0];
	{  0x0071,0x0090  }  ,	//AGMAX_BLACK_ADJ[7:0];
	{  0x0072,0x0000  }  ,	//IDR_SET[7:0];
	{  0x0073,0x0028  }  ,	//PWB_RG[7:0];
	{  0x0074,0x0000  }  ,	//PWB_GRG[7:0];
	{  0x0075,0x0000  }  ,	//PWB_GBG[7:0];
	{  0x0076,0x0058  }  ,	//PWB_BG[7:0];
	{  0x0077,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0078,0x0080  }  ,	//LSSC_SW/*/*/*/*/*/*/*/;
	{  0x0079,0x0052  }  ,	//*/*/*/*/*/*/*/*;
	{  0x007A,0x004F  }  ,	//*/*/*/*/*/*/*/*;
	{  0x007B,0x0084  }  ,	//LSSC_LEFT_RG[7:0];
	{  0x007C,0x004D  }  ,	//LSSC_LEFT_GG[7:0];
	{  0x007D,0x0044  }  ,	//LSSC_LEFT_BG[7:0];
	{  0x007E,0x00B1  }  ,	//LSSC_RIGHT_RG[7:0];
	{  0x007F,0x0077  }  ,	//LSSC_RIGHT_GG[7:0];
	{  0x0080,0x0067  }  ,	//LSSC_RIGHT_BG[7:0];
	{  0x0081,0x006D  }  ,	//LSSC_TOP_RG[7:0];
	{  0x0082,0x0050  }  ,	//LSSC_TOP_GG[7:0];
	{  0x0083,0x003C  }  ,	//LSSC_TOP_BG[7:0];
	{  0x0084,0x0078  }  ,	//LSSC_BOTTOM_RG[7:0];
	{  0x0085,0x004B  }  ,	//LSSC_BOTTOM_GG[7:0];
	{  0x0086,0x0031  }  ,	//LSSC_BOTTOM_BG[7:0];
	{  0x0087,0x0001  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0088,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0089,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x008A,0x0040  }  ,	//*/*/*/*/*/*/*/*;
	{  0x008B,0x0009  }  ,	//*/*/*/*/*/*/*/*;
	{  0x008C,0x00E0  }  ,	//*/*/*/*/*/*/*/*;
	{  0x008D,0x0020  }  ,	//*/*/*/*/*/*/*/*;
	{  0x008E,0x0020  }  ,	//*/*/*/*/*/*/*/*;
	{  0x008F,0x0020  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0090,0x0020  }  ,	//*/*/*/*/*/*/*/*;
	{  0x0091,0x0080  }  ,	//ANR_SW/*/*/*/TEST_ANR/*/*/*;
	{  0x0092,0x0030  }  ,	//AGMIN_ANR_WIDTH[7:0];
	{  0x0093,0x0040  }  ,	//AGMAX_ANR_WIDTH[7:0];
	{  0x0094,0x0040  }  ,	//AGMIN_ANR_MP[7:0];
	{  0x0095,0x0080  }  ,	//AGMAX_ANR_MP[7:0];
	{  0x0096,0x0080  }  ,	//DTL_SW/*/*/*/*/*/*/*/;
	{  0x0097,0x0020  }  ,	//AGMIN_HDTL_NC[7:0];
	{  0x0098,0x0068  }  ,	//AGMIN_VDTL_NC[7:0];
	{  0x0099,0x00FF  }  ,	//AGMAX_HDTL_NC[7:0];
	{  0x009A,0x00FF  }  ,	//AGMAX_VDTL_NC[7:0];
	{  0x009B,0x005C  }  ,	//AGMIN_HDTL_MG[7:0];
	{  0x009C,0x0028  }  ,	//AGMIN_HDTL_PG[7:0];
	{  0x009D,0x0040  }  ,	//AGMIN_VDTL_MG[7:0];
	{  0x009E,0x0028  }  ,	//AGMIN_VDTL_PG[7:0];
	{  0x009F,0x0000  }  ,	//AGMAX_HDTL_MG[7:0];
	{  0x00A0,0x0000  }  ,	//AGMAX_HDTL_PG[7:0];
	{  0x00A1,0x0000  }  ,	//AGMAX_VDTL_MG[7:0];
	{  0x00A2,0x0000  }  ,	//AGMAX_VDTL_PG[7:0];
	{  0x00A3,0x0080  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00A4,0x0082  }  ,	//HCBC_SW/*/*/*/*/*/*/*/;
	{  0x00A5,0x0028  }  ,	//AGMIN_HCBC_G[7:0];
	{  0x00A6,0x0018  }  ,	//AGMAX_HCBC_G[7:0];
	{  0x00A7,0x0098  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00A8,0x0098  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00A9,0x0098  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00AA,0x0010  }  ,	//LMCC_BMG_SEL/LMCC_BMR_SEL/*/LMCC_GMB_SEL/LMCC_GMR_SEL/*/;
	{  0x00AB,0x005B  }  ,	//LMCC_RMG_G[7:0];
	{  0x00AC,0x0000  }  ,	//LMCC_RMB_G[7:0];
	{  0x00AD,0x0000  }  ,	//LMCC_GMR_G[7:0];
	{  0x00AE,0x0000  }  ,	//LMCC_GMB_G[7:0];
	{  0x00AF,0x0000  }  ,	//LMCC_BMR_G[7:0];
	{  0x00B0,0x0048  }  ,	//LMCC_BMG_G[7:0];
	{  0x00B1,0x00C0  }  ,	//GAM_SW[1:0]/*/CGC_DISP/TEST_AWBDISP/*/YUVM_AWBDISP_SW/YU;
	{  0x00B2,0x004D  }  ,	//*/R_MATRIX[6:0];
	{  0x00B3,0x0010  }  ,	//*/B_MATRIX[6:0];
	{  0x00B4,0x00C8  }  ,	//UVG_SEL/BRIGHT_SEL/*/TEST_YUVM_PE/NEG_YMIN_SW/PIC_EFFECT;
	{  0x00B5,0x005F  }  ,	//CONTRAST[7:0];
	{  0x00B6,0x0047  }  ,	//BRIGHT[7:0];
	{  0x00B7,0x0000  }  ,	//Y_MIN[7:0];
	{  0x00B8,0x00FF  }  ,	//Y_MAX[7:0];
	{  0x00B9,0x0069  }  ,	//U_GAIN[7:0];
	{  0x00BA,0x0066  }  ,	//V_GAIN[7:0];
	{  0x00BB,0x0078  }  ,	//SEPIA_US[7:0];
	{  0x00BC,0x0090  }  ,	//SEPIA_VS[7:0];
	{  0x00BD,0x0008  }  ,	//U_CORING[7:0];
	{  0x00BE,0x0008  }  ,	//V_CORING[7:0];
	{  0x00BF,0x00C0  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00C0,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00C1,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00C2,0x0080  }  ,	//ALC_SW/ALC_LOCK/*/*/*/*/*/*/;
	{  0x00C3,0x0014  }  ,	//MES[7:0];
	{  0x00C4,0x0003  }  ,	//*/*/MES[13:8];
	{  0x00C5,0x0000  }  ,	//MDG[7:0];
	{  0x00C6,0x0074  }  ,	//MAG[7:0];
	{  0x00C7,0x0080  }  ,	//AGCONT_SEL[1:0]/*/*/MAG[11:8];
	{  0x00C8,0x0020  }  ,	//AG_MIN[7:0];
	{  0x00C9,0x0008  }  ,	//AG_MAX[7:0];
	{  0x00CA,0x0096  }  ,	//AUTO_LES_SW/AUTO_LES_MODE[2:0]/ALC_WEIGHT[1:0]/FLCKADJ[1;
	{  0x00CB,0x00E7  }  ,	//ALC_LV[7:0];
	{  0x00CC,0x0010  }  ,	//*/UPDN_MODE[2:0]/ALC_LV[9:8];
	{  0x00CD,0x000A  }  ,	//ALC_LVW[7:0];
	{  0x00CE,0x0093  }  ,	//L64P600S[7:0];
	{  0x00CF,0x0006  }  ,	//*/ALC_VWAIT[2:0]/L64P600S[11:8];
	{  0x00D0,0x0080  }  ,	//UPDN_SPD[7:0];
	{  0x00D1,0x0020  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00D2,0x0080  }  ,	//NEAR_SPD[7:0];
	{  0x00D3,0x0030  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00D4,0x008A  }  ,	//AC5060/*/ALC_SAFETY[5:0];
	{  0x00D5,0x0002  }  ,	//*/*/*/*/*/ALC_SAFETY2[2:0];
	{  0x00D6,0x004F  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00D7,0x0008  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00D8,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00D9,0x00FF  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00DA,0x0001  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00DB,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00DC,0x0014  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00DD,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00DE,0x0080  }  ,	//AWB_SW/AWB_LOCK/*/AWB_TEST/*/*/HEXG_SLOPE_SEL/COLGATE_SE;
	{  0x00DF,0x0080  }  ,	//WB_MRG[7:0];
	{  0x00E0,0x0080  }  ,	//WB_MGG[7:0];
	{  0x00E1,0x0080  }  ,	//WB_MBG[7:0];
	{  0x00E2,0x0022  }  ,	//WB_RBMIN[7:0];
	{  0x00E3,0x00F8  }  ,	//WB_RBMAX[7:0];
	{  0x00E4,0x0080  }  ,	//HEXA_SW/*/COLGATE_RANGE[1:0]/*/*/*/COLGATE_OPEN;
	{  0x00E5,0x002C  }  ,	//*/RYCUTM[6:0];
	{  0x00E6,0x0054  }  ,	//*/RYCUTP[6:0];
	{  0x00E7,0x0028  }  ,	//*/BYCUTM[6:0];
	{  0x00E8,0x0039  }  ,	//*/BYCUTP[6:0];
	{  0x00E9,0x00E4  }  ,	//RBCUTL[7:0];
	{  0x00EA,0x003C  }  ,	//RBCUTH[7:0];
	{  0x00EB,0x0081  }  ,	//SQ1_SW/SQ1_POL/*/*/*/*/*/YGATE_SW;
	{  0x00EC,0x0037  }  ,	//RYCUT1L[7:0];
	{  0x00ED,0x005A  }  ,	//RYCUT1H[7:0];
	{  0x00EE,0x00DE  }  ,	//BYCUT1L[7:0];
	{  0x00EF,0x0008  }  ,	//BYCUT1H[7:0];
	{  0x00F0,0x0018  }  ,	//YGATE_L[7:0];
	{  0x00F1,0x00FE  }  ,	//YGATE_H[7:0];
	{  0x00F2,0x0000  }  ,	//*/*/IPIX_DISP_SW/*/*/*/*/;
	{  0x00F3,0x0002  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00F4,0x0002  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00F5,0x0004  }  ,	//AWB_WAIT[7:0];
	{  0x00F6,0x0000  }  ,	//AWB_SPDDLY[7:0];
	{  0x00F7,0x0020  }  ,	//*/*/AWB_SPD[5:0];
	{  0x00F8,0x0086  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00F9,0x0000  }  ,	//*/*/*/*/*/*/*/*;
	{  0x00FA,0x0041  }  ,	//MR_HBLK_START[7:0];
	{  0x00FB,0x0050  }  ,	//*/MR_HBLK_WIDTH[6:0];
	{  0x00FC,0x000C  }  ,	//MR_VBLK_START[7:0];
	{  0x00FD,0x003C  }  ,	//*/*/MR_VBLK_WIDTH[5:0];
	{  0x00FE,0x0050  }  ,	//PIC_FORMAT[3:0]/PINTEST_SEL[3:0];
	{  0x00FF,0x0085  }  ,	//SLEEP/*/PARALLEL_OUTSW[1:0]/DCLK_POL/DOUT_CBLK_SW/*/AL;
#else
    {   0x18,0x9C   }   ,   //,LSTB/*/*/*/*/*/*/*;
    {   0x19,0x04   }   ,   ////,*/*/*/*/*/TSEL[2:0];
    {   0x1A,0x90   }   ,   ////,*/*/*/*/*/*/*/*;
    {   0x1B,0x00   }   ,   ////,*/*/*/CKREF_DIV[4:0];
    {   0x1C,0x00   }   ,   //,CKVAR_SS0DIV[7:0];
    {   0x1D,0x00   }   ,   ////,*/SPCK_SEL/*/EXTCLK_THROUGH/*/*/*/CKVAR_SS0DIV[8];
    {   0x1E,0x8E   }   ,   //,CKVAR_SS1DIV[7:0];
    {   0x1F,0x00   }   ,   //,MRCK_DIV[3:0]/*/*/*/CKVAR_SS1DIV[8];
    {   0x20,0x80   }   ,   //,VCO_DIV[1:0]/*/CLK_SEL[1:0]/AMON0SEL[1:0]/*;
    {   0x21,0x0B   }   ,   ////,*/*/*/*/*/*/*/*;
    {   0x22,0x07   }   ,   //,TBINV/RLINV//WIN_MODE//HV_INTERMIT[2:0]; //donggun 0103 vga camera πÊ«‚ 0x07
    {   0x23,0x96   }   ,   //,H_COUNT[7:0];
    {   0x24,0x00   }   ,   ////,*/*/*/*/*/*/*/*;
    {   0x25,0x42   }   ,   //,V_COUNT[7:0];
    {   0x26,0x00   }   ,   ////,*/*/*/*/*/V_COUNT[10:8];
    {   0x27,0x00   }   ,   ////,*/*/*/*/*/*/*/*;
    {   0x28,0x00   }   ,   ////,*/*/*/*/*/*/*/*;
    {   0x29,0x83   }   ,   ////,*/*/*/*/*/*/*/*;
    {   0x2A,0x84   }   ,   ////,*/*/*/*/*/*/*/*;
    {   0x2B,0xAE   }   ,   ////,*/*/*/*/*/*/*/*;
    {   0x2C,0x21   }   ,   ////,*/*/*/*/*/*/*/*;
    {   0x2D,0x00   }   ,   ////,*/*/*/*/*/*/*/*;
    {   0x2E,0x04   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x2F,0x7D   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x30,0x19   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x31,0x88   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x32,0x88   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x33,0x09   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x34,0x6C   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x35,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x36,0x90   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x37,0x22   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x38,0x0B   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x39,0xAA   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x3A,0x0A   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x3B,0x84   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x3C,0x03   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x3D,0x10   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x3E,0x4C   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x3F,0x1D   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x40,0x34   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x41,0x05   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x42,0x12   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x43,0xB0   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x44,0x3F   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x45,0xFF   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x46,0x44   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x47,0x44   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x48,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x49,0xE8   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x4A,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x4B,0x9F   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x4C,0x9B   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x4D,0x2B   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x4E,0x53   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x4F,0x50   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x50,0x0E   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x51,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x52,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x53,0x04   }   ,   //,TP_MODE[4:0]/TPG_DR_SEL/TPG_CBLK_SW/TPG_LINE_SW;
    {   0x54,0x08   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x55,0x14   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x56,0x84   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x57,0x30   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x58,0x80   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x59,0x80   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x5A,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x5B,0x06   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x5C,0xF0   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x5D,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x5E,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x5F,0xB0   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x60,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x61,0x1B   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x62,0x4F   }   ,   //,HYUKO_START[7:0];
    {   0x63,0x04   }   ,   //,VYUKO_START[7:0];
    {   0x64,0x10   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x65,0x20   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x66,0x30   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x67,0x28   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x68,0x66   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x69,0xC0   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x6A,0x30   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x6B,0x30   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x6C,0x3F   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x6D,0xBF   }   ,   ///,*/*/*/*/*/*/*/*;
    {   0x6E,0xAB   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x6F,0x30   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x70,0x80   }   ,   //,AGMIN_BLACK_ADJ[7:0];
    {   0x71,0x90   }   ,   //,AGMAX_BLACK_ADJ[7:0];
    {   0x72,0x00   }   ,   //,IDR_SET[7:0];
    {   0x73,0x28   }   ,   //,PWB_RG[7:0];
    {   0x74,0x00   }   ,   //,PWB_GRG[7:0];
    {   0x75,0x00   }   ,   //,PWB_GBG[7:0];
    {   0x76,0x58   }   ,   //,PWB_BG[7:0];
    {   0x77,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x78,0x80   }   ,   //,LSSC_SW/*/*/*/*/*/*/*/;
    {   0x79,0x52   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x7A,0x4F   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x7B,0x84   }   ,   //,LSSC_LEFT_RG[7:0];
    {   0x7C,0x4D   }   ,   //,LSSC_LEFT_GG[7:0];
    {   0x7D,0x44   }   ,   //,LSSC_LEFT_BG[7:0];
    {   0x7E,0xB1   }   ,   //,LSSC_RIGHT_RG[7:0];
    {   0x7F,0x77   }   ,   //,LSSC_RIGHT_GG[7:0];
    {   0x80,0x67   }   ,   //,LSSC_RIGHT_BG[7:0];
    {   0x81,0x6D   }   ,   //,LSSC_TOP_RG[7:0];
    {   0x82,0x50   }   ,   //,LSSC_TOP_GG[7:0];
    {   0x83,0x3C   }   ,   //,LSSC_TOP_BG[7:0];
    {   0x84,0x78   }   ,   //,LSSC_BOTTOM_RG[7:0];
    {   0x85,0x4B   }   ,   //,LSSC_BOTTOM_GG[7:0];
    {   0x86,0x31   }   ,   //,LSSC_BOTTOM_BG[7:0];
    {   0x87,0x01   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x88,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x89,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x8A,0x40   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x8B,0x09   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x8C,0xE0   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x8D,0x20   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x8E,0x20   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x8F,0x20   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x90,0x20   }   ,   //,*/*/*/*/*/*/*/*;
    {   0x91,0x80   }   ,   //,ANR_SW/*/*/*/TEST_ANR/*/*/*;
    {   0x92,0x30   }   ,   //,AGMIN_ANR_WIDTH[7:0];
    {   0x93,0x40   }   ,   //,AGMAX_ANR_WIDTH[7:0];
    {   0x94,0x40   }   ,   //,AGMIN_ANR_MP[7:0];
    {   0x95,0x80   }   ,   //,AGMAX_ANR_MP[7:0];
    {   0x96,0x80   }   ,   //,DTL_SW/*/*/*/*/*/*/*/;
    {   0x97,0x20   }   ,   //,AGMIN_HDTL_NC[7:0];
    {   0x98,0x68   }   ,   //,AGMIN_VDTL_NC[7:0];
    {   0x99,0xFF   }   ,   //,AGMAX_HDTL_NC[7:0];
    {   0x9A,0xFF   }   ,   //,AGMAX_VDTL_NC[7:0];
    {   0x9B,0x5C   }   ,   //,AGMIN_HDTL_MG[7:0];
    {   0x9C,0x28   }   ,   //,AGMIN_HDTL_PG[7:0];
    {   0x9D,0x40   }   ,   //,AGMIN_VDTL_MG[7:0];
    {   0x9E,0x28   }   ,   //,AGMIN_VDTL_PG[7:0];
    {   0x9F,0x00   }   ,   //,AGMAX_HDTL_MG[7:0];
    {   0xA0,0x00   }   ,   //,AGMAX_HDTL_PG[7:0];
    {   0xA1,0x00   }   ,   //,AGMAX_VDTL_MG[7:0];
    {   0xA2,0x00   }   ,   //,AGMAX_VDTL_PG[7:0];
    {   0xA3,0x80   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xA4,0x82   }   ,   //,HCBC_SW/*/*/*/*/*/*/*/;
    {   0xA5,0x28   }   ,   //,AGMIN_HCBC_G[7:0];
    {   0xA6,0x18   }   ,   //,AGMAX_HCBC_G[7:0];
    {   0xA7,0x98   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xA8,0x98   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xA9,0x98   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xAA,0x10   }   ,   //,LMCC_BMG_SEL/LMCC_BMR_SEL/*/LMCC_GMB_SEL/LMCC_GMR_SEL/*/;
    {   0xAB,0x5B   }   ,   //,LMCC_RMG_G[7:0];
    {   0xAC,0x00   }   ,   //,LMCC_RMB_G[7:0];
    {   0xAD,0x00   }   ,   //,LMCC_GMR_G[7:0];
    {   0xAE,0x00   }   ,   //,LMCC_GMB_G[7:0];
    {   0xAF,0x00   }   ,   //,LMCC_BMR_G[7:0];
    {   0xB0,0x48   }   ,   //,LMCC_BMG_G[7:0];
    {   0xB1,0xC0   }   ,   //,GAM_SW[1:0]/*/CGC_DISP/TEST_AWBDISP/*/YUVM_AWBDISP_SW/YU;
    {   0xB2,0x4D   }   ,   //,*/R_MATRIX[6:0];
    {   0xB3,0x10   }   ,   //,*/B_MATRIX[6:0];
    {   0xB4,0xC8   }   ,   //,UVG_SEL/BRIGHT_SEL/*/TEST_YUVM_PE/NEG_YMIN_SW/PIC_EFFECT;
    {   0xB5,0x5B   }   ,   //,CONTRAST[7:0];
    {   0xB6,0x47   }   ,   //,BRIGHT[7:0];
    {   0xB7,0x00   }   ,   //,Y_MIN[7:0];
    {   0xB8,0xFF   }   ,   //,Y_MAX[7:0];
    {   0xB9,0x69   }   ,   //,U_GAIN[7:0];
    {   0xBA,0x66   }   ,   //,V_GAIN[7:0];
    {   0xBB,0x78   }   ,   //,SEPIA_US[7:0];
    {   0xBC,0x90   }   ,   //,SEPIA_VS[7:0];
    {   0xBD,0x08   }   ,   //,U_CORING[7:0];
    {   0xBE,0x08   }   ,   //,V_CORING[7:0];
    {   0xBF,0xC0   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xC0,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xC1,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xC2,0x80   }   ,   //,ALC_SW/ALC_LOCK/*/*/*/*/*/*/;
    {   0xC3,0x14   }   ,   //,MES[7:0];
    {   0xC4,0x03   }   ,   //,*/*/MES[13:8];
    {   0xC5,0x00   }   ,   //,MDG[7:0];
    {   0xC6,0x74   }   ,   //,MAG[7:0];
    {   0xC7,0x80   }   ,   //,AGCONT_SEL[1:0]/*/*/MAG[11:8];
    {   0xC8,0x20   }   ,   //,AG_MIN[7:0];
    {   0xC9,0x08   }   ,   //,AG_MAX[7:0];
    {   0xCA,0x02   }   ,   //,AUTO_LES_SW/AUTO_LES_MODE[2:0]/ALC_WEIGHT[1:0]/FLCKADJ[1;
    {   0xCB,0xE7   }   ,   //,ALC_LV[7:0];
    {   0xCC,0x10   }   ,   //,*/UPDN_MODE[2:0]/ALC_LV[9:8];
    {   0xCD,0x0A   }   ,   //,ALC_LVW[7:0];
    {   0xCE,0x93   }   ,   //,L64P600S[7:0];
    {   0xCF,0x06   }   ,   //,*/ALC_VWAIT[2:0]/L64P600S[11:8];
    {   0xD0,0x80   }   ,   //,UPDN_SPD[7:0];
    {   0xD1,0x20   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xD2,0x80   }   ,   //,NEAR_SPD[7:0];
    {   0xD3,0x30   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xD4,0x8A   }   ,   //,AC5060/*/ALC_SAFETY[5:0];
    {   0xD5,0x02   }   ,   //,*/*/*/*/*/ALC_SAFETY2[2:0];
    {   0xD6,0x4F   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xD7,0x08   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xD8,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xD9,0xFF   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xDA,0x01   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xDB,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xDC,0x14   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xDD,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xDE,0x80   }   ,   //,AWB_SW/AWB_LOCK/*/AWB_TEST/*/*/HEXG_SLOPE_SEL/COLGATE_SE;
    {   0xDF,0x80   }   ,   ///,WB_MRG[7:0];
    {   0xE0,0x80   }   ,   //,WB_MGG[7:0];
    {   0xE1,0x80   }   ,   //,WB_MBG[7:0];
    {   0xE2,0x22   }   ,   //,WB_RBMIN[7:0];
    {   0xE3,0xF8   }   ,   //,WB_RBMAX[7:0];
    {   0xE4,0x80   }   ,   //,HEXA_SW/*/COLGATE_RANGE[1:0]/*/*/*/COLGATE_OPEN;
    {   0xE5,0x2C   }   ,   //,*/RYCUTM[6:0];
    {   0xE6,0x54   }   ,   //,*/RYCUTP[6:0];
    {   0xE7,0x28   }   ,   //,*/BYCUTM[6:0];
    {   0xE8,0x39   }   ,   //,*/BYCUTP[6:0];
    {   0xE9,0xE4   }   ,   //,RBCUTL[7:0];
    {   0xEA,0x3C   }   ,   //,RBCUTH[7:0];
    {   0xEB,0x81   }   ,   //,SQ1_SW/SQ1_POL/*/*/*/*/*/YGATE_SW;
    {   0xEC,0x37   }   ,   //,RYCUT1L[7:0];
    {   0xED,0x5A   }   ,   //,RYCUT1H[7:0];
    {   0xEE,0xDE   }   ,   //,BYCUT1L[7:0];
    {   0xEF,0x08   }   ,   //,BYCUT1H[7:0];
    {   0xF0,0x18   }   ,   //,YGATE_L[7:0];
    {   0xF1,0xFE   }   ,   //,YGATE_H[7:0];
    {   0xF2,0x00   }   ,   //,*/*/IPIX_DISP_SW/*/*/*/*/;
    {   0xF3,0x02   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xF4,0x02   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xF5,0x04   }   ,   //,AWB_WAIT[7:0];
    {   0xF6,0x00   }   ,   //,AWB_SPDDLY[7:0];
    {   0xF7,0x20   }   ,   //,*/*/AWB_SPD[5:0];
    {   0xF8,0x86   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xF9,0x00   }   ,   //,*/*/*/*/*/*/*/*;
    {   0xFA,0x41   }   ,   //,MR_HBLK_START[7:0];
    {   0xFB,0x50   }   ,   //,*/MR_HBLK_WIDTH[6:0];
    {   0xFC,0x0C   }   ,   //,MR_VBLK_START[7:0];
    {   0xFD,0x3C   }   ,   //,*/*/MR_VBLK_WIDTH[5:0];
    {   0xFE,0x50   }   ,   //,PIC_FORMAT[3:0]/PINTEST_SEL[3:0];
    {   0xFF,0x85   }       //,SLEEP/*/PARALLEL_OUTSW[1:0]/DCLK_POL/DOUT_CBLK_SW/*/AL;
#endif
};


/************************************************************
; slave address 0x7A
;	MCK:31M	PCK:60M
;
; Preview : 640x480
; Capture : 1280x960
; Preview frame rate MAX. 30fps (variable frame rate)
; 
;************************************************************/
struct tcm9000md_address_value_pair const tcm9000md_reg_init_vt_settings[] = {

};


/*effect*/
static const struct tcm9000md_address_value_pair const effect_off_reg_settings_array[] = {
#ifdef TCM9001MD_SENSOR
	//normal
	// write address : 0x7C   
	{0xB4,0xC8},  
#else
	{0xB4, 0xC8}	// donghyun.kwon (20110406) : add effect
#endif
};
static const struct tcm9000md_address_value_pair const effect_mono_reg_settings_array[] = {
#ifdef TCM9001MD_SENSOR
	//Black&White                         
	// write address : 0x7C     
	{0xB4,0xC9},   
#else
	{0xB4, 0xC9}	// donghyun.kwon (20110406) : add effect
#endif
};
static const struct tcm9000md_address_value_pair const effect_sepia_reg_settings_array[] = {	
#ifdef TCM9001MD_SENSOR
	//sepia(Color)
	// write address : 0x7C 
	{0xB4,0xCB},   
#else
	{0xB4, 0xCB}	// donghyun.kwon (20110406) : add effect
#endif
};
static const struct tcm9000md_address_value_pair const effect_negative_reg_settings_array[] = {	
#ifdef TCM9001MD_SENSOR
	//negative									   
	// write address : 0x7C 			 
	{0xB4,0xCC},  
#else
	{0xB4, 0xCC}	// donghyun.kwon (20110406) : add effect
#endif
};
static const struct tcm9000md_address_value_pair const effect_solarize_reg_settings_array[] = {	
};
static const struct tcm9000md_address_value_pair const effect_blue_reg_settings_array[] = {	

};


/*wb*/
static const struct tcm9000md_address_value_pair const wb_auto_reg_settings_array[] = {	
#ifdef TCM9001MD_SENSOR
	//AUTO(Default)
	{0x00DE,0x0080},  //AWB_SW/AWB_LOCK/*/AWB_TEST/*/*/HEXG_SLOPE_SEL/COLGATE_SE;
	{0x00DF,0x0080},  //WB_MRG[7:0];
	{0x00E1,0x0080},  //WB_MBG[7:0];
#else
	// donghyun.kwon (20110413) : add WB - start
	{0x73, 0x28},
	{0x74, 0x00},
	{0x75, 0x00},
	{0x76, 0x58},
	{0xE2, 0x22},
	{0xE3, 0xF8}, 
	// donghyun.kwon (20110413) : add WB - start
#endif
};
static const struct tcm9000md_address_value_pair const wb_incandescent_reg_settings_array[] = {	
#ifdef TCM9001MD_SENSOR
	//INCANDESECNT
	{0x00DE,0x0000},  //AWB_SW/AWB_LOCK/*/AWB_TEST/*/*/HEXG_SLOPE_SEL/COLGATE_SE;
	{0x00DF,0x0052},  //WB_MRG[7:0];
	{0x00E1,0x0095},  //WB_MBG[7:0];
#else
	// donghyun.kwon (20110413) : add WB - start
	{0x73, 0x01},
	{0x74, 0x1C},
	{0x75, 0x1C},
	{0x76, 0xAA},
	{0xE2, 0x7E},
	{0xE3, 0x82},
	// donghyun.kwon (20110413) : add WB - end
#endif
};
static const struct tcm9000md_address_value_pair const wb_fluorescent_reg_settings_array[] = {
#ifdef TCM9001MD_SENSOR
	//FLUORESCENT
	{0x00DE,0x0000},  //AWB_SW/AWB_LOCK/*/AWB_TEST/*/*/HEXG_SLOPE_SEL/COLGATE_SE;
	{0x00DF,0x006F},  //WB_MRG[7:0];
	{0x00E1,0x008B},  //WB_MBG[7:0];
#else
	// donghyun.kwon (20110413) : add WB - start
	{0x73, 0x0F},
	{0x74, 0x00},
	{0x75, 0x00},
	{0x76, 0x6E},
	{0xE2, 0x7E},
	{0xE3, 0x82},
	// donghyun.kwon (20110413) : add WB - end
#endif
};
static const struct tcm9000md_address_value_pair const wb_sunny_reg_settings_array[] = {	
#ifdef TCM9001MD_SENSOR
	//DAYLIGHT
	{0x00DE,0x0000},  //AWB_SW/AWB_LOCK/*/AWB_TEST/*/*/HEXG_SLOPE_SEL/COLGATE_SE;
	{0x00DF,0x0088},  //WB_MRG[7:0];
	{0x00E1,0x0067},  //WB_MBG[7:0];
#else
	// donghyun.kwon (20110413) : add WB - start
	{0x73, 0x2A},
	{0x74, 0x00},
	{0x75, 0x00},
	{0x76, 0x2B},
	{0xE2, 0x7E},
	{0xE3, 0x82},
	// donghyun.kwon (20110413) : add WB - end
#endif
};
static const struct tcm9000md_address_value_pair const wb_cloudy_reg_settings_array[] = {	
#ifdef TCM9001MD_SENSOR
	//CLOUDY
	{0x00DE,0x0000},  //AWB_SW/AWB_LOCK/*/AWB_TEST/*/*/HEXG_SLOPE_SEL/COLGATE_SE;
	{0x00DF,0x008F},  //WB_MRG[7:0];
	{0x00E1,0x0063},  //WB_MBG[7:0];
#else
	// donghyun.kwon (20110413) : add WB - start
	{0x73, 0x43},
	{0x74, 0x00},
	{0x75, 0x00},
	{0x76, 0x29},
	{0xE2, 0x7E},
	{0xE3, 0x82},
	// donghyun.kwon (20110413) : add WB - end
#endif
};

// donghyun.kwon(20110505) : add exposure compensation for VGA - start
static const struct tcm9000md_address_value_pair const brightness_lv0_reg_settings_array[] = {   //	-12
	{0xCB, 0x63},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv1_reg_settings_array[] = {   //	-11
	{0xCB, 0x6E},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv2_reg_settings_array[] = {   //	-10
	{0xCB, 0x79},	// AE control
	{0xCC, 0x10},
}; 
static const struct tcm9000md_address_value_pair const brightness_lv3_reg_settings_array[] = {   //	-9
	{0xCB, 0x84},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv4_reg_settings_array[] = {   //	-8
	{0xCB, 0x8F},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv5_reg_settings_array[] = {   //	-7
	{0xCB, 0x9A},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv6_reg_settings_array[] = {   //	-6
	{0xCB, 0xA5},	// AE control
	{0xCC, 0x10},
}; 
static const struct tcm9000md_address_value_pair const brightness_lv7_reg_settings_array[] = {   //	-5
	{0xCB, 0xB0},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv8_reg_settings_array[] = {   //	-4
	{0xCB, 0xBB},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv9_reg_settings_array[] = {   //	-3
	{0xCB, 0xC6},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv10_reg_settings_array[] = {   //	-2
	{0xCB, 0xD1},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv11_reg_settings_array[] = {   //	-1
	{0xCB, 0xDC},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv12_reg_settings_array[] = {   //	0
	{0xCB, 0xE7},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv13_reg_settings_array[] = {   //	1
	{0xCB, 0xF2},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv14_reg_settings_array[] = {   //	2
	{0xCB, 0xFD},	// AE control
	{0xCC, 0x10},
};
static const struct tcm9000md_address_value_pair const brightness_lv15_reg_settings_array[] = {   //	3
	{0xCB, 0x08},	// AE control
	{0xCC, 0x11},
};
static const struct tcm9000md_address_value_pair const brightness_lv16_reg_settings_array[] = {  // 4
	{0xCB, 0x13},	// AE control
	{0xCC, 0x11},
};
static const struct tcm9000md_address_value_pair const brightness_lv17_reg_settings_array[] = {  // 5
	{0xCB, 0x1E},	// AE control
	{0xCC, 0x11},
};
static const struct tcm9000md_address_value_pair const brightness_lv18_reg_settings_array[] = {  // 6
	{0xCB, 0x29},	// AE control
	{0xCC, 0x11},
};
static const struct tcm9000md_address_value_pair const brightness_lv19_reg_settings_array[] = {  // 7
	{0xCB, 0x34},	// AE control
	{0xCC, 0x11},
};
static const struct tcm9000md_address_value_pair const brightness_lv20_reg_settings_array[] = {  // 8
	{0xCB, 0x3F},	// AE control
	{0xCC, 0x11},
};
static const struct tcm9000md_address_value_pair const brightness_lv21_reg_settings_array[] = {  // 9
	{0xCB, 0x4A},	// AE control
	{0xCC, 0x11},
};
static const struct tcm9000md_address_value_pair const brightness_lv22_reg_settings_array[] = {  // 10
	{0xCB, 0x55},	// AE control
	{0xCC, 0x11},
};
static const struct tcm9000md_address_value_pair const brightness_lv23_reg_settings_array[] = {  // 11
	{0xCB, 0x60},	// AE control
	{0xCC, 0x11},
};
static const struct tcm9000md_address_value_pair const brightness_lv24_reg_settings_array[] = {  // 12
	{0xCB, 0x6B},	// AE control
	{0xCC, 0x11},
};
// donghyun.kwon(20110505) : add exposure compensation for VGA - end


// donghyun.kwon (20110421) : add night mode -start
static const struct tcm9000md_address_value_pair const nightmode_on_reg_settings_array[] = {   
#ifdef TCM9001MD_SENSOR
	{0xCA, 0xA2},	//AUTO_LES_SW/AUTO_LES_MODE[2:0]/ALC_WEIGHT[1:0]/FLCKADJ[1;
#else
	{0xCA, 0x92},
#endif	
};
static const struct tcm9000md_address_value_pair const nightmode_off_reg_settings_array[] = {   
#ifdef TCM9001MD_SENSOR
	{0xCA, 0x96},	//AUTO_LES_SW/AUTO_LES_MODE[2:0]/ALC_WEIGHT[1:0]/FLCKADJ[1;
#else
	{0xCA, 0x02},
#endif	
};
// donghyun.kwon (20110421) : add night mode -end

#ifdef LGE_SUPPORT_FRONTCAM_FPSRANGE
static const struct tcm9000md_address_value_pair const framerate_auto_reg_settings_array[] = {   
	{0x0020, 0x0080},
	{0x001E, 0x008E},
	{0x00CA, 0x009A},
};
static const struct tcm9000md_address_value_pair const framerate_fixed_7p5_reg_settings_array[] = {   
	{0x0020, 0x0040},
	{0x001E, 0x0060},
	{0x00CA, 0x0002},
};
static const struct tcm9000md_address_value_pair const framerate_fixed_10_reg_settings_array[] = {   
	{0x0020, 0x0080},
	{0x001E, 0x0060},
	{0x00CA, 0x0002},
};
static const struct tcm9000md_address_value_pair const framerate_fixed_15_reg_settings_array[] = {   
	{0x0020, 0x0080},
	{0x001E, 0x008E},
	{0x00CA, 0x0002},
};
#endif

struct tcm9000md_reg tcm9000md_regs = {
   /* VGA 22fps for normal mode */	
  .init_reg_settings 	  = tcm9000md_reg_init_settings,  
  .init_reg_settings_size = ARRAY_SIZE(tcm9000md_reg_init_settings),

   /* VGA 30fps for VT mode */ 
  .init_reg_vt_settings  = tcm9000md_reg_init_vt_settings,  
  .init_reg_vt_settings_size = ARRAY_SIZE(tcm9000md_reg_init_vt_settings),
   
	/*effect*/
  .effect_off_reg_settings 	  			= effect_off_reg_settings_array,
  .effect_off_reg_settings_size  		= ARRAY_SIZE(effect_off_reg_settings_array),
  .effect_mono_reg_settings 	  		= effect_mono_reg_settings_array,
  .effect_mono_reg_settings_size  		= ARRAY_SIZE(effect_mono_reg_settings_array),
  .effect_sepia_reg_settings 	  		= effect_sepia_reg_settings_array,
  .effect_sepia_reg_settings_size  		= ARRAY_SIZE(effect_sepia_reg_settings_array),
  .effect_negative_reg_settings 	  	= effect_negative_reg_settings_array,
  .effect_negative_reg_settings_size  	= ARRAY_SIZE(effect_negative_reg_settings_array),
  .effect_solarize_reg_settings 	  	= effect_solarize_reg_settings_array,
  .effect_solarize_reg_settings_size  	= ARRAY_SIZE(effect_solarize_reg_settings_array),
  .effect_blue_reg_settings 			= effect_blue_reg_settings_array,
  .effect_blue_reg_settings_size		= ARRAY_SIZE(effect_blue_reg_settings_array),

	/*wb*/
  .wb_auto_reg_settings 	  			= wb_auto_reg_settings_array,
  .wb_auto_reg_settings_size  			= ARRAY_SIZE(wb_auto_reg_settings_array),
  .wb_incandescent_reg_settings 	  	= wb_incandescent_reg_settings_array,
  .wb_incandescent_reg_settings_size  	= ARRAY_SIZE(wb_incandescent_reg_settings_array),
  .wb_fluorescent_reg_settings 	  		= wb_fluorescent_reg_settings_array,
  .wb_fluorescent_reg_settings_size  	= ARRAY_SIZE(wb_fluorescent_reg_settings_array),
  .wb_sunny_reg_settings 	  			= wb_sunny_reg_settings_array,
  .wb_sunny_reg_settings_size  			= ARRAY_SIZE(wb_sunny_reg_settings_array),
  .wb_cloudy_reg_settings 	  			= wb_cloudy_reg_settings_array,
  .wb_cloudy_reg_settings_size  		= ARRAY_SIZE(wb_cloudy_reg_settings_array),

  // donghyun.kwon(20110505) : add exposure compensation for VGA - start
  /*brightness*/
  .brightness_lv0_reg_settings		= brightness_lv0_reg_settings_array,
  .brightness_lv0_reg_settings_size	= ARRAY_SIZE(brightness_lv0_reg_settings_array),
  .brightness_lv1_reg_settings 		= brightness_lv1_reg_settings_array,
  .brightness_lv1_reg_settings_size	= ARRAY_SIZE(brightness_lv1_reg_settings_array),
  .brightness_lv2_reg_settings 		= brightness_lv2_reg_settings_array,
  .brightness_lv2_reg_settings_size	= ARRAY_SIZE(brightness_lv2_reg_settings_array),
  .brightness_lv3_reg_settings 		= brightness_lv3_reg_settings_array,
  .brightness_lv3_reg_settings_size	= ARRAY_SIZE(brightness_lv3_reg_settings_array),
  .brightness_lv4_reg_settings 		= brightness_lv4_reg_settings_array,
  .brightness_lv4_reg_settings_size	= ARRAY_SIZE(brightness_lv4_reg_settings_array),
  .brightness_lv5_reg_settings 		= brightness_lv5_reg_settings_array,
  .brightness_lv5_reg_settings_size	= ARRAY_SIZE(brightness_lv5_reg_settings_array),
  .brightness_lv6_reg_settings 		= brightness_lv6_reg_settings_array,
  .brightness_lv6_reg_settings_size	= ARRAY_SIZE(brightness_lv6_reg_settings_array),
  .brightness_lv7_reg_settings 		= brightness_lv7_reg_settings_array,
  .brightness_lv7_reg_settings_size	= ARRAY_SIZE(brightness_lv7_reg_settings_array),
  .brightness_lv8_reg_settings 		= brightness_lv8_reg_settings_array,
  .brightness_lv8_reg_settings_size	= ARRAY_SIZE(brightness_lv8_reg_settings_array),
  .brightness_lv9_reg_settings	  = brightness_lv9_reg_settings_array,
  .brightness_lv9_reg_settings_size = ARRAY_SIZE(brightness_lv9_reg_settings_array),

  .brightness_lv10_reg_settings	= brightness_lv10_reg_settings_array,
  .brightness_lv10_reg_settings_size = ARRAY_SIZE(brightness_lv10_reg_settings_array),
  .brightness_lv11_reg_settings = brightness_lv11_reg_settings_array,
  .brightness_lv11_reg_settings_size = ARRAY_SIZE(brightness_lv11_reg_settings_array),
  .brightness_lv12_reg_settings = brightness_lv12_reg_settings_array,
  .brightness_lv12_reg_settings_size = ARRAY_SIZE(brightness_lv12_reg_settings_array),
  .brightness_lv13_reg_settings = brightness_lv13_reg_settings_array,
  .brightness_lv13_reg_settings_size = ARRAY_SIZE(brightness_lv13_reg_settings_array),
  .brightness_lv14_reg_settings = brightness_lv14_reg_settings_array,
  .brightness_lv14_reg_settings_size = ARRAY_SIZE(brightness_lv14_reg_settings_array),
  .brightness_lv15_reg_settings = brightness_lv15_reg_settings_array,
  .brightness_lv15_reg_settings_size = ARRAY_SIZE(brightness_lv15_reg_settings_array),
  .brightness_lv16_reg_settings = brightness_lv16_reg_settings_array,
  .brightness_lv16_reg_settings_size = ARRAY_SIZE(brightness_lv16_reg_settings_array),
  .brightness_lv17_reg_settings = brightness_lv17_reg_settings_array,
  .brightness_lv17_reg_settings_size = ARRAY_SIZE(brightness_lv17_reg_settings_array),
  .brightness_lv18_reg_settings = brightness_lv18_reg_settings_array,
  .brightness_lv18_reg_settings_size = ARRAY_SIZE(brightness_lv18_reg_settings_array),
  .brightness_lv19_reg_settings = brightness_lv19_reg_settings_array,
  .brightness_lv19_reg_settings_size = ARRAY_SIZE(brightness_lv19_reg_settings_array),

  .brightness_lv20_reg_settings = brightness_lv20_reg_settings_array,
  .brightness_lv20_reg_settings_size = ARRAY_SIZE(brightness_lv20_reg_settings_array),
  .brightness_lv21_reg_settings = brightness_lv21_reg_settings_array,
  .brightness_lv21_reg_settings_size = ARRAY_SIZE(brightness_lv21_reg_settings_array),
  .brightness_lv22_reg_settings = brightness_lv22_reg_settings_array,
  .brightness_lv22_reg_settings_size = ARRAY_SIZE(brightness_lv22_reg_settings_array),
  .brightness_lv23_reg_settings = brightness_lv23_reg_settings_array,
  .brightness_lv23_reg_settings_size = ARRAY_SIZE(brightness_lv23_reg_settings_array),
  .brightness_lv24_reg_settings = brightness_lv24_reg_settings_array,
  .brightness_lv24_reg_settings_size = ARRAY_SIZE(brightness_lv24_reg_settings_array),
  // donghyun.kwon(20110505) : add exposure compensation for VGA - end

   // donghyun.kwon (20110421) : add night mode -start
   .nightmode_on_reg_settings 			 = nightmode_on_reg_settings_array,
   .nightmode_on_reg_settings_size		 = ARRAY_SIZE(nightmode_on_reg_settings_array),
   .nightmode_off_reg_settings			 = nightmode_off_reg_settings_array,
   .nightmode_off_reg_settings_size		 = ARRAY_SIZE(nightmode_off_reg_settings_array),
   // donghyun.kwon (20110421) : add night mode -end

#ifdef LGE_SUPPORT_FRONTCAM_FPSRANGE
	.framerate_auto_reg_settings			  = framerate_auto_reg_settings_array,
	.framerate_auto_reg_settings_size 	  = ARRAY_SIZE(framerate_auto_reg_settings_array),
	.framerate_fixed_7p5_reg_settings 		  = framerate_fixed_7p5_reg_settings_array,
	.framerate_fixed_7p5_reg_settings_size	  = ARRAY_SIZE(framerate_fixed_7p5_reg_settings_array),
	.framerate_fixed_10_reg_settings 		  = framerate_fixed_10_reg_settings_array,
	.framerate_fixed_10_reg_settings_size	  = ARRAY_SIZE(framerate_fixed_10_reg_settings_array),
	.framerate_fixed_15_reg_settings 		  = framerate_fixed_15_reg_settings_array,
	.framerate_fixed_15_reg_settings_size	  = ARRAY_SIZE(framerate_fixed_15_reg_settings_array),
#endif
};


