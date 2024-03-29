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
#ifndef _RTW_PHL_H_
#define _RTW_PHL_H_

//typedef struct rtw_phl_com_t HAL_DATA_TYPE; /*, *PHAL_DATA_TYPE;*/

#define GET_HAL_INFO(_dvobj)	(_dvobj->phl)
#define GET_PHL_DATA(_dvobj)	(_dvobj->phl_com)
//#define GET_HAL_SPEC(_dvobj)	(&(GET_HAL_DATA(_dvobj)->hal_spec))


/* refer to (registrypriv-> tx_nss,rx_nss / hal_spec->tx_nss_num,rx_nss_num)*/
//#define GET_HAL_TX_NSS(_dvobj) ((GET_HAL_DATA(_dvobj))->tx_nss)
//#define GET_HAL_RX_NSS(_dvobj) ((GET_HAL_DATA(_dvobj))->rx_nss)
//#define GET_HAL_RFPATH_NUM(_dvobj) ((GET_HAL_DATA(_dvobj))->rf_path_num)
/* refer to (hal_data->version_id.RFType / registrypriv->rf_path / 8814a from efuse or registrypriv)*/
//#define GET_HAL_RFPATH(_dvobj) ((GET_HAL_DATA(_dvobj))->rf_type)

#define GET_WIFI_ROLE_CURRENT_CH(_adapter) (_adapter->phl_role->chandef.ch)
#define WIFI_ROLE_IS_ON_24G(_adapter) (_adapter->phl_role->chandef.band == BAND_ON_24G)
#define WIFI_ROLE_IS_ON_5G(_adapter) (_adapter->phl_role->chandef.band == BAND_ON_5G)
#define WIFI_ROLE_IS_ON_6G(_adapter) (_adapter->phl_role->chandef.band == BAND_ON_6G)

#define RTW_LITEXMITBUF_NR 256
#define RTW_XMITURB_NR 256
#define RTW_LITERECVBUF_NR 8
#define RTW_RECVURB_NR 8
#define RTW_INTINBUF_NR 1
#define RTW_INTINURB_NR 1

s8 rtw_phl_rssi_to_dbm(u8 rssi);

void rtw_hw_dump_hal_spec(void *sel, struct dvobj_priv *dvobj);
void rtw_dump_phl_sta_info(void *sel, struct sta_info *sta);

bool rtw_hw_chk_band_cap(struct dvobj_priv *dvobj, u8 cap);
bool rtw_hw_chk_bw_cap(struct dvobj_priv *dvobj, u8 cap);
bool rtw_hw_chk_proto_cap(struct dvobj_priv *dvobj, u8 cap);
bool rtw_hw_chk_wl_func(struct dvobj_priv *dvobj, u8 func);
bool rtw_hw_is_band_support(struct dvobj_priv *dvobj, u8 band);
bool rtw_hw_is_band_support(struct dvobj_priv *dvobj, u8 bw);
bool rtw_hw_is_wireless_mode_support(struct dvobj_priv *dvobj, u8 mode);
u8 rtw_hw_get_wireless_mode(struct dvobj_priv *dvobj);
u8 rtw_hw_get_band_type(struct dvobj_priv *dvobj);
u8 rtw_hw_get_mac_addr(struct dvobj_priv *dvobj, u8 *hw_mac_addr);

bool rtw_hw_is_mimo_support(struct dvobj_priv *dvobj);
u8 rtw_hw_largest_bw(struct dvobj_priv *dvobj, u8 in_bw);
u8 rtw_hw_init(struct dvobj_priv *dvobj);
void rtw_hw_deinit(struct dvobj_priv *dvobj);


u8 rtw_hw_start(struct dvobj_priv *dvobj);
void rtw_hw_stop(struct dvobj_priv *dvobj);
bool rtw_hw_get_init_completed(struct dvobj_priv *dvobj);
bool rtw_hw_is_init_completed(struct dvobj_priv *dvobj);
void rtw_hw_cap_init(struct dvobj_priv *dvobj);

u8 rtw_hw_iface_init(_adapter *adapter);
u8 rtw_hw_iface_type_change(_adapter *adapter, u8 iface_type);
void rtw_hw_iface_deinit(_adapter *adapter);

/* security */
u8 rtw_sec_algo_drv2phl(enum security_type drv_algo);
int rtw_hw_add_key(struct _ADAPTER *a, struct sta_info *sta,
		   u8 keyid, enum security_type keyalgo, u8 keytype, u8 *key);
int rtw_hw_del_key(struct _ADAPTER *a, struct sta_info *sta,
		   u8 keyid, u8 keytype);
int rtw_hw_del_all_key(struct _ADAPTER *a, struct sta_info *sta);

/* settting */
int rtw_hw_set_ch_bw(struct _ADAPTER *a,
			u8 ch, enum channel_width bw, u8 offset, u8 do_rfk);
int rtw_hw_set_edca(struct _ADAPTER *a, u8 ac, u32 param);

/* connect */
int rtw_hw_prepare_connect(struct _ADAPTER *a, struct sta_info *sta, u8 *target_addr);
int rtw_hw_start_bss_network(struct _ADAPTER *a);
int rtw_hw_connect_abort(struct _ADAPTER *a, struct sta_info *sta);
int rtw_hw_connected(struct _ADAPTER *a, struct sta_info *sta);
int rtw_hw_connected_apmode(struct _ADAPTER *a, struct sta_info *sta);
int rtw_hw_disconnect(struct _ADAPTER *a, struct sta_info *sta);

void rtw_hw_update_chan_def(_adapter *adapter);

#ifdef CONFIG_RTW_ACS
u16 rtw_acs_get_channel_by_idx(struct _ADAPTER *a, u8 idx);
u8 rtw_acs_get_clm_ratio_by_idx(struct _ADAPTER *a, u8 idx);
s8 rtw_noise_query_by_idx(struct _ADAPTER *a, u8 idx);
#endif /* CONFIG_RTW_ACS */
#endif /* _RTW_HW_H_ */
