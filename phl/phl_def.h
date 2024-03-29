/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef _PHL_DEF_H_
#define _PHL_DEF_H_

/*HW_BAND0 - CMAC0 + PHY0 + S0*/
/*HW_BAND1 - CMAC1 + PHY1 + S1*/
/*wifi_role->hw_band*/
enum phl_band_idx {
	HW_BAND_0,
	HW_BAND_1,
	HW_BAND_MAX
};

/*wifi_role->hw_port*/
enum phl_hw_port {
	HW_PORT0,
	HW_PORT1,
	HW_PORT2,
	HW_PORT3,
	HW_PORT4,
	HW_PORT_MAX,
};

#define RTW_MAX_TID_NUM 8

/**
 * struct rtw_chan_def - channel defination
 * @chan: the (control/primary) channel
 * @center_ch: the center channel
 * @bw: channel bandwidth
 * @center_freq1: center frequency of first segment
 * @center_freq2: center frequency of second segment
 *	(only with 80+80 MHz)
 */

struct rtw_chan_def {
	enum band_type band; /* protocol -2.4G,5G,6G*/
	u8 chan; /*primary channel*/
	u8 center_ch;
	enum channel_width bw;
	enum chan_offset offset;
	u32 center_freq1;
	u32 center_freq2;
	u16 hw_value;
};

/**
 * struct rtw_chan_ctx - channel context
 * @list:
 * @chan_ctx_lock:
 * @chan_def:
 */
struct rtw_chan_ctx {
	_os_list list;
	struct rtw_chan_def chan_def;
	u8 role_map; /*used role_idx*/
	bool dfs_enabled;
};


#ifdef CONFIG_PCI_HCI
struct rtw_pci_info {
	u8 dummy;
};
#endif


#ifdef CONFIG_USB_HCI
struct rtw_usb_info {
	enum rtw_usb_speed usb_speed; /* USB 1.1, 2.0 or 3.0 */
	u16 usb_bulkout_size;
	u8 outep_num;
	u8 inep_num;
};

enum phl_usb_rx_agg_mode {
	PHL_RX_AGG_DISABLE,
	PHL_RX_AGG_DEFAULT,
	PHL_RX_AGG_SMALL_PKT,
	PHL_RX_AGG_USER_DEFINE,
};
/*
 * refers to _usb.h
 * #define SWITCHMODE           0x2
 * #define FORCEUSB3MODE        0x1
 * #define FORCEUSB2MODE        0x0
*/
enum rtw_usb_sw_ability {
	RTW_USB2_ONLY = 0,
	RTW_USB3_ONLY,
	RTW_USB_SUPPORT_SWITCH,
	RTW_USB_SUPPORT_MAX
};
#endif

#ifdef CONFIG_SDIO_HCI
struct rtw_sdio_info {
	unsigned int clock;
	unsigned int timing;
	u8 sd3_bus_mode;
	u16 block_sz;
	u16 io_align_sz;
	u16 tx_align_sz;
};
#endif

struct rtw_ic_info {
	enum rtl_ic_id ic_id;
	enum rtw_hci_type hci_type;
	#ifdef CONFIG_SDIO_HCI
	struct rtw_sdio_info sdio_info;
	#endif

	#ifdef CONFIG_USB_HCI
	struct rtw_usb_info usb_info;
	#endif

	#ifdef CONFIG_PCI_HCI
	struct rtw_pci_info pci_info;
	#endif
};

enum rtw_proc_cmd_type {
	RTW_PROC_CMD_UNKNOW,
	RTW_PROC_CMD_BB,	/* 1 */
	RTW_PROC_CMD_RF,	/* 2 */
	RTW_PROC_CMD_MAC,	/* 3 */
	RTW_PROC_CMD_PHL,	/* 4 */
	RTW_PROC_CMD_CORE,	/* 5 */
	RTW_PROC_CMD_BTC,	/* 6 */
	RTW_PROC_CMD_MAX
};

enum rtw_arg_type {
	RTW_ARG_TYPE_UNKNOW,
	RTW_ARG_TYPE_BUF,	/* 1 */
	RTW_ARG_TYPE_ARRAY,	/* 2 */
	RTW_ARG_TYPE_MAX
};

#define		MAX_ARGC	20
#define		MAX_ARGV	16


struct rtw_proc_cmd {
	enum rtw_arg_type in_type;
	u32 in_cnt_len;
	union {
		char *buf;
		char vector[MAX_ARGC][MAX_ARGV];
	}in;
};

enum rtw_para_src {
	RTW_PARA_SRC_INTNAL, /* 0 */
	RTW_PARA_SRC_EXTNAL, /* 1 */
	RTW_PARA_SRC_MAX
};

struct rtw_para_info_t {
	enum rtw_para_src para_src;
	char para_path[256];

	u32 *para_data;
	u32 para_data_len;
};

#define regd_name_max_size 32

struct rtw_para_pwrlmt_info_t {
	enum rtw_para_src para_src;
	char para_path[256];

	u32 *para_data;
	u32 para_data_len;

	char ext_regd_name[regd_name_max_size][10];
	u16 ext_regd_arridx;
	u8 *ext_reg_codemap;
	u16 ext_reg_map_num;
};

#define RTW_PHL_HANDLER_STATUS_INITIALIZED BIT0
#define RTW_PHL_HANDLER_STATUS_SET BIT1
#define RTW_PHL_HANDLER_STATUS_RELEASED BIT2

#define RTW_PHL_HANDLER_PRIO_HIGH 0
#define RTW_PHL_HANDLER_PRIO_NORMAL 1
#define RTW_PHL_HANDLER_PRIO_LOW 2

enum rtw_phl_evt {
	RTW_PHL_EVT_RX = BIT0,
	RTW_PHL_EVT_TX_RECYCLE = BIT1,

	RTW_PHL_EVT_MAX = BIT31
};

enum rtw_phl_config_int {
	RTW_PHL_STOP_RX_INT,
	RTW_PHL_RESUME_RX_INT,
	RTW_PHL_SER_HANDSHAKE_MODE,
	RTW_PHL_EN_HCI_INT,
	RTW_PHL_DIS_HCI_INT,
	RTW_PHL_CONFIG_INT_MAX
};

/**
 * phl_handler - scheduled by core layer or phl itself
 * and the properties is assigned by different hanlder type
 * @status: handler current status defined by RTW_PHL_HANDLER_STATUS_XXX
 * @type: define different properties of handler - tasklet, thread, workitem
 * @handle: store different type of handler structure
 * @callback: handler callback function
 * @context: context used in handler callback function
 */
struct rtw_phl_handler {
	char status;
	char type;
	void *drv_priv;
	struct _os_handler os_handler;
	void (*callback)(void *context);
	void *context;
};

struct rtw_xmit_req;
struct rtw_aoac_report;
struct rtw_phl_evt_ops {
	enum rtw_phl_status (*rx_process)(void *drv_priv);
	enum rtw_phl_status (*tx_recycle)(void *drv_priv, struct rtw_xmit_req *txreq);
	enum rtw_phl_status (*tx_test_recycle)(void *phl, struct rtw_xmit_req *txreq);
	void (*set_radio_state)(void *drv_priv, enum rtw_rf_state state_to_set);
	void (*wow_handle_sec_info_update)(void *drv_priv, struct rtw_aoac_report *aoac_info, u8 aoac_report_get_ok, u8 phase);
#ifdef CONFIG_WIN_HANDLE_INTERRUPT
	void (*interrupt_restore)(void *drv_priv, u8 rx);
#endif
	void (*indicate_wake_rsn)(void *drv_priv, u8 rsn);
};

enum rtw_cmd_type {
	CMD_TYPE_SCAN, /* 0 */
	CMD_TYPE_SNDING, /* 1 */
	CMD_TYPE_CSI, /* 2 */
	CMD_TYPE_MAX
};

enum role_type {
	PHL_RTYPE_NONE,
	PHL_RTYPE_STATION,
	PHL_RTYPE_AP,
	PHL_RTYPE_VAP,
	PHL_RTYPE_ADHOC,
	PHL_RTYPE_ADHOC_MASTER,
	PHL_RTYPE_MESH,
	PHL_RTYPE_MONITOR,
	PHL_RTYPE_P2P_DEVICE,
	PHL_RTYPE_P2P_GC,
	PHL_RTYPE_P2P_GO,
	PHL_RTYPE_TDLS,
	PHL_RTYPE_NAN,
	PHL_MLME_MAX
};

enum role_state {
	PHL_ROLE_START, /* 0 - PHL*/
	PHL_ROLE_STOP, /* 1 - PHL*/
	PHL_ROLE_CHG_TYPE, /* 2 - PHL*/
	PHL_ROLE_MSTS_STA_CONN_START, /*CORE*/
	PHL_ROLE_MSTS_STA_CONN_END,/*CORE*/
	PHL_ROLE_MSTS_STA_DIS_CONN,/*CORE*/
	PHL_ROLE_MSTS_AP_START,/*CORE*/
	PHL_ROLE_MSTS_AP_STOP,/*CORE*/
	PHL_ROLE_STATE_UNKNOWN,
};

enum mlme_state {
	MLME_NO_LINK,
	MLME_LINKING,
	MLME_LINKED
};
enum wr_chg_id {
	WR_CHG_TYPE,
	WR_CHG_MSTATE,
	WR_CHG_MADDR,
	WR_CHG_AP_PARAM,
	WR_CHG_EDCA_PARAM,
	WR_CHG_MU_EDCA_PARAM,
	WR_CHG_MU_EDCA_CFG,
	WR_CHG_BSS_COLOR,
	WR_CHG_MAX
};

enum wr_status{
	WR_STATUS_PS_ANN = BIT0,
	WR_STATUS_BCN_STOP = BIT1,
	WR_STATUS_MAX = BIT7
};

enum rtw_cfg_type { /* sync with pcfg_type */
	CFG_TBTT_AGG,
	CFG_TBTT_SHIFT,
	CFG_HIQ_WIN,
	CFG_HIQ_DTIM,
	CFG_HIQ_MAX,
	CFG_BCN_INTERVAL,	/* Beacon Interval */
	CFG_BSS_CLR
};

struct rtw_ap_param {
	u32 cfg_id;
	u32 value;
};

struct rtw_edca_param {
	/* Access Category, 0:BE, 1:BK, 2:VI, 3:VO */
	u8 ac;
	/*
	 * EDCA parameter
	 * |31...16|15...12|11...8|7...0|
	 * |   TXOP|  CWMAX| CWMIN| AIFS|
	 */
	u32 param;
};

struct rtw_mu_edca_param {
	u8 ac;
	u8 aifsn;
	u8 cw;
	u8 timer;
};

enum phl_module_id{
	/* 0 ~ 128 PHL background module starts from here*/
	PHL_BK_MDL_START = 0,
	PHL_MDL_PHY_MGNT = 1,
	PHL_MDL_TX = 2,
	PHL_MDL_RX = 3,
	/* above enum is fixed, add new module id from here*/
	/* 10 ~ 40 wifi role section*/
	PHL_BK_MDL_ROLE_START = 10,
	PHL_MDL_MRC = 10, /* Multi-Role Controller intead of STA/P2P role /NAN/AP*/
	PHL_BK_MDL_ROLE_END = 40,
	/* 41 ~ 70 mandatory background module section*/
	PHL_BK_MDL_MDRY_START = 41,
	PHL_MDL_POWER_MGNT = 41,
	PHL_MDL_SER = 42,
	PHL_MDL_GENERAL = 43,

	PHL_BK_MDL_MDRY_END = 70,
	/* 70 ~ 127 optional background module section*/
	PHL_BK_MDL_OPT_START = 71,
	PHL_MDL_BTC = 71,
	PHL_MDL_MCC = 72,
	PHL_MDL_CUSTOM = 73,
	PHL_MDL_SOUND = 74,
	PHL_MDL_WOW = 75,
	PHL_MDL_PSTS = 76,
	PHL_MDL_LED = 77,

	PHL_BK_MDL_OPT_END = 127,
	/* Fixed Max Value*/
	PHL_BK_MDL_END = 128,

	/* 129 ~ 256 PHL foreground module starts from here*/
	PHL_FG_MDL_START = 129,
	PHL_FUNC_MDL_TEST_MODULE = 129,
	PHL_FG_MDL_SCAN = 130,
	PHL_FG_MDL_CONNECT = 131,
	PHL_FG_MDL_DISCONNECT = 132,
	PHL_FG_MDL_AP_START = 133,
	PHL_FG_MDL_AP_STOP = 134,
	PHL_FG_MDL_END = 254,
	/* Fixed Max Value*/
	PHL_MDL_ID_MAX = 255
};

/* phl event which used to notify modules in PHL or CORE Layer
 * should define event ID here and share this common enum definition
 * private event that is handled only inside a particular module,
 * can still be added here, but not necessary.
 */
enum phl_msg_evt_id {
	MSG_EVT_NONE = 0,
	MSG_EVT_PHY_ON = 1,
	MSG_EVT_PHY_IDLE = 2,
	MSG_EVT_SCAN_START = 3,
	MSG_EVT_SCAN_END = 4,
	MSG_EVT_CONNECT_START = 5,
	MSG_EVT_CONNECT_END = 6,
	MSG_EVT_SER_L1 = 7,
	MSG_EVT_SER_L2 = 8,
	MSG_EVT_FWDL_OK = 9,
	MSG_EVT_FWDL_FAIL = 10,
	MSG_EVT_HAL_INIT_OK = 11,
	MSG_EVT_HAL_INIT_FAIL = 12,
	MSG_EVT_MP_CMD_DONE = 13,
	/* wow */
	MSG_EVT_WOW_ENTER = 14,
	MSG_EVT_WOW_LEAVE = 15,
	MSG_EVT_WOW_WAKE_RSN = 16,
	MSG_EVT_BCN_RESEND = 17,
	MSG_EVT_FW_WATCHDOG_TIMEOUT = 18,
	MSG_EVT_MP_RX_PHYSTS = 19,
	MSG_EVT_ROLE_NTFY = 20,
	MSG_EVT_RX_PSTS = 21,
	MSG_EVT_SWCH_START = 22,
	MSG_EVT_SWCH_DONE = 23,
	MSG_EVT_DISCONNECT = 24,
	MSG_EVT_TSF_SYNC_DONE = 25,
	MSG_EVT_TX_RESUME = 26,
	MSG_EVT_AP_START = 27,
	MSG_EVT_AP_START_END = 28,
	MSG_EVT_AP_STOP = 29,
	MSG_EVT_BATTERY_CHG = 30,
	MSG_EVT_PCIE_TRX_MIT = 31,
	MSG_EVT_MAX = 0xffff
};

enum phl_msg_custom_evt_id {
	/* Reserved for Custom Feature
	* be aware that the order of the following id are also used in
	* upper layer application, thus, it shall not be changed to
	* avoid mismatch error!!
	*/
	MSG_EVT_CUSTOME_NONE = 0,
	MSG_EVT_EDCA_ADJUST = 1,
	MSG_EVT_NAV_PADDING = 2,
	MSG_EVT_CUSTOM_CMD_DONE = 3,
};

enum phl_msg_recver_layer {
	MSG_RECV_PHL = 0,
	MSG_RECV_CORE = 1,
	MSG_RECV_MAX
};

enum phl_msg_indicator {
	MSG_INDC_PRE_PHASE = BIT0,
	MSG_INDC_FAIL = BIT1,
	MSG_INDC_CANCEL = BIT2
};

enum phl_msg_opt {
	MSG_OPT_SKIP_NOTIFY_OPT_MDL = BIT0,
	MSG_OPT_BLIST_PRESENT = BIT1,
	MSG_OPT_CLR_SNDR_MSG_IF_PENDING = BIT2,
	MSG_OPT_SEND_IN_ABORT = BIT3,
};


/* all module share this common enum definition */
enum phy_bk_module_opcode {
	BK_MODL_OP_NONE = 0,
	BK_MODL_OP_CHK_NEW_MSG,
	BK_MODL_OP_INPUT_CMD,
	BK_MODL_OP_MAX
};

/* Foreground cmd token opcode */
enum phy_fg_cmd_req_opcode {
	FG_REQ_OP_NONE = 0,
	FG_REQ_OP_GET_ROLE,
	FG_REQ_OP_GET_MDL_ID,
#ifdef RTW_WKARD_MRC_ISSUE_NULL_WITH_SCAN_OPS
	FG_REQ_OP_GET_SCAN_PARAM,
	FG_REQ_OP_GET_ISSUE_NULL_OPS,
#endif
	FG_REQ_OP_MAX
};

/* priority of phl background
module which would be considered when dispatching phl msg*/
enum phl_bk_module_priority {
	PHL_MDL_PRI_ROLE = 0,
	PHL_MDL_PRI_OPTIONAL,
	PHL_MDL_PRI_MANDATORY,
	PHL_MDL_PRI_MAX
};
/**
 * phl_msg - define a general msg format for PHL/CORE layer module to handle
 * one can easily extend additional mgnt info by encapsulating inside a file
 * refer to
 *		struct phl_msg_ex 		in phl_msg_hub.c
 *		struct phl_dispr_msg_ex		in phl_cmd_dispatcher.c
 *
 * @msg_id: indicate msg source & msg type
 *	    BYTE 3: RSVD
 *	    BYTE 2: PHL Module ID,  refer to enum phl_module_id
 *	    BYTE 0-1: event id, refer to enum phl_msg_evt_id
 * @inbuf: input buffer that sent along with msg
 * @inlen: input buffer length
 * @outbuf: output buffer that returned after all phl modules have recved msg.
 * @outlen: output buffer length
 * @band_idx: index of Band(PHY) which associate to this msg

 * @rsvd: feature reserved, passing object pointer.
 *	For example,
 *	- cmd_scan : [0] :wifi_role.
 */
struct phl_msg{
	u32 msg_id;
	u8* inbuf;
	u32 inlen;
	u8* outbuf;
	u32 outlen;
	enum phl_band_idx band_idx;
	void *rsvd[4];
};

struct msg_notify_map {
	u8* id_arr;
	u8 len;
};

struct msg_completion_routine {
	void* priv;
	void (*completion)(void* priv, struct phl_msg* msg);
};
/**
 * phl_msg_attribute: used in phl_dispr_send_msg
 * @opt: refers to enum phl_msg_opt.
 * @notify: input id array (refer to enum phl_module_id)
 * 	    for indicating additional dependency
 * @completion: completion routine
 */
struct phl_msg_attribute {
	u8 opt;
	struct msg_notify_map notify;
	struct msg_completion_routine completion;
};

/**
 * phl_module_op_info - set by core layer or phl itself,
 * op code process is an synchronous process.
 * which would be handled directly by module handler
 * @op_code: refer to enum phy_module_opcode
 * @inbuf: input buffer that sent along with msg
 * @inlen: input buffer length
 * @outbuf: output buffer that returned after all phy modules have recved msg.
 * @outlen: output buffer length
 */
struct phl_module_op_info{
	u32 op_code;
	u8* inbuf;
	u32 inlen;
	u8* outbuf;
	u32 outlen;
};

/**
 * phl_cmd_token_req - request foramt for applying token of a specific cmd
 * dispatcher.
 * cmd token request is regarded as foreground module, thus,
 * need to contend for cmd token.
 * Normally, these req would be linked to a specific wifi role
 * and acquiring RF resource for a specific task.
 *
 * @module_id: starting from PHL_FG_MDL_START
 * @priv: private context from requestor
 * @role: designated role info associated with current request.
 * -----------------------------------------
 * regarding on "return code" for following ops, refer to enum phl_mdl_ret_code
 * -----------------------------------------
 * @acquired: notify requestor when cmd token has acquired for this cmd.
 * @abort: notify requestor when cmd has been canceled
 *	   after calling rtw_phl_phy_cancel_token_req.
 * @msg_hdlr: notify requestor about incoming msg.
 * @set_info: notify requestor to handle specific op code.
 * @query_info: notify requestor to handle specific op code.
 */

struct phl_cmd_token_req{
	u8 module_id;
	void* priv;
	void* role;
	enum phl_mdl_ret_code (*acquired)(void* dispr, void* priv);
	enum phl_mdl_ret_code (*abort)(void* dispr, void* priv);
	enum phl_mdl_ret_code (*msg_hdlr)(void* dispr, void* priv,
							struct phl_msg* msg);
	enum phl_mdl_ret_code (*set_info)(void* dispr, void* priv,
					struct phl_module_op_info* info);
	enum phl_mdl_ret_code (*query_info)(void* dispr, void* priv,
				struct phl_module_op_info* info);
};
/**
 * phl_module_ops - standard interface for interacting with a cmd dispatcher.
 * -----------------------------------------
 * regarding on "return code" for following ops, refer to enum phl_mdl_ret_code
 * -----------------------------------------
 * @init: notify module for initialization.
 * @deinit: notify module for de-initialization.
 * @start: notify module to start.
 * @stop: notify module to stop.
 * @msg_hdlr: notify module about incoming msg.
 * @set_info: notify module to handle specific op code.
 * @query_info: notify module to handle specific op code.
 */
struct phl_bk_module_ops {
	enum phl_mdl_ret_code (*init)(void* phl_info, void* dispr, void** priv);
	void (*deinit)(void* dispr, void* priv);
	enum phl_mdl_ret_code (*start)(void* dispr, void* priv);
	enum phl_mdl_ret_code (*stop)(void* dispr, void* priv);
	enum phl_mdl_ret_code (*msg_hdlr)(void* dispr, void* priv,
							struct phl_msg* msg);
	enum phl_mdl_ret_code (*set_info)(void* dispr, void* priv,
			struct phl_module_op_info* info);
	enum phl_mdl_ret_code (*query_info)(void* dispr, void* priv,
			struct phl_module_op_info* info);
};

#define MSG_MDL_ID_FIELD(_msg_id) (((_msg_id) >> 16) & 0xFF)
#define MSG_EVT_ID_FIELD(_msg_id) ((_msg_id) & 0xFFFF)
#define MSG_INDC_FIELD(_msg_id) (((_msg_id) >> 24) & 0xFF)
#define IS_MSG_FAIL(_msg_id) ((_msg_id) & ( MSG_INDC_FAIL <<  24))
#define IS_MSG_IN_PRE_PHASE(_msg_id) ((_msg_id) & ( MSG_INDC_PRE_PHASE <<  24))
#define IS_MSG_CANCEL(_msg_id) ((_msg_id) & ( MSG_INDC_CANCEL <<  24))
#define SET_MSG_MDL_ID_FIELD(_msg_id, _id) \
	((_msg_id) = (((_msg_id) & 0xFF00FFFF) | ((u32)(_id) << 16)))
#define SET_MSG_EVT_ID_FIELD(_msg_id, _id) \
	((_msg_id) = (((_msg_id) & 0xFFFF0000) | (_id)))
#define SET_MSG_INDC_FIELD(_msg_id, _indc) \
	((_msg_id) = (((_msg_id) & 0x00FFFFFF) | ((u32)(_indc) << 24)))
#define CLEAR_MSG_INDC_FIELD(_msg_id, _indc) ((_msg_id) &= ~((_indc) << 24))

#define RTW_MAX_FW_SIZE 0x400000

enum rtw_fw_src {
	RTW_FW_SRC_INTNAL, /* 0 */
	RTW_FW_SRC_EXTNAL, /* 1 */
	RTW_FW_SRC_MAX
};

enum rtw_fw_rsn {
	RTW_FW_RSN_INIT, /* 0 */
	RTW_FW_RSN_SPIC, /* 1 */
	RTW_FW_RSN_LPS, /* 2 */
	RTW_FW_RSN_MCC, /* 3 */
	RTW_FW_RSN_WOW, /* 4 */
	RTW_FW_RSN_MAX
};

struct rtw_fw_cap_t {
	enum rtw_fw_src fw_src;
	u8 dlram_en;
	u8 dlrom_en;
};

#define INVALID_WIFI_ROLE_IDX MAX_WIFI_ROLE_NUMBER
#define UNSPECIFIED_ROLE_ID 0xFF
#define MAX_SECCAM_NUM_PER_ENTRY 7

/* Role hw TX CAP*/
struct role_cap_t {
	u8 rty_lmt;		/* retry limit for DATA frame, 0xFF: invalid */
	u8 rty_lmt_rts;		/* retry limit for RTS frame, 0xFF: invalid */
	enum wlan_mode wmode;
	enum channel_width bw;

	u8 tx_num_ampdu;
	u8 tx_amsdu_in_ampdu; /*from SW & HW*/
	u8 tx_ampdu_len_exp; /*from  SW & HW*/
	u8 tx_htc;
	u8 tx_sgi;

	u8 tx_ht_ldpc:1;
	u8 tx_vht_ldpc:1;
	u8 tx_he_ldpc:1;
	u8 tx_ht_stbc:1;
	u8 tx_vht_stbc:1;
	u8 tx_he_stbc:1;

	u8 supported_rates[12];
};

struct role_sw_cap_t {
	u8 temp_for_struct_empty;
	u16 bf_cap; /* use define : HW_CAP_BFER_XX_XX */
};

/*
Protocol - RX CAP from 80211 PKT,
driver TX related function need to
reference __rx__ of rtw_phl_stainfo_t->asoc_cap
*/
struct protocol_cap_t {
	/* MAC related */
	u16 bcn_interval;	/* beacon interval */
	u8 num_ampdu;
	u8 ampdu_density:3;	/* rx ampdu cap */
	u8 ampdu_len_exp; /* rx ampdu cap */
	u8 amsdu_in_ampdu:1; /* rx ampdu cap */
	u8 max_amsdu_len:2; /* 0: 4k, 1: 8k, 2: 11k */
	u8 htc_rx:1;
	u8 sm_ps:2;
	u8 trig_padding:2;
	u8 twt:6;
	u8 all_ack:1;
	u8 a_ctrl:4;
	u8 ops:1;
	u8 ht_vht_trig_rx:1;
	u8 bsscolor;

	u8 short_slot:1;	/* Short Slot Time */

	u8 preamble:1;		/* Preamble, 0: long, 1: short */
	u8 sgi_20:1;		/* HT Short GI for 20 MHz */
	u8 sgi_40:1;		/* HT Short GI for 40 MHz */
	u8 sgi_80:1;		/* VHT Short GI for 80 MHz */
	u8 sgi_160:1;		/* VHT Short GI for 160/80+80 MHz */
	struct rtw_edca_param edca[4]; 	/* Access Category, 0:BE, 1:BK, 2:VI, 3:VO */
	u8 mu_qos_info;
	struct rtw_mu_edca_param mu_edca[4];

	/* BB related */
	u8 ht_ldpc:1;
	u8 vht_ldpc:1;
	u8 he_ldpc:1;
	u8 he_su_bfmr:1;
	u8 he_su_bfme:1;
	u8 he_mu_bfmr:1;
	u8 he_mu_bfme:1;
	u8 bfme_sts:3;
	u8 num_snd_dim:3;

	u8 ht_su_bfmr:1;
	u8 ht_su_bfme:1;
	u8 vht_su_bfmr:1;
	u8 vht_su_bfme:1;
	u8 vht_mu_bfmr:1;
	u8 vht_mu_bfme:1;
	u8 ht_vht_ng:2;
	u8 ht_vht_cb:2;
	/*
	 * supported_rates: Supported data rate of CCK/OFDM.
	 * The rate definition follow Wi-Fi spec, unit is 500kb/s,
	 * and the MSB(bit 7) represent basic rate.
	 * ex. CCK 2Mbps not basic rate is encoded as 0x04,
	 *     and OFDM 6M basic rate is encoded as 0x8c.
	 * Suppose rates come from Supported Rates and Extended Supported
	 * Rates IE.
	 * Value 0 means it is end of array, and no more valid data rate follow.
	 */
	u8 supported_rates[12];
	u8 ht_rx_mcs[4];
	u8 ht_tx_mcs[4];
	u8 ht_basic_mcs[4];	/* Basic rate of HT */
	u8 vht_rx_mcs[2];
	u8 vht_tx_mcs[2];
	u8 vht_basic_mcs[2];	/* Basic rate of VHT */
	u8 he_rx_mcs[6];/*80,160,80+80*/
	u8 he_tx_mcs[6];/*80,160,80+80*/
	u8 he_basic_mcs[2];	/* Basic rate of HE */
	u8 stbc_ht_rx:2;
	u8 stbc_vht_rx:3;
	u8 stbc_he_rx:1;
	u8 stbc_tx:1;
	u8 ltf_gi;
	u8 doppler_tx:1;
	u8 doppler_rx:1;
	u8 dcm_max_const_tx:2;
	u8 dcm_max_nss_tx:1;
	u8 dcm_max_const_rx:2;
	u8 dcm_max_nss_rx:1;
	u8 partial_bw_su_in_mu:1;
	u8 bfme_sts_greater_80mhz:3;
	u8 num_snd_dim_greater_80mhz:3;
	u8 stbc_tx_greater_80mhz:1;
	u8 stbc_rx_greater_80mhz:1;
	u8 ng_16_su_fb:1;
	u8 ng_16_mu_fb:1;
	u8 cb_sz_su_fb:1;
	u8 cb_sz_mu_fb:1;
	u8 trig_su_bfm_fb:1;
	u8 trig_mu_bfm_fb:1;
	u8 trig_cqi_fb:1;
	u8 partial_bw_su_er:1;
	u8 pkt_padding:2;
	u8 ppe_thr[8][4];
	u8 pwr_bst_factor:1;
	u8 max_nc:3;
	u8 dcm_max_ru:2;
	u8 long_sigb_symbol:1;
	u8 non_trig_cqi_fb:1;
	u8 tx_1024q_ru:1;
	u8 rx_1024q_ru:1;
	u8 fbw_su_using_mu_cmprs_sigb:1;
	u8 fbw_su_using_mu_non_cmprs_sigb:1;
	u8 er_su:1;
	u8 tb_pe:3;
	u16 txop_du_rts_th;
	u8 he_rx_ndp_4x32:1;

	/* RF related */
	u8 nss_tx:3;
	u8 nss_rx:3;
};



#define LOAD_MAC_REG_FILE				BIT0
#define LOAD_BB_PHY_REG_FILE			BIT1
#define LOAD_BB_PHY_REG_MP_FILE			BIT2
#define LOAD_RF_RADIO_FILE				BIT3
#define LOAD_RF_TXPWR_BY_RATE			BIT4
#define LOAD_RF_TXPWR_TRACK_FILE		BIT5
#define LOAD_RF_TXPWR_LMT_FILE			BIT6
#define LOAD_RF_TXPWR_LMT_RU_FILE		BIT7

struct rtw_pcie_ltr_lat_ctrl {
	enum rtw_pcie_bus_func_cap_t ctrl;
	u32 val;
};

enum rtw_pcie_ltr_state {
	RTW_PCIE_LTR_SW_ACT = 1,
	RTW_PCIE_LTR_SW_IDLE = 2
};

struct bus_sw_cap_t {
#ifdef CONFIG_PCI_HCI
	enum rtw_pcie_bus_func_cap_t l0s_ctrl;
	enum rtw_pcie_bus_func_cap_t l1_ctrl;
	enum rtw_pcie_bus_func_cap_t l1ss_ctrl;
	enum rtw_pcie_bus_func_cap_t wake_ctrl;
	enum rtw_pcie_bus_func_cap_t crq_ctrl;
	u8 clkdly_ctrl;
	u8 l0sdly_ctrl;
	u8 l1dly_ctrl;
	u32 txbd_num;
	u32 rxbd_num;
	u32 rpbd_num;
	u32 rxbuf_num;
	u32 rpbuf_num;
	struct rtw_pcie_ltr_lat_ctrl ltr_act;
	struct rtw_pcie_ltr_lat_ctrl ltr_idle;
	u8 ltr_init_state;
#elif defined (CONFIG_USB_HCI)
	u32 tx_buf_size;
	u32 tx_buf_num;
	u32 tx_mgnt_buf_size;
	u32 tx_mgnt_buf_num;
	u32 tx_h2c_buf_num;
	u32 rx_buf_size;
	u32 rx_buf_num;
	u32 in_token_num;
#else
	u8 temp_for_struct_empty; /* for usb or sdio.*/
#endif
};
struct bus_cap_t {
#ifdef CONFIG_PCI_HCI
	enum rtw_pcie_bus_func_cap_t l0s_ctrl;
	enum rtw_pcie_bus_func_cap_t l1_ctrl;
	enum rtw_pcie_bus_func_cap_t l1ss_ctrl;
	enum rtw_pcie_bus_func_cap_t wake_ctrl;
	enum rtw_pcie_bus_func_cap_t crq_ctrl;
	u8 clkdly_ctrl;
	u8 l0sdly_ctrl;
	u8 l1dly_ctrl;
	u32 txbd_num;
	u32 rxbd_num;
	u32 rpbd_num;
	u32 rxbuf_num;
	u32 rpbuf_num;
	struct rtw_pcie_ltr_lat_ctrl ltr_act;
	struct rtw_pcie_ltr_lat_ctrl ltr_idle;
	u8 ltr_init_state;
	u8 ltr_cur_state;
#elif defined (CONFIG_USB_HCI)
	u32 tx_buf_size;
	u32 tx_buf_num;
	u32 tx_mgnt_buf_size;
	u32 tx_mgnt_buf_num;
	u32 tx_h2c_buf_num;
	u32 rx_buf_size;
	u32 rx_buf_num;
	u32 in_token_num;
#else
	u8 temp_for_struct_empty; /* for usb or sdio.*/
#endif
};

struct  rtw_wow_cap_t {
	u8 magic_sup;
	u8 pattern_sup;
	u8 arp_ofld_sup;
	u8 ns_oflod_sup;
	u8 gtk_ofld_sup;
};

struct  rtw_ps_cap_t {
	/* radio state*/
	enum rtw_rf_state init_rf_state;
	/* ips parameter */
	u8 ips_en;
	u8 ips_cap;
	/* unit: ms */
	u32 ips_defer_time;
	u8 ips_wow_en;
	u8 ips_wow_cap;
	/* lps parameter */
	u8 lps_en;
	u8 lps_cap;
	/* unit: ms */
	u32 lps_defer_time;
	u8 lps_awake_interval;
	u8 lps_listen_bcn_mode;
	u8 lps_smart_ps_mode;
	u8 lps_wow_en;
	u8 lps_wow_cap;
	u8 lps_wow_awake_interval;
	u8 lps_wow_listen_bcn_mode;
	u8 lps_wow_smart_ps_mode;
};

struct phy_sw_cap_t {
	struct rtw_para_info_t mac_reg_info;
	struct rtw_para_info_t bb_phy_reg_info;
	struct rtw_para_info_t bb_phy_reg_mp_info;

	struct rtw_para_info_t rf_radio_a_info;
	struct rtw_para_info_t rf_radio_b_info;
	struct rtw_para_info_t rf_txpwr_byrate_info;
	struct rtw_para_info_t rf_txpwrtrack_info;

	struct rtw_para_pwrlmt_info_t rf_txpwrlmt_info;
	struct rtw_para_pwrlmt_info_t rf_txpwrlmt_ru_info;

	u8 proto_sup;
	u8 band_sup;
	u8 bw_sup;
	u8 txss;
	u8 rxss;
};

/* final capability of phy */
struct phy_cap_t {
	u8 proto_sup;
	u8 band_sup;
	u8 bw_sup;
	u8 txss;
	u8 rxss;
};

/* final capability of device */
struct dev_cap_t {
	u64 hw_sup_flags;/*hw's feature support flags*/
#ifdef RTW_WKARD_LAMODE
	bool la_mode;
#endif
	u8 pkg_type;
	u8 rfe_type;
	u8 bypass_rfe_chk;
	u8 xcap;
	struct rtw_fw_cap_t fw_cap;
#ifdef CONFIG_MCC_SUPPORT
	bool mcc_sup;
#endif
#ifdef CONFIG_DBCC_SUPPORT
	bool dbcc_sup;
#endif

	struct rtw_wow_cap_t wow_cap;
	struct rtw_ps_cap_t ps_cap;
	u8 hw_hdr_conv;
	u8 domain;
	u8 btc_mode;
};

#ifdef RTW_PHL_BCN //phl def

#define BCN_ID_MAX		(0xFF)
#define MAX_BCN_SIZE	1000

enum bcn_offload_flags{
	BCN_HW_SEQ = 0,
	BCN_HW_TIM,

	BCN_HW_MAX = 32,
};

struct rtw_bcn_info_cmn {
	u8 role_idx;
	u8 bcn_id;
	u8 bcn_added;

	u8 bssid[6];
	u32 bcn_interval;

	u8 bcn_buf[MAX_BCN_SIZE];
	u32 bcn_length;
	u32 bcn_rate;

	u32 bcn_dtim;
	u32 ie_offset_tim;

	u32 bcn_offload;
};

struct rtw_bcn_info_hw {
	u8 band;
	u8 port;
	u8 mbssid;
	u8 mac_id;
};

struct rtw_bcn_entry {
	_os_list list;
	struct rtw_bcn_info_cmn	*bcn_cmn;	//fill by core
	struct rtw_bcn_info_hw 	bcn_hw;	//fill by phl //?? void mapping ?? for 8852, 8834 ...blabla
};
#endif

struct rtw_phl_com_t;

struct phl_msg_receiver {
		void* priv;
		void (*incoming_evt_notify)(void* priv, struct phl_msg *msg);
};

struct rtw_wifi_role_t {
	struct rtw_phl_com_t *phl_com;/*point to phl_com*/
	bool active;
	enum role_type type;/*will mapping to net type*/
	enum mlme_state mstate;
	u8 status;

	/*
	 * final protocol capability of role from intersection of
	 * sw role cap, sw protocol cap and hw protocol cap
	 */
	struct protocol_cap_t proto_role_cap;

	/*
	 * final capability of role from intersection of
	 * sw role cap, final phy cap and final dev cap
	 */
	struct role_cap_t cap;

	u8 mac_addr[MAC_ALEN];
	u8 hw_band; /*MAC Band0 or Band1*/
	u8 hw_port; /*MAC HW Port*/
	u8 hw_wmm; /*HW EDCA - wmm0 or wmm1*/
	u8 id;/* recode role_idx in phl_com */

	/*#ifdef CONFIG_AP*/
#ifdef RTW_PHL_BCN
	u8 hw_mbssid;
	struct rtw_bcn_info_cmn bcn_cmn; //todo: ieee mbssid case & multi-bcn (in one iface) case
#endif
	u32 hiq_win;
	u8 dtim_period;
	u8 mbid_num;
	/*#endif CONFIG_AP*/

	struct rtw_chan_def chandef;
	struct rtw_chan_ctx *chanctx;/*point to chanctx*/

	struct phl_queue assoc_sta_queue;

	void *core_data; /* Track back to counter part in core layer */
};

#define TXTP_CALC_DIFF_MS 1000
#define RXTP_CALC_DIFF_MS 1000

#define	TX_ULTRA_LOW_TP_THRES_KBPS 100
#define	RX_ULTRA_LOW_TP_THRES_KBPS 100
#define	TX_LOW_TP_THRES_MBPS 2
#define	RX_LOW_TP_THRES_MBPS 2
#define	TX_MID_TP_THRES_MBPS  10
#define	RX_MID_TP_THRES_MBPS  10
#define	TX_HIGH_TP_THRES_MBPS  50
#define	RX_HIGH_TP_THRES_MBPS  50


enum rtw_tfc_lvl {
	RTW_TFC_IDLE = 0,
	RTW_TFC_ULTRA_LOW = 1,
	RTW_TFC_LOW = 2,
	RTW_TFC_MID = 3,
	RTW_TFC_HIGH = 4,
	RTW_TFC_LVL_MAX = 0xFF
};

enum rtw_tfc_sts {
	TRAFFIC_CHANGED = BIT0,
	TRAFFIC_INCREASE = BIT1,
	TRAFFIC_DECREASE = BIT2,
	TRAFFIC_STS_MAX = BIT7
};

struct rtw_traffic_t {
	enum rtw_tfc_lvl lvl;
	enum rtw_tfc_sts sts;
};

struct rtw_stats_tp {
	u64 last_calc_bits;
	u32 last_calc_time_ms;
};
/*statistic*/
struct rtw_stats {
	u64 tx_byte_uni;/*unicast tx byte*/
	u64 rx_byte_uni;/*unicast rx byte*/
	u64 tx_byte_total;
	u64 rx_byte_total;
	u32 tx_tp_kbits;
	u32 rx_tp_kbits;
	u16 tx_moving_average_tp; /* tx average MBps*/
	u16 rx_moving_average_tp; /* rx average MBps*/
	u32 last_tx_time_ms;
	u32 last_rx_time_ms;
	u32 txreq_num;
	u32 rx_rate_nmr[RTW_DATA_RATE_HE_NSS4_MCS11 +1];
	u64 ser_event[6]; /* RTW_PHL_SER_MAX */
	struct rtw_stats_tp txtp;
	struct rtw_stats_tp rxtp;
	struct rtw_traffic_t tx_traffic;
	struct rtw_traffic_t rx_traffic;
};
enum sta_chg_id {
	STA_CHG_BW,
	STA_CHG_NSS,
	STA_CHG_RAMASK,
	STA_CHG_SEC_MODE,
	STA_CHG_MAX
};

enum phl_upd_mode {
	PHL_UPD_ROLE_CREATE,
	PHL_UPD_ROLE_REMOVE,
	PHL_UPD_ROLE_TYPE_CHANGE,
	PHL_UPD_ROLE_INFO_CHANGE,
	PHL_UPD_STA_INFO_CHANGE,
	PHL_UPD_STA_CON_DISCONN,
	PHL_UPD_ROLE_MAX
};

#ifdef CONFIG_PHL_TXSC
#define PHL_TXSC_ENTRY_NUM 8
#define MAX_WD_SIZE	128

struct phl_txsc_entry {
	bool txsc_wd_cached;
	u8 txsc_wd_cache[MAX_WD_SIZE];
	u8 txsc_wd_len;
	u32 txsc_cache_hit;
};
#endif

struct rtw_hal_stainfo_t;
struct rtw_phl_stainfo_t {
	_os_list list;
	struct rtw_wifi_role_t *wrole;
	bool active;
	u16 aid;
	u16 macid;
	u8 mac_addr[MAC_ALEN];

	struct rtw_chan_def chandef;
	enum wlan_mode wmode;

	struct rtw_stats stats;
	/*mlme protocol or MAC related CAP*/
	u8 bcn_hit_cond;
	u8 hit_rule;
	u8 tf_trs;
	u8 tgt_ind;
	u8 frm_tgt_ind;

	/* rx agg */
	struct phl_tid_ampdu_rx *tid_rx[8]; /* TID_MAX_NUM */
	_os_lock tid_rx_lock;               /* guarding @tid_rx */
	_os_event comp_sync;     /* reorder timer completion event */
	_os_timer reorder_timer; /* reorder timer for all @tid_rx of the
	                          * stainfo */
	/* TODO: add missing part */

	/*mlme protocol or PHY related CAP*/
	struct protocol_cap_t asoc_cap;
	enum rtw_protect_mode protect;

	/*security related*/
	u8 wapi;
	u8 sec_mode;

	struct rtw_hal_stainfo_t *hal_sta;

#ifdef CONFIG_PHL_TXSC
	struct phl_txsc_entry phl_txsc[PHL_TXSC_ENTRY_NUM];
#endif

	void *core_data; /* Track back to counter part in core layer */
};




#define WL_FUNC_P2P		BIT0
#define WL_FUNC_MIRACAST	BIT1
#define WL_FUNC_TDLS		BIT2
#define WL_FUNC_FTM		BIT3
#define WL_FUNC_BIT_NUM	4


/* HW MAC capability*/
#define HW_SUP_DBCC			BIT0
#define HW_SUP_AMSDU			BIT1
#define HW_SUP_TCP_TX_CHKSUM		BIT2
#define HW_SUP_TCP_RX_CHKSUM		BIT3
#define HW_SUP_TXPKT_CONVR		BIT4
#define HW_SUP_RXPKT_CONVR		BIT5
#define HW_SUP_MULTI_BSSID		BIT6
#define HW_SUP_OFDMA			BIT7
#define HW_SUP_CHAN_INFO		BIT8
#define HW_SUP_TSSI			BIT9
#define HW_SUP_TANK_K			BIT10

/*BUS Section CAP */
#define HW_SUP_PCIE_PLFH		BIT20	/*payload from host*/
#define HW_SUP_USB_MULTI_FUN		BIT21
#define HW_SUP_SDIO_MULTI_FUN		BIT22

/* Beamform CAP */
#define HW_CAP_BF_NON_SUPPORT 0
#define HW_CAP_BFEE_HT_SU BIT(0)
#define HW_CAP_BFER_HT_SU BIT(1)
#define HW_CAP_BFEE_VHT_SU BIT(2)
#define HW_CAP_BFER_VHT_SU BIT(3)
#define HW_CAP_BFEE_VHT_MU BIT(4)
#define HW_CAP_BFER_VHT_MU BIT(5)
#define HW_CAP_BFEE_HE_SU BIT(6)
#define HW_CAP_BFER_HE_SU BIT(7)
#define HW_CAP_BFEE_HE_MU BIT(8)
#define HW_CAP_BFER_HE_MU BIT(9)
#define HW_CAP_HE_NON_TB_CQI BIT(10)
#define HW_CAP_HE_TB_CQI BIT(11)


struct hal_spec_t {
	char *ic_name;
	u16 macid_num;

	u8 sec_cam_ent_num;
	u8 sec_cap;
	u8 wow_cap;

	#if 1 // Neo : rtk_wifi_driver
	u8 macid_cap;
	u16 macid_txrpt;
	u8 macid_txrpt_pgsz;
	#endif

	u8 rfpath_num_2g:4;	/* used for tx power index path */
	u8 rfpath_num_5g:4;	/* used for tx power index path */
	u8 rf_reg_path_num;

	#if 1 // Neo : rtk_wifi_driver
	u8 rf_reg_path_avail_num;
	u8 rf_reg_trx_path_bmp; /* [7:4]TX path bmp, [0:3]RX path bmp */
	#endif

	u8 max_tx_cnt;

	u8 tx_nss_num:4;
	u8 rx_nss_num:4;

	u8 band_cap;	/* value of BAND_CAP_XXX */
	u8 bw_cap;		/* value of BW_CAP_XXX */
	u8 port_num;
	u8 proto_cap;	/* value of PROTO_CAP_XXX */

	#if 1 // Neo : rtk_wifi_driver
	u8 txgi_max; /* maximum tx power gain index */
	u8 txgi_pdbm; /* tx power gain index per dBm */
	#endif

	u8 wl_func;		/* value of WL_FUNC_XXX */

	#if 1 // Neo : rtk_wifi_driver
	u8 tx_aclt_unit_factor; /* how many 32us */

	u8 rx_tsf_filter:1;

	u8 pg_txpwr_saddr; /* starting address of PG tx power info */
	u8 pg_txgi_diff_factor; /* PG tx power gain index diff to tx power gain index */

	u8 hci_type;	/* value of HCI Type */
	#endif

	/********* xmit ************/


	/********* recv ************/
	u8 rx_bd_info_sz;

	u16 rx_tag[2];
	#ifdef CONFIG_USB_HCI
	u8 max_bulkin_num;
	u8 max_bulkout_num;
	#endif
	#ifdef CONFIG_PCI_HCI
	u16 txbd_multi_tag;
	u8 txbd_upd_lmt;
#ifdef RTW_WKARD_BUSCAP_IN_HALSPEC
	u8 phyaddr_num;
#endif
	#endif
	u8 cts2_thres_en;
	u16 cts2_thres;
	/********* beamformer ************/
	u8 max_csi_buf_su_nr;
	u8 max_csi_buf_mu_nr;
	u8 max_bf_ent_nr;
	u8 max_su_sta_nr;
	u8 max_mu_sta_nr;
};

#define phl_get_hci_type(_phlcom) (_phlcom->hci_type)
#define phl_get_ic_spec(_phlcom) (&_phlcom->hal_spec)
#define phl_get_fw_buf(_phlcom) (_phlcom->fw_info.ram_buff)
#define phl_get_fw_size(_phlcom) (_phlcom->fw_info.ram_size)

enum rtw_drv_mode {
	RTW_DRV_MODE_NORMAL = 0,
	RTW_DRV_MODE_EQC = 1,

	/* 11~20 for MP submodule section*/
	RTW_DRV_MODE_MP_SMDL_START = 11,
	RTW_DRV_MODE_MP = 11,
	RTW_DRV_MODE_HOMOLOGATION = 12,
	RTW_DRV_MODE_MP_SMDL_END = 20,

	/* 21~30 for FPGA submodule section*/
	RTW_DRV_MODE_FPGA_SMDL_START = 21,
	RTW_DRV_MODE_FPGA_SMDL_END = 30,

	/* 31~60 for VERIFY submodule section*/
	RTW_DRV_MODE_VERIFY_SMDL_START = 31,
	RTW_DRV_MODE_VERIFY_SMDL_END = 60,

	/* 61~80 for TOOL submodule section*/
	RTW_DRV_MODE_TOOL_SMDL_START = 61,
	RTW_DRV_MODE_TOOL_SMDL_END = 80,

	/* Fixed Max Value*/
	RTW_DRV_MODE_MAX = 255
};

struct rtw_evt_info_t {
	_os_lock evt_lock;
	enum rtw_phl_evt evt_bitmap;
};

// WiFi FW
struct rtw_fw_info_t {
	u8 fw_en;
	u8 fw_src;
	u8 dlram_en;
	u8 dlrom_en;
	u8 *rom_buff;
	u32 rom_addr;
	u32 rom_size;
	char rom_path[256];
	u8 *ram_buff;
	u32 ram_size;
	char ram_path[256];
	u8 fw_type;
};


#ifdef CONFIG_PHL_DFS
enum dfs_regd_t {
	DFS_REGD_UNKNOWN	= 0,
	DFS_REGD_FCC	= 1,
	DFS_REGD_JAP	= 2,
	DFS_REGD_ETSI	= 3,
};
struct rtw_dfs_t {
	u8 region_domain;
	bool dfs_enabled;
};
#endif

#ifdef CONFIG_PHL_CHANNEL_INFO

#define CHAN_INFO_MAX_SIZE 65535

struct csi_header_t {
	u8 mac_addr[6];			/* mdata: u8 ta[6]? */
	u32 hw_assigned_timestamp;	/* mdata: u32 freerun_cnt */
	u8 channel;			/* Drv define */
	u8 bandwidth;			/* mdata: u8 bw */
	u16 rx_data_rate;		/* mdata: u16 rx_rate */
	u8 nc;				/* ch_rpt_hdr_info */
	u8 nr;				/* ch_rpt_hdr_info */
	u16 num_sub_carrier;		/* Drv define*/
	u8 num_bit_per_tone;		/* Drv define per I/Q */
	u8 avg_idle_noise_pwr;		/* ch_rpt_hdr_info */
	u8 evm[2];			/* ch_rpt_hdr_info */
	u8 rssi[2];			/* phy_info_rpt */
	u32 csi_data_length;		/* ch_rpt_hdr_info */
	u8 rxsc;			/* phy_info_rpt */
	u8 ch_matrix_report;		/* mdata: u8 get_ch_info */
	u8 csi_valid;			/* ch_rpt_hdr_info */
};


struct chan_info_t {
	u8* chan_info_buffer;
	u32 length;
	struct csi_header_t csi_header;
};
#endif /* CONFIG_PHL_CHANNEL_INFO */

#ifdef CONFIG_MCC_SUPPORT
enum rtw_phl_mcc_coex_mode {
	RTW_PHL_MCC_COEX_MODE_NONE = 0,
	RTW_PHL_MCC_COEX_MODE_BT_MASTER,
	RTW_PHL_MCC_COEX_MODE_WIFI_MASTER,
	RTW_PHL_MCC_COEX_MODE_BT_WIFI_BALANCE
};

enum rtw_phl_mcc_mode {
	RTW_PHL_MCC_MODE_NONE = 0,
	RTW_PHL_MCC_AP_CLIENT_MODE,
	RTW_PHL_MCC_2CLIENTS_MODE
};

enum rtw_phl_mcc_rpt {
	RTW_MCC_RPT_OFF = 0,
	RTW_MCC_RPT_FAIL_ONLY,
	RTW_MCC_RPT_ALL
};

/*Export to core layer. Core get NOA info to update p2p beacon*/
struct rtw_phl_mcc_noa {
	struct rtw_wifi_role_t *wrole;
	u32 start_t_h;
	u32 start_t_l;
	u8 dur;
	u8 cnt;
	u16 interval;
};

struct rtw_phl_mcc_ops {
	void *priv; /* ops private, define by core layer*/
	int (*mcc_update_noa)(void *priv, struct rtw_phl_mcc_noa *param);
};

struct rtw_phl_mcc_macid_bitmap {
	u32 *bitmap;
	u8 len;
};

struct rtw_phl_mcc_sync_tsf_info {
	u8 sync_en;
	u16 source;
	u16 target;
	u8 offset;
};

struct rtw_phl_mcc_policy_info {
	u8 c2h_rpt;
	u8 tx_null_early;
	u8 dis_tx_null;
	u8 in_curr_ch;
	u8 dis_sw_retry;
	u8 sw_retry_count;
	u8 duration;
};

struct rtw_phl_mcc_role {
	struct rtw_wifi_role_t *wrole;
	u16 macid;
	u16 bcn_intvl;
	struct rtw_phl_mcc_macid_bitmap used_macid;
	struct rtw_chan_def *chandef;
	u8 group;
	struct rtw_phl_mcc_policy_info policy;
};

struct rtw_phl_mcc_pattern {
	u8 tob_r;
	u8 toa_r;
	u8 tob_a;
	u8 toa_a;
};

struct rtw_phl_mcc_en_info {
	struct rtw_phl_mcc_role mcc_role[MCC_ROLE_NUM];
	struct rtw_phl_mcc_sync_tsf_info sync_tsf_info;
	u8 ref_role_idx;
	u8 role_map; /*use mcc role map*/
	u16 mcc_intvl;
	struct rtw_phl_mcc_pattern m_pattern;
	u32 tsf_high; /*Start TSF*/
	u32 tsf_low;
};
#endif /* CONFIG_MCC_SUPPORT */

/*multi-roles control components*/
enum mr_op_mode {
	MR_OP_NON,
	MR_OP_SCC,
	MR_OP_MCC,
	MR_OP_MAX,
};

struct mr_info {
	u8 sta_num;
	u8 ld_sta_num;
	u8 lg_sta_num;		/* WIFI_STATION_STATE && WIFI_UNDER_LINKING */

	u8 ap_num;
	u8 ld_ap_num;	/*&& asoc_sta_count > 2*/
	u8 monitor_num;

	u8 p2p_device_num;
	u8 p2p_gc_num;
	u8 p2p_go_num;

#if 0
#ifdef CONFIG_TDLS
	u8 ld_tdls_num;	/* adapter.tdlsinfo.link_established */
#endif
#ifdef CONFIG_AP_MODE
	u8 starting_ap_num;	/*WIFI_FW_AP_STATE*/
#endif
	u8 adhoc_num;		/* (WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE) && WIFI_ASOC_STATE */
	u8 ld_adhoc_num;	/* (WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE) && WIFI_ASOC_STATE && asoc_sta_count > 2 */
#ifdef CONFIG_RTW_MESH
	u8 mesh_num;		/* WIFI_MESH_STATE &&  WIFI_ASOC_STATE */
	u8 ld_mesh_num;		/* WIFI_MESH_STATE &&  WIFI_ASOC_STATE && asoc_sta_count > 2 */
#endif
#endif
};

struct hw_band_ctl_t {
	_os_lock lock;
	u8 id;
	u8 port_map; /*used port_idx*/
	u8 role_map; /*used role_idx*/
	u8 wmm_map;
	struct mr_info cur_info;
	enum mr_op_mode op_mode;
	struct phl_queue chan_ctx_queue;/*struct rtw_chan_ctx*/
	enum phl_hw_port tsf_sync_port;
#ifdef CONFIG_MCC_SUPPORT
	void *mcc_info; /*struct phl_mcc_info*/
#endif
};

#define MAX_BAND_NUM 2
struct rtw_hal_com_t;
struct mr_ctl_t {
	_os_lock lock;
	struct rtw_hal_com_t *hal_com;
	u8 role_map;
	bool is_sb;
	struct hw_band_ctl_t band_ctrl[MAX_BAND_NUM];
	struct phl_bk_module_ops bk_ops;
#ifdef CONFIG_MCC_SUPPORT
	u8 init_mcc;
	void *com_mcc;/*struct phl_com_mcc_info*/
#endif
};

struct rtw_phl_mr_ops {
	u8 dummy;
#ifdef CONFIG_MCC_SUPPORT
	struct rtw_phl_mcc_ops *mcc_ops;
#endif
};

enum rtw_rssi_type {
	RTW_RSSI_DATA_ACAM,
	RTW_RSSI_DATA_ACAM_A1M,
	RTW_RSSI_DATA_OTHER,
	RTW_RSSI_CTRL_ACAM,
	RTW_RSSI_CTRL_ACAM_A1M,
	RTW_RSSI_CTRL_OTHER,
	RTW_RSSI_MGNT_ACAM,
	RTW_RSSI_MGNT_ACAM_A1M,
	RTW_RSSI_MGNT_OTHER,
	RTW_RSSI_UNKNOWN,
	RTW_RSSI_TYPE_MAX
};

#define PHL_MAX_RSSI 110
#define PHL_RSSI_MAVG_NUM 16
#define UPDATE_MA_RSSI(_RSSI, _TYPE, _VAL) \
	do { \
		u8 oldest_rssi = 0; \
		if(_RSSI->ma_rssi_ele_idx[_TYPE] < PHL_RSSI_MAVG_NUM) { \
			oldest_rssi = _RSSI->ma_rssi_ele[_TYPE][\
					_RSSI->ma_rssi_ele_idx[_TYPE]]; \
			_RSSI->ma_rssi_ele[_TYPE][_RSSI->ma_rssi_ele_idx[_TYPE]] = \
				((_VAL > PHL_MAX_RSSI) ? PHL_MAX_RSSI : _VAL ); \
		} else { \
			_RSSI->ma_rssi_ele_idx[_TYPE] = 0; \
			oldest_rssi = _RSSI->ma_rssi_ele[_TYPE][0]; \
			_RSSI->ma_rssi_ele[_TYPE][0] = \
				((_VAL > PHL_MAX_RSSI) ? PHL_MAX_RSSI : _VAL ); \
		} \
		_RSSI->ma_rssi_ele_sum[_TYPE] -= oldest_rssi;\
		_RSSI->ma_rssi_ele_sum[_TYPE] += \
			((_VAL > PHL_MAX_RSSI) ? PHL_MAX_RSSI : _VAL ); \
		_RSSI->ma_rssi_ele_idx[_TYPE]++; \
		if(_RSSI->ma_rssi_ele_cnt[_TYPE] < PHL_RSSI_MAVG_NUM) \
			_RSSI->ma_rssi_ele_cnt[_TYPE]++; \
		_RSSI->ma_rssi[_TYPE] = (u8)(_RSSI->ma_rssi_ele_sum[_TYPE] / \
			_RSSI->ma_rssi_ele_cnt[_TYPE]);\
	} while (0)

struct rtw_phl_rssi_stat {
	_os_lock lock;
	u8 ma_rssi_ele_idx[RTW_RSSI_TYPE_MAX];
	u8 ma_rssi_ele_cnt[RTW_RSSI_TYPE_MAX]; /* maximum : PHL_RSSI_MAVG_NUM */
	u8 ma_rssi_ele[RTW_RSSI_TYPE_MAX][PHL_RSSI_MAVG_NUM]; /* rssi element for moving average */
	u32 ma_rssi_ele_sum[RTW_RSSI_TYPE_MAX];
	u8 ma_rssi[RTW_RSSI_TYPE_MAX]; /* moving average : 0~PHL_MAX_RSSI (dBm = rssi - PHL_MAX_RSSI) */
};

#define PHL_MAX_PPDU_CNT 8
#define PHL_MAX_PPDU_STA_NUM 4
struct rtw_phl_ppdu_sts_sta_ent {
	u8 vld;
	u8 rssi;
	u16 macid;
};
struct rtw_phl_ppdu_phy_info {
	bool is_valid;
	u8 rssi; /*signal power : 0 - PHL_MAX_RSSI, rssi dbm = PHL_MAX_RSSI - value*/
	u8 rssi_path[RTW_PHL_MAX_RF_PATH];/*PATH A, PATH B ... PATH D*/
	u8 ch_idx;
	u8 tx_bf;
	u8 frame_type; /* type + subtype */
};
#ifdef CONFIG_PHY_INFO_NTFY
struct rtw_phl_ppdu_sts_ntfy {
	bool vld;
	u8 frame_type;
	u8 src_mac_addr[MAC_ADDRESS_LENGTH];
	struct rtw_phl_ppdu_phy_info phy_info;
};
#endif
struct rtw_phl_ppdu_sts_ent {
	/* from normal data */
	u8 frame_type;
	u8 addr_cam_vld;
	u8 crc32;
	u8 ppdu_type;
	u16 rx_rate;
	u8 src_mac_addr[MAC_ADDRESS_LENGTH];
	/* from ppdu status */
	bool valid;
	bool phl_done;
	u32 freerun_cnt;
	struct rtw_phl_ppdu_phy_info phy_info;
	struct rtw_phl_ppdu_sts_sta_ent sta[PHL_MAX_PPDU_STA_NUM];
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
	/* for ppdu status per pkt */
	struct phl_queue frames;
#endif
};
struct rtw_phl_ppdu_sts_info {
	struct rtw_phl_ppdu_sts_ent sts_ent[HW_BAND_MAX][PHL_MAX_PPDU_CNT];
	u8 cur_rx_ppdu_cnt[HW_BAND_MAX];
	bool en_ppdu_sts[HW_BAND_MAX];
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
	bool en_psts_per_pkt;
	bool psts_ampdu;
#define RTW_PHL_PSTS_FLTR_MGNT BIT(RTW_FRAME_TYPE_MGNT)
#define RTW_PHL_PSTS_FLTR_CTRL BIT(RTW_FRAME_TYPE_CTRL)
#define RTW_PHL_PSTS_FLTR_DATA BIT(RTW_FRAME_TYPE_DATA)
#define RTW_PHL_PSTS_FLTR_EXT_RSVD BIT(RTW_FRAME_TYPE_EXT_RSVD)
	u8 ppdu_sts_filter;
	u8 en_fake_psts;
	u8 cur_ppdu_cnt[HW_BAND_MAX];
#endif
#ifdef CONFIG_PHY_INFO_NTFY
#define MAX_PSTS_MSG_AGGR_NUM 10
	struct rtw_phl_ppdu_sts_ntfy msg_aggr_buf[MAX_PSTS_MSG_AGGR_NUM];
	u8 msg_aggr_cnt;
#endif
};

struct rtw_iot_t {
	u32 id;
};

struct rtw_phl_evt_ops;
struct rtw_phl_com_t {
	struct rtw_wifi_role_t wifi_roles[MAX_WIFI_ROLE_NUMBER];
	struct mr_ctl_t mr_ctrl; /*multi wifi_role control module*/
	struct rtw_phl_evt_ops evt_ops;
	enum rtw_hci_type hci_type;
	enum rtw_drv_mode drv_mode;/*normal or mp mode*/

	struct hal_spec_t hal_spec;

	struct role_sw_cap_t role_sw_cap; /* SW control capability of role for any purpose */
	struct protocol_cap_t proto_sw_cap[2]; /* SW control wifi protocol capability for any purpose */
	struct phy_sw_cap_t phy_sw_cap[2]; /* SW control phy capability for any purpose */
	struct phy_cap_t phy_cap[2]; /* final capability of phy (intersection of sw/hw) */

	struct dev_cap_t dev_sw_cap;
	struct dev_cap_t dev_cap; /* final capability of device (intersection of sw/hw) */

	struct bus_sw_cap_t bus_sw_cap; /* SW controlled bus capability */

	struct rtw_fw_info_t fw_info;
	struct rtw_evt_info_t evt_info;
	struct rtw_stats phl_stats;
	#ifdef CONFIG_PHL_DFS
	struct rtw_dfs_t dfs_info;
	#endif
	struct rtw_iot_t id;
	/* Flags to control/check RX packets */
	bool append_fcs;
	bool accept_icv_err;

	u8 tx_nss; /*tx Spatial Streams - GET_HAL_TX_NSS, get_min from registery and hal_spec*/
	u8 rx_nss; /*rx Spatial Streams - GET_HAL_RX_NSS, get_min from registery and hal_spec*/
	u8 rf_type; /*enum rf_type , is RF_PATH - GET_HAL_RFPATH*/
	u8 rf_path_num; /*GET_HAL_RFPATH_NUM*/
	u8 regulation;  /*regulation*/
	u8 edcca_mode;

	void *scan;
	void *tx;
	void *rx;
	void *phl_priv; /* pointer to phl_info */
	void *drv_priv;
	void *test_mgnt;
	void *wow_info;
	void *pkt_ofld;

	#ifdef CONFIG_PHL_CHANNEL_INFO
	struct chan_info_t chan_info; /* Handle channel info packet */
	#endif /* CONFIG_PHL_CHANNEL_INFO */

	struct rtw_phl_ppdu_sts_info ppdu_sts_info;
	struct rtw_phl_rssi_stat rssi_stat;
};

struct phl_sec_param_h {
	u8 keyid;
	u8 enc_type;
	u8 key_type;
	u8 key_len;
	u8 spp;
};

#define PHL_MAX_AGG_WSIZE 32

struct mp_usr_sw_tx_gen_in {
	u32 data_rate : 9;
	u32 mcs : 6;
	u32 mpdu_len : 14;
	u32 n_mpdu : 9;
	u32 fec : 1;
	u32 dcm : 1;
	u32 rsvd0 : 1;
	u32 aid : 12;
	u32 scrambler_seed : 8; // rand (1~255)
	u32 random_init_seed : 8; // rand (1~255)
	u32 rsvd1 : 4;
	u32 apep : 22;
	u32 ru_alloc : 8;
	u32 rsvd2 : 2;
	u32 nss : 4;
	u32 txbf : 1;
	u32 pwr_boost_db : 5;
	u32 rsvd3 : 22;
};


struct mp_sw_tx_param_t {
	u32 dbw : 2; //0:BW20, 1:BW40, 2:BW80, 3:BW160/BW80+80
	u32 source_gen_mode : 2;
	u32 locked_clk : 1;
	u32 dyn_bw : 1;
	u32 ndp_en : 1;
	u32 long_preamble_en : 1; //bmode
	u32 stbc : 1;
	u32 gi : 2; //0:0.4,1:0.8,2:1.6,3:3.2
	u32 tb_l_len : 12;
	u32 tb_ru_tot_sts_max : 3;
	u32 vht_txop_not_allowed : 1;
	u32 tb_disam : 1;
	u32 doppler : 2;
	u32 he_ltf_type : 2;//0:1x,1:2x,2:4x

	u32 ht_l_len : 12;
	u32 preamble_puncture : 1;
	u32 he_mcs_sigb : 3;//0~5
	u32 he_dcm_sigb : 1;
	u32 he_sigb_compress_en : 1;
	u32 max_tx_time_0p4us : 14;


	u32 ul_flag : 1;
	u32 tb_ldpc_extra : 1;
	u32 bss_color : 6;
	u32 sr : 4;
	u32 beamchange_en : 1;
	u32 he_er_u106ru_en : 1;
	u32 ul_srp1 : 4;
	u32 ul_srp2 : 4;
	u32 ul_srp3 : 4;
	u32 ul_srp4 : 4;
	u32 mode : 2;

	u32 group_id : 6;
	u32 ppdu_type : 4;//0: bmode,1:Legacy,2:HT_MF,3:HT_GF,4:VHT,5:HE_SU,6:HE_ER_SU,7:HE_MU,8:HE_TB
	u32 txop : 7;
	u32 tb_strt_sts : 3;
	u32 tb_pre_fec_padding_factor : 2;
	u32 cbw : 2;
	u32 txsc : 4;
	u32 tb_mumimo_mode_en : 1;
	u32 rsvd1 : 3;

	u8 nominal_t_pe : 2; // def = 2
	u8 ness : 2; // def = 0
	u8 rsvd2 : 4;

	u8 n_user;
	u16 tb_rsvd : 9;//def = 0
	u16 rsvd3 : 7;

	struct mp_usr_sw_tx_gen_in usr[4];
};

struct mp_usr_plcp_gen_in {
	u32 mcs : 6;
	u32 mpdu_len : 14;
	u32 n_mpdu : 9;
	u32 fec : 1;
	u32 dcm : 1;
	u32 rsvd0 : 1;
	u32 aid : 12;
	u32 scrambler_seed : 8; // rand (1~255)
	u32 random_init_seed : 8; // rand (1~255)
	u32 rsvd1 : 4;
	u32 apep : 22;
	u32 ru_alloc : 8;
	u32 rsvd2 : 2;
	u32 nss : 4;
	u32 txbf : 1;
	u32 pwr_boost_db : 5;
	u32 rsvd3 : 22;
};

enum pkt_ofld_type {
	PKT_TYPE_PROBE_RSP = 0,
	PKT_TYPE_PS_POLL = 1,
	PKT_TYPE_NULL_DATA = 2,
	PKT_TYPE_QOS_NULL = 3,
	PKT_TYPE_CTS2SELF = 4,
	PKT_TYPE_ARP_RSP = 5,
	PKT_TYPE_NDP = 6,
	PKT_TYPE_EAPOL_KEY = 7,
	PKT_TYPE_SA_QUERY = 8,
	PKT_OFLD_TYPE_MAX,
};

struct mp_plcp_param_t {
	u32 dbw : 2; //0:BW20, 1:BW40, 2:BW80, 3:BW160/BW80+80
	u32 source_gen_mode : 2;
	u32 locked_clk : 1;
	u32 dyn_bw : 1;
	u32 ndp_en : 1;
	u32 long_preamble_en : 1; //bmode
	u32 stbc : 1;
	u32 gi : 2; //0:0.4,1:0.8,2:1.6,3:3.2
	u32 tb_l_len : 12;
	u32 tb_ru_tot_sts_max : 3;
	u32 vht_txop_not_allowed : 1;
	u32 tb_disam : 1;
	u32 doppler : 2;
	u32 he_ltf_type : 2;//0:1x,1:2x,2:4x

	u32 ht_l_len : 12;
	u32 preamble_puncture : 1;
	u32 he_mcs_sigb : 3;//0~5
	u32 he_dcm_sigb : 1;
	u32 he_sigb_compress_en : 1;
	u32 max_tx_time_0p4us : 14;


	u32 ul_flag : 1;
	u32 tb_ldpc_extra : 1;
	u32 bss_color : 6;
	u32 sr : 4;
	u32 beamchange_en : 1;
	u32 he_er_u106ru_en : 1;
	u32 ul_srp1 : 4;
	u32 ul_srp2 : 4;
	u32 ul_srp3 : 4;
	u32 ul_srp4 : 4;
	u32 mode : 2;

	u32 group_id : 6;
	u32 ppdu_type : 4;//0: bmode,1:Legacy,2:HT_MF,3:HT_GF,4:VHT,5:HE_SU,6:HE_ER_SU,7:HE_MU,8:HE_TB
	u32 txop : 7;
	u32 tb_strt_sts : 3;
	u32 tb_pre_fec_padding_factor : 2;
	u32 cbw : 2;
	u32 txsc : 4;
	u32 tb_mumimo_mode_en : 1;
	u32 rsvd1 : 3;

	u8 nominal_t_pe : 2; // def = 2
	u8 ness : 2; // def = 0
	u8 rsvd2 : 4;

	u8 n_user;
	u16 tb_rsvd : 9;//def = 0
	u16 rsvd3 : 7;

	struct mp_usr_plcp_gen_in usr[4];
};



#define MP_MAC_AX_MAX_RU_NUM	4

struct mp_mac_ax_tf_depend_user_para {
	u8 pref_AC: 2;
	u8 rsvd: 6;
};

struct mp_mac_ax_tf_user_para {
	u16 aid12: 12;
	u16 ul_mcs: 4;
	u8 macid;
	u8 ru_pos;

	u8 ul_fec_code: 1;
	u8 ul_dcm: 1;
	u8 ss_alloc: 6;
	u8 ul_tgt_rssi: 7;
	u8 rsvd: 1;
	u16 rsvd2;
};


struct mp_mac_ax_tf_pkt_para {
	u8 ul_bw: 2;
	u8 gi_ltf: 2;
	u8 num_he_ltf: 3;
	u8 ul_stbc: 1;
	u8 doppler: 1;
	u8 ap_tx_power: 6;
	u8 rsvd0: 1;
	u8 user_num: 3;
	u8 pktnum: 3;
	u8 rsvd1: 2;
	u8 pri20_bitmap;

	struct mp_mac_ax_tf_user_para user[MP_MAC_AX_MAX_RU_NUM];
	struct mp_mac_ax_tf_depend_user_para dep_user[MP_MAC_AX_MAX_RU_NUM];
};

struct mp_mac_ax_tf_wd_para {
	u16 datarate: 9;
	u16 mulport_id: 3;
	u16 pwr_ofset: 3;
	u16 rsvd: 1;
};

struct mp_mac_ax_f2p_test_para {
	struct mp_mac_ax_tf_pkt_para tf_pkt;
	struct mp_mac_ax_tf_wd_para tf_wd;
	u8 mode: 2;
	u8 frexch_type: 6;
	u8 sigb_len;
};

struct mp_mac_ax_f2p_wd {
	/* dword 0 */
	u32 cmd_qsel:6;
	u32 rsvd0:2;
	u32 rsvd1:2;
	u32 ls:1;
	u32 fs:1;
	u32 total_number:4;
	u32 seq:8;
	u32 length:8;
	/* dword 1 */
	u32 rsvd2;
};

struct mp_mac_ax_f2p_tx_cmd {
	/* dword 0 */
	u32 cmd_type:8;
	u32 cmd_sub_type:8;
	u32 dl_user_num:5;
	u32 bw:2;
	u32 tx_power:9;
	/* dword 1 */
	u32 fw_define:16;
	u32 ss_sel_mode:2;
	u32 next_qsel:6;
	u32 twt_group:4;
	u32 dis_chk_slp:1;
	u32 ru_mu_2_su:1;
	u32 dl_t_pe:2;
	/* dword 2 */
	u32 sigb_ch1_len:8;
	u32 sigb_ch2_len:8;
	u32 sigb_sym_num:6;
	u32 sigb_ch2_ofs:5;
	u32 dis_htp_ack:1;
	u32 tx_time_ref:2;
	u32 pri_user_idx:2;
	/* dword 3 */
	u32 ampdu_max_txtime:14;
	u32 rsvd0:2;
	u32 group_id:6;
	u32 rsvd1:2;
	u32 rsvd2:4;
	u32 twt_chk_en:1;
	u32 twt_port_id:3;
	/* dword 4 */
	u32 twt_start_time:32;
	/* dword 5 */
	u32 twt_end_time:32;
	/* dword 6 */
	u32 apep_len:12;
	u32 tri_pad:2;
	u32 ul_t_pe:2;
	u32 rf_gain_idx:10;
	u32 fixed_gain_en:1;
	u32 ul_gi_ltf:3;
	u32 ul_doppler:1;
	u32 ul_stbc:1;
	/* dword 7 */
	u32 ul_mid_per:1;
	u32 ul_cqi_rrp_tri:1;
	u32 rsvd3:6;
	u32 rsvd4:8;
	u32 sigb_dcm:1;
	u32 sigb_comp:1;
	u32 doppler:1;
	u32 stbc:1;
	u32 mid_per:1;
	u32 gi_ltf_size:3;
	u32 sigb_mcs:3;
	u32 rsvd5:5;
	/* dword 8 */
	u32 macid_u0:8;
	u32 ac_type_u0:2;
	u32 mu_sta_pos_u0:2;
	u32 dl_rate_idx_u0:9;
	u32 dl_dcm_en_u0:1;
	u32 rsvd6:2;
	u32 ru_alo_idx_u0:8;
	/* dword 9 */
	u32 pwr_boost_u0:5;
	u32 agg_bmp_alo_u0:3;
	u32 ampdu_max_txnum_u0:8;
	u32 user_define_u0:8;
	u32 user_define_ext_u0:8;
	/* dword 10 */
	u32 ul_addr_idx_u0:8;
	u32 ul_dcm_u0:1;
	u32 ul_fec_cod_u0:1;
	u32 ul_ru_rate_u0:7;
	u32 rsvd8:7;
	u32 ul_ru_alo_idx_u0:8;
	/* dword 11 */
	u32 rsvd9:32;
	/* dword 12 */
	u32 macid_u1:8;
	u32 ac_type_u1:2;
	u32 mu_sta_pos_u1:2;
	u32 dl_rate_idx_u1:9;
	u32 dl_dcm_en_u1:1;
	u32 rsvd10:2;
	u32 ru_alo_idx_u1:8;
	/* dword 13 */
	u32 pwr_boost_u1:5;
	u32 agg_bmp_alo_u1:3;
	u32 ampdu_max_txnum_u1:8;
	u32 user_define_u1:8;
	u32 user_define_ext_u1:8;
	/* dword 14 */
	u32 ul_addr_idx_u1:8;
	u32 ul_dcm_u1:1;
	u32 ul_fec_cod_u1:1;
	u32 ul_ru_rate_u1:7;
	u32 rsvd12:7;
	u32 ul_ru_alo_idx_u1:8;
	/* dword 15 */
	u32 rsvd13:32;
	/* dword 16 */
	u32 macid_u2:8;
	u32 ac_type_u2:2;
	u32 mu_sta_pos_u2:2;
	u32 dl_rate_idx_u2:9;
	u32 dl_dcm_en_u2:1;
	u32 rsvd14:2;
	u32 ru_alo_idx_u2:8;
	/* dword 17 */
	u32 pwr_boost_u2:5;
	u32 agg_bmp_alo_u2:3;
	u32 ampdu_max_txnum_u2:8;
	u32 user_define_u2:8;
	u32 user_define_ext_u2:8;
	/* dword 18 */
	u32 ul_addr_idx_u2:8;
	u32 ul_dcm_u2:1;
	u32 ul_fec_cod_u2:1;
	u32 ul_ru_rate_u2:7;
	u32 rsvd16:7;
	u32 ul_ru_alo_idx_u2:8;
	/* dword 19 */
	u32 rsvd17:32;
	/* dword 20 */
	u32 macid_u3:8;
	u32 ac_type_u3:2;
	u32 mu_sta_pos_u3:2;
	u32 dl_rate_idx_u3:9;
	u32 dl_dcm_en_u3:1;
	u32 rsvd18:2;
	u32 ru_alo_idx_u3:8;
	/* dword 21 */
	u32 pwr_boost_u3:5;
	u32 agg_bmp_alo_u3:3;
	u32 ampdu_max_txnum_u3:8;
	u32 user_define_u3:8;
	u32 user_define_ext_u3:8;
	/* dword 22 */
	u32 ul_addr_idx_u3:8;
	u32 ul_dcm_u3:1;
	u32 ul_fec_cod_u3:1;
	u32 ul_ru_rate_u3:7;
	u32 rsvd20:7;
	u32 ul_ru_alo_idx_u3:8;
	/* dword 23 */
	u32 rsvd21:32;
	/* dword 24 */
	u32 pkt_id_0:12;
	u32 rsvd22:3;
	u32 valid_0:1;
	u32 ul_user_num_0:4;
	u32 rsvd23:12;
	/* dword 25 */
	u32 pkt_id_1:12;
	u32 rsvd24:3;
	u32 valid_1:1;
	u32 ul_user_num_1:4;
	u32 rsvd25:12;
	/* dword 26 */
	u32 pkt_id_2:12;
	u32 rsvd26:3;
	u32 valid_2:1;
	u32 ul_user_num_2:4;
	u32 rsvd27:12;
	/* dword 27 */
	u32 pkt_id_3:12;
	u32 rsvd28:3;
	u32 valid_3:1;
	u32 ul_user_num_3:4;
	u32 rsvd29:12;
	/* dword 28 */
	u32 pkt_id_4:12;
	u32 rsvd30:3;
	u32 valid_4:1;
	u32 ul_user_num_4:4;
	u32 rsvd31:12;
	/* dword 29 */
	u32 pkt_id_5:12;
	u32 rsvd32:3;
	u32 valid_5:1;
	u32 ul_user_num_5:4;
	u32 rsvd33:12;
};

u8 mp_start(void *priv);

enum hw_ps_cap {
	HWPS_CAP_PWROFF = BIT0,
	HWPS_CAP_PWR_GATED = BIT1,
	HWPS_CAP_CLK_GATED = BIT2,
	HWPS_CAP_RF_OFF = BIT3,
	HWPS_CAP_PWRON = BIT4,
	HWPS_CAP_MAX = BIT5,
};

enum pwr_cmd_comp {
	PWRCMD_COMP_IO_MAC = BIT0,
	PWRCMD_COMP_IO_BB = BIT1,
	PWRCMD_COMP_IO_RF = BIT2,
	PWRCMD_COMP_IO_AFE = BIT3,
	PWRCMD_COMP_IO_SIE = BIT4,
	PWRCMD_COMP_IO_EFUSE = BIT5,
	PWRCMD_COMP_IO_HCI = BIT6,
};

#ifdef CONFIG_DBCC_SUPPORT
enum dbcc_test_id {
	DBCC_PRE_CFG,
	DBCC_CFG,
	DBCC_CLEAN_TXQ,
};
#endif

struct role_ntfy_info {
	u8 role_id;
	u16 macid;
	enum role_state rstate;
};

struct battery_chg_ntfy_info {
	bool ips_allow;
	bool lps_allow;
};

struct ps_ntfy_info {
	bool sync;
	void *ctx;
	void (*cb)(void *phl, void *hdl, void *ctx, enum rtw_phl_status stat);
};

struct set_rf_ntfy_info {
	enum rtw_rf_state state_to_set;
	_os_event done;
};

#endif /*_PHL_DEF_H_*/
