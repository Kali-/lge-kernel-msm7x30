EXTRA_CFLAGS += -Iarch/arm/mach-msm

# Loopback		taeho.youn@lge.com
obj-$(CONFIG_MACH_MSM8X55_VICTOR) += lge_audio_misc_ctrl.o
obj-$(CONFIG_MACH_MSM8X55_VICTOR) += board-victor.o
obj-$(CONFIG_MACH_MSM8X55_VICTOR) += board-victor-pm.o
obj-$(CONFIG_MACH_MSM8X55_VICTOR) += board-victor-sound.o
obj-$(CONFIG_MACH_MSM8X55_VICTOR) += board-victor-misc.o
obj-$(CONFIG_MACH_MSM8X55_VICTOR) += board-victor-mmc.o
obj-$(CONFIG_MACH_MSM8X55_VICTOR) += board-victor-input.o
obj-$(CONFIG_MACH_MSM8X55_VICTOR) += board-victor-panel.o
obj-$(CONFIG_MACH_MSM8X55_VICTOR) += board-victor-bt.o
obj-$(CONFIG_MACH_MSM8X55_VICTOR) += board-victor-camera.o
obj-$(CONFIG_LGE_BROADCAST) += board-victor-dmb.o
obj-$(CONFIG_MACH_LGE) += devices_lge.o

obj-$(CONFIG_LGE_HEADSET_MAX14579) += lge_headset_MAX14579.o
obj-$(CONFIG_LGE_PROC_COMM) += lge_proc_comm.o
obj-$(CONFIG_LGE_PROC_COMM) += lge_ftm.o

obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats.o
obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats_cmd.o
obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats_eta.o
obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats_mmc.o
obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats_event_log.o
obj-$(CONFIG_LGE_SUPPORT_AT_CMD) += lge_ats_flex.o # LGE_CHANGE [irene.park@lge.com] 2010-06-04 - to get flex value from ARM9 

obj-$(CONFIG_LGE_HANDLE_PANIC) += lge_handle_panic.o
obj-$(CONFIG_LGE_ATCMD_TTY) += lge_tty_atcmd.o
# START [sangki.hyun@lge.com][dom_testmode] 20100615 LAB1_FW LGE_TEST_MODE {
obj-$(CONFIG_MSM_RPCSERVER_TESTMODE) += rpc_server_testmode.o
obj-$(CONFIG_MSM_RPCSERVER_TESTMODE) += rpc_server_testmode_cam.o
obj-$(CONFIG_MSM_RPCSERVER_TESTMODE) += rpc_server_testmode_memory.o
obj-$(CONFIG_MSM_RPCSERVER_TESTMODE) += rpc_server_testmode_sound.o
obj-$(CONFIG_MSM_RPCSERVER_TESTMODE) += rpc_server_testmode_motor.o
obj-$(CONFIG_MSM_RPCSERVER_TESTMODE) += rpc_server_testmode_keytest.o
obj-$(CONFIG_MSM_RPCSERVER_TESTMODE) += rpc_server_testmode_sleep.o
obj-$(CONFIG_MSM_RPCSERVER_TESTMODE) += testmode_sensor.o
obj-$(CONFIG_MSM_RPCSERVER_TESTMODE) += testmode_input.o
# END [sangki.hyun@lge.com][dom_testmode] 20100615 LAB1_FW }
