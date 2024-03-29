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
#ifndef _HAL_API_H_
#define _HAL_API_H_

/******************* IO  APIs *******************/
u8 rtw_hal_read8(void *h, u32 addr);
u16 rtw_hal_read16(void *h, u32 addr);
u32 rtw_hal_read32(void *h, u32 addr);
void rtw_hal_write8(void *h, u32 addr, u8 val);
void rtw_hal_write16(void *h, u32 addr, u16 val);
void rtw_hal_write32(void *h, u32 addr, u32 val);

u32 rtw_hal_read_macreg(void *h, u32 offset, u32 bit_mask);
void rtw_hal_write_macreg(void *h,
			u32 offset, u32 bit_mask, u32 data);
u32 rtw_hal_g6_read_bbreg(void *h, u32 offset, u32 bit_mask);
void rtw_hal_g6_write_bbreg(void *h,
			u32 offset, u32 bit_mask, u32 data);
u32 rtw_hal_g6_read_rfreg(void *h,
			enum rf_path path, u32 offset, u32 bit_mask);
void rtw_hal_g6_write_rfreg(void *h,
			enum rf_path path, u32 offset, u32 bit_mask, u32 data);

#ifdef PHL_PLATFORM_LINUX
void rtw_hal_mac_reg_dump(void *sel, void *h);
void rtw_hal_bb_reg_dump(void *sel, void *h);
void rtw_hal_bb_reg_dump_ex(void *sel, void *h);
void rtw_hal_rf_reg_dump(void *sel, void *h);
#endif

enum rtw_hal_status
rtw_hal_get_addr_cam(void *h, u16 num, u8 *buf, u16 size);
enum rtw_hal_status
rtw_hal_get_sec_cam(void *h, u16 num, u8 *buf, u16 size);

void rtw_hal_g6_enable_interrupt(struct rtw_phl_com_t *phl_com, void *h);
void rtw_hal_g6_disable_interrupt(struct rtw_phl_com_t *phl_com, void *h);
void rtw_hal_config_interrupt(void *h, enum rtw_phl_config_int int_mode);
bool rtw_hal_recognize_interrupt(void *h);
bool rtw_hal_recognize_halt_c2h_interrupt(void *h);
void rtw_hal_clear_interrupt(void *h);
void rtw_hal_restore_interrupt(struct rtw_phl_com_t *phl_com, void *h);

u32 rtw_hal_interrupt_handler(void *h);
void rtw_hal_restore_rx_interrupt(void *h);

enum rtw_hal_status rtw_hal_get_pwr_state(void *h, u8 *pwr_state);

enum rtw_hal_status rtw_hal_init(void *drv_priv,
	struct rtw_phl_com_t *phl_com, void **hal, enum rtl_ic_id chip_id);
struct rtw_hal_com_t *rtw_hal_get_halcom(void *hal);
void rtw_hal_deinit(struct rtw_phl_com_t *phl_com, void *hal);

bool rtw_hal_is_inited(struct rtw_phl_com_t *phl_com, void *hal);

u32 rtw_hal_hci_cfg(struct rtw_phl_com_t *phl_com, void *hal,
					struct rtw_ic_info *ic_info);
u32 rtw_hal_g6_read_chip_info(struct rtw_phl_com_t *phl_com, void *hal);

void rtw_hal_set_default_var(void *hal);

u32 rtw_hal_var_init(struct rtw_phl_com_t *phl_com, void *hal);

u32
rtw_hal_ser_get_error_status(void *hal, u32 *err, bool *ignored);

u32
rtw_hal_ser_set_error_status(void *hal, u32 err);

u32
rtw_hal_lv1_rcvy(void *hal,      u32 step);

void
rtw_hal_dbg_dump_fw_rsvd_ple(void *phl);

enum rtw_hal_status rtw_hal_cfg_fw_ps_log(void *hal, u8 en);

enum rtw_hal_status
rtw_hal_download_fw(struct rtw_phl_com_t *phl_com, void *hal);

enum rtw_hal_status
rtw_hal_redownload_fw(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_fw_dbg_dump(void *hal, u8 is_low_power);

enum rtw_hal_status rtw_hal_preload(struct rtw_phl_com_t *phl_com, void *hal);
enum rtw_hal_status rtw_hal_start(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_g6_stop(struct rtw_phl_com_t *phl_com, void *hal);
enum rtw_hal_status rtw_hal_restart(struct rtw_phl_com_t *phl_com, void *hal);

#ifdef CONFIG_WOWLAN
enum rtw_hal_status rtw_hal_get_wake_rsn(void *hal, enum rtw_wow_wake_reason *wake_rsn);
enum rtw_hal_status rtw_hal_cfg_wow_sleep(void *hal, u8 sleep);
enum rtw_hal_status rtw_hal_get_wow_aoac_rpt(void *hal, struct rtw_aoac_report *aoac_info, u8 rx_ready);
enum rtw_hal_status rtw_hal_get_wow_fw_status(void *hal, u8 *status);
enum rtw_hal_status rtw_hal_wow_cfg_txdma(void *hal, u8 en);

enum rtw_hal_status rtw_hal_wow_start(struct rtw_phl_com_t *phl_com, void *hal, struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status rtw_hal_wow_stop(struct rtw_phl_com_t *phl_com, void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status rtw_hal_wow_func_en(struct rtw_phl_com_t *phl_com, void *hal, u16 macid, struct rtw_hal_wow_cfg *cfg);
enum rtw_hal_status rtw_hal_wow_func_dis(struct rtw_phl_com_t *phl_com, void *hal, u16 macid);

enum rtw_hal_status rtw_hal_set_wowlan(struct rtw_phl_com_t *phl_com, void *hal, u8 enter);

#endif /* CONFIG_WOWLAN */

void
 rtw_hal_dl_all_para_file(struct rtw_phl_com_t *phl_com, char *ic_name, void *hal);

enum rtw_hal_status rtw_hal_trx_init(void *hal, u8 *txbd_buf, u8 *rxbd_buf);
void rtw_hal_trx_deinit(void *hal);

enum rtw_hal_status
rtw_hal_role_cfg(void *hal, struct rtw_wifi_role_t *wrole);
enum rtw_hal_status
rtw_hal_role_cfg_ex(void *hal, struct rtw_wifi_role_t *wrole,
				enum pcfg_type type, void *param);
enum rtw_hal_status
rtw_hal_beacon_stop(void *hal, struct rtw_wifi_role_t *wrole, bool stop);

enum rtw_hal_status
rtw_hal_stainfo_init(void *hal, struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_stainfo_deinit(void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_add_sta_entry(void *hal, struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_del_sta_entry(void *hal, struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_update_sta_entry(void *hal, struct rtw_phl_stainfo_t *sta,
							bool is_connect);
enum rtw_hal_status
rtw_hal_change_sta_entry(void *hal, struct rtw_phl_stainfo_t *sta,
							enum phl_upd_mode mode);
u8 rtw_hal_get_sta_rssi(struct rtw_phl_stainfo_t *sta);

bool rtw_hal_is_sta_linked(void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_set_sta_rx_sts(struct rtw_phl_stainfo_t *sta, u8 rst,
									struct rtw_r_meta_data *meta);

enum rtw_hal_status
rtw_hal_start_ba_session(void *hal, struct rtw_phl_stainfo_t *sta,
                            u8 dialog_token, u16 timeout, u16 start_seq_num,
                            u16 ba_policy, u16 tid, u16 buf_size);
enum rtw_hal_status
rtw_hal_stop_ba_session(void *hal, struct rtw_phl_stainfo_t *sta, u16 tid);

/**
 * rtw_hal_set_edca() - setup WMM EDCA parameter
 * @hal:	struct hal_info_t *
 * @wrole:	struct rtw_wifi_role_t *
 * @ac:		Access Category, 0:BE, 1:BK, 2:VI, 3:VO
 * @param:	AIFS:BIT[7:0], CWMIN:BIT[11:8], CWMAX:BIT[15:12],
 *		TXOP:BIT[31:16]
 *
 * Setup WMM EDCA parameter set for sending packets.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status
rtw_hal_set_edca(void *hal, struct rtw_wifi_role_t *wrole, u8 ac, u32 edca);

enum rtw_hal_status
rtw_hal_cfg_tx_ampdu(void *hal, struct rtw_phl_stainfo_t *sta);


enum rtw_hal_status
rtw_hal_set_key(void *hal, struct rtw_phl_stainfo_t *sta, u8 type, u8 ext_key, u8 spp,
				u8 keyid, u8 keytype, u8 *keybuf);

u32
rtw_hal_search_key_idx(void *hal, struct rtw_phl_stainfo_t *sta,
						u8 keyid, u8 keytype);
/**
 * rtw_hal_tx_chnl_mapping - query hw tx dma channel mapping to the sw xmit ring
 * identified by macid, tid and band
 * @hal: see struct hal_info_t
 * @macid: input target macid is 0 ~ 127
 * @cat: input target packet category, see enum rtw_phl_ring_cat
 * @band: input target band, 0 for band 0 / 1 for band 1
 *
 * returns the mapping hw tx dma channel
 */
u8 rtw_hal_tx_chnl_mapping(void *hal, u16 macid,
			   enum rtw_phl_ring_cat cat, u8 band);


/**
 * rtw_hal_convert_qsel_to_tid - convert qsel to tid value
 * @hal: see struct hal_info_t
 * @qsel: HW queue selection
 * @tid_indic: tid indicator
 *
 * returns enum RTW_HAL_STATUS
 */
u8 rtw_hal_convert_qsel_to_tid(void *hal, u8 qsel_id, u8 tid_indic);


/**
 * rtw_hal_tx_res_query - query current HW tx resource with specifc dma channel
 * @hal: see struct hal_info_t
 * @dma_ch: the target dma channel
 * @host_idx: current host index of this channel
 * @hw_idx: current hw index of this channel
 *
 * this function returns the number of available tx resource
 * NOTE, input host_idx and hw_idx ptr shall NOT be NULL
 */
u16 rtw_hal_tx_res_query(void *hal, u8 dma_ch, u16 *host_idx, u16 *hw_idx);

/**
 * rtw_hal_rx_res_query - query current HW rx resource with specifc dma channel
 * @hal: see struct hal_info_t
 * @dma_ch: the target dma channel
 * @host_idx: current host index of this channel
 * @hw_idx: current hw index of this channel
 *
 * this function returns the number of available tx resource
 * NOTE, input host_idx and hw_idx ptr shall NOT be NULL
 */
u16 rtw_hal_rx_res_query(void *hal, u8 dma_ch, u16 *host_idx, u16 *hw_idx);


/**
 * rtw_hal_query_txch_num - query total hw tx dma channels number
 *
 * returns the number of  hw tx dma channel
 */
u8 rtw_hal_query_txch_num(void *hal);

/**
 * rtw_hal_query_rxch_num - query total hw rx dma channels number
 *
 * returns the number of  hw rx dma channel
 */
u8 rtw_hal_query_rxch_num(void *hal);


/**
 * rtw_hal_update_wd_page - update wd page for xmit packet
 * @hal: see struct hal_info_t
 * @phl_pkt_req: packet xmit request from phl, see struct rtw_phl_pkt_req
 *
 * returns enum RTW_HAL_STATUS
 */
enum rtw_hal_status rtw_hal_update_wd_page(void *hal, void *phl_pkt_req);

enum rtw_hal_status
rtw_hal_handle_rx_buffer(struct rtw_phl_com_t *phl_com, void* hal,
				u8 *buf, u32 buf_size,
				struct rtw_phl_rx_pkt *rxpkt);

enum rtw_hal_status
rtw_hal_query_info(void* hal, u8 info_id, void *value);

#ifdef CONFIG_PHL_DFS
bool rtw_hal_in_radar_domain(void *hal, u8 ch, enum channel_width bw);
enum rtw_hal_status rtw_hal_radar_detect_cfg(void *hal, bool dfs_enable);
#endif /*CONFIG_PHL_DFS*/

enum rtw_hal_status
rtw_hal_set_ch_bw(void *hal, u8 band_idx,
		u8 chan, enum channel_width bw, enum chan_offset offset,
		bool do_rfk);
u8 rtw_hal_get_cur_ch(void *hal, u8 band_idx);
void rtw_hal_sync_cur_ch(void *hal, u8 band_idx, struct rtw_chan_def chandef);

u8 rtw_hal_get_fwcmd_queue_idx(void* hal);
void rtw_hal_cfg_dma_io(void *hal, u8 en);
void rtw_hal_cfg_txdma(void *hal, u8 en, u8 dma_ch);
void rtw_hal_cfg_txhci(void *hal, u8 en);
void rtw_hal_cfg_rxhci(void *hal, u8 en);
void rtw_hal_clear_rwptr(void *hal);
void rtw_hal_rst_bdram(void *hal);
u8 rtw_hal_poll_txdma_idle(void *hal);
void rtw_hal_cfg_rsvd_ctrl(void *hal);
enum rtw_hal_status rtw_hal_chk_allq_empty(void *hal, u8 *empty);

#ifdef CONFIG_RTW_ACS
void rtw_hal_acs_mntr_trigger(void *hal, u16 monitor_time);
enum rtw_hal_status rtw_hal_acs_mntr_result(void *hal, void *rpt);
#endif /* CONFIG_RTW_ACS */

#ifdef CONFIG_PCI_HCI
/**
 * rtw_hal_update_txbd - update tx bd for xmit packet
 * @hal: see struct hal_info_t
 * @txbd: the target txbd to update
 * @wd: buffer pointer of wd page to fill in txbd
 *
 * returns enum RTW_HAL_STATUS
 * NOTE, this function is PCIe specific function
 */
enum rtw_hal_status
rtw_hal_update_txbd(void *hal, void *txbd, void *wd, u8 dma_ch, u16 wd_num);

/**
 * rtw_hal_update_rxbd - update rx bd for recv packet
 * @hal: see struct hal_info_t
 * @rxbd: the target rxbd to update
 * @wd: buffer pointer of wd page to fill in rxbd
 *
 * returns enum RTW_HAL_STATUS
 * NOTE, this function is PCIe specific function
 */
enum rtw_hal_status
rtw_hal_update_rxbd(void *hal, struct rx_base_desc *rxbd,
					struct rtw_rx_buf *rxbuf);

/**
 * rtw_hal_update_trigger_txstart - trigger hw to start tx
 * @hal: see struct hal_info_t
 * @txbd: the target txbd to update
 * @dma_ch: the dma channel index of this txbd_ring
 *
 * returns enum RTW_HAL_STATUS
 */
enum rtw_hal_status rtw_hal_trigger_txstart(void *hal, void *txbd, u8 dma_ch);

/**
 * rtw_hal_notify_rxdone - notify hw rx done
 * @hal: see struct hal_info_t
 * @rxbd: the target rxbd to update
 * @ch: the target rx channel
 * @rxcnt: handling done rx count this function will notify hw
 *
 * returns enum RTW_HAL_STATUS
 */
enum rtw_hal_status rtw_hal_notify_rxdone(void* hal, void *rxbd, u8 ch,
										u16 rxcnt);

/**
 * rtw_hal_handle_wp_rpt -parsing the wp recycle report packet
 * @hal: see struct hal_info_t
 * @rp: input, the pointer of target recycle report buffer to parse
 * @len: input, the length of this report buffer
 * @sw_retry: output, the packet mentioned in this report needs sw retry if sw_retry == 1
 * @dma_ch: output, the tx dma channel of this packet mentioned in this report
 * @wp_seq: output, the wp_seq of this packet mentioned in this report
 * @txsts: output, the tx status of this packet mentioned in this report
 *
 * returns the length of report buffer which has been parsed in this function
 */
u16 rtw_hal_handle_wp_rpt(void *hal, u8 *rp, u16 len, u8 *sw_retry, u8 *dma_ch,
			  u16 *wp_seq, u8 *txsts);

void rtw_hal_query_freerun(void *hal, u32 *freerun_l,
							u32 *freerun_h);

u8 rtw_hal_check_rxrdy(struct rtw_phl_com_t *phl_com, void* hal, u8 *rxbuf, u8 dma_ch);
u8 rtw_hal_handle_rxbd_info(void* hal, u8 *rxbuf, u16 *buf_size);
enum rtw_hal_status rtw_hal_set_l2_leave(void *hal);

#endif /*CONFIG_PCI_HCI*/

#ifdef CONFIG_USB_HCI

u8 rtw_hal_get_bulkout_id(void *hal, u8 dma_ch, u8 mode);

enum rtw_hal_status rtw_hal_fill_wd(void *hal,
				struct rtw_xmit_req *tx_req,
				u8 *wd_buf, u32 *wd_len);

enum rtw_hal_status
	rtw_hal_usb_tx_agg_cfg(void *hal, u8* wd_buf, u8 agg_num);

enum rtw_hal_status
	rtw_hal_usb_rx_agg_cfg(void *hal, u8 mode, u8 agg_mode,
	u8 drv_define, u8 timeout, u8 size, u8 pkt_num);

u8 rtw_hal_get_max_bulkout_wd_num(void *hal);
u32 rtwl_hal_get_cur_usb_mode(void *h);
u32 rtwl_hal_get_usb_support_ability(void *h);
enum rtw_hal_status rtw_hal_force_usb_switch(void *h, enum usb_type type);
#endif

#ifdef CONFIG_SDIO_HCI
/**
 * rtw_hal_sdio_tx_cfg - SDIO TX related setting
 * @hal:	pointer of struct hal_info_t
 *
 * Configure setting for SDIO TX.
 *
 * No return value for this function.
 */
void rtw_hal_sdio_tx_cfg(void *hal);

enum rtw_hal_status rtw_hal_sdio_tx(void *hal, u8 dma_ch, u8 *buf, u32 buf_len,
				    u8 agg_count, u16 *pkt_len, u8 *wp_offset);
void rtw_hal_sdio_rx_agg_cfg(void *hal, bool enable, u8 drv_define,
			     u8 timeout, u8 size, u8 pkt_num);
int rtw_hal_sdio_rx(void *hal, struct rtw_rx_buf *rxbuf);
int rtw_hal_sdio_parse_rx(void *hal, struct rtw_rx_buf *rxbuf);
#endif /* CONFIG_SDIO_HCI */

/* HAL SOUND API */
enum rtw_hal_status rtw_hal_snd_query_proc_sta_res(
	void *hal, struct rtw_phl_stainfo_t *sta,
	bool mu, enum channel_width bw, bool en_swap);

enum rtw_hal_status
rtw_hal_snd_release_proc_sta_res(void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_snd_proc_pre_cfg_sta(void *hal, struct rtw_phl_stainfo_t *sta);

void
rtw_hal_snd_ndpa_sta_info_vht(struct rtw_phl_stainfo_t *psta_info,
			      u32 *ndpa, u8 mu);

void
rtw_hal_snd_ndpa_sta_info_he(struct rtw_phl_stainfo_t *psta_info,
			     u32 *ndpa, enum channel_width bw, u8 fb_type);

enum rtw_hal_status
rtw_hal_snd_proc_post_cfg_sta(void *hal,
				struct rtw_phl_stainfo_t *sta, bool mu);

enum rtw_hal_status
rtw_hal_snd_proc_post_cfg_gid(void *hal, u8 gid, void *ba_info);

enum rtw_hal_status
rtw_hal_snd_proc_post_cfg(void *hal, bool he, bool mu, bool en_fixed_mode);

enum rtw_hal_status
rtw_hal_snd_mac_ctrl(void *hal, u8 band, u8 ctrl);

enum rtw_hal_status
rtw_hal_snd_chk_bf_res(void *hal, struct rtw_phl_stainfo_t *sta,
		       bool mu, enum channel_width bw);

void
rtw_hal_snd_polling_snd_sts(void *hal, struct rtw_phl_stainfo_t *sta);

/* fw sounding commmand ralated */
void rtw_hal_snd_set_fw_cmd_dialogtkn(void *hal, u8 *buf, u8 he, u8 token);

void rtw_hal_snd_vht_fwcmd_su(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u32 *npda_sta);

void rtw_hal_snd_vht_fwcmd_mu_pri(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u8 sta_nr, u32 *ndpa_sta);

void rtw_hal_snd_vht_fwcmd_mu_add_sta(void *hal, u8 *buf, u32 *ndpa_sta,
				      struct rtw_phl_stainfo_t *sta,
				      u8 ndpa_idx, u8 last);

void rtw_hal_snd_ax_fwcmd_nontb(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u32 *npda_sta);

void rtw_hal_snd_ax_fwcmd_tb_pri(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u8 sta_nr1, u8 sta_nr2);

void rtw_hal_snd_ax_fwcmd_tb_add_sta(void *hal, u8 *buf, u32 *ndpa_sta,
				     struct rtw_phl_stainfo_t *sta, u8 ru_idx,
				     u8 ndpa_idx, u8 bfrp_idx, u8 bfrp_u_idx);

u8 *rtw_hal_snd_prepare_snd_cmd(void *hal);
enum rtw_hal_status rtw_hal_snd_release_snd_cmd(void *hal, u8 *buf);
enum rtw_hal_status rtw_hal_snd_send_fw_cmd(void *hal, u8 *cmd);

/*HAL CSI Buffer Mgnt APIs*/
enum channel_width
rtw_hal_get_csi_buf_bw(void *buf);

bool
rtw_hal_get_csi_buf_type(void *buf);

/*HAL Beamform Mgnt APIs*/
void rtw_hal_bf_dbg_dump_entry(void *entry);
void rtw_hal_bf_dbg_dump_entry_all(void *hal);
u8 rtw_hal_bf_get_sumu_idx(void *hal, void *entry);
bool rtw_hal_bf_chk_bf_type(void *hal_info,
			struct rtw_phl_stainfo_t *sta, bool mu);
void rtw_hal_bf_preset_mu_ba_info(void *hal,
			struct rtw_phl_stainfo_t *psta, void *hal_ba_info);
void rtw_hal_bf_set_txmu_para(void *hal, u8 gid , u8 en,
			      enum rtw_hal_protection_type rts_type,
			      enum rtw_hal_ack_resp_type ack_type);
enum rtw_hal_status
rtw_hal_bf_set_fix_mode(void *hal, bool mu, bool he);
enum rtw_hal_status
rtw_hal_bf_get_entry_snd_sts(void *entry);

/******************************************************************************
 *
 * BTC APIs
 *
 *****************************************************************************/
#ifdef CONFIG_BTCOEX
void rtw_hal_btc_update_role_info_ntfy(void *hinfo,  u8 role_id,
						struct rtw_wifi_role_t *wrole,
						struct rtw_phl_stainfo_t *sta,
						enum role_state rstate);
void rtw_hal_btc_specific_packet_ntfy(void *hinfo, u8 pkt_type);
void rtw_hal_btc_radio_state_ntfy(void *hinfo, bool rf_on);
void rtw_hal_btc_customerize_ntfy(void *hinfo, u8 type, u16 len, u8 *buf);
void rtw_hal_btc_wl_status_ntfy(void *hinfo, struct rtw_phl_com_t *phl_com, u8 ntfy_num,
					struct rtw_phl_stainfo_t *sta[],
									u8 reason);
void rtw_hal_btc_timer(void *hinfo);
void rtw_hal_btc_fwinfo_ntfy(void *hinfo);
#endif

enum rtw_hal_status rtw_hal_notify_scan_start(void *hinfo,
						enum phl_phy_idx phy_idx);
enum rtw_hal_status rtw_hal_notify_scan_complete(void *hinfo,
						enum phl_phy_idx phy_idx);
enum rtw_hal_status rtw_hal_scan_set_rxfltr_by_mode(void *hinfo,
	enum phl_phy_idx phy_idx, bool off_channel, u8 *mode);
enum rtw_hal_status rtw_hal_scan_flush_queue(void *hinfo,
	struct rtw_wifi_role_t *wrole);
enum rtw_hal_status rtw_hal_scan_pause_tx_fifo(void *hinfo,
	u8 band_idx, bool off_ch);

enum rtw_hal_status rtw_hal_dfs_pause_tx(void *hinfo,
	u8 band_idx, bool off_ch);
/*****************************************************************************/

#ifdef RTW_PHL_BCN
enum rtw_hal_status
rtw_hal_add_beacon(struct rtw_phl_com_t *phl_com, void *hal, void *bcn_cmn);

enum rtw_hal_status
rtw_hal_update_beacon(struct rtw_phl_com_t *phl_com, void *hal, u8 bcn_id);

enum rtw_hal_status
rtw_hal_free_beacon(struct rtw_phl_com_t *phl_com, void *hal, u8 bcn_id);
#endif

enum rtw_hal_status
rtw_hal_proc_cmd(void *hal, char proc_cmd, struct rtw_proc_cmd *incmd,
							char *output, u32 out_len);

void rtw_hal_get_fw_ver(void *hal, char *ver_str, u16 len);

enum rtw_hal_status
rtw_hal_tx_pause(struct rtw_hal_com_t *hal_com,
			u8 band_idx, bool tx_pause, enum tx_pause_rson rson);

bool rtw_hal_is_macid_pause(struct rtw_hal_com_t *hal_com, u16 macid);

enum rtw_hal_status
rtw_hal_set_macid_pause(struct rtw_hal_com_t *hal_com, u16 macid, bool pause);

/**
 * rtw_hal_set_rxfltr_by_mode - Set rx filter option by scenario
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @mode:	scenario mode
 *
 * Set RX filter setting by scenario.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_set_rxfltr_by_mode(void *hal, u8 band,
					       enum rtw_rx_fltr_mode mode);

/**
 * rtw_hal_get_rxfltr_mode - Get rx filter option
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 *
 * Get RX filter mode
 *
 * Return rtw_rx_fltr_mode
 */
enum rtw_rx_fltr_mode rtw_hal_get_rxfltr_mode(void *hal, u8 band);

/**
 * rtw_hal_mac_acpt_crc_err_pkt - Accept CRC error packets or not
 * @hal:	pointer of struct rtw_hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @enable:	0: deny, 1: accept
 *
 * Control accepting CRC error packets or not.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_acpt_crc_err_pkt(void *hal, u8 band, u8 enable);

/**
 * rtw_hal_set_rxfltr_mpdu_size - Set max MPDU size
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @size:	MPDU max size, unit: byte. 0 for no limit.
 *
 * MPDU size exceed max size would be dropped.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_set_rxfltr_mpdu_size(void *hal, u8 band, u16 size);
enum rtw_hal_status rtw_hal_set_rxfltr_by_type(void *hal, u8 band, u8 type,u8 target);


enum rtw_hal_status
rtw_hal_fw_log_cfg(void *hal, u8 op, u8 type, u32 value);

enum rtw_hal_status
rtw_hal_dbg_status_dump(void *hal, u32 *val, u8 *en);
/******************************************************************************
 *
 * Packet Offload APIs
 *
 *****************************************************************************/
enum rtw_hal_status rtw_hal_reset_pkt_ofld_state(void *hal);

enum rtw_hal_status rtw_hal_pkt_ofld(void *hal, u8 *id, u8 op,
					u8 *pkt, u16 *len);
enum rtw_hal_status rtw_hal_pkt_update_ids(void *hal,
					struct pkt_ofld_entry *entry);
void rtw_hal_fw_cap_pre_config(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_bus_cap_pre_config(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_fw_final_cap_config(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_final_cap_decision(struct rtw_phl_com_t *phl_com, void *hal);

/******************************************************************************
 *
 * Power Save APIs
 *
 *****************************************************************************/

u8 rtw_hal_ps_pwr_lvl_judge(void *hal, u8 ps_cap, u32 cmd_comp);
enum rtw_hal_status
rtw_hal_ps_pwr_lvl_cfg(struct rtw_phl_com_t *phl_com, void *hal, u8 current_pwr_lvl, u32 target_pwr_lvl);
enum rtw_hal_status rtw_hal_ps_lps_cfg(void *hal, void *lps);

enum rtw_hal_status
rtw_hal_get_bf_proto_cap(struct rtw_phl_com_t *phl_com, void *hal,
			 u8 band, struct protocol_cap_t *proto_cap);

enum rtw_hal_status rtw_hal_rf_chl_rfk_trigger(void *hal, u8 phy_idx, u8 force);

enum rtw_hal_status rtw_hal_watchdog(void *hal);
enum rtw_hal_status rtw_hal_ps_lps_chk_access(void *hal, u32 offset);


/******************************************************************************
 *
 * BB APIs
 *
 *****************************************************************************/
/*@--------------------------[Prptotype]-------------------------------------*/
/**
 * rtw_hal_bb_get_txsc input arguments:
 * @hal_com: hal com info
 * @pri_ch: Spec-defined primary channel index
 * @central_ch: Spec-defined central channel index
 * @cbw: Channel BW
 * @dbw: Data BW
 */
u8 rtw_hal_bb_get_txsc(struct rtw_hal_com_t *hal_com, u8 pri_ch,
		u8 central_ch, enum channel_width cbw, enum channel_width dbw);


#ifdef CONFIG_DBCC_SUPPORT
enum rtw_hal_status
rtw_hal_dbcc_pre_cfg(void *hal, struct rtw_phl_com_t *phl_com, bool dbcc_en);

enum rtw_hal_status
rtw_hal_dbcc_cfg(void *hal, struct rtw_phl_com_t *phl_com, bool dbcc_en);

enum rtw_hal_status
rtw_hal_clean_tx_queue(void *hal);

enum rtw_hal_status
rtw_hal_dbcc_tx_cnt(void *hal, struct rtw_phl_com_t *phl_com, u8 band);

enum rtw_hal_status
rtw_hal_dbcc_reset_tx_cnt(void *hal, struct rtw_phl_com_t *phl_com, u8 band);
#endif

/*
 * API for config channel info CR
 */
#ifdef CONFIG_PHL_CHANNEL_INFO

enum rtw_hal_status
rtw_hal_cfg_chinfo(void *hal, struct rtw_phl_stainfo_t *sta);

#endif /* CONFIG_PHL_CHANNEL_INFO */

enum rtw_hal_status
rtw_hal_get_efuse_info(void *hal, enum rtw_efuse_info info_type,
		       void *value, u8 size);

enum rtw_hal_status
rtw_hal_cfg_trx_path(void *hal, enum rf_path tx, u8 tx_nss,
		     enum rf_path rx, u8 rx_nss);

enum rtw_hal_status
rtw_hal_tsf_sync(void *hal, u8 wrole_sync_from, u8 wrole_sync_to,
			enum phl_band_idx band, s32 sync_offset_tu,
			enum hal_tsf_sync_act act);

/**
 * rtw_hal_fill_txdesc() - Fill hardware tx header
 * @hal:	pointer of struct hal_info_t
 * @treq:	the xmit request for this tx descriptor
 * @wd_buf:	the wd buffer to fill
 * @wd_len:	output, return the total length of filled wd
 *
 * Fill hardware tx header/tx descriptor/wifi descriptor
 *
 * Return RTW_HAL_STATUS_SUCCESS when everything is ok.
 */
enum rtw_hal_status rtw_hal_fill_txdesc(void *hal, struct rtw_xmit_req *treq,
					u8 *wd_buf, u32 *wd_len);
#ifdef RTW_WKARD_DYNAMIC_BFEE_CAP
enum rtw_hal_status rtw_hal_bf_bfee_ctrl(void *hal, u8 band, bool ctrl);
#endif


enum rtw_hal_status
rtw_hal_set_mu_edca(void *hal, u8 band, u8 ac,
	u16 timer, u8 cw_min, u8 cw_max, u8 aifsn);
enum rtw_hal_status
rtw_hal_set_mu_edca_ctrl(void *hal, u8 band, u8 wmm, u8 set);

enum rtw_hal_status rtw_hal_led_set_ctrl_mode(void *hal, enum rtw_led_id led_id,
					      enum rtw_led_ctrl_mode ctrl_mode);
enum rtw_hal_status rtw_hal_led_control(void *hal, enum rtw_led_id led_id,
					u8 high);

enum rtw_hal_status rtw_hal_pcie_trx_mit(void *hal, u32 tx_timer, u8 tx_counter,
					 u32 rx_timer, u8 rx_counter);

enum rtw_hal_status rtw_hal_get_tsf(void *hal, u8 port, u32 *tsf_h, u32 *tsf_l);

#ifdef CONFIG_MCC_SUPPORT
enum rtw_hal_status rtw_hal_mcc_get_2ports_tsf(void *hal, u8 group,
			u16 macid_x, u16 macid_y, u32 *tsf_x_h, u32 *tsf_x_l,
			u32 *tsf_y_h, u32 *tsf_y_l);

enum rtw_hal_status rtw_hal_mcc_update_macid_bitmap(void *hal, u8 group,
			u16 macid, struct rtw_phl_mcc_macid_bitmap *info);

enum rtw_hal_status rtw_hal_mcc_sync_enable(void *hal,
					struct rtw_phl_mcc_en_info *info);

enum rtw_hal_status rtw_hal_mcc_set_duration(void *hal,
					struct rtw_phl_mcc_en_info *info);

enum rtw_hal_status rtw_hal_mcc_disable(void *hal, u8 group, u16 macid);

enum rtw_hal_status rtw_hal_mcc_enable(void *hal, struct rtw_phl_mcc_en_info *info);
#endif /* CONFIG_MCC_SUPPORT */

#endif /*_HAL_API_H_*/
