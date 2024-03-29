/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
#ifndef _PHL_STRUCT_H_
#define _PHL_STRUCT_H_
#define PHL_MACID_MAX_ARRAY_NUM 8 /* 8x32=256 */
#define PHL_MACID_MAX_NUM (PHL_MACID_MAX_ARRAY_NUM * 32)

#define PHL_STA_TID_NUM (16)    /* TODO: */

struct hci_info_t {
	/* enum rtw_hci_type hci_type; */

#if defined(CONFIG_PCI_HCI)

	u8 total_txch_num;
	u8 total_rxch_num;
	u8 *txbd_buf;
	u8 *rxbd_buf;
#elif defined(CONFIG_USB_HCI)
	u16 usb_bulkout_size;
#elif defined(CONFIG_SDIO_HCI)
	u8 *txbuf;
	u32 txbuf_alloc_len;
	u32 txbuf_used_len;
	u8 dma_ch;
	u8 agg_count;
#define PHL_SDIO_TX_AGG_MAX	10
	u16 pkt_len[PHL_SDIO_TX_AGG_MAX];
	u8 wp_offset[PHL_SDIO_TX_AGG_MAX];
	u32 tx_drop_cnt;	/* bit31 means overflow or not */
#endif

	u8 *wd_ring;
	u8 *txbuf_pool;
	u8 *rxbuf_pool;
	u8 *wp_tag;
	u16 wp_seq[PHL_MACID_MAX_NUM]; 	/* maximum macid number */

};


#define MAX_PHL_RING_STATUS_NUMBER 64
#define RX_REORDER_RING_NUMBER PHL_MACID_MAX_NUM
#define PCIE_BUS_EFFICIENCY 4
#define ETH_ALEN 6

struct phl_ring_status {
	_os_list list;
	u16 macid;
	u16 req_busy;
	struct rtw_phl_tx_ring *ring_ptr;
};

struct phl_ring_sts_pool {
	struct phl_ring_status ring_sts[MAX_PHL_RING_STATUS_NUMBER];
	_os_list idle;
	_os_list busy;
	_os_lock idle_lock;
	_os_lock busy_lock;
};

/**
 * struct phl_hci_trx_ops - interface specific operations
 *
 * @hci_trx_init: the function for HCI trx init
 * @hci_trx_deinit: the function for HCI trx deinit
 * @prepare_tx: prepare packets for hal transmission
 * @recycle_rx_buf: recycle rx buffer
 * @tx: tx packet to hw
 * @rx: rx packet to sw
 */
struct phl_info_t;
struct phl_hci_trx_ops {
	enum rtw_phl_status (*hci_trx_init)(struct phl_info_t *phl);
	void (*hci_trx_deinit)(struct phl_info_t *phl);
	enum rtw_phl_status (*prepare_tx)(struct phl_info_t *phl,
					struct rtw_xmit_req *tx_req);
	enum rtw_phl_status (*recycle_rx_buf)(struct phl_info_t *phl,
					void *r, u8 ch, enum rtw_rx_type type);
	enum rtw_phl_status (*tx)(struct phl_info_t *phl);
	enum rtw_phl_status (*rx)(struct phl_info_t *phl);
	enum rtw_phl_status (*trx_cfg)(struct phl_info_t *phl);
	void (*trx_stop)(struct phl_info_t *phl);
	enum rtw_phl_status (*pltfm_tx)(struct phl_info_t *phl, void *pkt);
	void (*free_h2c_pkt_buf)(struct phl_info_t *phl_info,
				   struct rtw_h2c_pkt *_h2c_pkt);
	enum rtw_phl_status (*alloc_h2c_pkt_buf)(struct phl_info_t *phl_info,
		struct rtw_h2c_pkt *_h2c_pkt, u32 buf_len);
	void (*trx_reset)(struct phl_info_t *phl);
	void (*trx_resume)(struct phl_info_t *phl, u8 type);
	void (*req_tx_stop)(struct phl_info_t *phl);
	void (*req_rx_stop)(struct phl_info_t *phl);
	bool (*is_tx_pause)(struct phl_info_t *phl);
	bool (*is_rx_pause)(struct phl_info_t *phl);
	void *(*get_txbd_buf)(struct phl_info_t *phl);
	void *(*get_rxbd_buf)(struct phl_info_t *phl);
	void (*recycle_rx_pkt)(struct phl_info_t *phl,
			       struct rtw_phl_rx_pkt *phl_rx);
	enum rtw_phl_status (*register_trx_hdlr)(struct phl_info_t *phl);
	void (*rx_handle_normal)(struct phl_info_t *phl_info,
						struct rtw_phl_rx_pkt *phl_rx);
	void (*tx_watchdog)(struct phl_info_t *phl_info);

#ifdef CONFIG_PCI_HCI
	enum rtw_phl_status (*recycle_busy_wd)(struct phl_info_t *phl);
	enum rtw_phl_status (*recycle_busy_h2c)(struct phl_info_t *phl);
#endif

#ifdef CONFIG_USB_HCI
	enum rtw_phl_status (*pend_rxbuf)(struct phl_info_t *phl, void *rxobj,
						u32 inbuf_len, u8 status_code);
	enum rtw_phl_status (*recycle_tx_buf)(void *phl, u8 *tx_buf_ptr);
#endif

#ifdef CONFIG_SDIO_HCI
#endif
};

/**
 * struct phl_tid_ampdu_rx - TID aggregation information (Rx).
 *
 * @reorder_buf: buffer to reorder incoming aggregated MPDUs.
 * @reorder_time: time when frame was added
 * @sta: station we are attached to
 * @head_seq_num: head sequence number in reordering buffer.
 * @stored_mpdu_num: number of MPDUs in reordering buffer
 * @ssn: Starting Sequence Number expected to be aggregated.
 * @buf_size: buffer size for incoming A-MPDUs
 * @timeout: reset timer value (in TUs).
 * @tid: TID number
 * @started: this session has started (head ssn or higher was received)
 */
struct phl_tid_ampdu_rx {
	struct rtw_phl_rx_pkt **reorder_buf;
	u32 *reorder_time;
	struct rtw_phl_stainfo_t *sta;
	u16 head_seq_num;
	u16 stored_mpdu_num;
	u16 ssn;
	u16 buf_size;
	u16 tid;
	u8 started:1,
	   removed:1;

	void *drv_priv;
	struct phl_info_t *phl_info;
};

struct g6_macid_ctl_t {
	_os_lock lock;
	/*  used macid bitmap share for all wifi role */
	u32 used_map[PHL_MACID_MAX_ARRAY_NUM];
	/* record bmc macid bitmap for all wifi role */
	u32 bmc_map[PHL_MACID_MAX_ARRAY_NUM];
	/* record used macid bitmap for each wifi role */
	u32 wifi_role_usedmap[MAX_WIFI_ROLE_NUMBER][PHL_MACID_MAX_ARRAY_NUM];
	/* record bmc TX macid for wifi role */
	u16 wrole_bmc[MAX_WIFI_ROLE_NUMBER];
	/* record total stainfo by macid */
	struct rtw_phl_stainfo_t *sta[PHL_MACID_MAX_NUM];
	u16 max_num;
};

struct stainfo_ctl_t {
	struct phl_info_t *phl_info;
	u8 *allocated_stainfo_buf;
	int allocated_stainfo_sz;
	u8 *stainfo_buf;
	struct phl_queue free_sta_queue;
};

struct phl_h2c_pkt_pool {
	struct rtw_h2c_pkt *h2c_pkt_buf;
	struct phl_queue idle_h2c_pkt_cmd_list;
	struct phl_queue idle_h2c_pkt_data_list;
	struct phl_queue idle_h2c_pkt_ldata_list;
	struct phl_queue busy_h2c_pkt_list;
	_os_lock recycle_lock;
};

#ifdef CONFIG_RTW_ACS

#ifndef MAX_CHANNEL_NUM
#define	MAX_CHANNEL_NUM		42
#endif

struct auto_chan_sel {
	u8 clm_ratio[MAX_CHANNEL_NUM];
	u8 nhm_pwr[MAX_CHANNEL_NUM];
	u8 curr_idx;
	u16 chset[MAX_CHANNEL_NUM];
};
#endif

#define PHL_TX_STATUS_IDLE 0
#define PHL_TX_STATUS_RUNNING 1
#define PHL_TX_STATUS_STOP_INPROGRESS 2
#define PHL_TX_STATUS_SW_PAUSE 3
#define PHL_RX_STATUS_IDLE 0
#define PHL_RX_STATUS_RUNNING 1
#define PHL_RX_STATUS_STOP_INPROGRESS 2
#define PHL_RX_STATUS_SW_PAUSE 3
#define PHL_REQ_PAUSE_TX BIT0
#define PHL_REQ_PAUSE_RX BIT1


struct phl_info_t {
	struct g6_macid_ctl_t macid_ctrl;
	struct stainfo_ctl_t sta_ctrl;

	struct rtw_regulation regulation;

	struct rtw_phl_com_t *phl_com;
	struct rtw_phl_handler phl_tx_handler;
	struct rtw_phl_handler phl_rx_handler;
	struct rtw_phl_handler phl_event_handler;
	struct rtw_phl_rx_ring phl_rx_ring;
	_os_atomic phl_tx_status;
	_os_atomic phl_rx_status;
	_os_lock t_ring_list_lock;
	_os_lock rx_ring_lock;
	_os_lock t_fctrl_result_lock;
	_os_lock t_ring_free_list_lock;
	_os_list t_ring_list;
	_os_list t_fctrl_result;
	_os_list t_ring_free_list;
	void *ring_sts_pool;
	void *rx_pkt_pool;
	struct phl_h2c_pkt_pool  *h2c_pool;

	struct hci_info_t *hci;
	struct phl_hci_trx_ops *hci_trx_ops;

	struct phl_cmd_dispatch_engine disp_eng;
	void *msg_hub;
	void *cmd_que;
	void *hal;

	void *fsm_root;
	void *cmd_fsm;
	void *cmd_obj;

	void *scan_fsm;
	void *scan_obj;

	void *ser_fsm;
	void *ser_obj;

	void *btc_fsm;
	void *btc_obj;

	void *snd_fsm;
	void *snd_obj;

	void *ps_fsm;
	void *ps_obj;

	void *led_ctrl;

#ifdef PHL_RX_BATCH_IND
	u8 rx_new_pending;
#endif

#ifdef CONFIG_RTW_ACS
	struct auto_chan_sel acs;
#endif
};

#endif /*_PHL_STRUCT_H_*/
