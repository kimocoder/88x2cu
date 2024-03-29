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
#ifndef __PHL_TX_H_
#define __PHL_TX_H_

void phl_datapath_deinit(struct phl_info_t *phl_info);
enum rtw_phl_status phl_datapath_init(struct phl_info_t *phl_info);
enum rtw_phl_status phl_datapath_start(struct phl_info_t *phl_info);
void phl_datapath_stop(struct phl_info_t *phl_info);

struct phl_ring_status *phl_alloc_ring_sts(struct phl_info_t *phl_info);
void phl_release_ring_sts(struct phl_info_t *phl_info,
				struct phl_ring_status *ring_sts);

u8 phl_check_xmit_ring_resource(struct phl_info_t *phl_info,
				_os_list *sta_list);
void phl_tx_flow_ctrl(struct phl_info_t *phl_info, _os_list *sta_list);
enum rtw_phl_status rtw_phl_tx_req_notify(void *phl);
enum rtw_phl_status phl_register_handler(struct rtw_phl_com_t *phl_com,
				struct rtw_phl_handler *handler);
enum rtw_phl_status phl_deregister_handler(struct rtw_phl_com_t *phl_com,
				struct rtw_phl_handler *handler);
enum rtw_phl_status phl_schedule_handler(struct rtw_phl_com_t *phl_com,
				struct rtw_phl_handler *handler);
enum rtw_phl_status phl_indic_pkt_complete(void *phl);

enum rtw_phl_status phl_register_tx_ring(void *phl, u16 macid);
enum rtw_phl_status phl_deregister_tx_ring(void *phl, u16 macid);
void phl_free_deferred_tx_ring(struct phl_info_t *phl_info);

enum rtw_phl_status phl_enqueue_busy_h2c_pkt(struct phl_info_t *phl_info,
				struct rtw_h2c_pkt *h2c_pkt, u8 pos);

enum rtw_phl_status phl_enqueue_idle_h2c_pkt(struct phl_info_t *phl_info,
				struct rtw_h2c_pkt *h2c_pkt);

struct rtw_h2c_pkt *phl_query_busy_h2c_pkt(struct phl_info_t *phl_info);
struct rtw_h2c_pkt *phl_query_idle_h2c_pkt(struct phl_info_t *phl_info, u8 type);

/**
 * this function will be used in read / write pointer mechanism and
 * return the number of available read pointer
 * @rptr: input, the read pointer
 * @wptr: input, the write pointer
 * @bndy: input, the boundary of read / write pointer mechanism
 */
u16 phl_calc_avail_rptr(u16 rptr, u16 wptr, u16 bndy);

/**
 * this function will be used in read / write pointer mechanism and
 * return the number of available write pointer
 * @rptr: input, the read pointer
 * @wptr: input, the write pointer
 * @bndy: input, the boundary of read / write pointer mechanism
 */
u16 phl_calc_avail_wptr(u16 rptr, u16 wptr, u16 bndy);

void phl_dump_sorted_ring(_os_list *sorted_ring);
void phl_dump_tx_plan(_os_list *sta_list);
void phl_dump_t_fctrl_result(_os_list *t_fctrl_result);
void phl_tx_watchdog(struct phl_info_t *phl_info);
void phl_reset_tx_stats(struct rtw_stats *stats);

#endif	/* __PHL_TX_H_ */
