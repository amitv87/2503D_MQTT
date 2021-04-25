# Define source file lists to SRC_LIST
SRC_LIST = bcm\bcm_init.c \
           bcm\bcm_create.c \
           bcm\bcm_main.c \
           bcm\adp\src\bcmadp.c \
           bcm\adp\src\comm_post_event.c \
           bcm\adp\src\bcm_event_handler.c \
           bcm\cm\src\bcm_btcmSrvInterface.c \
           bcm\cm\src\bcm_btcmSrvMain.c \
           bcm\cm\src\bcm_btcmSrvPrmt.c \
           bcm\cm\src\bcm_btcmUtility.c \
           bcm\cm\src\bcm_btcmApp.c \
           bcm\cm\src\bcm_btcmCmd.c

ifneq ($(filter __BT_SPP_PROFILE__, $(strip $(MODULE_DEFS))),)
	SRC_LIST += bcm\profile\spp\src\BcmSppSrv.c \
				bcm\profile\spp\src\bcmSppUtility.c\
				bcm\profile\spp\src\bcm_spp_app.c
endif

ifneq ($(filter __BT_PBAP_CLIENT__, $(strip $(MODULE_DEFS))),)
	SRC_LIST += bcm\profile\pbap\bcm_pbapcApp.c \
				bcm\profile\pbap\bcm_pbapcCmd.c \
				bcm\profile\pbap\bcm_pbapcSrv.c 				
endif

ifneq ($(filter __BCM_PBAP_SERVER__, $(strip $(MODULE_DEFS))),)
	SRC_LIST += bcm\profile\pbap\bcm_phbSrv.c \
				bcm\profile\pbap\bcm_PbapSrv.c \
				bcm\profile\pbap\bcm_PhbPBAPSrv.c\
				bcm\profile\pbap\bcm_PhbCache.c\
				bcm\profile\pbap\bcm_GSM7bit.c\
				bcm\profile\pbap\bcm_PbapApp.c
endif

ifneq ($(filter __BT_OPP_PROFILE__, $(strip $(MODULE_DEFS))),)
	SRC_LIST += bcm\profile\opp\bcm_OppApp.c \
				bcm\profile\opp\bcm_OppCmd.c \
				bcm\profile\opp\bcm_OppSrv.c
endif

ifneq ($(filter __BT_HF_PROFILE__, $(strip $(MODULE_DEFS))),)
    SRC_LIST += bcm\profile\hfp\src\bcm_BthfSrv.c \
                bcm\profile\hfp\src\bcm_BtAudioSrv.c \
                bcm\profile\hfp\src\bcm_BTDialerSrv.c \
                bcm\profile\hfp\src\bcm_mdi_audio.c \
                bcm\profile\hfp\src\bcm_mdi_bitstream.c \
                bcm\profile\hfp\src\bcm_ProfilesSrvMain.c \
                bcm\profile\hfp\src\bcm_hfp_comm.c \
                bcm\profile\hfp\src\bcm_hfpApp.c \
                bcm\profile\hfp\src\bcm_BthScoPathSrv.c
else
	ifneq ($(filter __BT_HFG_PROFILE__, $(strip $(MODULE_DEFS))),)
	    SRC_LIST += bcm\profile\hfp\src\bcm_BthfSrv.c \
	                bcm\profile\hfp\src\bcm_BtAudioSrv.c \
	                bcm\profile\hfp\src\bcm_mdi_audio.c \
	                bcm\profile\hfp\src\bcm_mdi_bitstream.c \
	                bcm\profile\hfp\src\bcm_ProfilesSrvMain.c \
	                bcm\profile\hfp\src\bcm_hfp_comm.c \
	                bcm\profile\hfp\src\bcm_BthScoPathSrv.c
	endif
endif

ifneq ($(filter __BT_A2DP_PROFILE__, $(strip $(MODULE_DEFS))),)
    SRC_LIST += bcm\profile\a2dp\A2dpSrv\bcm_A2dpSrv.c
    SRC_LIST += bcm\profile\a2dp\A2dpSrv\bcm_A2dpSrvUtil.c
endif
ifneq ($(filter __BT_AVRCP_PROFILE__, $(strip $(MODULE_DEFS))),)
    SRC_LIST += bcm\profile\avrcp\AvrcpSrv\bcm_AvrcpSrv.c \
                bcm\profile\avrcp\AvrcpSrv\bcm_AvrcpSrvUtil.c
endif

# Define include path lists to INC_DIR
INC_DIR = bcm\adp\inc \
          bcm\cm\inc \
          interface\bcm \
          interface\bt \
          blueangel\btcore\btutils\inc \
          interface\hal\bluetooth \
          bcm

ifneq ($(filter __BT_SPP_PROFILE__, $(strip $(MODULE_DEFS))),)
	INC_DIR += bcm\profile\spp\inc
endif

ifneq ($(filter __BT_PBAP_CLIENT__, $(strip $(MODULE_DEFS))),)
	INC_DIR += bcm\profile\pbap\inc			
endif

ifneq ($(filter __BCM_PBAP_SERVER__, $(strip $(MODULE_DEFS))),)
	INC_DIR += bcm\profile\pbap\inc
endif

ifneq ($(filter __BT_OPP_PROFILE__, $(strip $(MODULE_DEFS))),)
	INC_DIR += bcm\profile\opp\inc
endif

ifneq ($(filter __BT_HF_PROFILE__, $(strip $(MODULE_DEFS))),)
	INC_DIR += bcm\profile\hfp\inc
	INC_DIR += custom\common
	
else
	ifneq ($(filter __BT_HFG_PROFILE__, $(strip $(MODULE_DEFS))),)
		INC_DIR += bcm\profile\hfp\inc
		INC_DIR += custom\common
	endif	
endif

#ifneq ($(filter __BT_A2DP_PROFILE__, $(strip $(MODULE_DEFS))),)
  INC_DIR += bcm\profile\a2dp\A2dpSrv
#endif

#ifneq ($(filter __BT_AVRCP_PROFILE__, $(strip $(MODULE_DEFS))),)
  INC_DIR += bcm\profile\avrcp\AvrcpSrv
#endif

# Define the specified compile options to COMP_DEFS
COMP_DEFS = 