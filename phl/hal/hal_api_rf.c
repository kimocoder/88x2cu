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
#define _HAL_API_RF_C_
#include "hal_headers.h"

#if 0
#include "phy/rf/halrf_api.h"
#include "phy/rf/halrf_export_fun.h"
#endif

#ifdef USE_TRUE_PHY

#if 0 // NEO TODO 

enum rtw_hal_status
rtw_hal_rf_init(struct rtw_phl_com_t *phl_com,
			struct hal_info_t *hal_info)
{
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = halrf_init(phl_com, hal_com, &(hal_info->rf));

	if ((hal_status != RTW_HAL_STATUS_SUCCESS) ||
		(hal_info->rf == NULL)) {
		PHL_ERR("[PHL] rtw_halrf_init failed status(%d), hal_info->rf(%p)\n",
			hal_status, hal_info->rf);
	}

	return hal_status;
}

void rtw_hal_rf_deinit(struct rtw_phl_com_t *phl_com,
			struct hal_info_t *hal_info)
{
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	halrf_deinit(phl_com, hal_com, hal_info->rf);
}

void rtw_hal_init_rf_reg(struct rtw_phl_com_t *phl_com, void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_para_info_t *phy_reg_info = NULL;
	struct rtw_para_pwrlmt_info_t *pwrlmt_info = NULL;
	u8 phy_idx = 0;

	for (phy_idx = 0; phy_idx < 1 ; phy_idx++) {
		phy_reg_info = &phl_com->phy_sw_cap[phy_idx].rf_radio_a_info;
		halrf_config_radio_a_reg(hal_info->rf, phy_reg_info->para_src,
				phy_reg_info->para_data_len, phy_reg_info->para_data);
	}

	for (phy_idx = 0; phy_idx < 1 ; phy_idx++) {
		phy_reg_info = &phl_com->phy_sw_cap[phy_idx].rf_radio_b_info;
		halrf_config_radio_b_reg(hal_info->rf , phy_reg_info->para_src,
				phy_reg_info->para_data_len, phy_reg_info->para_data);
	}

	for (phy_idx = 0; phy_idx < 1 ; phy_idx++) {
		phy_reg_info = &phl_com->phy_sw_cap[phy_idx].rf_txpwr_byrate_info;
		halrf_config_store_power_by_rate(hal_info->rf , phy_reg_info->para_src,
				phy_reg_info->para_data_len, phy_reg_info->para_data);
	}
	for (phy_idx = 0; phy_idx < 1 ; phy_idx++) {
			phy_reg_info = &phl_com->phy_sw_cap[phy_idx].rf_txpwrtrack_info;
			halrf_config_store_power_track(hal_info->rf , phy_reg_info->para_src,
					phy_reg_info->para_data_len, phy_reg_info->para_data);
	}

	for (phy_idx = 0; phy_idx < 1 ; phy_idx++) {
		pwrlmt_info = &phl_com->phy_sw_cap[phy_idx].rf_txpwrlmt_info;
		halrf_config_store_power_limit(hal_info->rf , pwrlmt_info->para_src,
				pwrlmt_info->para_data_len, pwrlmt_info->para_data);
	}

	for (phy_idx = 0; phy_idx < 1 ; phy_idx++) {
		pwrlmt_info = &phl_com->phy_sw_cap[phy_idx].rf_txpwrlmt_ru_info;
		halrf_config_store_power_limit_ru(hal_info->rf , pwrlmt_info->para_src,
				pwrlmt_info->para_data_len, pwrlmt_info->para_data);
	}
}

void rtw_hal_rf_dm_init(struct hal_info_t *hal_info)
{
	halrf_dm_init(hal_info->rf);
}

enum rtw_hal_status
rtw_hal_rf_get_efuse_info(struct rtw_hal_com_t *hal_com,
	u8 *efuse_map, enum rtw_efuse_info info_type, void *value,
	u8 size, u8 map_valid)
{
	struct hal_info_t *hal_info = hal_com->hal_priv;

	PHL_INFO("%s\n", __FUNCTION__);

	if(halrf_get_efuse_info(hal_info->rf, efuse_map, info_type, value, size,
							map_valid))
		return RTW_HAL_STATUS_SUCCESS;
	else
		return RTW_HAL_STATUS_FAILURE;
}

enum rtw_hal_status rtw_hal_rf_read_pwr_table(
	struct rtw_hal_com_t *hal_com, u8 rf_path, u16 rate,
	u8 bandwidth, u8 channel, u8 offset, u8 dcm,
	u8 beamforming, s16 *get_item)
{
	int ret = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal_com->hal_priv;

	PHL_INFO("[MP HAL API] %s \n", __FUNCTION__);

	*get_item = halrf_get_power(hal_info->rf, rf_path, rate, dcm,offset, bandwidth, beamforming, channel);

	return ret;
}

enum rtw_hal_status rtw_hal_rf_wl_tx_power_control(struct rtw_hal_com_t *hal_com,
	u32 tx_power_val)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal_com->hal_priv;

	PHL_INFO("%s\n", __FUNCTION__);

	halrf_wl_tx_power_control(hal_info->rf, tx_power_val);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_pwrtrack(struct hal_info_t *hal_info, u8 *txpwr_track_status)
{
	int ret = RTW_HAL_STATUS_SUCCESS;

	PHL_INFO("[MP HAL API] %s \n", __FUNCTION__);

	*txpwr_track_status = halrf_get_power_track(hal_info->rf);

	return ret;
}

enum rtw_hal_status rtw_hal_rf_set_pwrtrack(struct hal_info_t *hal_info, u8 phy_idx, u8 txpwr_track_status)
{
	int ret = RTW_HAL_STATUS_SUCCESS;

	PHL_INFO("[MP HAL API] %s \n", __FUNCTION__);

	halrf_set_power_track(hal_info->rf, phy_idx, txpwr_track_status);

	return ret;
}

enum rtw_hal_status rtw_hal_rf_get_thermal(struct hal_info_t *hal_info, u8 rf_path, u8 *thermal)
{
	int ret = RTW_HAL_STATUS_SUCCESS;

	PHL_INFO("[MP HAL API] %s \n", __FUNCTION__);

	*thermal = halrf_get_thermal(hal_info->rf, rf_path);

	return ret;
}

enum rtw_hal_status rtw_hal_rf_set_tssi(struct hal_info_t *hal_info, u8 phy_idx, u8 rf_path, u32 tssi_de)
{
	int ret = RTW_HAL_STATUS_SUCCESS;

	PHL_INFO("[MP HAL API] %s \n", __FUNCTION__);

	halrf_set_tssi_de_for_tx_verify(hal_info->rf, phy_idx, tssi_de, rf_path);

	return ret;

}

enum rtw_hal_status rtw_hal_rf_set_tssi_offset(struct hal_info_t *hal_info, u8 phy_idx, u32 tssi_de_offset, u8 rf_path)
{
	int ret = RTW_HAL_STATUS_SUCCESS;

	PHL_INFO("[MP HAL API] %s \n", __FUNCTION__);

	halrf_set_tssi_de_offset(hal_info->rf, phy_idx, tssi_de_offset, rf_path);

	return ret;

}

enum rtw_hal_status rtw_hal_rf_get_tssi(struct hal_info_t *hal_info, u8 phy_idx, u8 rf_path, u32 *tssi_de)
{
	int ret = RTW_HAL_STATUS_SUCCESS;

	PHL_INFO("[MP HAL API] %s \n", __FUNCTION__);

	*tssi_de = halrf_get_tssi_de(hal_info->rf, phy_idx, rf_path);

	return ret;
}

enum rtw_hal_status rtw_hal_rf_get_online_tssi_de(struct hal_info_t *hal_info, u8 phy_idx, u8 rf_path,s32 dbm, s32 pout, s32 *tssi_de)
{
	int ret = RTW_HAL_STATUS_SUCCESS;

	PHL_INFO("[MP HAL API] %s \n", __FUNCTION__);

	*tssi_de = halrf_get_online_tssi_de(hal_info->rf, phy_idx, rf_path, dbm, pout);

	return ret;
}

enum rtw_hal_status
rtw_hal_rf_set_continuous_tx(struct hal_info_t *hal_info)
{
	PHL_INFO("%s\n", __FUNCTION__);
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_set_singletone_tx(struct hal_info_t *hal_info,
						u8 is_on,enum rf_path path)
{
	PHL_INFO("%s: enable = %d path = %x\n", __FUNCTION__, is_on, path);
	halrf_lo_test(hal_info->rf, is_on, path);
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_set_suppression_tx(struct hal_info_t *hal_info)
{
	PHL_INFO("%s\n", __FUNCTION__);
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_ctrl_dbcc(struct rtw_hal_com_t *hal_com,
	bool dbcc_en)
{
	/*need replace with rf api*/
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal_com->hal_priv;

	if (halrf_set_dbcc(hal_info->rf, dbcc_en))
		hal_status = RTW_HAL_STATUS_SUCCESS;

	return hal_status;
}

#define DBG_RFK_TIME
enum rtw_hal_status rtw_hal_rf_chl_rfk_trigger(void *hal,
						u8 phy_idx, u8 force)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	#ifdef DBG_RFK_TIME
	u32 iqk_start = _os_get_cur_time_ms();
	#endif

	hal_status = halrf_chl_rfk_trigger(hal_info->rf, phy_idx, force);

	#ifdef DBG_RFK_TIME
	PHL_INFO("[DBG_RFK] RFK take %d (ms)\n", phl_get_passing_time_ms(iqk_start));
	#endif
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_rf_dack_trigger(struct hal_info_t *hal_info,
						u8 force)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

#if 0
	hal_status = halrf_dack_trigger(hal_info->rf, force);
#else
	hal_status = RTW_HAL_STATUS_SUCCESS;
#endif

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_rf_iqk_trigger(struct hal_info_t *hal_info,
						u8 phy_idx, u8 force)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = halrf_iqk_trigger(hal_info->rf, phy_idx, force);

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_rf_lck_trigger(struct hal_info_t *hal_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

#if 0
	hal_status = halrf_lck_trigger(hal_info->rf);
#else
	hal_status = RTW_HAL_STATUS_SUCCESS;
#endif

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_rf_dpk_trigger(struct hal_info_t *hal_info,
						u8 phy_idx, u8 force)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = halrf_dpk_trigger(hal_info->rf, phy_idx, force);

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_rf_tssi_trigger(struct hal_info_t *hal_info,
				u8 phy_idx)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

#if 0
	hal_status = halrf_tssi_trigger(hal_info->rf, phy_idx);
#else
	hal_status = RTW_HAL_STATUS_SUCCESS;
#endif

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_rf_gapk_trigger(struct hal_info_t *hal_info,
						u8 phy_idx, u8 force)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

#if 0
	hal_status = halrf_gapk_trigger(hal_info->rf, phy_idx, force);
#else
	hal_status = RTW_HAL_STATUS_SUCCESS;
#endif

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}


enum rtw_hal_status rtw_hal_rf_set_capability_dack(struct hal_info_t *hal_info,
				u8 enable)
{
#if 0
	halrf_dack_onoff(hal_info->rf, enable);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_capability_iqk(struct hal_info_t *hal_info,
				u8 enable)
{
#if 0
	halrf_iqk_onoff(hal_info->rf, enable);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_capability_dpk(struct hal_info_t *hal_info,
						u8 enable)
{
#if 0
	halrf_dpk_onoff(hal_info->rf, enable);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_capability_dpk_track(struct hal_info_t *hal_info,
						u8 enable)
{
#if 0
	halrf_dpk_track_onoff(hal_info->rf, enable);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status rtw_hal_rf_set_capability_tssi(struct hal_info_t *hal_info,
				u8 enable)
{
#if 0
	halrf_tssi_onoff(hal_info->rf, enable);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_capability_gapk(struct hal_info_t *hal_info,
						u8 enable)
{
#if 0
	halrf_gapk_onoff(hal_info->rf, enable);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status rtw_hal_rf_get_capability_dack(struct hal_info_t *hal_info,
						u8 *enable)
{
#if 0
	*enable = halrf_get_dack_onoff(hal_info->rf);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_capability_iqk(struct hal_info_t *hal_info,
				u8 *enable)
{
#if 0
	*enable = halrf_get_iqk_onoff(hal_info->rf);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_capability_dpk(struct hal_info_t *hal_info,
					u8 *enable)
{
#if 0
	*enable = halrf_get_dpk_onoff(hal_info->rf);
#endif
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_capability_dpk_track(struct hal_info_t *hal_info,
				u8 *enable)
{
#if 0
	*enable = halrf_get_dpk_track_onoff(hal_info->rf);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_capability_tssi(struct hal_info_t *hal_info,
				u8 *enable)
{
#if 0
	*enable = halrf_get_tssi_onoff(hal_info->rf);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_capability_gapk(struct hal_info_t *hal_info,
					u8 *enable)
{
#if 0
	*enable = halrf_get_gapk_onoff(hal_info->rf);
#endif

	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status rtw_hal_rf_get_tssi_de_value(struct hal_info_t *hal_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

#if 0
	hal_status = halrf_get_tssi_de_value(hal_info->rf);
#else
	hal_status = RTW_HAL_STATUS_SUCCESS;
#endif

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_rf_set_tssi_de_tx_verify(struct hal_info_t *hal_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

#if 0
	hal_status = halrf_set_tssi_de_tx_verify(hal_info->rf);
#else
	hal_status = RTW_HAL_STATUS_SUCCESS;
#endif

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_rf_get_txpwr_final_abs(struct hal_info_t *hal_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

#if 0
	hal_status = halrf_get_txpwr_final_abs(hal_info->rf);
#else
	hal_status = RTW_HAL_STATUS_SUCCESS;
#endif

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}

#endif // if 0 NEO

bool
rtw_hal_rf_proc_cmd(struct hal_info_t *hal_info,
					struct rtw_proc_cmd *incmd,
					char *output, u32 out_len)
{
	RTW_ERR("%s TODO NEO\n", __func__);
	return false;
#if 0 // NEO TODO
	if(incmd->in_type == RTW_ARG_TYPE_BUF)
		halrf_cmd(hal_info->rf, incmd->in.buf, output, out_len);

	else if(incmd->in_type == RTW_ARG_TYPE_ARRAY){

		halrf_cmd_parser(hal_info->rf, incmd->in.vector,
					incmd->in_cnt_len, output, out_len);
	}

	return true;
#endif
}

#if 0 // NEO TODO
/* workaround for 8852A A-cut, reg overwrite issue */
enum rtw_hal_status
rtw_hal_rf_recover(struct hal_info_t *hal_info, u32 offset, u32 val,
					enum rf_path path)
{
	u8	tmp_offset;
	bool reload_check = false;

	/* exclude mac register range */
	if(offset <= 0xffff)
		return RTW_HAL_STATUS_SUCCESS;

	/* exclude rf dac register range */
	if(offset >= 0x14c00 && offset <= 0x14dff)
		return RTW_HAL_STATUS_SUCCESS;

	reload_check = ((offset&0xFF) == 0)?true:false;

	offset = (offset/4)*4;

	tmp_offset = (u8)offset&0xff;

	if(tmp_offset <= 0x9F)
	{
		halrf_dack_recover(hal_info->rf, tmp_offset, path , val, reload_check);
		PHL_DBG("%s offset(0x%x), val(0x%x), reload(%d)\n",
				 __FUNCTION__, tmp_offset, val, reload_check);
	}

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_watchdog(struct hal_info_t *hal_info)
{

	halrf_watchdog(hal_info->rf);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_set_power(struct hal_info_t *hal_info, enum phl_phy_idx phy,
					enum phl_pwr_table pwr_table)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	if(halrf_set_power(hal_info->rf, phy, pwr_table))
	{
		hal_status = RTW_HAL_STATUS_SUCCESS;
	}
	else
	{
		hal_status = RTW_HAL_STATUS_FAILURE;
	}

	return hal_status;
}

enum rtw_hal_status rtw_hal_rf_set_gain_offset(struct hal_info_t *hal_info, u8 cur_phy_idx,
						s8 offset, u8 rf_path)
{

	enum rtw_hal_status ret=RTW_HAL_STATUS_SUCCESS;
	PHL_INFO("[MP HAL API]%s\n", __FUNCTION__);

	halrf_set_rx_gain_offset_for_rx_verify(hal_info->rf, cur_phy_idx, offset, rf_path);

	return ret;
}

enum rtw_hal_status rtw_hal_rf_trigger_dpk_tracking(struct hal_info_t *hal_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = halrf_dpk_tracking(hal_info->rf);

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("[MP HAL API] %s failed status(%d)\n",__FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status
rtw_hal_rf_get_default_rfe_type(struct rtw_hal_com_t *hal_com)
{
	struct hal_info_t *hal_info = hal_com->hal_priv;

	hal_com->dev_hw_cap.rfe_type = halrf_get_default_rfe_type(hal_info->rf);
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_get_default_xtal(struct rtw_hal_com_t *hal_com)
{
	struct hal_info_t *hal_info = hal_com->hal_priv;

	hal_com->dev_hw_cap.xcap = halrf_get_default_xtal(hal_info->rf);
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_tssi_avg(struct hal_info_t *hal_info, u8 cur_phy_idx,
						s32 xdbm)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;

	halrf_set_tssi_avg_mp(hal_info->rf, cur_phy_idx, xdbm);

	return hal_status;
}

void rtw_hal_rf_do_tssi_scan(struct hal_info_t *hal_info, u8 cur_phy_idx)
{
	halrf_do_tssi_scan(hal_info->rf, cur_phy_idx);
}
enum rtw_hal_status
rtw_hal_rf_config_radio_to_fw(struct hal_info_t *hal_info)
{
	halrf_config_radio_to_fw(hal_info->rf);

	return RTW_HAL_STATUS_SUCCESS;
}

bool
rtw_hal_rf_check_efuse_data(struct rtw_hal_com_t *hal_com, enum phl_phy_idx phy_idx)
{
	struct hal_info_t *hal_info = hal_com->hal_priv;

	return halrf_tssi_check_efuse_data(hal_info->rf, phy_idx);
}

#endif // if 0 NEO

#else /*ifdef USE_TRUE_PHY*/
enum rtw_hal_status
rtw_hal_rf_init(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal_info)
{
	return RTW_HAL_STATUS_SUCCESS;
}
void rtw_hal_rf_deinit(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal_info)
{
}

void rtw_hal_init_rf_reg(struct rtw_phl_com_t *phl_com, void *hal)
{
}

void rtw_hal_rf_dm_init(struct hal_info_t *hal_info)
{
}

enum rtw_hal_status
rtw_hal_rf_get_efuse_info(struct rtw_hal_com_t *hal_com, u8 *efuse_map,
				enum rtw_efuse_info info_type, void *value,
				u8 size, u8 map_valid)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_read_pwr_table(struct rtw_hal_com_t *hal_com, u8 rf_path, u16 rate,
				u8 bandwidth, u8 channel, u8 offset, u8 dcm,
				u8 beamforming, s16 *get_item)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_wl_tx_power_control(struct rtw_hal_com_t *hal_com,
				u32 tx_power_val)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_pwrtrack(struct hal_info_t *hal_info,
						u8 *txpwr_track_status)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_pwrtrack(struct hal_info_t *hal_info,
				u8 phy_idx, u8 txpwr_track_status)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_thermal(struct hal_info_t *hal_info,
						u8 rf_path, u8 *thermal)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_tssi(struct hal_info_t *hal_info,
				u8 phy_idx, u8 rf_path, u32 tssi_de)
{
	return RTW_HAL_STATUS_SUCCESS;
}
enum rtw_hal_status rtw_hal_rf_set_tssi_offset(struct hal_info_t *hal_info, u8 phy_idx, u32 tssi_de_offset, u8 rf_path)
{
	return RTW_HAL_STATUS_SUCCESS;
}
enum rtw_hal_status rtw_hal_rf_get_tssi(struct hal_info_t *hal_info,
					u8 phy_idx, u8 rf_path, u32 *tssi_de)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_online_tssi_de(
	struct hal_info_t *hal_info, u8 phy_idx, u8 rf_path,
	s32 dbm, s32 pout, s32 *tssi_de)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_set_continuous_tx(struct hal_info_t *hal_info)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_set_singletone_tx(struct hal_info_t *hal_info,
						u8 is_on,enum rf_path path)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_set_suppression_tx(struct hal_info_t *hal_info)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_ctrl_dbcc(struct rtw_hal_com_t *hal_com,
	bool dbcc_en)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_chl_rfk_trigger(void *hal, u8 phy_idx, u8 force)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_dack_trigger(struct hal_info_t *hal_info,
						u8 force)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_iqk_trigger(struct hal_info_t *hal_info,
						u8 phy_idx, u8 force)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_lck_trigger(struct hal_info_t *hal_info)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_dpk_trigger(struct hal_info_t *hal_info,
						u8 phy_idx, u8 force)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_tssi_trigger(struct hal_info_t *hal_info,
						u8 phy_idx)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_gapk_trigger(struct hal_info_t *hal_info,
						u8 phy_idx, u8 force)
{
	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status rtw_hal_rf_set_capability_dack(struct hal_info_t *hal_info,
						u8 enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_capability_iqk(struct hal_info_t *hal_info,
						u8 enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_capability_dpk(struct hal_info_t *hal_info,
						u8 enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_set_capability_dpk_track(struct hal_info_t *hal_info, u8 enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status
rtw_hal_rf_set_capability_tssi(struct hal_info_t *hal_info, u8 enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_capability_gapk(struct hal_info_t *hal_info,
						u8 enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status rtw_hal_rf_get_capability_dack(struct hal_info_t *hal_info,
						u8 *enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_capability_iqk(struct hal_info_t *hal_info,
						u8 *enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_capability_dpk(struct hal_info_t *hal_info,
						u8 *enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_get_capability_dpk_track(struct hal_info_t *hal_info, u8 *enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_capability_tssi(struct hal_info_t *hal_info,
						u8 *enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_capability_gapk(struct hal_info_t *hal_info,
						u8 *enable)
{
	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status rtw_hal_rf_get_tssi_de_value(struct hal_info_t *hal_info)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_tssi_de_tx_verify(struct hal_info_t *hal_info)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_get_txpwr_final_abs(struct hal_info_t *hal_info)
{
	return RTW_HAL_STATUS_SUCCESS;
}

bool rtw_hal_rf_proc_cmd(struct hal_info_t *hal_info,
				struct rtw_proc_cmd *incmd,
				char *output, u32 out_len)
{
	return true;
}

enum rtw_hal_status rtw_hal_rf_recover(struct hal_info_t *hal_info, u32 offset,
						u32 val, enum rf_path path)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_watchdog(struct hal_info_t *hal_info)
{
	return RTW_HAL_STATUS_SUCCESS;
}
enum rtw_hal_status
rtw_hal_rf_set_power(struct hal_info_t *hal_info, enum phl_phy_idx phy,
					enum phl_pwr_table pwr_table)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_set_gain_offset(struct hal_info_t *hal_info, u8 cur_phy_idx,
						s8 offset, u8 rf_path)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_rf_trigger_dpk_tracking(struct hal_info_t *hal_info)
{
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status
rtw_hal_rf_config_radio_to_fw(struct hal_info_t *hal_info)
{
	return RTW_HAL_STATUS_SUCCESS;
}


#endif /*ifdef USE_TRUE_PHY*/
