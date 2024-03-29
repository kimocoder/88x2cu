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
#define _HAL_BTC_DBG_CMD_C_
#include "../hal_headers_le.h"
#include "hal_btc.h"
#include "halbtc_fw.h"
#include "halbtc_fwdef.h"
#include "halbtc_action.h"
#include "halbtc_def.h"

#ifdef CONFIG_BTCOEX

#define	BTC_CNSL(max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
	do {							\
		u32 *used_len_tmp = &(used_len);			\
		if (*used_len_tmp < max_buff_len)			\
			*used_len_tmp += _os_snprintf(buff_addr, remain_len, fmt, ##__VA_ARGS__);\
	} while (0)


struct halbtc_cmd_info {
	char name[16];
	u8 id;
};

enum HALBTC_CMD_ID {
	HALBTC_HELP,
	HALBTC_SHOW,
	HALBTC_READ_BT,
	HALBTC_WRITE_BT,
	HALBTC_SET_COEX,
	HALBTC_UPDATE_POLICY,
	HALBTC_TDMA,
	HALBTC_SLOT,
	HALBTC_SIG_GDBG_EN,
	HALBTC_SGPIO_MAP,
	HALBTC_TRACE_STEP,
	HALBTC_WL_TX_POWER,
	HALBTC_WL_RX_LNA,
	HALBTC_BT_AFH_MAP,
	HALBTC_BT_TX_POWER,
	HALBTC_BT_RX_LNA,
	HALBTC_BT_IGNO_WLAN,
	HALBTC_SET_GNT_WL,
	HALBTC_SET_GNT_BT,
	HALBTC_SET_BT_PSD,
	HALBTC_TEST
};

struct halbtc_cmd_info halbtc_cmd_i[] = {
	{"-h", HALBTC_HELP},
	{"show", HALBTC_SHOW},
	{"rb", HALBTC_READ_BT},
	{"wb", HALBTC_WRITE_BT},
	{"mode", HALBTC_SET_COEX},
	{"update", HALBTC_UPDATE_POLICY},
	{"tdma", HALBTC_TDMA},
	{"slot", HALBTC_SLOT},
	{"sig", HALBTC_SIG_GDBG_EN},
	{"gpio", HALBTC_SGPIO_MAP},
	{"step", HALBTC_TRACE_STEP},
	{"wpwr", HALBTC_WL_TX_POWER},
	{"wlna", HALBTC_WL_RX_LNA},
	{"afh", HALBTC_BT_AFH_MAP},
	{"bpwr", HALBTC_BT_TX_POWER},
	{"blna", HALBTC_BT_RX_LNA},
	{"igwl", HALBTC_BT_IGNO_WLAN},
	{"gwl", HALBTC_SET_GNT_WL},
	{"gbt", HALBTC_SET_GNT_BT},
	{"bpsd", HALBTC_SET_BT_PSD},
	{"test", HALBTC_TEST}
};

static void _cmd_rb(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 i = 0, type = 0, addr = 0;

	if (input_num < 3)
		goto help;

	_os_sscanf(input[1], "%d", &type);
	_os_sscanf(input[2], "%x", &addr);

	if (type > 3)
		goto help;

	btc->dbg.rb_done = false;
	btc->dbg.rb_val = 0xffffffff;
	_read_bt_reg(btc, (u8)(type), (u16)addr);
	for (i = 0; i < 50; i++) {
		if (!btc->dbg.rb_done)
			hal_mdelay(btc->hal, 10);
		else
			goto exit;
	}

exit:
	if (!btc->dbg.rb_done) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" timeout !! \n");
	} else {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" rb %d(0x%x), val = 0x%x !! \n", type, addr, btc->dbg.rb_val);
	}
	return;

help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" rb <type --- 0:rf, 1:modem, 2:bluewize, 3:vendor> <addr:16bits> \n");
	return;
}

static void _show_cx_info(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btc_dm *dm = &btc->dm;
	struct btc_bt_info *bt = &btc->cx.bt;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct rtw_hal_com_t *h = btc->hal;
	u32 ver_main = 0, ver_sub = 0, ver_hotfix = 0, id_branch = 0;

	if (!(dm->coex_info_map & BTC_COEX_INFO_CX))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r========== [BTC COEX INFO (%s)] ==========",
		     id_to_str(BTC_STR_CHIPID, (u32)btc->chip->chip_id));

	ver_main = (coex_ver & bMASKB3) >> 24;
	ver_sub = (coex_ver & bMASKB2) >> 16;
	ver_hotfix = (coex_ver & bMASKB1) >> 8;
	id_branch = coex_ver & bMASKB0;
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : Coex:%d.%d.%d(branch:%s), ",
		     "[coex_version]", ver_main, ver_sub, ver_hotfix,
		     id_to_str(BTC_STR_BRANCH, id_branch));

	if (dm->wl_fw_cx_offload != BTC_CX_FW_OFFLOAD)
		dm->error.map.offload_mismatch = true;
	else
		dm->error.map.offload_mismatch = false;

	ver_main = (wl->ver_info.fw_coex & bMASKB3) >> 24;
	ver_sub = (wl->ver_info.fw_coex & bMASKB2) >> 16;
	ver_hotfix = (wl->ver_info.fw_coex & bMASKB1) >> 8;
	id_branch = wl->ver_info.fw_coex & bMASKB0;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"WL_FW_coex:%d.%d.%d(branch:%s)",
		     ver_main, ver_sub, ver_hotfix,
		     id_to_str(BTC_STR_BRANCH, id_branch));

	ver_main = (btc->chip->wlcx_desired & bMASKB3) >> 24;
	ver_sub = (btc->chip->wlcx_desired & bMASKB2) >> 16;
	ver_hotfix = (btc->chip->wlcx_desired & bMASKB1) >> 8;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"(%s, desired:%d.%d.%d), ",
		     (wl->ver_info.fw_coex >= btc->chip->wlcx_desired ?
		     "Match" : "Mis-Match"),
		     ver_main, ver_sub, ver_hotfix);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"BT_FW_coex:%d.0.0(%s, desired:%d.0.0)",
		     bt->ver_info.fw_coex,
		     (bt->ver_info.fw_coex >= btc->chip->btcx_desired ?
		     "Match" : "Mis-Match"),
		     btc->chip->btcx_desired);

	if (bt->enable.now && bt->ver_info.fw == 0)
		hal_btc_fw_en_rpt(btc, RPT_EN_BT_VER_INFO, 1);
	else
		hal_btc_fw_en_rpt(btc, RPT_EN_BT_VER_INFO, 0);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : WL_FW:%d.%d.%d.%d, BT_FW:0x%x(%s)",
		     "[sub_module]", (wl->ver_info.fw & bMASKB3) >> 24,
		     (wl->ver_info.fw & bMASKB2) >> 16,
		     (wl->ver_info.fw & bMASKB1) >> 8,
		     (wl->ver_info.fw & bMASKB0), bt->ver_info.fw,
		     (bt->run_patch_code ? "patch" : "ROM"));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : kt_ver:%x, rfe_type:0x%x, ant_iso:%d, ant_pg:%d, %s",
		     "[hw_info]", btc->mdinfo.kt_ver, btc->mdinfo.rfe_type,
		     btc->mdinfo.ant.isolation, btc->mdinfo.ant.num,
		     (btc->mdinfo.ant.num > 1? "" : (btc->mdinfo.ant.single_pos?
		     "1Ant_Pos:S1, " : "1Ant_Pos:S0, ")));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"3rd_coex:%d, dbcc:%d, tx_num:%d, rx_num:%d",
		     btc->cx.other.type, h->dbcc_en, h->rfpath_tx_num,
		     h->rfpath_rx_num);
}

static void _show_wl_role_info(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btc_wl_link_info *plink = NULL;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_wl_dbcc_info *wl_dinfo = &wl->dbcc_info;
	struct btc_traffic t;
	u8 i;

	if (btc->hal->dbcc_en) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : PHY0_band(op:%d/scan:%d/real:%d), ",
			     "[dbcc_info]", wl_dinfo->op_band[HW_PHY_0],
			     wl_dinfo->scan_band[HW_PHY_0],
			     wl_dinfo->real_band[HW_PHY_0]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"PHY1_band(op:%d/scan:%d/real:%d)",
			     wl_dinfo->op_band[HW_PHY_1],
			     wl_dinfo->scan_band[HW_PHY_1],
			     wl_dinfo->real_band[HW_PHY_1]);
	}

	for (i = 0; i < MAX_WIFI_ROLE_NUMBER; i++) {
		plink = &btc->cx.wl.link_info[i];

		if (!plink->active)
			continue;

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r port_%-10d : role=%s(phy-%d), connect=%s, mode=%s, center_ch=%d, bw=%s, ",
			     plink->pid,
			     id_to_str(BTC_STR_ROLE, (u32)plink->role),
			     plink->phy,
			     id_to_str(BTC_STR_MSTATE, (u32)plink->connected),
			     id_to_str(BTC_STR_WLMODE, (u32)plink->mode),
			     plink->ch,
			     id_to_str(BTC_STR_WLBW, (u32)plink->bw));

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"mac_id=%d, max_tx_time=%dus, max_tx_retry=%d",
			     plink->mac_id, plink->tx_time, plink->tx_retry);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r port_%-10d : rssi=%d, busy=%d, dir=%s, ",
			     plink->pid, plink->stat.rssi, plink->busy,
			     (plink->dir == TRAFFIC_UL ? "UL" : "DL"));

		t = plink->stat.traffic;

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"tx[rate:%s/busy_level:%d/sts:0x%x], ",
			     id_to_str(BTC_STR_RATE, (u32)t.tx_rate),
			     t.tx_lvl, t.tx_sts);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"rx[rate:%s/busy_level:%d/sts:0x%x]",
			     id_to_str(BTC_STR_RATE, (u32)t.rx_rate),
			     t.rx_lvl, t.rx_sts);
	}
}

static void _show_wl_info(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btc_cx *cx = &btc->cx;
	struct btc_wl_info *wl = &cx->wl;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;

	if (!(btc->dm.coex_info_map & BTC_COEX_INFO_WL))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r========== [WL Status] ==========");

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : link_mode:%s, ",
		     "[status]",
		     id_to_str(BTC_STR_WLLINK, (u32)wl_rinfo->link_mode));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"rf_off:%s, scan:%s(band:%d/phy_map:0x%x), ",
		     (wl->status.map.rf_off? "Y" : "N"),
		     (wl->status.map.scan? "Y" : "N"),
		     wl->scan_info.band[HW_PHY_0], wl->scan_info.phy_map);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"connecting:%s, roam:%s, 4way:%s, init_ok:%s",
		     (wl->status.map.connecting? "Y" : "N"),
		     (wl->status.map.roaming?  "Y" : "N"),
		     (wl->status.map._4way? "Y" : "N"),
		     (wl->status.map.init_ok? "Y" : "N"));

	_show_wl_role_info(btc, used, input, input_num, output, out_len);
}

static void _show_bt_profile_info(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btc_bt_link_info *bt_linfo = &btc->cx.bt.link_info;
	struct btc_bt_hfp_desc hfp = bt_linfo->hfp_desc;
	struct btc_bt_hid_desc hid = bt_linfo->hid_desc;
	struct btc_bt_a2dp_desc a2dp = bt_linfo->a2dp_desc;
	struct btc_bt_pan_desc pan = bt_linfo->pan_desc;

	if (hfp.exist) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : type:%s, sut_pwr:%d, golden-rx:%d",
			     "[HFP]",
			     (hfp.type == 0? "SCO" : "eSCO"),
			      bt_linfo->sut_pwr_level[0],
			      bt_linfo->golden_rx_shift[0]);
	}

	if (hid.exist) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : type:%s%s%s%s%s pair-cnt:%d, sut_pwr:%d, golden-rx:%d",
			     "[HID]",
			     (hid.type & BTC_HID_218? "2/18," : ""),
			     (hid.type & BTC_HID_418? "4/18," : ""),
			     (hid.type & BTC_HID_BLE? "BLE," : ""),
			     (hid.type & BTC_HID_RCU? "RCU," : ""),
			     (hid.type & BTC_HID_RCU_VOICE? "RCU-Voice," : ""),
			      hid.pair_cnt, bt_linfo->sut_pwr_level[1],
			      bt_linfo->golden_rx_shift[1]);
	}

	if (a2dp.exist) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : type:%s, bit-pool:%d, flush-time:%d, ",
			     "[A2DP]",
			     (a2dp.type == BTC_A2DP_LEGACY ? "Legacy" : "TWS"),
			      a2dp.bitpool, a2dp.flush_time);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"vid:0x%x, Dev-name:0x%x, sut_pwr:%d, golden-rx:%d",
			     a2dp.vendor_id, a2dp.device_name,
			     bt_linfo->sut_pwr_level[2],
			     bt_linfo->golden_rx_shift[2]);
	}

	if (pan.exist) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : sut_pwr:%d, golden-rx:%d",
			     "[PAN]",
			     bt_linfo->sut_pwr_level[3],
			     bt_linfo->golden_rx_shift[3]);
	}
}

static void _show_bt_info(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btc_cx *cx = &btc->cx;
	struct btc_bt_info *bt = &cx->bt;
	struct btc_wl_info *wl = &cx->wl;
	struct btc_module *module = &btc->mdinfo;
	struct btc_bt_link_info *bt_linfo = &bt->link_info;
	u8 *afh = bt_linfo->afh_map;
	u16 polt_cnt = 0;

	btc->chip->ops->update_bt_cnt(btc);
	_chk_err_state(btc, BTC_DCNT_BTCNT_FREEZE, 0);

	if (!(btc->dm.coex_info_map & BTC_COEX_INFO_BT))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"%s", "\n\r========== [BT Status] ==========");

	/*bt->btg_type = (bt->ver_info.fw & BIT(28) ? BTC_BT_BTG: BTC_BT_ALONE);*/

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : enable:%s, btg:%s%s, connect:%s, ",
		     "[status]", (bt->enable.now? "Y" : "N"),
		     (bt->btg_type? "Y" : "N"),
		     (bt->enable.now && (bt->btg_type != module->bt_pos)?
		      "(efuse-mismatch!!)" : ""),
		     (bt_linfo->status.map.connect? "Y" : "N"));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"igno_wl:%s, mailbox_avl:%s, rfk_state:0x%x",
		     (bt->igno_wl? "Y" : "N"),
		     (bt->mbx_avl? "Y" : "N"), bt->rfk_info.val);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : profile:%s%s%s%s%s ",
		     "[profile]",
		     ((bt_linfo->profile_cnt.now == 0) ? "None," : ""),
		     (bt_linfo->hfp_desc.exist? "HFP," : ""),
		     (bt_linfo->hid_desc.exist? "HID," : ""),
		     (bt_linfo->a2dp_desc.exist?
		     (bt_linfo->a2dp_desc.sink ? "A2DP_sink," :"A2DP,") : ""),
		     (bt_linfo->pan_desc.exist? "PAN," : ""));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"multi-link:%s, role:%s, ble-connect:%s, CQDDR:%s, A2DP_active:%s, PAN_active:%s",
		     (bt_linfo->multi_link.now? "Y" : "N"),
		     (bt_linfo->slave_role ? "Slave" : "Master"),
		     (bt_linfo->status.map.ble_connect? "Y" : "N"),
		     (bt_linfo->cqddr? "Y" : "N"),
		     (bt_linfo->a2dp_desc.active? "Y" : "N"),
		     (bt_linfo->pan_desc.active? "Y" : "N"));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : rssi:%ddBm,%s%s%s",
		     "[link]", bt_linfo->rssi-100,
		     (bt_linfo->status.map.inq_pag? " inq-page!!" : ""),
		     (bt_linfo->status.map.acl_busy? " acl_busy!!" : ""),
		     (bt_linfo->status.map.mesh_busy? " mesh_busy!!" : ""));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"%s afh_map[%02x%02x_%02x%02x_%02x%02x_%02x%02x_%02x%02x], ",
		     (bt_linfo->relink.now? " ReLink!!" : ""),
		     afh[0], afh[1], afh[2], afh[3], afh[4],
		     afh[5], afh[6], afh[7], afh[8], afh[9]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"wl_ch_map[en:%d/ch:%d/bw:%d]",
		     wl->afh_info.en, wl->afh_info.ch, wl->afh_info.bw);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : retry:%d, relink:%d, reinit:%d, reenable:%d, ",
		     "[stat_cnt]", cx->cnt_bt[BTC_BCNT_RETRY],
		     cx->cnt_bt[BTC_BCNT_RELINK],
		     cx->cnt_bt[BTC_BCNT_REINIT],
		     cx->cnt_bt[BTC_BCNT_REENABLE]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"role-switch:%d, afh:%d, inq_page:%d(inq:%d/page:%d), igno_wl:%d",
		     cx->cnt_bt[BTC_BCNT_ROLESW],
		     cx->cnt_bt[BTC_BCNT_AFH],
		     cx->cnt_bt[BTC_BCNT_INQPAG],
		     cx->cnt_bt[BTC_BCNT_INQ],
		     cx->cnt_bt[BTC_BCNT_PAGE],
		     cx->cnt_bt[BTC_BCNT_IGNOWL]);

	_show_bt_profile_info(btc, used, input, input_num, output, out_len);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : raw_data[%02x %02x %02x %02x %02x %02x] (type:%s/cnt:%d/same:%d)",
		     "[bt_info]",
		     bt->raw_info[2], bt->raw_info[3],
		     bt->raw_info[4], bt->raw_info[5],
		     bt->raw_info[6], bt->raw_info[7],
		     (bt->raw_info[0] == BTC_BTINFO_AUTO ? "auto" : "reply"),
		     cx->cnt_bt[BTC_BCNT_INFOUPDATE],
		     cx->cnt_bt[BTC_BCNT_INFOSAME]);

	rtw_hal_mac_get_bt_polt_cnt(btc->hal, HW_PHY_0, &polt_cnt);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : Hi-rx = %d, Hi-tx = %d, Lo-rx = %d, Lo-tx = %d (bt_polut_wl_tx = %d)",
		     "[trx_req_cnt]", cx->cnt_bt[BTC_BCNT_HIPRI_RX],
		     cx->cnt_bt[BTC_BCNT_HIPRI_TX],
		     cx->cnt_bt[BTC_BCNT_LOPRI_RX],
		     cx->cnt_bt[BTC_BCNT_LOPRI_TX],
		     polt_cnt);
}

static void _show_dm_info(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btc_module *module = &btc->mdinfo;
	struct btc_dm *dm = &btc->dm;

	if (!(dm->coex_info_map & BTC_COEX_INFO_DM))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r========== [Mechanism Status %s] ==========",
		     (btc->ctrl.manual? "(Manual)":"(Auto)"));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : type:%s, reason:%s(), action:%s(), ant_path:%s, run_cnt:%d",
		     "[status]",
		     (module->ant.type == BTC_ANT_SHARED ?
		      "shared" : "dedicated"),
		     dm->run_reason, dm->run_action,
		     id_to_str(BTC_STR_ANTPATH, dm->set_ant_path & 0xff),
		     dm->cnt_dm[BTC_DCNT_RUN]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : wl_only:%d, bt_only:%d, igno_bt:%d, free_run:%d, wl_ps_ctrl:%d, wl_mimo_ps:%d, ",
		     "[dm_flag]", dm->wl_only, dm->bt_only, btc->ctrl.igno_bt,
		     dm->freerun, dm->wl_ps_ctrl, dm->wl_mimo_ps);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"leak_ap:%d, noisy_level:%d, fw_offload:%s%s",
		     dm->leak_ap, dm->noisy_level,
		     (BTC_CX_FW_OFFLOAD? "Y" : "N"),
		     (dm->wl_fw_cx_offload == BTC_CX_FW_OFFLOAD?
		     "" : "(Mis-Match!!)"));

	if (dm->rf_trx_para.wl_tx_power == 0xff)
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : wl_tx_pwr:original, ",
			     "[trx_ctrl]");

	else
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : wl_tx_pwr:%d, ",
			     "[trx_ctrl]", dm->rf_trx_para.wl_tx_power);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"wl_rx_lvl:%d, bt_tx_pwr_dec:%d, bt_rx_lna:%d, ",
		     dm->rf_trx_para.wl_rx_gain, dm->rf_trx_para.bt_tx_power,
		     dm->rf_trx_para.bt_rx_gain);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"wl_btg_rx:%d, wl_tx_limit[en:%d/max_time:%dus/max_retry:%d]",
		     dm->wl_btg_rx, dm->wl_tx_limit.enable,
		     dm->wl_tx_limit.tx_time, dm->wl_tx_limit.tx_retry);
}

static void _show_mreg(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_mreg_val *pmreg = NULL;
	struct fbtc_gpio_dbg *gdbg = NULL;
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_cx *cx = &btc->cx;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_bt_info *bt = &btc->cx.bt;
	struct btc_gnt_ctrl gnt[2] = {0};
	u8 i = 0, type = 0, cnt = 0;
	u32 val, offset;

	if (!(btc->dm.coex_info_map & BTC_COEX_INFO_MREG))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r========== [HW Status] ==========");

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : WL->BT:0x%08x(cnt:%d), BT->WL:0x%08x(cnt:%d)",
		     "[scoreboard]", wl->scbd, cx->cnt_wl[BTC_WCNT_SCBDUPDATE],
		     bt->scbd, cx->cnt_bt[BTC_BCNT_SCBDUPDATE]);

	rtw_hal_mac_get_grant(h, (u8*)gnt);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : pta_owner:%s, phy-0[gnt_wl:%s-%d/gnt_bt:%s-%d], ",
		     "[gnt_status]",
		     (_read_cx_ctrl(btc) == BTC_CTRL_BY_WL? "WL" : "BT"),
		     (gnt[0].gnt_wl_sw_en? "SW" : "HW"), gnt[0].gnt_wl,
		     (gnt[0].gnt_bt_sw_en? "SW" : "HW"), gnt[0].gnt_bt);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"phy-1[gnt_wl:%s-%d/gnt_bt:%s-%d]",
		     (gnt[1].gnt_wl_sw_en? "SW" : "HW"), gnt[1].gnt_wl,
		     (gnt[1].gnt_bt_sw_en? "SW" : "HW"), gnt[1].gnt_bt);

	pcinfo = &pfwinfo->rpt_fbtc_mregval.cinfo;
	if (!pcinfo->valid)
		return;

	pmreg = &pfwinfo->rpt_fbtc_mregval.finfo;

	for (i = 0; i < pmreg->reg_num; i++) {
		type = (u8)btc->chip->mon_reg[i].type;
		offset = btc->chip->mon_reg[i].offset;
		val = pmreg->mreg_val[i];

		if (cnt % 6 == 0)
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				"\n\r %-15s : %s_0x%x=0x%x",
				     "[reg]",
				     id_to_str(BTC_STR_REG, (u32)type),
				     offset, val);
	 	else
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				", %s_0x%x=0x%x",
				     id_to_str(BTC_STR_REG, (u32)type),
				     offset, val);
		cnt++;
	}

	pcinfo = &pfwinfo->rpt_fbtc_gpio_dbg.cinfo;
	if (!pcinfo->valid)
		return;

	gdbg = &pfwinfo->rpt_fbtc_gpio_dbg.finfo;
	if (!gdbg->en_map)
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : enable_map:0x%08x",
		     "[gpio_dbg]", gdbg->en_map);

	for (i = 0; i < BTC_DBG_MAX1; i++) {
		if (!(gdbg->en_map & BIT(i)))
			continue;
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			", %s->GPIO%d",
			     id_to_str(BTC_STR_GDBG, (u32)i),
			     gdbg->gpio_map[i]);
	}
}

static void _show_summary(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_rpt_ctrl *prptctrl = NULL;
	struct btc_cx *cx = &btc->cx;
	struct btc_dm *dm = &btc->dm;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_bt_info *bt = &cx->bt;
	u32 cnt_sum = 0, *cnt = btc->dm.cnt_notify;
	u8 i;

	if (!(dm->coex_info_map & BTC_COEX_INFO_SUMMARY))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"%s", "\n\r========== [Statistics] ==========");

	pcinfo = &pfwinfo->rpt_ctrl.cinfo;
	if (pcinfo->valid) {
		prptctrl = &pfwinfo->rpt_ctrl.finfo;

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : h2c_cnt=%d(fail:%d, fw_recv:%d), c2h_cnt=%d(fw_send:%d), ",
			     "[summary]",
			     pfwinfo->cnt_h2c, pfwinfo->cnt_h2c_fail, prptctrl->h2c_cnt,
			     pfwinfo->cnt_c2h, prptctrl->c2h_cnt);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"rpt_cnt=%d(fw_send:%d), rpt_map= 0x%x, dm_error_map:0x%x",
			     pfwinfo->event[BTF_EVNT_RPT], prptctrl->rpt_cnt,
			     prptctrl->rpt_enable, dm->error.val);

		btc->fwinfo.rpt_en_map = prptctrl->rpt_enable;
		wl->ver_info.fw_coex = prptctrl->wl_fw_coex_ver;
		wl->ver_info.fw = prptctrl->wl_fw_ver;
		dm->wl_fw_cx_offload = !!(prptctrl->wl_fw_cx_offload);

		_chk_err_state(btc, BTC_DCNT_RPT_FREEZE,
			       pfwinfo->event[BTF_EVNT_RPT]);

		if (dm->error.map.wl_fw_hang) {
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				" (WL FW Hang!!!)");
		}

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : send_ok:%d, send_fail:%d, recv:%d",
			     "[mailbox]", prptctrl->mb_send_ok_cnt,
			     prptctrl->mb_send_fail_cnt, prptctrl->mb_recv_cnt);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"(A2DP_empty:%d, A2DP_flowstop:%d, A2DP_full:%d)",
			     prptctrl->mb_a2dp_empty_cnt,
			     prptctrl->mb_a2dp_flct_cnt,
			     prptctrl->mb_a2dp_full_cnt);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : wl_rfk[req:%d/go:%d/reject:%d/timeout:%d]",
		     	"[RFK]", cx->cnt_wl[BTC_WCNT_RFK_REQ],
		     	cx->cnt_wl[BTC_WCNT_RFK_GO],
		     	cx->cnt_wl[BTC_WCNT_RFK_REJECT],
		     	cx->cnt_wl[BTC_WCNT_RFK_TIMEOUT]);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			", bt_rfk[req:%d/go:%d/reject:%d/timeout:%d/fail:%d]",
			     prptctrl->bt_rfk_cnt[BTC_BCNT_RFK_REQ],
			     prptctrl->bt_rfk_cnt[BTC_BCNT_RFK_GO],
			     prptctrl->bt_rfk_cnt[BTC_BCNT_RFK_REJECT],
			     prptctrl->bt_rfk_cnt[BTC_BCNT_RFK_TIMEOUT],
			     prptctrl->bt_rfk_cnt[BTC_BCNT_RFK_FAIL]);

		if (prptctrl->bt_rfk_cnt[BTC_BCNT_RFK_TIMEOUT] > 0)
			bt->rfk_info.map.timeout = 1;
		else
			bt->rfk_info.map.timeout = 0;

		dm->error.map.wl_rfk_timeout = bt->rfk_info.map.timeout;
	} else {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : h2c_cnt=%d(fail:%d), c2h_cnt=%d, rpt_cnt=%d",
			     "[summary]", pfwinfo->cnt_h2c,
			     pfwinfo->cnt_h2c_fail, pfwinfo->cnt_c2h,
			     pfwinfo->event[BTF_EVNT_RPT]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			" (WL FW report invalid!!)");
	}

	for (i = 0; i < BTC_NCNT_MAX; i++)
		cnt_sum += dm->cnt_notify[i];

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : total=%d, show_coex_info=%d, power_on=%d, init_coex=%d, ",
		     "[notify_cnt]", cnt_sum, cnt[BTC_NCNT_SHOW_COEX_INFO],
		     cnt[BTC_NCNT_POWER_ON], cnt[BTC_NCNT_INIT_COEX]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"power_off=%d, radio_state=%d, role_info=%d, wl_rfk=%d, wl_sta=%d",
		     cnt[BTC_NCNT_POWER_OFF], cnt[BTC_NCNT_RADIO_STATE],
		     cnt[BTC_NCNT_ROLE_INFO], cnt[BTC_NCNT_WL_RFK],
		     cnt[BTC_NCNT_WL_STA]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : scan_start=%d, scan_finish=%d, switch_band=%d, special_pkt=%d, ",
		     "[notify_cnt]", cnt[BTC_NCNT_SCAN_START],
		     cnt[BTC_NCNT_SCAN_FINISH], cnt[BTC_NCNT_SWITCH_BAND],
		     cnt[BTC_NCNT_SPECIAL_PACKET]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"timer=%d, control=%d, customerize=%d",
		     cnt[BTC_NCNT_TIMER], cnt[BTC_NCNT_CONTROL],
		     cnt[BTC_NCNT_CUSTOMERIZE]);
}

static void _show_error(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct fbtc_cysta *pcysta = NULL;

	pcysta = &pfwinfo->rpt_fbtc_cysta.finfo;

	if (pfwinfo->event[BTF_EVNT_BUF_OVERFLOW] == 0 &&
	    pcysta->except_cnt == 0 &&
	    !pfwinfo->len_mismch && !pfwinfo->fver_mismch)
	    return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : ", "[error]");

	if (pfwinfo->event[BTF_EVNT_BUF_OVERFLOW]) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"overflow-cnt: %d, ",
			     pfwinfo->event[BTF_EVNT_BUF_OVERFLOW]);
	}

	if (pfwinfo->len_mismch) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"len-mismatch: 0x%x, ",
			     pfwinfo->len_mismch);
	}

	if (pfwinfo->fver_mismch) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"fver-mismatch: 0x%x, ",
			     pfwinfo->fver_mismch);
	}

	/* cycle statistics exceptions */
	if (pcysta->exception || pcysta->except_cnt) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"exception-type: 0x%x, exception-cnt = %d",
			     pcysta->exception, pcysta->except_cnt);
	}
}

static void _show_fbtc_tdma(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_tdma *t = NULL;
	struct fbtc_slot *s = NULL;
	struct btc_dm *dm = &btc->dm;
	u8 i, cnt = 0;

	pcinfo = &pfwinfo->rpt_fbtc_tdma.cinfo;
	if (!pcinfo->valid)
		return;

	t = &pfwinfo->rpt_fbtc_tdma.finfo;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : ", "[tdma_policy]");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"type:%s, rx_flow_ctrl:%d, tx_pause:%d, ",
		     id_to_str(BTC_STR_TDMA, (u32)t->type),
		     t->rxflctrl, t->txpause);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"wl_toggle_n:%d, leak_n:%d, ext_ctrl:%d, ",
		     t->wtgle_n, t->leak_n, t->ext_ctrl);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"policy_type:%s",
		     id_to_str(BTC_STR_POLICY, (u32)btc->policy_type));

	s = pfwinfo->rpt_fbtc_slots.finfo.slot;

	for (i = 0; i < CXST_MAX; i++) {
		if (dm->update_slot_map == BIT(CXST_MAX) - 1)
			break;

		if (!(dm->update_slot_map & BIT(i)))
			continue;

		if (cnt % 6 == 0)
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				"\n\r %-15s : %s[%d/0x%x/%d]",
				     "[slot_policy]",
				     id_to_str(BTC_STR_SLOT, (u32)i),
				     s[i].dur, s[i].cxtbl, s[i].cxtype);
		else
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				", %s[%d/0x%x/%d]",
				     id_to_str(BTC_STR_SLOT, (u32)i),
				     s[i].dur, s[i].cxtbl, s[i].cxtype);
		cnt++;
	}
}

static void _show_fbtc_slots(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_slots *pslots = NULL;
	struct fbtc_slot s;
	u8 i = 0;

	pcinfo = &pfwinfo->rpt_fbtc_slots.cinfo;
	if (!pcinfo->valid)
		return;

	pslots = &pfwinfo->rpt_fbtc_slots.finfo;

	for (i = 0; i < CXST_MAX; i++) {
		s = pslots->slot[i];
		if (i % 6 == 0)
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				"\n\r %-15s : %s[%d/0x%x/%d]",
				     "[slot_list]",
				     id_to_str(BTC_STR_SLOT, (u32)i),
				     s.dur, s.cxtbl, s.cxtype);
		else
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				", %s[%d/0x%x/%d]",
				     id_to_str(BTC_STR_SLOT, (u32)i),
				     s.dur, s.cxtbl, s.cxtype);
	}
}

static void _show_fbtc_cysta(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_dm *dm = &btc->dm;
	struct btc_bt_a2dp_desc *a2dp = &btc->cx.bt.link_info.a2dp_desc;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_cysta *pcysta = NULL;
	union fbtc_rxflct r;
	u8 i;

	pcinfo = &pfwinfo->rpt_fbtc_cysta.cinfo;
	if (!pcinfo->valid)
		return;

	pcysta = &pfwinfo->rpt_fbtc_cysta.finfo;
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : cycle:%d, bcn_erly:%d",
		     "[cycle_cnt]", pcysta->cycles, pcysta->brly_cnt);

	_chk_err_state(btc, BTC_DCNT_CYCLE_FREEZE, (u32)pcysta->cycles);

	for (i = 0; i < CXST_MAX; i++) {
		if (!pcysta->slot_cnt[i])
			continue;
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			", %s:%d",
			     id_to_str(BTC_STR_SLOT, (u32)i),
			     pcysta->slot_cnt[i]);
	}

	if (dm->tdma_now.rxflctrl) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			", leak_rx:%d", pcysta->leakrx_cnt);
	}

	if (pcysta->collision_cnt) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			", collision:%d", pcysta->collision_cnt);
	}

	_chk_err_state(btc, BTC_DCNT_W1_FREEZE, pcysta->slot_cnt[CXST_W1]);
	_chk_err_state(btc, BTC_DCNT_B1_FREEZE, pcysta->slot_cnt[CXST_B1]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : avg_t[wl:%d/bt:%d/lk:%d.%03d]",
		     "[cycle_time]",
		     pcysta->tavg_cycle[CXT_WL],
		     pcysta->tavg_cycle[CXT_BT],
		     pcysta->tavg_lk/1000, pcysta->tavg_lk%1000);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		", max_t[wl:%d/bt:%d/lk:%d.%03d]",
		     pcysta->tmax_cycle[CXT_WL],
		     pcysta->tmax_cycle[CXT_BT],
		     pcysta->tmax_lk/1000, pcysta->tmax_lk%1000);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		", maxdiff_t[wl:%d/bt:%d]",
		     pcysta->tmaxdiff_cycle[CXT_WL],
		     pcysta->tmaxdiff_cycle[CXT_BT]);

	if (a2dp->exist) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"\n\r %-15s : a2dp_ept:%d, a2dp_late:%d",
			     "[a2dp_t_sta]",
			     pcysta->a2dpept, pcysta->a2dpeptto);
		r.val = dm->tdma_now.rxflctrl;
		if (r.type && r.tgln_n) {
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				", cycle[PSTDMA:%d/TDMA:%d], ",
				     pcysta->cycles_a2dp[CXT_FLCTRL_ON],
				     pcysta->cycles_a2dp[CXT_FLCTRL_OFF]);

			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				"avg_t[PSTDMA:%d/TDMA:%d], ",
				     pcysta->tavg_a2dp[CXT_FLCTRL_ON],
				     pcysta->tavg_a2dp[CXT_FLCTRL_OFF]);

			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				"max_t[PSTDMA:%d/TDMA:%d]",
				     pcysta->tmax_a2dp[CXT_FLCTRL_ON],
				     pcysta->tmax_a2dp[CXT_FLCTRL_OFF]);
		}
	}
}

static void _show_fbtc_nullsta(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_cynullsta *ns = NULL;
	u8 i = 0;

	if (!btc->dm.tdma_now.rxflctrl)
		return;

	pcinfo = &pfwinfo->rpt_fbtc_nullsta.cinfo;
	if (!pcinfo->valid)
		return;

	ns = &pfwinfo->rpt_fbtc_nullsta.finfo;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		"\n\r %-15s : ", "[null_sta]");

	for (i = 0; i < 2; i++) {
		if (i != 0)
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				", null-%d", i);
		else
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				"null-%d", i);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"[ok:%d/", ns->result[i][1]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"fail:%d/", ns->result[i][0]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"on_time:%d/", ns->result[i][2]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"retry:%d/", ns->result[i][3]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"avg_t:%d.%03d/",
			     ns->avg_t[i]/1000, ns->avg_t[i]%1000);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"max_t:%d.%03d]",
			     ns->max_t[i]/1000, ns->max_t[i]%1000);
	}
}

static void _show_fbtc_step(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_steps *pstep = NULL;
	u8 i, type, val, cnt;
	u16 diff_t;

	pcinfo = &pfwinfo->rpt_fbtc_step.cinfo;
	if (!pcinfo->valid)
		return;

	pstep = &pfwinfo->rpt_fbtc_step.finfo;

	cnt = 0;
	for (i = 0; i < btc->ctrl.trace_step; i++) {
		type = pstep->step[i].type;
		val = pstep->step[i].val;
		diff_t = pstep->step[i].difft;

		if (type == CXSTEP_NONE || type >= CXSTEP_MAX)
			continue;

		if (cnt % 10 == 0) {
			BTC_CNSL(out_len, *used, output + *used, out_len - *used,
				"\n\r %-15s : ", "[steps]");
		}

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			"-> %s(%d)",
			     (type == CXSTEP_SLOT?
			     id_to_str(BTC_STR_SLOT, (u32)val) :
			     id_to_str(BTC_STR_EVENT, (u32)val)), diff_t);
		cnt++;

		if (cnt == pstep->cnt)
			break;
	}
}

static void _show_fw_dm_msg(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	if (!(btc->dm.coex_info_map & BTC_COEX_INFO_DM))
		return;

	_show_error(btc, used, input, input_num, output, out_len);
	_show_fbtc_tdma(btc, used, input, input_num, output, out_len);
	_show_fbtc_slots(btc, used, input, input_num, output, out_len);
	_show_fbtc_cysta(btc, used, input, input_num, output, out_len);
	_show_fbtc_nullsta(btc, used, input, input_num, output, out_len);
	_show_fbtc_step(btc, used, input, input_num, output, out_len);
}

static void _cmd_show(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	_show_cx_info(btc, used, input, input_num, output, out_len);
	_show_wl_info(btc, used, input, input_num, output, out_len);
	_show_bt_info(btc, used, input, input_num, output, out_len);
	_show_dm_info(btc, used, input, input_num, output, out_len);
	_show_fw_dm_msg(btc, used, input, input_num, output, out_len);
	_show_mreg(btc, used, input, input_num, output, out_len);
	_show_summary(btc, used, input, input_num, output, out_len);
}

static void _cmd_wb(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 type = 0, addr = 0, val = 0;

	if (input_num < 4)
		goto help;

	_os_sscanf(input[1], "%d", &type);
	_os_sscanf(input[2], "%x", &addr);
	_os_sscanf(input[3], "%x", &val);

	if (type > 3)
		goto help;

	_write_bt_reg(btc, (u8)type, (u16)addr, val);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" wb type=%d, addr=0x%x, val=0x%x !! \n", type, addr, val);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" wb <type --- 0:rf, 1:modem, 2:bluewize, 3:vendor> <addr:16bits> <val> \n");
	return;
}

static void _cmd_set_coex(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		     u32 input_num, char *output, u32 out_len)
{
	RTW_ERR("%s NEO TODO\n", __func__);
#if 0
	u32 mode = 0;

	if (input_num < 2)
		goto help;

	_os_sscanf(input[1], "%d", &mode);

	switch(mode) {
	case 0: /* original */
		btc->ctrl.manual = 0;
		btc->ctrl.always_freerun = 0;
		btc->dm.wl_only = 0;
		btc->dm.bt_only = 0;
		_set_init_info(btc);
		hal_btc_fw_set_drv_info(btc, CXDRVINFO_INIT);
		hal_btc_fw_set_drv_info(btc, CXDRVINFO_CTRL);
		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " recovery original coex mechanism !! \n");
		break;
	case 1: /* freeze */
		btc->ctrl.manual = 1;
		hal_btc_fw_set_drv_info(btc, CXDRVINFO_CTRL);
		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " freeze coex mechanism !! \n");
		break;
	case 2: /* fix freerun */
		btc->ctrl.always_freerun = 1;
		hal_btc_fw_set_drv_info(btc, CXDRVINFO_CTRL);
		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " freerun coex mechanism !! \n");
		break;
	case 3: /* fix wl only */
		btc->dm.wl_only = 1;
		_set_init_info(btc);
		hal_btc_fw_set_drv_info(btc, CXDRVINFO_INIT);
		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " always WL-only coex mechanism!! \n");
		break;
	case 4: /* fix bt only */
		btc->dm.bt_only = 1;
		_set_init_info(btc);
		hal_btc_fw_set_drv_info(btc, CXDRVINFO_INIT);
		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " always BT-only coex mechanism!! \n");
		break;
	}
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" mode <val--- 0:original, 1:freeze coex, 2:always-freerun, 3:always-WLonly, 4:always-BTonly> \n");
	return;
#endif // if 0
}

static void _cmd_update_policy(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			    u32 input_num, char *output, u32 out_len)
{
	hal_btc_fw_set_policy(btc, FC_EXEC, (u16)BTC_CXP_MANUAL<<8, __func__);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " Update tdma/slot policy !! \n");
}

static void _cmd_tdma(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 para1 = 0, para2 = 0;
	u8 buf[8] = {0};

	if (input_num < 3)
		goto help;

	_os_sscanf(input[1], "%x", &para1);
	_os_sscanf(input[2], "%x", &para2);

	buf[0] = (u8)((para1 & bMASKB3) >> 24);
	buf[1] = (u8)((para1 & bMASKB2) >> 16);
	buf[2] = (u8)((para1 & bMASKB1) >> 8);
	buf[3] = (u8)(para1 & bMASKB0);

	buf[4] = (u8)((para2 & bMASKB3) >> 24);
	buf[5] = (u8)((para2 & bMASKB2) >> 16);
	buf[6] = (u8)((para2 & bMASKB1) >> 8);
	buf[7] = (u8)(para2 & bMASKB0);

	if (!hal_btc_fw_set_1tdma(btc, (u16)sizeof(struct fbtc_tdma), buf))
		goto help;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set tdma parameters type=%s, rxflctrl=0x%x, txpause=%d, wtgle_n=%d, leak_n=%d, ext_ctrl=%d\n",
		 id_to_str(BTC_STR_TDMA, (u32)buf[0]), buf[1], buf[2], buf[3], buf[4], buf[5]);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " tdma <para1:32bits> <para2:32bits>\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "       <para1>  bit[31:24]-> type --- 0:off, 1:fix, 2:auto, 3:auto2\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[23:19]-> rx_flow_ctrl --- send rx flow ctrl every n*w2b-slot\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[18:16]-> rx_flow_ctrl_type --- type 0:off, 1:null, 2:Qos-null, 3:cts\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[15:8] -> tx_pause_at_BT_slot --- 0:off, 1:on\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[7:0]  -> wtgle_n --- toggle coex table every n*w1-slot\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "       <para2>  bit[31:24]-> leak_n --- enter leak slot every n*w2b-slot\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[23:16]-> ext_ctrl --- 0:off, 1:bcn-early_protect only 2:ext\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[15:8] -> resevrd\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[7:0]  -> resevrd\n");
	return;
}

static void _cmd_slot(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 sid = 0, dur = 0, cx_tbl = 0, cx_type = 0, i;
	u32 *tmp32 = NULL;
	u8 buf[9] = {0};

	if (input_num < 5)
		goto help;

	_os_sscanf(input[1], "%d", &sid);
	_os_sscanf(input[2], "%d", &dur);
	_os_sscanf(input[3], "%x", &cx_tbl);
	_os_sscanf(input[4], "%d", &cx_type);

	buf[0] = (u8) sid;
	buf[1] = (u8)(dur & bMASKB0);
	buf[2] = (u8)((dur & bMASKB1) >> 8);

	tmp32 = (u32 *)&buf[3];
	*tmp32 = cx_tbl;

	buf[7] = (u8)(cx_type & bMASKB0);
	buf[8] = (u8)((cx_type & bMASKB1) >> 8);

	if (!hal_btc_fw_set_1slot(btc, (u16)sizeof(struct fbtc_slot)+1, buf))
		goto help;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set slot parameters: slot:%s, duration:%d, table:%08x, type=%s!!\n",
		 id_to_str(BTC_STR_SLOT, sid), dur, cx_tbl,
		 (cx_type? "iso" : "mix"));
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " slot <slot_id:8bits> <slot_duration:16bits> <coex_table:32bits> <type:16bits>\n");

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "      <slot_id> \n");

	for (i = 0; i < CXST_MAX; i++) {
		if ((i+1) % 5 == 1)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "                %d: %s",
				 i, id_to_str(BTC_STR_SLOT, i));
		else
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 ", %d: %s", i, id_to_str(BTC_STR_SLOT, i));

		if (((i+1) % 5 == 0) || (i == CXST_MAX - 1))
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "\n");
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "      <slot_duration> unit: ms\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "      <coex_table> 32bits coex table\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "      <type> BT packet at WL slot 0: allowed, 1: isolated\n");
	return;
}

static void _cmd_sig_gdbg_en(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			     u32 input_num, char *output, u32 out_len)
{
	u32 map = 0, i;

	if (input_num < 2)
		goto help;

	_os_sscanf(input[1], "%x", &map);

	hal_btc_fw_set_gpio_dbg(btc, CXDGPIO_EN_MAP, map);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " signal to gpio debug map = 0x%08x!!\n", map);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " sig <enable_map:hex/32bits> \n");

	for (i = 0; i < BTC_DBG_MAX1; i++) {
		if ((i+1) % 8 == 1)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "      %d: %s",
				 i, id_to_str(BTC_STR_GDBG, i));
		else
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 ", %d: %s", i, id_to_str(BTC_STR_GDBG, i));

		if (((i+1) % 8 == 0) || (i == BTC_DBG_MAX1 - 1))
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "\n");
	}
	return;
}

static void _cmd_sgpio_map(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			   u32 input_num, char *output, u32 out_len)
{
	u32 sig = 0, gpio = 0;

	if (input_num < 3)
		goto help;

	_os_sscanf(input[1], "%d", &sig);
	_os_sscanf(input[2], "%d", &gpio);

	if ((sig > 31) || (gpio > 7))
		goto help;

	hal_btc_fw_set_gpio_dbg(btc, CXDGPIO_MUX_MAP, ((gpio << 8) + sig));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " signal-%s -> gpio-%d\n", id_to_str(BTC_STR_GDBG, sig), gpio);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " gpio <sig:0~31> <gpio:0~7>\n");
	return;
}

static void _cmd_trace_step(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			    u32 input_num, char *output, u32 out_len)
{
	RTW_ERR("%s TODO NEO\n", __func__);
#if 0
	u32 step = 0;

	if (input_num < 2)
		goto help;

	_os_sscanf(input[1], "%d", &step);

	if (step >= FCXMAX_STEP)
		btc->ctrl.trace_step = FCXMAX_STEP;
	else if (step == 0)
		btc->ctrl.trace_step = FCXDEF_STEP;
	else
		btc->ctrl.trace_step = (u16) step;

	hal_btc_fw_set_drv_info(btc, CXDRVINFO_CTRL);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set tdma trace step = %d!!\n", btc->ctrl.trace_step);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" step <tdma_trace_step: 1~%d > \n", FCXMAX_STEP);
	return;
#endif
}

static void _cmd_wl_tx_power(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			     u32 input_num, char *output, u32 out_len)
{
	u32 pwr = 0;
	u8 is_negative = 0;

	if (input_num < 2)
		goto help;

	_os_sscanf(input[1], "%d", &pwr);
	if (pwr & BIT(31)) {
		pwr = ~pwr + 1;
		is_negative = 1;
	}

	pwr = pwr & bMASKB0;

	if ((pwr != 255) && (pwr > 20))
		goto help;

	if (is_negative)
		pwr |= BIT(7);

	_set_wl_tx_power(btc, pwr);

	if (pwr == 0xff)
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set wl tx power level to original!!\n");
	else if (is_negative)
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set wl tx power level = -%d dBm!!\n", pwr & 0x7f);
	else
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set wl tx power level = +%d dBm!!\n", pwr & 0x7f);

	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" wpwr <wl_tx_power(dBm): -20dbm ~ +20dBm, 255-> original tx power\n");
	return;
}

static void _cmd_wl_rx_lna(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			   u32 input_num, char *output, u32 out_len)
{
	u32 lna = 0;

	if (input_num < 2)
		goto help;

	_os_sscanf(input[1], "%d", &lna);

	_set_wl_rx_gain(btc, lna);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set wl rx level = %d!!\n", lna);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" wlna <wl_rx_level: 0~7> \n");
	return;
}

static void _cmd_bt_afh_map(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			    u32 input_num, char *output, u32 out_len)
{
	u32 en = 0, ch = 0, bw = 0;

	if (input_num < 4)
		goto help;

	_os_sscanf(input[1], "%d", &en);
	_os_sscanf(input[2], "%d", &ch);
	_os_sscanf(input[3], "%d", &bw);

	btc->cx.wl.afh_info.en = (u8)en;
	btc->cx.wl.afh_info.ch = (u8)ch;
	btc->cx.wl.afh_info.bw = (u8)bw;
	hal_btc_fw_set_bt(btc, SET_BT_WL_CH_INFO, 3, (u8*)&btc->cx.wl.afh_info);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set bt afh map: en=%d, ch=%d, map=%d!!\n", en, ch, bw);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" afh <en: 0/1> <ch: 0~255> <bw: 0~255>\n");
	return;
}

static void _cmd_bt_tx_power(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			     u32 input_num, char *output, u32 out_len)
{
	u32 pwr = 0;

	if (input_num < 2)
		goto help;

	_os_sscanf(input[1], "%d", &pwr);

	_set_bt_tx_power(btc, pwr);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " decrease bt tx power level = %d!!\n", pwr);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" bpwr <decrease power: 0~255 > \n");
	return;
}

static void _cmd_bt_rx_lna(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			   u32 input_num, char *output, u32 out_len)
{
	u32 lna = 0;

	if (input_num < 2)
		goto help;

	_os_sscanf(input[1], "%d", &lna);

	if (lna > 7)
		goto help;

	_set_bt_rx_gain(btc, lna);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set bt rx lna constrain level = %d!!\n", lna);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " blna <lna_constrain: 0~7> \n");
	return;
}

static void _cmd_bt_igno_wlan(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			      u32 input_num, char *output, u32 out_len)
{
	u32 igno = 0;

	if (input_num < 2)
		goto help;

	_os_sscanf(input[1], "%d", &igno);

	igno = igno & BIT(0);

	_set_bt_ignore_wlan_act(btc, (u8)igno);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set bt ignore wlan = %d!!\n", igno);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" igwl <0: don't ignore wlan, 1: ignore wlan > \n");
	return;
}

static void _cmd_set_gnt_wl(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			      u32 input_num, char *output, u32 out_len)
{
	u32 gwl = 0, phy_map = BTC_PHY_ALL;

	if ((input_num != 2) && (input_num != 3))
		goto help;

	_os_sscanf(input[1], "%d", &gwl);

	if (input_num == 3) {
		_os_sscanf(input[2], "%d", &phy_map);
		phy_map = (phy_map == 0? BTC_PHY_0 : BTC_PHY_1);
	}

	switch(gwl) {
	case 0:
		_set_gnt_wl(btc, (u8)phy_map, BTC_GNT_SW_LO);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_wl = SW-0 (phy_map=0x%x)\n", phy_map);
		break;
	case 1:
		_set_gnt_wl(btc, (u8)phy_map, BTC_GNT_SW_HI);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_wl = SW-1 (phy_map=0x%x)\n", phy_map);
		break;
	case 2:
		_set_gnt_wl(btc, (u8)phy_map, BTC_GNT_HW);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_wl = HW-PTA ctrl (phy_map=0x%x)\n", phy_map);
		break;
	}

	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" gwl <0:SW_0, 1:SW_1, 2:HW_PTA> <0:PHY-0, 1:PHY-1, none:All> \n");
	return;
}

static void _cmd_set_gnt_bt(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			      u32 input_num, char *output, u32 out_len)
{
	u32 gbt = 0, phy_map = BTC_PHY_ALL;

	if ((input_num != 2) && (input_num != 3))
		goto help;

	_os_sscanf(input[1], "%d", &gbt);

	if (input_num == 3) {
		_os_sscanf(input[2], "%d", &phy_map);
		phy_map = (phy_map == 0? BTC_PHY_0 : BTC_PHY_1);
	}

	switch(gbt) {
	case 0:
		_set_gnt_bt(btc, (u8)phy_map, BTC_GNT_SW_LO);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_bt = SW-0 (phy_map=0x%x)\n", phy_map);
		break;
	case 1:
		_set_gnt_bt(btc, (u8)phy_map, BTC_GNT_SW_HI);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_bt = SW-1 (phy_map=0x%x)\n", phy_map);
		break;
	case 2:
		_set_gnt_bt(btc, (u8)phy_map, BTC_GNT_HW);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_bt = HW-PTA ctrl (phy_map=0x%x)\n", phy_map);
		break;
	}

	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" gbt <0:SW_0, 1:SW_1, 2:HW_PTA> <0:PHY-0, 1:PHY-1, none:All> \n");
	return;
}

static void _cmd_set_bt_psd(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			    u32 input_num, char *output, u32 out_len)
{
	u32 idx = 0, type = 0;

	if (input_num < 3)
		goto help;

	_os_sscanf(input[1], "%d", &idx);
	_os_sscanf(input[2], "%d", &type);

	_set_bt_psd_report(btc, (u8)idx, (u8)type);
	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		" bpsd <start_idex, dec-8bit> <report_type, dec-8bit> \n");
	return;
}

void halbtc_cmd_parser(struct btc_t *btc, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 used = 0;
	u8 id = 0;
	u32 i;
	u32 array_size = sizeof(halbtc_cmd_i) / sizeof(struct halbtc_cmd_info);

	BTC_CNSL(out_len, used, output + used, out_len - used, "\n");

	/* Parsing Cmd ID */
	if (input_num) {
		for (i = 0; i < array_size; i++) {
			if (_os_strcmp(halbtc_cmd_i[i].name, input[0]) == 0) {
				id = halbtc_cmd_i[i].id;
				break;
			}
		}
	}

	switch (id) {
	case HALBTC_TEST:
		break;
	case HALBTC_SHOW:
		_cmd_show(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_WRITE_BT:
		_cmd_wb(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_READ_BT:
		_cmd_rb(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SET_COEX:
		_cmd_set_coex(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_UPDATE_POLICY:
		_cmd_update_policy(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_TDMA:
		_cmd_tdma(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SLOT:
		_cmd_slot(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SIG_GDBG_EN:
		_cmd_sig_gdbg_en(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SGPIO_MAP:
		_cmd_sgpio_map(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_TRACE_STEP:
		_cmd_trace_step(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_WL_TX_POWER:
		_cmd_wl_tx_power(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_WL_RX_LNA:
		_cmd_wl_rx_lna(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_BT_AFH_MAP:
		_cmd_bt_afh_map(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_BT_TX_POWER:
		_cmd_bt_tx_power(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_BT_RX_LNA:
		_cmd_bt_rx_lna(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_BT_IGNO_WLAN:
		_cmd_bt_igno_wlan(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SET_GNT_WL:
		_cmd_set_gnt_wl(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SET_GNT_BT:
		_cmd_set_gnt_bt(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SET_BT_PSD:
		_cmd_set_bt_psd(btc, &used, input, input_num, output, out_len);
		break;
	default:
		BTC_CNSL(out_len, used, output + used, out_len - used,
			 "command not supported !!\n");

		/* fall through */

	case HALBTC_HELP:
		BTC_CNSL(out_len, used, output + used, out_len - used,
			 "BTC cmd ==>\n");

		for (i = 0; i < array_size - 1; i++)
			BTC_CNSL(out_len, used, output + used, out_len - used,
				 " %s\n", halbtc_cmd_i[i + 1].name);
		break;
	}
}

s32 halbtc_cmd(struct btc_t *btc, char *input, char *output, u32 out_len)
{
	char *token;
	u32 argc = 0;
	char argv[MAX_ARGC][MAX_ARGV];

	do {
		token = _os_strsep(&input, ", ");
		if (token) {
			if (_os_strlen((u8 *)token) <= MAX_ARGV)
				_os_strcpy(argv[argc], token);

			argc++;
		} else {
			break;
		}
	} while (argc < MAX_ARGC);

	if (argc == 1)
		argv[0][_os_strlen((u8 *)argv[0]) - 1] = '\0';

	halbtc_cmd_parser(btc, argv, argc, output, out_len);
	return 0;
}

#endif

