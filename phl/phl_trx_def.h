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
#ifndef __PHL_TRX_DEF_H_
#define __PHL_TRX_DEF_H_

/* core / phl common structrue */

#define MAX_PHL_RING_ENTRY_NUM 4096
#define MAX_PHL_RING_CAT_NUM 10 /* 8 tid + 1 mgnt + 1 hiq*/
#define MAX_PHL_RING_RX_PKT_NUM 8192
#define MAX_RX_BUF_SEG_NUM 4

#define _H2CB_CMD_QLEN 32
#define _H2CB_DATA_QLEN 32
#define _H2CB_LONG_DATA_QLEN 200 /* should be refined */
#define MAX_H2C_PKT_NUM (_H2CB_CMD_QLEN + _H2CB_DATA_QLEN + _H2CB_LONG_DATA_QLEN)

#define FWCMD_HDR_LEN 8
#define _WD_BODY_LEN 24
#define H2C_CMD_LEN 64
#define H2C_DATA_LEN 256
#define H2C_LONG_DATA_LEN 2048

#define get_h2c_size_by_range(i) \
	((i < _H2CB_CMD_QLEN) ? \
	(FWCMD_HDR_LEN + _WD_BODY_LEN + H2C_CMD_LEN) : \
	((i < (_H2CB_CMD_QLEN + _H2CB_DATA_QLEN)) ? \
	(FWCMD_HDR_LEN + _WD_BODY_LEN + H2C_DATA_LEN) : \
	(FWCMD_HDR_LEN + _WD_BODY_LEN + H2C_LONG_DATA_LEN)))

struct rtw_h2c_pkt {
	_os_list list;
	u8 *vir_head; /* should not reset */
	u8 *vir_data;
	u8 *vir_end;
	u8 *vir_tail;
	u8 type;
	u32 id; /* h2c id */
	u32 buf_len;
	u32 data_len;

	u32 phy_addr_l;
	u32 phy_addr_h;
	u8 cache;
	u16 host_idx;
	u8 h2c_seq; /* h2c seq */
};

/**
 * the category of phl ring
 */
enum rtw_phl_ring_cat {
	RTW_PHL_RING_CAT_TID0 = 0,
	RTW_PHL_RING_CAT_TID1 = 1,
	RTW_PHL_RING_CAT_TID2 = 2,
	RTW_PHL_RING_CAT_TID3 = 3,
	RTW_PHL_RING_CAT_TID4 = 4,
	RTW_PHL_RING_CAT_TID5 = 5,
	RTW_PHL_RING_CAT_TID6 = 6,
	RTW_PHL_RING_CAT_TID7 = 7,
	RTW_PHL_RING_CAT_MGNT = 8,
	RTW_PHL_RING_CAT_HIQ = 9,
	RTW_PHL_RING_CAT_MAX = 0xff
};


/**
 * @RTW_PHL_TREQ_TYPE_PHL_UPDATE_TXSC:
 *    this is for phl tx shortcut entry to update
 * @RTW_PHL_TREQ_TYPE_CORE_TXSC:
 *    it means this txreq is a shortcut pkt, so it need a txsc recycle
 * @RTW_PHL_TREQ_TYPE_PHL_ADD_TXSC:
 *    it means this txreq is a new cache in core layer and also need cache
 *    in phl layer
*/

enum rtw_treq_type {
#if defined(CONFIG_CORE_TXSC) || defined(CONFIG_PHL_TXSC)
	RTW_PHL_TREQ_TYPE_PHL_UPDATE_TXSC = 0x80,
#endif
	RTW_PHL_TREQ_TYPE_NORMAL = 0,
	RTW_PHL_TREQ_TYPE_TEST_PATTERN = 1,
#if defined(CONFIG_CORE_TXSC) || defined(CONFIG_PHL_TXSC)
	RTW_PHL_TREQ_TYPE_CORE_TXSC = 2,
	RTW_PHL_TREQ_TYPE_PHL_ADD_TXSC = 3,
#endif
	RTW_PHL_TREQ_TYPE_MAX = 0xFF
};


enum rtw_packet_type {
	RTW_PHL_PKT_TYPE_DATA = 0,
	RTW_PHL_PKT_TYPE_MGNT = 1,
	RTW_PHL_PKT_TYPE_H2C = 2,
	RTW_PHL_PKT_TYPE_CTRL = 3
};

struct rtw_t_meta_data {
/* temp elements */
	u8 *ta;
	u8 *ra;
	u8 da[6];
	u8 sa[6];
	u8 to_ds;
	u8 from_ds;
	u8 band;
	u8 type;

	u8 hw_seq_mode;		/* DW0 [0:1] */
	u8 hw_ssn_sel;		/* DW0 [2:3] */
	u8 smh_en;		/* DW0 [4:4] */
	u8 hw_amsdu;		/* DW0 [5:5] */
	u8 hw_aes_iv;		/* DW0 [6:6] */
	u8 wd_page_size;	/* DW0 [7:7] */
	u8 chk_en;		/* DW0 [8:8] */
	u8 wp_int;		/* DW0 [9:9] */
	u8 stf_mode;		/* DW0 [10:10] */
	u8 hdr_len;		/* DW0 [11:15] if data frame : hdr with llc len */
	u8 dma_ch;		/* DW0 [16:19] */
	u8 usb_pkt_ofst;	/* DW0 [21:21] */
	u8 wdinfo_en;		/* DW0 [22:22] */
	u8 more_data;		/* DW0 [23:23] */
	u8 wp_offset;		/* DW0 [24:31] */

	u8 shcut_camid;		/* DW1 [0:7] */
	u8 usb_txagg_num;	/* DW1 [8:15] */

	u16 pktlen;		/* DW2 [0:13] MAC header length + frame body length */
	u8 q_sel;		/* DW2 [17:22] */
	u8 tid;			/* DW2 [23:23] */
	u16 macid;		/* DW2 [24:30] */

	u16 sw_seq;		/* DW3 [0:11] */
	u8 ampdu_en;		/* DW3 [12:12] */
	u8 bk;			/* DW3 [13:13] */

	u8 mbssid;		/* DW6 [0:3] */
	u8 hal_port;		/* DW6 [4:6] */
	u8 data_bw_er;		/* DW6 [8:8] */
	u8 dis_rts_rate_fb;	/* DW6 [9:9] */
	u8 dis_data_rate_fb;	/* DW6 [10:10] */
	u8 f_ldpc;		/* DW6 [11:11] valid if userate_sel = 1 */
	u8 f_stbc;		/* DW6 [12:13] valid if userate_sel = 1 */
	u8 f_dcm;		/* DW6 [14:14] valid if userate_sel = 1 */
	u8 f_er;		/* DW6 [15:15] valid if userate_sel = 1 */
	u16 f_rate;		/* DW6 [16:24] valid if userate_sel = 1 */
	u8 f_gi_ltf;		/* DW6 [25:27] valid if userate_sel = 1 */
	u8 f_bw;		/* DW6 [28:29] valid if userate_sel = 1 */
	u8 userate_sel;		/* DW6 [30:30] */
	u8 ack_ch_info;		/* DW6 [31:31] */

	u8 max_agg_num;		/* DW7 [0:7] */
	u8 bcn_srch_seq;	/* DW7 [8:9] */
	u8 nav_use_hdr;		/* DW7 [10:10] */
	u8 bc;			/* DW7 [11:11] */
	u8 mc;			/* DW7 [11:11] */
	u8 a_ctrl_bqr;		/* DW7 [12:12] */
	u8 a_ctrl_uph;		/* DW7 [13:13] */
	u8 a_ctrl_bsr;		/* DW7 [14:14] */
	u8 a_ctrl_cas;		/* DW7 [15:15] */
	u8 data_rty_lowest_rate;/* DW7 [16:24] */
	u8 data_tx_cnt_lmt;	/* DW7 [25:30] */
	u8 data_tx_cnt_lmt_en;	/* DW7 [31:31] */

	u8 sec_cam_idx;		/* DW8 [0:7] */
	u8 sec_hw_enc;		/* DW8 [8:8] */
	u8 sec_type;		/* DW8 [9:12] */
	u8 life_time_sel;	/* DW8 [13:15] */
	u8 force_txop;		/* DW8 [17:17] */
	u8 ampdu_density;	/* DW8 [18:20] */
	u8 lsig_txop_en;	/* DW8 [21:21] */
	u8 txpwr_ofset_type;	/* DW8 [22:24] */
	u8 no_ack;				/* DW8 [25:25] */
	u8 obw_cts_dup_type;	/* DW8 [26:29] */

	u8 signaling_ta_pkt_en;	/* DW9 [0:0] */
	u8 ndpa;		/* DW9 [1:2] */
	u8 snd_pkt_sel;		/* DW9 [3:5] */
	u8 sifs_tx;		/* DW9 [6:6] */
	u8 ht_data_snd;		/* DW9 [7:7] */
	u8 rtt_en;		/* DW9 [9:9] */
	u8 spe_rpt;		/* DW9 [10:10] */
	u8 bt_null;		/* DW9 [11:11] */
	u8 wmm_tri_frame;	/* DW9 [12:12] */
	u8 null_1;		/* DW9 [13:13] */
	u8 null_0;		/* DW9 [14:14] */
	u8 raw;			/* DW9 [15:15] */
	u8 group_bit_ie_ofst;	/* DW9 [16:23] */
	u8 bcn_tsf_ctrl;	/* DW9 [26:26] */
	u8 signaling_ta_pkt_sc;	/* DW9 [27:30] */
	u8 force_bss_clr;	/* DW9 [31:31] */

	u8 sw_define;		/* DW10 [0:3] */
	u8 rts_en;		/* DW10 [27:27] */
	u8 cts2self;		/* DW10 [28:28] */
	u8 rts_cca_mode;	/* DW10 [29:30] */
	u8 hw_rts_en;		/* DW10 [31:31] */

	u16 ndpa_dur;		/* DW11 [16:31] */
};


/**
 * packet recv information
 */
struct rtw_r_meta_data {
	u8 dma_ch;
	u8 hal_port;
	u8 ta[6]; /* Transmitter Address */

	u16 pktlen;		/* DW0 [0:13] */
	u8 shift;		/* DW0 [14:15] */
	u8 wl_hd_iv_len;	/* DW0 [16:21] */
	u8 bb_sel;		/* DW0 [22:22] */
	u8 mac_info_vld;	/* DW0 [23:23] */
	u8 rpkt_type;		/* DW0 [24:27] */
	u8 drv_info_size;	/* DW0 [28:30] */
	u8 long_rxd;		/* DW0 [31:31] */

	u8 ppdu_type;		/* DW1 [0:3] */
	u8 ppdu_cnt;		/* DW1 [4:6] */
	u8 sr_en;		/* DW1 [7:7] */
	u8 user_id;		/* DW1 [8:15] */
	u16 rx_rate;		/* DW1 [16:24] */
	u8 rx_gi_ltf;		/* DW1 [25:27] */
	u8 non_srg_ppdu;	/* DW1 [28:28] */
	u8 inter_ppdu;		/* DW1 [29:29] */
	u8 bw;			/* DW1 [30:31] */

	u32 freerun_cnt;	/* DW2 [0:31] */

	u8 a1_match;		/* DW3 [0:0] */
	u8 sw_dec;		/* DW3 [1:1] */
	u8 hw_dec;		/* DW3 [2:2] */
	u8 ampdu;		/* DW3 [3:3] */
	u8 ampdu_end_pkt;	/* DW3 [4:4] */
	u8 amsdu;		/* DW3 [5:5] */
	u8 amsdu_cut;		/* DW3 [6:6] */
	u8 last_msdu;		/* DW3 [7:7] */
	u8 bypass;		/* DW3 [8:8] */
	u8 crc32;		/* DW3 [9:9] */
	u8 icverr;		/* DW3 [10:10] */
	u8 magic_wake;		/* DW3 [11:11] */
	u8 unicast_wake;	/* DW3 [12:12] */
	u8 pattern_wake;	/* DW3 [13:13] */
	u8 get_ch_info;		/* DW3 [14:15] */
	u8 pattern_idx;		/* DW3 [16:20] */
	u8 target_idc;		/* DW3 [21:23] */
	u8 chksum_ofld_en;	/* DW3 [24:24] */
	u8 with_llc;		/* DW3 [25:25] */
	u8 rx_statistics;	/* DW3 [26:26] */

	u8 frame_type;		/* DW4 [0:1] */
	u8 mc;			/* DW4 [2:2] */
	u8 bc;			/* DW4 [3:3] */
	u8 more_data;		/* DW4 [4:4] */
	u8 more_frag;		/* DW4 [5:5] */
	u8 pwr_bit;		/* DW4 [6:6] */
	u8 qos;			/* DW4 [7:7] */
	u8 tid;			/* DW4 [8:11] */
	u8 eosp;		/* DW4 [12:12] */
	u8 htc;			/* DW4 [13:13] */
	u8 q_null;		/* DW4 [14:14] */
	u16 seq;		/* DW4 [16:27] */
	u8 frag_num;		/* DW4 [28:31] */

	u8 sec_cam_idx;		/* DW5 [0:7] */
	u8 addr_cam;		/* DW5 [8:15] */
	u16 macid;		/* DW5 [16:23] */
	u8 rx_pl_id;		/* DW5 [24:27] */
	u8 addr_cam_vld;	/* DW5 [28:28] */
	u8 addr_fwd_en;		/* DW5 [29:29] */
	u8 rx_pl_match;		/* DW5 [30:30] */

	u8 mac_addr[6];		/* DW6 [0:31] DW7 [0:15] */
	u8 smart_ant;		/* DW7 [16:16] */
	u8 sec_type;		/* DW7 [17:20] */
};


/**
 * rtw_pkt_buf_list -- store pakcet from upper layer(ex. ndis, kernel, ethernet..)
 * @vir_addr: virtual address of this packet
 * @phy_addr_l: lower 32-bit physical address of this packet
 * @phy_addr_h: higher 32-bit physical address of this packet
 * @length: length of this packet
 * @type: tbd
 */
struct rtw_pkt_buf_list {
	u8 *vir_addr;
	u32 phy_addr_l;
	u32 phy_addr_h;
	u16 length;
};

/**
 * the xmit request from core layer, store in xmit phl ring
 * @list: list
 * @os_priv: the private context from core layer
 * @mdata: see structure rtw_t_meta_data
 * @tx_time: xmit requset tx time, unit in ms
 * @shortcut_id: short cut id this packet will use in phl/hal
 * @total_len: the total length of pkt_list
 * @pkt_cnt: the packet number of pkt_list
 * @pkt_list: see structure rtw_pkt_buf_list
 *
 * Note, this structure are visible to core, phl and hal layer
 */
struct rtw_xmit_req {
	_os_list list;
	void *os_priv;
	enum rtw_treq_type treq_type;
	struct rtw_t_meta_data mdata;
	u32 tx_time;
	u8 shortcut_id;
	u32 total_len;
	u8 pkt_cnt;
	u8 *pkt_list;
};

/**
 * the recv packet to core layer, store in recv phl ring
 * @os_priv: the private context from core layer
 * @mdata: see structure rtw_r_meta_data
 * @shortcut_id: short cut id this packet will use in phl/hal
 * @pkt_cnt: the packet counts of pkt_list
 * @rx_role: the role to which the RX packet is targeted
 * @tx_sta: the phl sta that sends this packet
 * @pkt_list: see structure rtw_pkt_buf_list
 *
 * Note, this structure are visible to core, phl and hal layer
 */
struct rtw_recv_pkt {
	void *os_priv;
	struct rtw_r_meta_data mdata;
	u8 shortcut_id;
	u8 pkt_cnt;
	struct rtw_wifi_role_t *rx_role;
	struct rtw_phl_stainfo_t *tx_sta;
	struct rtw_pkt_buf_list pkt_list[MAX_RX_BUF_SEG_NUM];
	struct rtw_phl_ppdu_phy_info phy_info;
};


/**
 * the phl ring which stores XMIT requests can be access by both
 * core and phl, and all the requests in this ring have the same TID value
 * @tid: the TID value of this phl ring
 * @dma_ch: dma channel of this phl ring, query by rtw_hal_tx_chnl_mapping()
 * @band: band of this phl ring, 0 for band 0 / 1 for band 1
 * @tx_thres: tx threshold of this phl ring for batch handling tx requests
 * @core_idx: record the index of latest entry accessed by core layer
 * @phl_idx: record the index of handling done by phl layer
 * @phl_next_idx: record the index of latest entry accessed by phl layer
 * @entry: store the pointer of requests assigned to this phl ring
 */
struct rtw_phl_tx_ring {
	u8 tid;
	u8 dma_ch;
	u8 band;
	u16 tx_thres;
	u16 core_idx;
	_os_atomic phl_idx;
	_os_atomic phl_next_idx;
	u8 *entry[MAX_PHL_RING_ENTRY_NUM];/* change to dynamic allocation */
};

/**
 * this structure stores sorted tx rings having frames to tx to the same sta
 * it will change everytime _phl_check_tring_list() executed
 * @list: link to the next sta which has frames to transmit
 * @sleep: true if this macid is under power-saving mode
 * @has_mgnt: true if this macid has management frames to transmit
 * @has_hiq: true if this macid has hiq frames to transmit
 * @sorted_ring: pre-sorted phl ring status list of this macid
 */
struct phl_tx_plan {
	_os_list list;
	bool sleep;
	bool has_mgnt;
	bool has_hiq;
	_os_list sorted_ring;
};

/**
 * this phl ring list contains a list of phl TX rings that have the same macid
 * and different tid, and it can be access by both core and phl
 * @list: link to next phl ring list with other macid
 * @macid: the MACID value of this phl ring list
 * @phl_ring: the phl rings with same macid but different tid, see rtw_phl_tx_ring
 * @tx_plan: transmission plan for this macid, decide by _phl_check_tring_list()
 */
struct rtw_phl_tring_list {
	_os_list list;
	u16 macid;
	struct rtw_phl_tx_ring phl_ring[MAX_PHL_RING_CAT_NUM];/* tid 0~7, 8:mgnt, 9:hiq */
	struct phl_tx_plan tx_plan;
};

/**
 * this phl RX ring can be access by both core and phl
 * @core_idx: record the index of latest entry accessed by core layer
 * @phl_idx: record the index of handling done by phl layer
 * @entry: store the pointer of requests assigned to this phl ring
 */
struct rtw_phl_rx_ring {
	_os_atomic core_idx;
	_os_atomic phl_idx;
	struct rtw_recv_pkt *entry[MAX_PHL_RING_ENTRY_NUM];/* change to dynamic allocation */
};


/**
 * the physical address list
 */
struct rtw_phy_addr_list {
	_os_list list;
	u32 phy_addr_l;
	u32 phy_addr_h;
};

/**
 * the phl pkt tx request from phl layer to hal layer
 * @wd_page: the buffer of wd page allocated by phl and filled by hal
 * @wd_len: the phl tx shortcut cached wd_page length, if wd_len = 0 means no phl txsc
 * @wp_seq: pcie only, wp sequence of this phl packet request
 * @wp_num: the number of wp address contain in this phl packet request
 * @tx_req: see struct rtw_xmit_req
 *
 * Note, this structure should be visible to phl and hal layer (hana_todo)
 */
struct rtw_phl_pkt_req {
	u8 *wd_page;
	u8 wd_len;
	u16 wp_seq;
	u8 wp_num;
	struct rtw_xmit_req *tx_req;
};

/*
0000: WIFI packet
0001: PPDU status
0010: channel info
0011: BB scope mode
0100: F2P TX CMD report
0101: SS2FW report
0110: TX report
0111: TX payload release to host
1000: DFS report
1001: TX payload release to WLCPU
1010: C2H packet */
enum rtw_rx_type {
	RTW_RX_TYPE_WIFI = 0,
	RTW_RX_TYPE_PPDU_STATUS = 1,
	RTW_RX_TYPE_CHANNEL_INFO = 2,
	RTW_RX_TYPE_TX_RPT = 3,
	RTW_RX_TYPE_TX_WP_RELEASE_HOST = 4,
	RTW_RX_TYPE_DFS_RPT = 5,
	RTW_RX_TYPE_C2H = 6,
	RTW_RX_TYPE_MAX = 0xFF
};

struct rtw_phl_rx_pkt {
	_os_list list;
	enum rtw_rx_type type;
	u8 *rxbuf_ptr;
	struct rtw_recv_pkt r;
};


struct rtw_xmit_recycle {
	u16 wp_seq;
	struct rtw_xmit_req *tx_req;
};

enum rtw_traffic_dir {
	TRAFFIC_UL = 0, /* Uplink */
	TRAFFIC_DL, /* Downlink */
	TRAFFIC_BALANCE,
	TRAFFIC_MAX
};

enum rtw_rx_fltr_mode {
	RX_FLTR_MODE_SNIFFER,		/* 0 */
	RX_FLTR_MODE_SCAN,
	RX_FLTR_MODE_STA_LINKING,
	RX_FLTR_MODE_STA_NORMAL,
	RX_FLTR_MODE_AP_NORMAL,
};

#endif	/* __PHL_TRX_DEF_H_ */
