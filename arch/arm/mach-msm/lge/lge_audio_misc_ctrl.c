/*  arch/arm/mach-msm/qdsp5v2/lge_audio_misc_ctl.c
 *
 * Copyright (C) 2009 LGE, Inc.
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
#include <mach/qdsp5v2/snddev_icodec.h>
#include <mach/qdsp5v2/snddev_ecodec.h>
#include <mach/qdsp5v2/snddev_mi2s.h>
#include <mach/qdsp5v2/snddev_virtual.h>
#include <mach/lge_audio_misc_ctl.h>

#include <linux/fs.h>
#include <linux/fcntl.h> 
#include <mach/debug_mm.h>
#include <linux/fcntl.h> 
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/limits.h>
//#include <mach/qdsp5v2/lge_tpa2055-amp.h>
#include <mach/qdsp5v2/audio_dev_ctl.h>

/*RPC versions*/
#define VOEM_IFPROG					0x300000A3
#define VOEM_IFVERS					0x00020002
/*RPC loopback proc */
#define CALLBACK_NULL     0xffffffff
#define RPC_VOEM_PACKET_LOOPBACK_PROC 5
#define RPC_VOEM_HAC_VC_PROC 6
/*Packet loopback device ID's*/
#define HANDSET_PACKET_LOOPBACK_ID 0x1
#define HEADSET_PACKET_LOOPBACK_ID 0x2
#define QTR_HANDSET_PACKET_LOOPBACK_ID 0x3
/* Loopback actions*/
#define ACOUSTIC_OFF 0x00		//Packet Loopback
#define ACOUSTIC_ON 0x01		//packet loopback off
#define HEADSET_LB_PATH 0x02	//set headset path
#define HANDSET_LB_PATH 0x03	//set handset path
#define ACOUSTIC_LOOPBACK_ON 0x04//dsp loopback
#define ACOUSTIC_LOOPBACK_OFF 0x05//dsp loopback off

/*******************************************************************************
*  Structure for misc controls.  Add variables if needed.
*
********************************************************************************/
struct audio {
	struct mutex lock;
	struct msm_rpc_endpoint *ept;
	int opened;
	int path_set;
	int prev_path_set;
	int dsp_loopback_opened;
	int packet_loopback_opened;
};
static struct audio misc_audio;
/*******************************************************************************
*  Structure for loopback RPC .  Add variables if needed.
*
********************************************************************************/
struct rpc_snd_set_loopback_mode_args {
     uint32_t mode;
     uint32_t cb_func;
     uint32_t client_data;
};
struct snd_set_loopback_mode_msg {
    struct rpc_request_hdr hdr;
    struct rpc_snd_set_loopback_mode_args args;
};
struct snd_set_loopback_mode_msg lmsg;

#ifdef CONFIG_LGE_MODEL_E739
/*******************************************************************************
*	Function Name :  audio_misc_driver_test
*	Args : test function ID.
*	dependencies : TEST_MISC_DRVR macro 
*	Note: Exposed this function for kernal. Can test driver from any kernal event
********************************************************************************/

#define ADIE_CODEC_PACK_ENTRY(reg, mask, val) ((val)|(mask << 8)|(reg << 16))

#define ADIE_CODEC_UNPACK_ENTRY(packed, reg, mask, val) \
	do { \
		((reg) = ((packed >> 16) & (0xff))); \
		((mask) = ((packed >> 8) & (0xff))); \
		((val) = ((packed) & (0xff))); \
	} while (0);

char * strtok(char *s, const char *delim);
char *strtok_r(char *s, const char *delim, char **last);
char strtol(const char *nptr, char **endptr, int base);
int atoi(const char *str);

extern char amp_cal_data[];
extern struct adie_codec_action_unit *codec_cal[];
bool bReadCalData = true;
char file_data_array[1500] = {0,};
  
extern struct snddev_icodec_data earpiece_voice_rx_data;
extern struct snddev_icodec_data headset_mono_voice_rx_data;
extern struct snddev_icodec_data speaker_stereo_voice_rx_data;
extern struct snddev_ecodec_data bt_sco_voice_rx_data;

#ifdef CONFIG_LGE_MODEL_E739
bool bTTY_Headset = false;
#endif

enum {
  QTR_EARPIECE = 0,
  QTR_HEADSET_VOICE,
  QTR_SPEAKER_VOICE,
  QTR_TTY,
  QTR_HEADSET_AUDIO,
  QTR_HEADSET_SPEAKER,
  QTR_SPEAKER_AUDIO,
  QTR_HANDSET_MIC,
  QTR_HEADSET_MIC,
  QTR_SPEAKER_MIC,
  QTR_TTY_MIC,
  QTR_REC_MIC,
  QTR_CAM_MIC,
  QTR_VOICE_MIC,
  QTR_CAL_MAX
};

enum {
  VOICE_EARPIECE = 0,
  VOICE_HEADSET,
  VOICE_SPEAKER,
  VOICE_BT
};

  int QTR_index_data[14][4] = {
    {17,-1,-1,-1},
    {6,7,26,27},
    {6,7,27,28},
    {6,7,26,27},
    {6,7,25,26},
    {6,7,25,26},
    {6,7,27,28},
    {23,24,9,-1},
    {23,24,15,-1},
    {23,24,9,-1},
    {23,24,15,-1},
    {23,24,9,-1},
    {23,24,9,-1},
    {23,24,9,-1}};

u8 QTR_cal_data[QTR_CAL_MAX][4] = {
    {0x2, 0xFF,0xFF,0xFF},
    {0x08,0x08,0x10,0x10},
    {0x10,0x10,0x10,0x10},
    {0x07,0x07,0x10,0x10},
    {0x0,0x0,0x10,0x10},
    {0x0,0x0,0x10,0x10},
    {0x0,0x0,0x10,0x10},
    {0x09,0x09,0xD0,0xFF},
    {0x02,0x02,0xC8,0xFF},
    {0x05,0x05,0xC1,0xFF},
    {0x0,0x0,0x88,0xFF},
    {0x1B,0x1B,0xC1,0xFF},
    {0x1B,0x1B,0xC1,0xFF},
    {0x05,0x05,0xD0,0xFF}};
    
  s16 voice_level_data[4][2] = {
    {400,-2000},
    {-700,-2200},
    {500,-1500},
    {400,-1100}};

int atoi(const char *str)
{
  int nSign = 0, nSum = 0;

  if (*str == '-' || (*str >= '0' && *str <= '9'))
  {
    if (*str == '-')
    {
      nSign = 1;
      str++;
    }

    while(*str >= '0' && *str <= '9')
    {
      nSum = (nSum * 10) + (*str - '0');
      str++;
    }

    if (nSign)
      return (-1) * nSum;
    else
      return nSum;
  }
  else
    return 0;
}
  
char *strtok(char *s, const char *delim)
{
	static char *last;

	return strtok_r(s, delim, &last);
}

char *strtok_r(char *s, const char *delim, char **last)
{
	char *spanp;
	int c, sc;
	char *tok;


	if (s == NULL && (s = *last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		*last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

char strtol(const char *nptr, char **endptr, int base)
{
	const char *s;
	long acc, cutoff;
	int c;
	int neg, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	s = nptr;
	do {
		c = (unsigned char) *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? LONG_MIN : LONG_MAX;
	cutlim = cutoff % base;
	cutoff /= base;
	if (neg) {
		if (cutlim > 0) {
			cutlim -= base;
			cutoff += 1;
		}
		cutlim = -cutlim;
	}
	for (acc = 0, any = 0;; c = (unsigned char) *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0)
			continue;
		if (neg) {
			if (acc < cutoff || (acc == cutoff && c > cutlim)) {
				any = -1;
				acc = LONG_MIN;
			} else {
				any = 1;
				acc *= base;
				acc -= c;
			}
		} else {
			if (acc > cutoff || (acc == cutoff && c > cutlim)) {
				any = -1;
				acc = LONG_MAX;
			} else {
				any = 1;
				acc *= base;
				acc += c;
			}
		}
	}
	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);
	return (char)acc;
}

void set_ampcal_data(void)
{
  struct file *fp_ampcal  = NULL;
  ssize_t read_size = 0;
   char *token = NULL;
  char *separator = "\n,:";
  mm_segment_t old_fs=get_fs();
  int nIndex = 0;
  
  set_fs(get_ds());

  fp_ampcal = filp_open("/system/tuning/amp.txt", O_RDONLY |O_LARGEFILE, 0);


  if( IS_ERR_OR_NULL(fp_ampcal))
  {
    MM_INFO("open fail\n");
    return;
  }

  read_size = fp_ampcal->f_op->read(fp_ampcal, file_data_array, 1000, &fp_ampcal->f_pos);
  token = strtok(file_data_array, separator);
  
  while ((token = strtok(NULL, separator)) != NULL)
  {
    token = strtok(NULL, separator);
    amp_cal_data[nIndex] = (char)strtol(token, NULL, 16);
     
    MM_INFO("AMP CHANGE %s -> %x\n", token, amp_cal_data[nIndex]);
    
    nIndex++;
    if (nIndex == 13)
      break;
  }
  filp_close(fp_ampcal,NULL);

  set_fs(old_fs);

}
void set_QTRcal_data(void)
{
  struct file *fp_QTRcal = NULL;
  ssize_t read_size = 0;
  char *token = NULL;
  char *separator = "\n,:";
  mm_segment_t old_fs=get_fs();
  int nIndex1 = 0, nIndex2 = 0;
  u8 reg=0, mask=0, val=0;
  int i, j;
  struct adie_codec_action_unit *temp;
    
  set_fs(get_ds());

  fp_QTRcal = filp_open("/system/tuning/QTR.txt", O_RDONLY |O_LARGEFILE, 0);

  if(IS_ERR_OR_NULL(fp_QTRcal))
  {
    MM_INFO("open fail\n");
    return;
  }

  read_size = fp_QTRcal->f_op->read(fp_QTRcal, file_data_array, 1500, &fp_QTRcal->f_pos);
  
  token = strtok(file_data_array, separator);
  if ((u8)strtol(token, NULL, 16) == 0)
  {
    bReadCalData = false;
  }
  else
  {
    bReadCalData = true;
  }

  if (bReadCalData == false)
  {
    filp_close(fp_QTRcal,NULL);
    set_fs(old_fs);
    return;
  }
      
  while ((token = strtok(NULL, separator)) != NULL)
  {
    token = strtok(NULL, separator);
    QTR_cal_data[nIndex1][nIndex2] = (u8)strtol(token, NULL, 16);
    nIndex2++;
    if (nIndex2 == 4)
    {
      nIndex1++;
      nIndex2 = 0;
    }
  }

  for (i = 0; i < QTR_CAL_MAX; i++)
  {
    temp = codec_cal[i];

    for (j = 0; j < 4; j++)
   {
      if (QTR_cal_data[i][j] != 0xFF && QTR_index_data[i][j] != -1)
      {
        ADIE_CODEC_UNPACK_ENTRY(temp[QTR_index_data[i][j]].action,reg, mask, val);
        MM_INFO("QTR CHANGE %d -->> %d\n", val, QTR_cal_data[i][j]);
        temp[QTR_index_data[i][j]].action = ADIE_CODEC_PACK_ENTRY(reg, 0xFF, QTR_cal_data[i][j]);
      }
    }
  }

  filp_close(fp_QTRcal,NULL);

  set_fs(old_fs);
}

void set_voicecal_data(void)
{
  struct file *fp_voicecal = NULL;
  ssize_t read_size = 0;
  char data_array[200];
  char *token = NULL;
  char *separator = "\n,:";
  int nIndex1 = 0, nIndex2 = 0;
  struct msm_snddev_info *dev_info;

  set_fs(get_ds());

  fp_voicecal = filp_open("/system/tuning/voice.txt", O_RDONLY |O_LARGEFILE, 0);

  if( IS_ERR_OR_NULL(fp_voicecal))
  {
    MM_INFO("open fail\n");
    return;
  }

  read_size = fp_voicecal->f_op->read(fp_voicecal, data_array, 1000, &fp_voicecal->f_pos);
  
  token = strtok(data_array, separator);

    MM_INFO("11voice READ :::: %d\n",  earpiece_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
    MM_INFO("11voice READ :::: %d\n",  headset_mono_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
    MM_INFO("11voice READ :::: %d\n",  speaker_stereo_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
    MM_INFO("11voice READ :::: %d\n",  bt_sco_voice_rx_data.max_voice_rx_vol[VOC_NB_INDEX]);
  while ((token = strtok(NULL, separator)) != NULL)
  {
    token = strtok(NULL, separator);
    voice_level_data[nIndex1][nIndex2] = atoi(token);    
    MM_INFO("voice SET :::: %s\n", token);
    MM_INFO("voice SET :::: %d\n", voice_level_data[nIndex1][nIndex2]);
    nIndex2++;
    if (nIndex2 == 2)
    {
      nIndex1++;
      nIndex2 = 0;
    }
  }

	dev_info = audio_dev_ctrl_find_dev(DEVICE_ID_INTERNAL_HANDSET_VOICE_RX);
	dev_info->max_voc_rx_vol[0] = voice_level_data[VOICE_EARPIECE][0];
	dev_info->min_voc_rx_vol[0] = voice_level_data[VOICE_EARPIECE][1];
	dev_info->max_voc_rx_vol[1] = voice_level_data[VOICE_EARPIECE][0];
	dev_info->max_voc_rx_vol[1] = voice_level_data[VOICE_EARPIECE][1];

	dev_info = audio_dev_ctrl_find_dev(DEVICE_ID_INTERNAL_HEADSET_VOICE_RX);
	dev_info->max_voc_rx_vol[0] = voice_level_data[VOICE_HEADSET][0];
	dev_info->min_voc_rx_vol[0] = voice_level_data[VOICE_HEADSET][1];
	dev_info->max_voc_rx_vol[1] = voice_level_data[VOICE_HEADSET][0];
	dev_info->max_voc_rx_vol[1] = voice_level_data[VOICE_HEADSET][1];

	dev_info = audio_dev_ctrl_find_dev(DEVICE_ID_INTERNAL_SPEAKER_VOICE_RX);
	dev_info->max_voc_rx_vol[0] = voice_level_data[VOICE_SPEAKER][0];
	dev_info->min_voc_rx_vol[0] = voice_level_data[VOICE_SPEAKER][1];
	dev_info->max_voc_rx_vol[1] = voice_level_data[VOICE_SPEAKER][0];
	dev_info->max_voc_rx_vol[1] = voice_level_data[VOICE_SPEAKER][1];

	dev_info = audio_dev_ctrl_find_dev(DEVICE_ID_EXTERNAL_BT_SCO_VOICE_RX);
	dev_info->max_voc_rx_vol[0] = voice_level_data[VOICE_BT][0];
	dev_info->min_voc_rx_vol[0] = voice_level_data[VOICE_BT][1];
	dev_info->max_voc_rx_vol[1] = voice_level_data[VOICE_BT][0];
	dev_info->max_voc_rx_vol[1] = voice_level_data[VOICE_BT][1];

}
#endif 

static long audio_misc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct audio *audio = file->private_data;
	int rc = -EINVAL;
	int lb_path = 0;
	MM_INFO("ioctl cmd = %d\n", cmd);

	mutex_lock(&audio->lock);
	switch (cmd)
	{

        case SET_AUDIO_CAL:
#ifdef CONFIG_LGE_MODEL_E739
		  if (copy_from_user(&lb_path,(void *)arg,sizeof(int)))
			  return 0;
		  MM_INFO("ioctl arg = %d\n", lb_path);
		  if (lb_path <10)
		  {
			lmsg.args.mode = cpu_to_be32( lb_path );
			  lmsg.args.cb_func = CALLBACK_NULL;
			  lmsg.args.client_data = 0;


			  if (lb_path == 0)
				  bTTY_Headset = false;
			  else
				  bTTY_Headset = true;

			  if (rc < 0)
				  MM_ERR("failed to do loopback\n");
			  else
				  MM_INFO("TTY SET function called successfully\n");
		  }
		  else
		  {
			  set_QTRcal_data();
			  if (bReadCalData) {
					set_ampcal_data();
					set_voicecal_data();
	          }
		  }
#endif	  
          break;
    
	    case AUDIO_START_VOICE:
            if (copy_from_user(&lb_path,(void *)arg,sizeof(int)))
        		return 0;
        	MM_INFO("ioctl arg = %d\n", lb_path);
          
  	          lmsg.args.mode = cpu_to_be32( lb_path );
		    lmsg.args.cb_func = CALLBACK_NULL;
		    lmsg.args.client_data = 0;
		    rc = msm_rpc_call(audio->ept,RPC_VOEM_PACKET_LOOPBACK_PROC,&lmsg, sizeof(lmsg), 5 * HZ);
		    if (rc < 0)
			    MM_ERR("failed to do loopback\n");
		    else
			    MM_INFO("AUDIO_START_VOICE function called successfully\n");
        
			break;

	    case AUDIO_STOP_VOICE:		
            if (copy_from_user(&lb_path,(void *)arg,sizeof(int)))
        		return 0;
        	MM_INFO("ioctl arg = %d\n", lb_path);
          
		    lmsg.args.mode = cpu_to_be32( 0x00 );
		    lmsg.args.cb_func = CALLBACK_NULL;
		    lmsg.args.client_data = 0;
		    rc = msm_rpc_call(audio->ept,RPC_VOEM_PACKET_LOOPBACK_PROC,&lmsg, sizeof(lmsg), 5 * HZ);
		    if (rc < 0)
			    MM_ERR("failed to do loopback\n");
		    else
			    MM_INFO("AUDIO_STOP_VOICE function called successfully\n");
			break;

	    case SET_HAC_VC_MODE:
            if (copy_from_user(&lb_path,(void *)arg,sizeof(int)))
        		return 0;
        	MM_INFO("ioctl arg = %d\n", lb_path);
          
  	        lmsg.args.mode = cpu_to_be32( lb_path );
		    lmsg.args.cb_func = CALLBACK_NULL;
		    lmsg.args.client_data = 0;
		    rc = msm_rpc_call(audio->ept,RPC_VOEM_HAC_VC_PROC,&lmsg, sizeof(lmsg), 5 * HZ);
		    if (rc < 0)
			    MM_ERR("failed to do loopback\n");
		    else
			    MM_INFO("SET_HAC_VC_MODE function called successfully\n");
        
			break;
			
		default:
			MM_ERR("Not supported action \n");
			rc =0;
      }
	
	mutex_unlock(&audio->lock);
	return rc;
}
/*******************************************************************************
*  close function. 
*
********************************************************************************/
static int audio_misc_close(struct inode *inode, struct file *file)
{
	struct audio *audio = file->private_data;
	int rc;
	MM_DBG("audio instance 0x%08x freeing\n", (int)audio);
	mutex_lock(&audio->lock);
	rc = msm_rpc_close(audio->ept);
	if (rc < 0)
	    MM_ERR("msm_rpc_close failed\n");
	audio->ept = NULL;
	audio->opened = 0;
	mutex_unlock(&audio->lock);
	return 0;
}
/*******************************************************************************
*  close function. 
*
********************************************************************************/
static int audio_misc_open(struct inode *inode, struct file *file)
{
	struct audio *audio = &misc_audio;
	int rc = 0;
	MM_DBG("audio_misc_open 0x%08x\n", (int)audio);
	mutex_lock(&audio->lock);
	if (audio->opened)
		return -EPERM;
	if (audio->ept == NULL)
	{
		audio->ept = msm_rpc_connect_compatible(VOEM_IFPROG,VOEM_IFVERS, 0);
		if (IS_ERR(audio->ept))
		{
			rc = PTR_ERR(audio->ept);
			audio->ept = NULL;
			MM_ERR("failed to connect VOEM svc\n");
			goto err;
		}
	}
	audio->opened = 1;
	audio->path_set = -1;
	audio->prev_path_set = -1;
	audio->dsp_loopback_opened = -1;
	audio->packet_loopback_opened = -1;
	file->private_data = audio;
err:
	mutex_unlock(&audio->lock);
	return rc;	
}
/*******************************************************************************
*  Init part
*
********************************************************************************/

static const struct file_operations audio_misc_fops = {
	.owner		= THIS_MODULE,
	.open		= audio_misc_open,
	.release	    = audio_misc_close,
	.unlocked_ioctl	= audio_misc_ioctl,
};

struct miscdevice misc_audio_ctrl = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "msm_audio_misc",
	.fops	= &audio_misc_fops,
};

int __init audio_misc_init(void)
{
	struct audio *audio = &misc_audio;
	mutex_init(&audio->lock);
	return misc_register(&misc_audio_ctrl);
}

device_initcall(audio_misc_init);

MODULE_DESCRIPTION("MSM MISC driver");
MODULE_LICENSE("GPL v2");
