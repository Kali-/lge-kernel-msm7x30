/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <asm/uaccess.h>
#include <mach/qdsp5v2/snddev_ecodec.h>
#include <mach/qdsp5v2/audio_dev_ctl.h>
#include <mach/qdsp5v2/audio_interct.h>
#include <mach/qdsp5v2/aux_pcm.h>
#include <mach/qdsp5v2/afe.h>
#include <mach/debug_mm.h>
#include <linux/slab.h>

/* Context for each external codec device */
struct snddev_ecodec_state {
	struct snddev_ecodec_data *data;
	u32 sample_rate;
	bool enabled;
};

/* Global state for the driver */
struct snddev_ecodec_drv_state {
	struct mutex dev_lock;
	u32 rx_active; /* ensure one rx device at a time */
	u32 tx_active; /* ensure one tx device at a time */
	struct clk *lpa_core_clk;
	struct clk *ecodec_clk;
};

#define ADSP_CTL 1

static struct snddev_ecodec_drv_state snddev_ecodec_drv;

#if !defined(CONFIG_LGE_MODEL_E739)
#ifdef CONFIG_LGE_AUDIO_LOOPBACK
	/* LGE_CHANGE
	  * added for BT loopback
	  * 2010-03-22, bob.cho@lge.com
	  */
#define TESTMODE_HANDSET_PCM_CTL_VAL (PCM_CTL__RPCM_WIDTH__LINEAR_V | PCM_CTL__TPCM_WIDTH__LINEAR_V)
#define TESTMODE_AUX_CODEC_INTF   AUX_CODEC_INTF_CTL__PCMINTF_DATA_EN_V
#define TESTMODE_DATA_FORMAT_PADDING (DATA_FORMAT_PADDING_INFO__RPCM_FORMAT_V | DATA_FORMAT_PADDING_INFO__TPCM_FORMAT_V)

static int lb_status = 0;

static void bt_sco_set_loopback_mode(u32 loop)
{
	int trc;
	struct msm_afe_config afe_config;
	struct snddev_ecodec_drv_state *drv = &snddev_ecodec_drv;

	MM_INFO("[BTUI] bt_sco_set_loopback_mode : %d \n", loop);

	if(loop & 1)
	{
		/**********************************************************
		* Aux PCM Common set up
		***********************************************************/
		/* request GPIO */
		trc = aux_pcm_gpios_request();
		if (trc)
		{
			MM_ERR("[BTUI] ###### GPIO enable failed\n");
		}
		/* config clocks */
		clk_enable(drv->lpa_core_clk);
		/* enable ecodec clk */
		clk_enable(drv->ecodec_clk);
		/* let ADSP confiure AUX PCM regs */
		aux_codec_adsp_codec_ctl_en(ADSP_CTL);
		/* let adsp configure pcm path */
		aux_codec_pcm_path_ctl_en(ADSP_CTL);
		/* choose ADSP_A */
		audio_interct_aux_regsel(AUDIO_ADSP_A);
		audio_interct_tpcm_source(AUDIO_ADSP_A);
		audio_interct_rpcm_source(AUDIO_ADSP_A);
		clk_disable(drv->lpa_core_clk);
		/* send AUX_CODEC_CONFIG to AFE */
		trc = afe_config_aux_codec(TESTMODE_HANDSET_PCM_CTL_VAL,TESTMODE_AUX_CODEC_INTF,TESTMODE_DATA_FORMAT_PADDING);
		if (IS_ERR_VALUE(trc))
		{
			MM_ERR("[BTUI] ###### afe_config_aux_codec failed\n");
		}
		/**********************************************************
		* Aux PCM Rx path set up
		***********************************************************/
		/* send CODEC CONFIG to AFE */
		afe_config.sample_rate = 8;
		afe_config.channel_mode = 1;
		afe_config.volume = AFE_VOLUME_UNITY;
		trc = afe_enable(AFE_HW_PATH_AUXPCM_RX, &afe_config);
		if (IS_ERR_VALUE(trc)){
			MM_ERR("[BTUI] ###### afe_enable failed\n");
		}
		/**********************************************************
		* Aux PCM Tx path set up
		***********************************************************/
		/* send CODEC CONFIG to AFE */
		afe_config.sample_rate = 8;
		afe_config.channel_mode = 1;
		afe_config.volume = AFE_VOLUME_UNITY;
		trc = afe_enable(AFE_HW_PATH_AUXPCM_TX, &afe_config);
		if (IS_ERR_VALUE(trc)) {
			MM_ERR("[BTUI] ###### afe_enable tx failed\n");
		}
		/************************************************************************************
		*	Start LoopBack 
		*************************************************************************************/	 
		/* Set the volume level to non unity, to avoid
		loopback effect */
		afe_device_volume_ctrl(AFE_HW_PATH_AUXPCM_RX, 0x0500);
		lb_status = loop;
		MM_INFO("[BTUI] bt_sco_set_loopback_mode : [ENABLED] \n");
		afe_config_ext_pcm_loopback(0xFFFF);
	}
	else	{
		/*release GPIOs*/
		aux_pcm_gpios_free();
		afe_config_ext_pcm_loopback(0x0);
		
		/*disable clocks*/
		clk_disable(drv->ecodec_clk);
		/*Disable afe configuration */
		afe_disable(AFE_HW_PATH_AUXPCM_RX);
		afe_disable(AFE_HW_PATH_AUXPCM_TX);
		lb_status = 0;
		MM_INFO("[BTUI] bt_sco_set_loopback_mode : [DISABLED] \n");
	}
}


static ssize_t store_btlb_value(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int lb = 0;
	sscanf(buf, "%d", &lb);
	MM_INFO("[BTUI] store_btlb_value : old(%d) new(%d) \n", lb_status,lb);

	/* skip when same status */
	if (lb == lb_status)
		return count;

	/* set loopback */
	bt_sco_set_loopback_mode(lb);
	return count;
}

static ssize_t show_btlb_value(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", lb_status);
}
static DEVICE_ATTR(btlb, 0664, show_btlb_value, store_btlb_value);

#endif /*CONFIG_LGE_AUDIO_LOOPBACK*/
#endif

static int snddev_ecodec_open_rx(struct snddev_ecodec_state *ecodec)
{
	int rc = 0;
	struct snddev_ecodec_drv_state *drv = &snddev_ecodec_drv;
	struct msm_afe_config afe_config;

	MM_DBG("snddev_ecodec_open_rx\n");

	if (!drv->tx_active) {
		/* request GPIO */
		rc = aux_pcm_gpios_request();
		if (rc) {
			MM_ERR("GPIO enable failed\n");
			goto done;
		}
		/* config clocks */
		clk_enable(drv->lpa_core_clk);

		/* enable ecodec clk */
		clk_enable(drv->ecodec_clk);

		/* let ADSP confiure AUX PCM regs */
		aux_codec_adsp_codec_ctl_en(ADSP_CTL);

		/* let adsp configure pcm path */
		aux_codec_pcm_path_ctl_en(ADSP_CTL);

		/* choose ADSP_A */
		audio_interct_aux_regsel(AUDIO_ADSP_A);
		audio_interct_tpcm_source(AUDIO_ADSP_A);
		audio_interct_rpcm_source(AUDIO_ADSP_A);

		clk_disable(drv->lpa_core_clk);

		/* send AUX_CODEC_CONFIG to AFE */
		rc = afe_config_aux_codec(ecodec->data->conf_pcm_ctl_val,
				ecodec->data->conf_aux_codec_intf,
				ecodec->data->conf_data_format_padding_val);
		if (IS_ERR_VALUE(rc))
			goto error;
	}
	/* send CODEC CONFIG to AFE */
	afe_config.sample_rate = ecodec->sample_rate / 1000;
	afe_config.channel_mode = ecodec->data->channel_mode;
	afe_config.volume = AFE_VOLUME_UNITY;
	rc = afe_enable(AFE_HW_PATH_AUXPCM_RX, &afe_config);
	if (IS_ERR_VALUE(rc)) {
		if (!drv->tx_active) {
			aux_pcm_gpios_free();
			clk_disable(drv->ecodec_clk);
		}
		goto done;
	}

	ecodec->enabled = 1;
	return 0;

error:
	aux_pcm_gpios_free();
	clk_disable(drv->ecodec_clk);
done:
	return rc;
}

static int snddev_ecodec_close_rx(struct snddev_ecodec_state *ecodec)
{
	struct snddev_ecodec_drv_state *drv = &snddev_ecodec_drv;

	/* free GPIO */
	if (!drv->tx_active) {
		aux_pcm_gpios_free();
		clk_disable(drv->ecodec_clk);
	}

	/* disable AFE */
	afe_disable(AFE_HW_PATH_AUXPCM_RX);

	ecodec->enabled = 0;

	return 0;
}

static int snddev_ecodec_open_tx(struct snddev_ecodec_state *ecodec)
{
	int rc = 0;
	struct snddev_ecodec_drv_state *drv = &snddev_ecodec_drv;
	struct msm_afe_config afe_config;

	MM_DBG("snddev_ecodec_open_tx\n");

	/* request GPIO */
	if (!drv->rx_active) {
		rc = aux_pcm_gpios_request();
		if (rc) {
			MM_ERR("GPIO enable failed\n");
			goto done;
		}
		/* config clocks */
		clk_enable(drv->lpa_core_clk);

		/* enable ecodec clk */
		clk_enable(drv->ecodec_clk);

		/* let ADSP confiure AUX PCM regs */
		aux_codec_adsp_codec_ctl_en(ADSP_CTL);

		/* let adsp configure pcm path */
		aux_codec_pcm_path_ctl_en(ADSP_CTL);

		/* choose ADSP_A */
		audio_interct_aux_regsel(AUDIO_ADSP_A);
		audio_interct_tpcm_source(AUDIO_ADSP_A);
		audio_interct_rpcm_source(AUDIO_ADSP_A);

		clk_disable(drv->lpa_core_clk);

		/* send AUX_CODEC_CONFIG to AFE */
		rc = afe_config_aux_codec(ecodec->data->conf_pcm_ctl_val,
			ecodec->data->conf_aux_codec_intf,
			ecodec->data->conf_data_format_padding_val);
		if (IS_ERR_VALUE(rc))
			goto error;
	}
	/* send CODEC CONFIG to AFE */
	afe_config.sample_rate = ecodec->sample_rate / 1000;
	afe_config.channel_mode = ecodec->data->channel_mode;
	afe_config.volume = AFE_VOLUME_UNITY;
	rc = afe_enable(AFE_HW_PATH_AUXPCM_TX, &afe_config);
	if (IS_ERR_VALUE(rc)) {
		if (!drv->rx_active) {
			aux_pcm_gpios_free();
			clk_disable(drv->ecodec_clk);
		}
		goto done;
	}

	ecodec->enabled = 1;
	return 0;

error:
	clk_disable(drv->ecodec_clk);
	aux_pcm_gpios_free();
done:
	return rc;
}

static int snddev_ecodec_close_tx(struct snddev_ecodec_state *ecodec)
{
	struct snddev_ecodec_drv_state *drv = &snddev_ecodec_drv;

	/* free GPIO */
	if (!drv->rx_active) {
		aux_pcm_gpios_free();
		clk_disable(drv->ecodec_clk);
	}

	/* disable AFE */
	afe_disable(AFE_HW_PATH_AUXPCM_TX);

	ecodec->enabled = 0;

	return 0;
}


static int snddev_ecodec_open(struct msm_snddev_info *dev_info)
{
	int rc = 0;
	struct snddev_ecodec_state *ecodec;
	struct snddev_ecodec_drv_state *drv = &snddev_ecodec_drv;

	if (!dev_info) {
		rc = -EINVAL;
		goto error;
	}

	ecodec = dev_info->private_data;

	if (ecodec->data->capability & SNDDEV_CAP_RX) {
		mutex_lock(&drv->dev_lock);
		if (drv->rx_active) {
			mutex_unlock(&drv->dev_lock);
			rc = -EBUSY;
			goto error;
		}
		rc = snddev_ecodec_open_rx(ecodec);
		if (!IS_ERR_VALUE(rc))
			drv->rx_active = 1;
		mutex_unlock(&drv->dev_lock);
	} else {
		mutex_lock(&drv->dev_lock);
		if (drv->tx_active) {
			mutex_unlock(&drv->dev_lock);
			rc = -EBUSY;
			goto error;
		}
		rc = snddev_ecodec_open_tx(ecodec);
		if (!IS_ERR_VALUE(rc))
			drv->tx_active = 1;
		mutex_unlock(&drv->dev_lock);
	}
error:
	return rc;
}

static int snddev_ecodec_close(struct msm_snddev_info *dev_info)
{
	int rc = 0;
	struct snddev_ecodec_state *ecodec;
	struct snddev_ecodec_drv_state *drv = &snddev_ecodec_drv;
	if (!dev_info) {
		rc = -EINVAL;
		goto error;
	}

	ecodec = dev_info->private_data;

	if (ecodec->data->capability & SNDDEV_CAP_RX) {
		mutex_lock(&drv->dev_lock);
		if (!drv->rx_active) {
			mutex_unlock(&drv->dev_lock);
			rc = -EPERM;
			goto error;
		}
		rc = snddev_ecodec_close_rx(ecodec);
		if (!IS_ERR_VALUE(rc))
			drv->rx_active = 0;
		mutex_unlock(&drv->dev_lock);
	} else {
		mutex_lock(&drv->dev_lock);
		if (!drv->tx_active) {
			mutex_unlock(&drv->dev_lock);
			rc = -EPERM;
			goto error;
		}
		rc = snddev_ecodec_close_tx(ecodec);
		if (!IS_ERR_VALUE(rc))
			drv->tx_active = 0;
		mutex_unlock(&drv->dev_lock);
	}

error:
	return rc;
}

static int snddev_ecodec_set_freq(struct msm_snddev_info *dev_info, u32 rate)
{
	int rc = 0;

	if (!dev_info) {
		rc = -EINVAL;
		goto error;
	}
	return 8000;

error:
	return rc;
}

static int snddev_ecodec_probe(struct platform_device *pdev)
{
	int rc = 0, i;
	struct snddev_ecodec_data *pdata;
	struct msm_snddev_info *dev_info;
	struct snddev_ecodec_state *ecodec;

	if (!pdev || !pdev->dev.platform_data) {
		printk(KERN_ALERT "Invalid caller \n");
		rc = -1;
		goto error;
	}
	pdata = pdev->dev.platform_data;

	ecodec = kzalloc(sizeof(struct snddev_ecodec_state), GFP_KERNEL);
	if (!ecodec) {
		rc = -ENOMEM;
		goto error;
	}

	dev_info = kzalloc(sizeof(struct msm_snddev_info), GFP_KERNEL);
	if (!dev_info) {
		kfree(ecodec);
		rc = -ENOMEM;
		goto error;
	}

	dev_info->name = pdata->name;
	dev_info->copp_id = pdata->copp_id;
	dev_info->acdb_id = pdata->acdb_id;
	dev_info->private_data = (void *) ecodec;
	dev_info->dev_ops.open = snddev_ecodec_open;
	dev_info->dev_ops.close = snddev_ecodec_close;
	dev_info->dev_ops.set_freq = snddev_ecodec_set_freq;
	dev_info->dev_ops.enable_sidetone = NULL;
	dev_info->capability = pdata->capability;
	dev_info->opened = 0;

	msm_snddev_register(dev_info);
	ecodec->data = pdata;
	ecodec->sample_rate = 8000; /* Default to 8KHz */
	 if (pdata->capability & SNDDEV_CAP_RX) {
		for (i = 0; i < VOC_RX_VOL_ARRAY_NUM; i++) {
			dev_info->max_voc_rx_vol[i] =
				pdata->max_voice_rx_vol[i];
			dev_info->min_voc_rx_vol[i] =
				pdata->min_voice_rx_vol[i];
		}
	}

#if !defined(CONFIG_LGE_MODEL_E739)
#ifdef CONFIG_LGE_AUDIO_LOOPBACK
	/* LGE_CHANGE
	  * added for BT loopback
	  * 2010-03-22, bob.cho@lge.com
	  */
	/* Register sysfs hooks */
	rc = device_create_file(&pdev->dev, &dev_attr_btlb);
	if (rc) {
		printk(KERN_ALERT "sysfs register failed for loopback attr\n");
		goto error;
	}
#endif /*CONFIG_LGE_AUDIO_LOOPBACK*/
#endif

error:
	return rc;
}

static int snddev_ecodec_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver snddev_ecodec_driver = {
	.probe = snddev_ecodec_probe,
	.remove = snddev_ecodec_remove,
	.driver = { .name = "msm_snddev_ecodec" }
};

static int __init snddev_ecodec_init(void)
{
	int rc = 0;
	struct snddev_ecodec_drv_state *ecodec_drv = &snddev_ecodec_drv;

	MM_INFO("snddev_ecodec_init\n");
	rc = platform_driver_register(&snddev_ecodec_driver);
	if (IS_ERR_VALUE(rc))
		goto error_platform_driver;
	ecodec_drv->ecodec_clk = clk_get(NULL, "ecodec_clk");
	if (IS_ERR(ecodec_drv->ecodec_clk))
		goto error_ecodec_clk;
	ecodec_drv->lpa_core_clk = clk_get(NULL, "lpa_core_clk");
	if (IS_ERR(ecodec_drv->lpa_core_clk))
		goto error_lpa_core_clk;


	mutex_init(&ecodec_drv->dev_lock);
	ecodec_drv->rx_active = 0;
	ecodec_drv->tx_active = 0;
	return 0;

error_lpa_core_clk:
	clk_put(ecodec_drv->ecodec_clk);
error_ecodec_clk:
	platform_driver_unregister(&snddev_ecodec_driver);
error_platform_driver:

	MM_ERR("encounter error\n");
	return -ENODEV;
}

static void __exit snddev_ecodec_exit(void)
{
	struct snddev_ecodec_drv_state *ecodec_drv = &snddev_ecodec_drv;

	platform_driver_unregister(&snddev_ecodec_driver);
	clk_put(ecodec_drv->ecodec_clk);

	return;
}

module_init(snddev_ecodec_init);
module_exit(snddev_ecodec_exit);

MODULE_DESCRIPTION("ECodec Sound Device driver");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL v2");
