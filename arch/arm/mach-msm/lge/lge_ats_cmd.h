#ifndef _LGE_ATS_CMD_H_
#define _LGE_ATS_CMD_H_

#ifdef CONFIG_LGE_BROADCAST	
struct broadcast_mtv_sig_info
{
	unsigned int	dab_ok;
	unsigned int	msc_ber;
	unsigned int	sync_lock;
	unsigned int	afc_ok;
	unsigned int	cir;
	unsigned int	fic_ber;
	unsigned int	tp_lock;
	unsigned int	sch_ber;
	unsigned int	tp_err_cnt;
	unsigned int	va_ber;
	unsigned char	srv_state_flag;
};

extern int8_t broadcast_tdmb_blt_power_on(void);
extern int8_t broadcast_tdmb_blt_power_off(void);
extern int8_t broadcast_tdmb_blt_open(void);
extern int8_t broadcast_tdmb_blt_close(void);
extern int8_t broadcast_tdmb_blt_tune_set_ch(int32_t freq_num);
extern int8_t broadcast_tdmb_blt_get_sig_info(void* sig_info);
#endif
extern int external_memory_test(void);

#endif /*_LGE_ATS_CMD_H_*/
