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
#define _PHL_RX_C_
#include "phl_headers.h"


struct rtw_phl_rx_pkt *rtw_phl_query_phl_rx(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_rx_pkt_pool *rx_pkt_pool = NULL;
	struct rtw_phl_rx_pkt *phl_rx = NULL;

	rx_pkt_pool = (struct phl_rx_pkt_pool *)phl_info->rx_pkt_pool;

	_os_spinlock(drv_priv, &rx_pkt_pool->idle_lock, _bh, NULL);

	if (false == list_empty(&rx_pkt_pool->idle)) {
		phl_rx = list_first_entry(&rx_pkt_pool->idle,
					struct rtw_phl_rx_pkt, list);
		list_del(&phl_rx->list);
		rx_pkt_pool->idle_cnt--;
	}

	_os_spinunlock(drv_priv, &rx_pkt_pool->idle_lock, _bh, NULL);

	return phl_rx;
}

u8 rtw_phl_is_phl_rx_idle(struct phl_info_t *phl_info)
{
	struct phl_rx_pkt_pool *rx_pkt_pool = NULL;
	u8 res = false;

	rx_pkt_pool = (struct phl_rx_pkt_pool *)phl_info->rx_pkt_pool;

	_os_spinlock(phl_to_drvpriv(phl_info), &rx_pkt_pool->idle_lock, _bh, NULL);

	if (MAX_PHL_RING_RX_PKT_NUM == rx_pkt_pool->idle_cnt)
		res = true;
	else
		res = false;

	_os_spinunlock(phl_to_drvpriv(phl_info), &rx_pkt_pool->idle_lock, _bh, NULL);

	return res;
}

void phl_dump_rx_stats(struct rtw_stats *stats)
{
	PHL_TRACE(COMP_PHL_XMIT, _PHL_DEBUG_,
		  "Dump Rx statistics\n"
		  "rx_byte_uni = %lld\n"
		  "rx_byte_total = %lld\n"
		  "rx_tp_kbits = %d\n"
		  "last_rx_time_ms = %d\n",
		  stats->rx_byte_uni,
		  stats->rx_byte_total,
		  stats->rx_tp_kbits,
		  stats->last_rx_time_ms);
}

void phl_reset_rx_stats(struct rtw_stats *stats)
{
	stats->rx_byte_uni = 0;
	stats->rx_byte_total = 0;
	stats->rx_tp_kbits = 0;
	stats->last_rx_time_ms = 0;
	stats->rxtp.last_calc_time_ms = 0;
	stats->rxtp.last_calc_time_ms = 0;
	stats->rx_traffic.lvl = RTW_TFC_IDLE;
	stats->rx_traffic.sts = 0;
}

void phl_update_rx_stats(struct rtw_stats *stats, struct rtw_recv_pkt *rx_pkt)
{
	u32 diff_t = 0, cur_time = _os_get_cur_time_ms();
	u64 diff_bits = 0;

	stats->last_rx_time_ms = cur_time;
	stats->rx_byte_total += rx_pkt->mdata.pktlen;
	if (rx_pkt->mdata.bc == 0 && rx_pkt->mdata.mc == 0)
		stats->rx_byte_uni += rx_pkt->mdata.pktlen;

	if (0 == stats->rxtp.last_calc_time_ms ||
		0 == stats->rxtp.last_calc_bits) {
		stats->rxtp.last_calc_time_ms = stats->last_rx_time_ms;
		stats->rxtp.last_calc_bits = stats->rx_byte_uni * 8;
	} else {
		if (cur_time >= stats->rxtp.last_calc_time_ms) {
			diff_t = cur_time - stats->rxtp.last_calc_time_ms;
		} else {
			diff_t = RTW_U32_MAX - stats->rxtp.last_calc_time_ms +
				cur_time + 1;
		}
		if (diff_t > RXTP_CALC_DIFF_MS && stats->rx_byte_uni != 0) {
			diff_bits = (stats->rx_byte_uni * 8) -
				stats->rxtp.last_calc_bits;
			stats->rx_tp_kbits = (u32)_os_division64(diff_bits,
								 diff_t);
			stats->rxtp.last_calc_bits = stats->rx_byte_uni * 8;
			stats->rxtp.last_calc_time_ms = cur_time;
		}
	}
}


void phl_rx_statistics(struct phl_info_t *phl_info, struct rtw_recv_pkt *rx_pkt)
{
	RTW_ERR("TODO %s for rtw_phl_get_stainfo_by_macid\n", __func__);
#if 0 // NEO TODO mark off first
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_stats *phl_stats = &phl_com->phl_stats;
	struct rtw_stats *sta_stats = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u16 macid = rx_pkt->mdata.macid;

	if (!phl_macid_is_valid(phl_info, macid))
		goto dev_stat;

	sta = rtw_phl_get_stainfo_by_macid(phl_info, macid);

	if (NULL == sta)
		goto dev_stat;
	sta_stats = &sta->stats;

	phl_update_rx_stats(sta_stats, rx_pkt);
dev_stat:
	phl_update_rx_stats(phl_stats, rx_pkt);
#endif // if 0 NEO
}


void phl_release_phl_rx(struct phl_info_t *phl_info,
				struct rtw_phl_rx_pkt *phl_rx)
{
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_rx_pkt_pool *rx_pkt_pool = NULL;

	rx_pkt_pool = (struct phl_rx_pkt_pool *)phl_info->rx_pkt_pool;

	_os_spinlock(drv_priv, &rx_pkt_pool->idle_lock, _bh, NULL);
	_os_mem_set(phl_to_drvpriv(phl_info), &phl_rx->r, 0, sizeof(phl_rx->r));
	phl_rx->type = RTW_RX_TYPE_MAX;
	phl_rx->rxbuf_ptr = NULL;
	INIT_LIST_HEAD(&phl_rx->list);
	list_add_tail(&phl_rx->list, &rx_pkt_pool->idle);
	rx_pkt_pool->idle_cnt++;
	_os_spinunlock(drv_priv, &rx_pkt_pool->idle_lock, _bh, NULL);
}

static void phl_free_recv_pkt_pool(struct phl_info_t *phl_info)
{
	struct phl_rx_pkt_pool *rx_pkt_pool = NULL;
	u32 buf_len = 0;
	FUNCIN();

	rx_pkt_pool = (struct phl_rx_pkt_pool *)phl_info->rx_pkt_pool;
	if (NULL != rx_pkt_pool) {
		_os_spinlock_free(phl_to_drvpriv(phl_info),
					&rx_pkt_pool->idle_lock);
		_os_spinlock_free(phl_to_drvpriv(phl_info),
					&rx_pkt_pool->busy_lock);

		buf_len = sizeof(*rx_pkt_pool);
		_os_mem_free(phl_to_drvpriv(phl_info), rx_pkt_pool, buf_len);
	}

	FUNCOUT();
}

void phl_rx_deinit(struct phl_info_t *phl_info)
{
	/* TODO: rx reorder deinit */

	/* TODO: peer info deinit */

	phl_free_recv_pkt_pool(phl_info);
}


static enum rtw_phl_status phl_alloc_recv_pkt_pool(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_rx_pkt_pool *rx_pkt_pool = NULL;
	struct rtw_phl_rx_pkt *phl_rx = NULL;
	u32 buf_len = 0, i = 0;
	FUNCIN_WSTS(pstatus);

	buf_len = sizeof(*rx_pkt_pool);
	rx_pkt_pool = _os_mem_alloc(phl_to_drvpriv(phl_info), buf_len);

	if (NULL != rx_pkt_pool) {
		_os_mem_set(phl_to_drvpriv(phl_info), rx_pkt_pool, 0, buf_len);
		INIT_LIST_HEAD(&rx_pkt_pool->idle);
		INIT_LIST_HEAD(&rx_pkt_pool->busy);
		_os_spinlock_init(phl_to_drvpriv(phl_info),
					&rx_pkt_pool->idle_lock);
		_os_spinlock_init(phl_to_drvpriv(phl_info),
					&rx_pkt_pool->busy_lock);
		rx_pkt_pool->idle_cnt = 0;

		for (i = 0; i < MAX_PHL_RING_RX_PKT_NUM; i++) {
			phl_rx = &rx_pkt_pool->phl_rx[i];
			INIT_LIST_HEAD(&phl_rx->list);
			list_add_tail(&phl_rx->list, &rx_pkt_pool->idle);
			rx_pkt_pool->idle_cnt++;
		}

		phl_info->rx_pkt_pool = rx_pkt_pool;

		pstatus = RTW_PHL_STATUS_SUCCESS;
	}

	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		phl_free_recv_pkt_pool(phl_info);
	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

enum rtw_phl_status phl_rx_init(struct phl_info_t *phl_info)
{
	enum rtw_phl_status status;

	/* Allocate rx packet pool */
	status = phl_alloc_recv_pkt_pool(phl_info);
	if (status != RTW_PHL_STATUS_SUCCESS)
		return status;

	/* TODO: Peer info init */


	/* TODO: Rx reorder init */

	return RTW_PHL_STATUS_SUCCESS;
}

void phl_recycle_rx_buf(struct phl_info_t *phl_info,
				struct rtw_phl_rx_pkt *phl_rx)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;
	struct rtw_rx_buf *rx_buf = NULL;

	do {
		if (NULL == phl_rx) {
			PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "[WARNING]phl_rx is NULL!\n");
			break;
		}

		rx_buf = (struct rtw_rx_buf *)phl_rx->rxbuf_ptr;

		PHL_TRACE(COMP_PHL_RECV, _PHL_DEBUG_, "[4] %s:: [%p]\n",
								__FUNCTION__, rx_buf);
		if (phl_rx->rxbuf_ptr) {
			pstatus = hci_trx_ops->recycle_rx_buf(phl_info, rx_buf,
								phl_rx->r.mdata.dma_ch,
								phl_rx->type);
		}
		if (RTW_PHL_STATUS_SUCCESS != pstatus && phl_rx->rxbuf_ptr)
			PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "[WARNING]recycle hci rx buf error!\n");

		phl_release_phl_rx(phl_info, phl_rx);

	} while (false);

}

void _phl_indic_new_rxpkt(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct rtw_evt_info_t *evt_info = &phl_info->phl_com->evt_info;
	void *drv_priv = phl_to_drvpriv(phl_info);
	FUNCIN_WSTS(pstatus);

	do {
		_os_spinlock(drv_priv, &evt_info->evt_lock, _bh, NULL);
		evt_info->evt_bitmap |= RTW_PHL_EVT_RX;
		_os_spinunlock(drv_priv, &evt_info->evt_lock, _bh, NULL);

		pstatus = phl_schedule_handler(phl_info->phl_com,
							&phl_info->phl_event_handler);
	} while (false);

	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "[WARNING] Trigger rx indic event fail!\n");

	FUNCOUT_WSTS(pstatus);

#ifdef PHL_RX_BATCH_IND
	phl_info->rx_new_pending = 0;
#endif
}

enum rtw_phl_status _phl_add_rx_pkt(struct phl_info_t *phl_info,
				    struct rtw_phl_rx_pkt *phl_rx)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_rx_ring *ring = &phl_info->phl_rx_ring;
	struct rtw_recv_pkt *recvpkt = &phl_rx->r;
	u16 ring_res = 0, wptr = 0, rptr = 0;
	void *drv = phl_to_drvpriv(phl_info);

	FUNCIN_WSTS(pstatus);
	_os_spinlock(drv, &phl_info->rx_ring_lock, _bh, NULL);

	if (!ring)
		goto out;

	wptr = (u16)_os_atomic_read(drv, &ring->phl_idx);
	rptr = (u16)_os_atomic_read(drv, &ring->core_idx);

	ring_res = phl_calc_avail_wptr(rptr, wptr, MAX_PHL_RING_ENTRY_NUM);
	PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_,
		"[3] _phl_add_rx_pkt::[Query] phl_idx =%d , core_idx =%d , ring_res =%d\n",
		_os_atomic_read(drv, &ring->phl_idx),
		_os_atomic_read(drv, &ring->core_idx),
		ring_res);
	if (ring_res <= 0) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "no ring resource to add new rx pkt!\n");
		pstatus = RTW_PHL_STATUS_RESOURCE;
		goto out;
	}

	wptr = wptr + 1;
	if (wptr >= MAX_PHL_RING_ENTRY_NUM)
		wptr = 0;

	ring->entry[wptr] = recvpkt;

	if (wptr)
		_os_atomic_inc(drv, &ring->phl_idx);
	else
		_os_atomic_set(drv, &ring->phl_idx, 0);

#ifdef PHL_RX_BATCH_IND
	phl_info->rx_new_pending = 1;
	pstatus = RTW_PHL_STATUS_SUCCESS;
#endif

out:
	_os_spinunlock(drv, &phl_info->rx_ring_lock, _bh, NULL);

	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

void
phl_handle_rx_frame_list(struct phl_info_t *phl_info,
						 _os_list *frames)
{
	struct rtw_phl_rx_pkt *pos, *n;
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	phl_list_for_loop_safe(pos, n, struct rtw_phl_rx_pkt, frames, list) {
		list_del(&pos->list);
		status = _phl_add_rx_pkt(phl_info, pos);
		if (RTW_PHL_STATUS_RESOURCE == status) {
			hci_trx_ops->recycle_rx_pkt(phl_info, pos);
		}
	}
#ifndef PHL_RX_BATCH_IND
	_phl_indic_new_rxpkt(phl_info);
#endif

}


#define SEQ_MODULO 0x1000
#define SEQ_MASK	0xfff

static inline int seq_less(u16 sq1, u16 sq2)
{
	return ((sq1 - sq2) & SEQ_MASK) > (SEQ_MODULO >> 1);
}

static inline u16 seq_inc(u16 sq)
{
	return (sq + 1) & SEQ_MASK;
}

static inline u16 seq_sub(u16 sq1, u16 sq2)
{
	return (sq1 - sq2) & SEQ_MASK;
}

static inline u16 reorder_index(struct phl_tid_ampdu_rx *r, u16 seq)
{
	return seq_sub(seq, r->ssn) % r->buf_size;
}

static void phl_release_reorder_frame(struct phl_info_t *phl_info,
									struct phl_tid_ampdu_rx *r,
									int index, _os_list *frames)
{
	struct rtw_phl_rx_pkt *pkt = r->reorder_buf[index];

	if (!pkt)
		goto out;

	/* release the frame from the reorder ring buffer */
	r->stored_mpdu_num--;
	r->reorder_buf[index] = NULL;
	list_add_tail(&pkt->list, frames);

out:
	r->head_seq_num = seq_inc(r->head_seq_num);
}

#define HT_RX_REORDER_BUF_TIMEOUT_MS 100

/*
 * If the MPDU at head_seq_num is ready,
 *     1. release all subsequent MPDUs with consecutive SN and
 *     2. if there's MPDU that is ready but left in the reordering
 *        buffer, find it and set reorder timer according to its reorder
 *        time
 *
 * If the MPDU at head_seq_num is not ready and there is no MPDU ready
 * in the buffer at all, return.
 *
 * If the MPDU at head_seq_num is not ready but there is some MPDU in
 * the buffer that is ready, check whether any frames in the reorder
 * buffer have timed out in the following way.
 *
 * Basically, MPDUs that are not ready are purged and MPDUs that are
 * ready are released.
 *
 * The process goes through all the buffer but the one at head_seq_num
 * unless
 *     - there's a MPDU that is ready AND
 *     - there are one or more buffers that are not ready.
 * In this case, the process is stopped, the head_seq_num becomes the
 * first buffer that is not ready and the reorder_timer is reset based
 * on the reorder_time of that ready MPDU.
 */
static void phl_reorder_release(struct phl_info_t *phl_info,
								struct phl_tid_ampdu_rx *r, _os_list *frames)
{
	/* ref ieee80211_sta_reorder_release() and wil_reorder_release() */

	int index, i, j;
	u32 cur_time = _os_get_cur_time_ms();

	/* release the buffer until next missing frame */
	index = reorder_index(r, r->head_seq_num);
	if (!r->reorder_buf[index] && r->stored_mpdu_num) {
		/*
		 * No buffers ready to be released, but check whether any
		 * frames in the reorder buffer have timed out.
		 */
		int skipped = 1;
		for (j = (index + 1) % r->buf_size; j != index;
			j = (j + 1) % r->buf_size) {
			if (!r->reorder_buf[j]) {
				skipped++;
				continue;
			}
			if (skipped && cur_time < r->reorder_time[j] +
				HT_RX_REORDER_BUF_TIMEOUT_MS)
				goto set_release_timer;

			/* don't leave incomplete A-MSDUs around */
			for (i = (index + 1) % r->buf_size; i != j;
				i = (i + 1) % r->buf_size)
				phl_recycle_rx_buf(phl_info, r->reorder_buf[i]);

			PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "release an RX reorder frame due to timeout on earlier frames\n");

			phl_release_reorder_frame(phl_info, r, j, frames);

			/*
			 * Increment the head seq# also for the skipped slots.
			 */
			r->head_seq_num =
				(r->head_seq_num + skipped) & SEQ_MASK;
			skipped = 0;
		}
	} else while (r->reorder_buf[index]) {
		phl_release_reorder_frame(phl_info, r, index, frames);
		index = reorder_index(r, r->head_seq_num);
	}

	if (r->stored_mpdu_num) {
		j = index = r->head_seq_num % r->buf_size;

		for (; j != (index - 1) % r->buf_size;
			j = (j + 1) % r->buf_size) {
			if (r->reorder_buf[j])
				break;
		}

set_release_timer:

		if (!r->removed)
			_os_set_timer(r->drv_priv, &r->sta->reorder_timer,
			              HT_RX_REORDER_BUF_TIMEOUT_MS);
	} else {
		/* TODO: implementation of cancel timer on Linux is
			del_timer_sync(), it can't be called with same spinlock
			held with the expiration callback, that causes a potential
			deadlock. */
		_os_cancel_timer_async(r->drv_priv, &r->sta->reorder_timer);
	}
}

void phl_sta_rx_reorder_timer_expired(void *t)
{
	/* ref sta_rx_agg_reorder_timer_expired() */

	struct rtw_phl_stainfo_t *sta = (struct rtw_phl_stainfo_t *)t;
	struct rtw_phl_com_t *phl_com = sta->wrole->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	void *drv_priv = phl_to_drvpriv(phl_info);
	u8 i = 0;

	PHL_INFO("Rx reorder timer expired, sta=0x%p\n", sta);

	for (i = 0; i < ARRAY_SIZE(sta->tid_rx); i++) {
		_os_list frames;

		INIT_LIST_HEAD(&frames);

		_os_spinlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
		if (sta->tid_rx[i])
			phl_reorder_release(phl_info, sta->tid_rx[i], &frames);
		_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

		phl_handle_rx_frame_list(phl_info, &frames);
#ifdef PHL_RX_BATCH_IND
		_phl_indic_new_rxpkt(phl_info);
#endif
	}

	_os_event_set(drv_priv, &sta->comp_sync);
}

static void phl_release_reorder_frames(struct phl_info_t *phl_info,
										struct phl_tid_ampdu_rx *r,
										u16 head_seq_num, _os_list *frames)
{
	/* ref ieee80211_release_reorder_frames() and
		wil_release_reorder_frames() */

	int index;

	/* note: this function is never called with
	 * hseq preceding r->head_seq_num, i.e it is always true
	 * !seq_less(hseq, r->head_seq_num)
	 * and thus on loop exit it should be
	 * r->head_seq_num == hseq
	 */
	while (seq_less(r->head_seq_num, head_seq_num) &&
		r->stored_mpdu_num) { /* Note: do we need to check this? */
		index = reorder_index(r, r->head_seq_num);
		phl_release_reorder_frame(phl_info, r, index, frames);
	}
	r->head_seq_num = head_seq_num;
}

static bool phl_manage_sta_reorder_buf(struct phl_info_t *phl_info,
										struct rtw_phl_rx_pkt *pkt,
										struct phl_tid_ampdu_rx *r,
										_os_list *frames)
{
	/* ref ieee80211_sta_manage_reorder_buf() and wil_rx_reorder() */

	struct rtw_r_meta_data *meta = &pkt->r.mdata;
	u16 mpdu_seq_num = meta->seq;
	u16 head_seq_num, buf_size;
	int index;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	buf_size = r->buf_size;
	head_seq_num = r->head_seq_num;

	/*
	 * If the current MPDU's SN is smaller than the SSN, it shouldn't
	 * be reordered.
	 */
	if (!r->started) {
		if (seq_less(mpdu_seq_num, head_seq_num))
			return false;
		r->started = true;
	}

	/* frame with out of date sequence number */
	if (seq_less(mpdu_seq_num, head_seq_num)) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "Rx drop: old seq 0x%03x head 0x%03x\n",
				meta->seq, r->head_seq_num);
		hci_trx_ops->recycle_rx_pkt(phl_info, pkt);
		return true;
	}

	/*
	 * If frame the sequence number exceeds our buffering window
	 * size release some previous frames to make room for this one.
	 */
	if (!seq_less(mpdu_seq_num, head_seq_num + buf_size)) {
		head_seq_num = seq_inc(seq_sub(mpdu_seq_num, buf_size));
		/* release stored frames up to new head to stack */
		phl_release_reorder_frames(phl_info, r, head_seq_num, frames);
	}

	/* Now the new frame is always in the range of the reordering buffer */

	index = reorder_index(r, mpdu_seq_num);

	/* check if we already stored this frame */
	if (r->reorder_buf[index]) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "Rx drop: old seq 0x%03x head 0x%03x\n",
				meta->seq, r->head_seq_num);
		hci_trx_ops->recycle_rx_pkt(phl_info, pkt);
		return true;
	}

	/*
	 * If the current MPDU is in the right order and nothing else
	 * is stored we can process it directly, no need to buffer it.
	 * If it is first but there's something stored, we may be able
	 * to release frames after this one.
	 */
	if (mpdu_seq_num == r->head_seq_num &&
		r->stored_mpdu_num == 0) {
		r->head_seq_num = seq_inc(r->head_seq_num);
		return false;
	}

	/* put the frame in the reordering buffer */
	r->reorder_buf[index] = pkt;
	r->reorder_time[index] = _os_get_cur_time_ms();
	r->stored_mpdu_num++;
	phl_reorder_release(phl_info, r, frames);

	return true;

}

enum rtw_phl_status phl_rx_reorder(struct phl_info_t *phl_info,
									struct rtw_phl_rx_pkt *phl_rx,
									_os_list *frames)
{
#if 0 // NEO TODO 
	/* ref wil_rx_reorder() and ieee80211_rx_reorder_ampdu() */

	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_r_meta_data *meta = &phl_rx->r.mdata;
	u16 tid = meta->tid;
	struct rtw_wifi_role_t *wrole;
	struct rtw_phl_stainfo_t *sta = NULL;
	struct phl_tid_ampdu_rx *r;
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

	/* Remove FCS if is is appended
	 * ToDo: handle more then one in pkt_list
	 */
	do {
		if (!phl_info->phl_com->append_fcs)
			break;
		/* Only last MSDU of A-MSDU includes FCS.
		 * ToDo:
		 * If A-MSDU cut processing is in HAL, should only deduct FCS
		 * from length of last one of pkt_list. For such case,
		 * phl_rx->r should have pkt_list length.
		 */
		if (meta->amsdu_cut && !meta->last_msdu)
			break;
		if (phl_rx->r.pkt_list[0].length <= 4) {
			PHL_ERR("%s, pkt_list[0].length(%d) too short\n",
			__func__, phl_rx->r.pkt_list[0].length);
			goto drop_frame;
		} else
			phl_rx->r.pkt_list[0].length -= 4;
	} while (0);

	if (meta->bc || meta->mc)
		goto dont_reorder;

	if (!meta->qos)
		goto dont_reorder;

	if (meta->q_null)
		goto dont_reorder;

	/* TODO: check ba policy is either ba or normal */

	/* if the mpdu is fragmented, don't reorder */
	if (meta->more_frag || meta->frag_num) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "Receive QoS Data with more_frag=%u, frag_num=%u\n",
				meta->more_frag, meta->frag_num);
		goto dont_reorder;
	}


	/* Use MAC ID from address CAM if this packet is address CAM matched */
	while (meta->addr_cam_vld) {
		struct rtw_recv_pkt *rx = &phl_rx->r;
		rx->tx_sta = rtw_phl_get_stainfo_by_macid(phl_info,
							 meta->macid);
		if (rx->tx_sta == NULL) {
			PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_,
				  "No STA info w. matched A%d M%d\n",
				  meta->addr_cam, meta->macid);
			break;
		}

		rx->rx_role = rx->tx_sta->wrole;
		if (rx->rx_role == NULL) {
			PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_,
			          "No RX role in STA of M%d\n", meta->macid);
			rx->tx_sta = NULL;
			break;
		}
		sta = rx->tx_sta;
		wrole = rx->rx_role;
		/* ToDo: Check match to self and BMC */
		break;
	}
	/* Otherwise, search STA by TA */
	if (sta == NULL) {
		wrole = phl_get_wrole_by_addr(phl_info, meta->mac_addr);

		if (wrole == NULL) {
			PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_,
			          "Failed to get wrole for reordering, drop frame\n");
			goto drop_frame;
		}

		/* TODO: a hash table search would be more prefferable on rx path */
		sta = rtw_phl_get_stainfo_by_addr(phl_info, wrole, meta->ta);
		if (!sta) {
			PHL_ERR("Failed to get sta of %pM for reordering, drop frame\n",
			        meta->ta);
			goto drop_frame;
		}
		phl_rx->r.tx_sta = sta;
		phl_rx->r.rx_role = wrole;
	}
	rtw_hal_set_sta_rx_sts(sta, false, meta);

	if (tid >= ARRAY_SIZE(sta->tid_rx)) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "Fail: tid (%u) index out of range (%u)\n", tid, 8);
		goto drop_frame;
	}

	_os_spinlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	r = sta->tid_rx[tid];
	if (!r) {
		_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
		goto dont_reorder;
	}

	if (!phl_manage_sta_reorder_buf(phl_info, phl_rx, r, frames)) {
		_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
		goto dont_reorder;
	}

	_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	return RTW_PHL_STATUS_SUCCESS;

drop_frame:
	hci_trx_ops->recycle_rx_pkt(phl_info, phl_rx);
	return RTW_PHL_STATUS_FAILURE;

dont_reorder:
	list_add_tail(&phl_rx->list, frames);

#endif // if 0 NEO
	return RTW_PHL_STATUS_SUCCESS;
}


u8 phl_check_recv_ring_resource(struct phl_info_t *phl_info)
{
	struct rtw_phl_rx_ring *ring = &phl_info->phl_rx_ring;
	u16 avail = 0, wptr = 0, rptr = 0;
	void *drv_priv = phl_to_drvpriv(phl_info);

	wptr = (u16)_os_atomic_read(drv_priv, &ring->phl_idx);
	rptr = (u16)_os_atomic_read(drv_priv, &ring->core_idx);
	avail = phl_calc_avail_wptr(rptr, wptr, MAX_PHL_RING_ENTRY_NUM);

	if (0 == avail)
		return false;
	else
		return true;
}

void dump_phl_rx_ring(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	s16	diff = 0;
	u16 idx = 0, endidx = 0;
	u16 phl_idx = 0, core_idx = 0;

	PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "===Dump PHL RX Ring===\n");
	phl_idx = (u16)_os_atomic_read(drv_priv, &phl_info->phl_rx_ring.phl_idx);
	core_idx = (u16)_os_atomic_read(drv_priv, &phl_info->phl_rx_ring.core_idx);
	PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_,
			"core_idx = %d\n"
			"phl_idx = %d\n",
			core_idx,
			phl_idx);

	diff= phl_idx-core_idx;
	if(diff < 0)
		diff= 4096+diff;

	endidx = diff > 5 ? (core_idx+6): phl_idx;
	for (idx = core_idx+1; idx < endidx; idx++) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "entry[%d] = %p\n", idx,
				phl_info->phl_rx_ring.entry[idx%4096]);
	}
}


void phl_event_indicator(void *context)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_handler *phl_handler
		= (struct rtw_phl_handler *)phl_container_of(context,
							struct rtw_phl_handler,
							os_handler);
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_handler->context;
	struct rtw_phl_evt_ops *ops = NULL;
	struct rtw_evt_info_t *evt_info = NULL;
	void *drv_priv = NULL;
	enum rtw_phl_evt evt_bitmap = 0;
	FUNCIN_WSTS(sts);

	if (NULL != phl_info) {
		ops = &phl_info->phl_com->evt_ops;
		evt_info = &phl_info->phl_com->evt_info;
		drv_priv = phl_to_drvpriv(phl_info);

		_os_spinlock(drv_priv, &evt_info->evt_lock, _bh, NULL);
		evt_bitmap = evt_info->evt_bitmap;
		_os_spinunlock(drv_priv, &evt_info->evt_lock, _bh, NULL);

		if (RTW_PHL_EVT_RX & evt_bitmap) {
			if (NULL != ops->rx_process) {
				sts = ops->rx_process(drv_priv);
			}
			dump_phl_rx_ring(phl_info);
			_os_spinlock(drv_priv, &evt_info->evt_lock, _bh, NULL);
			evt_info->evt_bitmap &= ~(RTW_PHL_EVT_RX);
			_os_spinunlock(drv_priv, &evt_info->evt_lock, _bh, NULL);
		}
	}
	FUNCOUT_WSTS(sts);

}

void
_phl_rx_traffic_upd(struct rtw_stats *sts)
{
	u32 tp_k = 0, tp_m = 0;
	enum rtw_tfc_lvl rx_tfc_lvl = RTW_TFC_IDLE;
	tp_k = sts->rx_tp_kbits;
	tp_m = sts->rx_tp_kbits >> 10;

	if (tp_m >= RX_HIGH_TP_THRES_MBPS)
		rx_tfc_lvl = RTW_TFC_HIGH;
	else if (tp_m >= RX_MID_TP_THRES_MBPS)
		rx_tfc_lvl = RTW_TFC_MID;
	else if (tp_m >= RX_LOW_TP_THRES_MBPS)
		rx_tfc_lvl = RTW_TFC_LOW;
	else if (tp_k >= RX_ULTRA_LOW_TP_THRES_KBPS)
		rx_tfc_lvl = RTW_TFC_ULTRA_LOW;
	else
		rx_tfc_lvl = RTW_TFC_IDLE;

	if (sts->rx_traffic.lvl > rx_tfc_lvl) {
		sts->rx_traffic.sts = (TRAFFIC_CHANGED | TRAFFIC_DECREASE);
		sts->rx_traffic.lvl = rx_tfc_lvl;
	} else if (sts->rx_traffic.lvl < rx_tfc_lvl) {
		sts->rx_traffic.sts = (TRAFFIC_CHANGED | TRAFFIC_INCREASE);
		sts->rx_traffic.lvl = rx_tfc_lvl;
	} else if (sts->rx_traffic.sts &
		(TRAFFIC_CHANGED | TRAFFIC_INCREASE | TRAFFIC_DECREASE)) {
		sts->rx_traffic.sts &= ~(TRAFFIC_CHANGED | TRAFFIC_INCREASE |
					 TRAFFIC_DECREASE);
	}
}

void _phl_rx_statistics_reset(struct phl_info_t *phl_info)
{
	RTW_ERR("NEO TODO %s\n", __func__);
#if 0 // NEO TODO
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_phl_stainfo_t *sta = NULL;
	struct rtw_wifi_role_t *role = NULL;
	void *drv = phl_to_drvpriv(phl_info);
	struct phl_queue *sta_queue;
	_os_spinlockfg sp_flags;
	u8 i;

	for (i = 0; i< MAX_WIFI_ROLE_NUMBER; i++) {
		role = &phl_com->wifi_roles[i];
		if (role->active && (role->mstate == MLME_LINKED)) {
			sta_queue = &role->assoc_sta_queue;
			_os_spinlock(drv, &sta_queue->lock, _irq, &sp_flags);
			phl_list_for_loop(sta, struct rtw_phl_stainfo_t,
						&sta_queue->queue, list) {
				if (sta)
					rtw_hal_set_sta_rx_sts(sta, true, NULL);
			}
			_os_spinunlock(drv, &sta_queue->lock, _irq, &sp_flags);
		}
	}
#endif // if 0 NEO
}

void
phl_rx_watchdog(struct phl_info_t *phl_info)
{
	struct rtw_stats *phl_stats = &phl_info->phl_com->phl_stats;

	/* hana_todo: update traffic info of each station */
	_phl_rx_traffic_upd(phl_stats);
	_phl_rx_statistics_reset(phl_info);
}

u16 rtw_phl_query_new_rx_num(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_rx_ring *ring = NULL;
	u16 new_rx = 0, wptr = 0, rptr = 0;

	if (NULL != phl_info) {
		ring = &phl_info->phl_rx_ring;
		wptr = (u16)_os_atomic_read(phl_to_drvpriv(phl_info),
						&ring->phl_idx);
		rptr = (u16)_os_atomic_read(phl_to_drvpriv(phl_info),
						&ring->core_idx);
		new_rx = phl_calc_avail_rptr(rptr, wptr,
						MAX_PHL_RING_ENTRY_NUM);
	}

	return new_rx;
}

struct rtw_recv_pkt *rtw_phl_query_rx_pkt(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_rx_ring *ring = NULL;
	struct rtw_recv_pkt *recvpkt = NULL;
	void *drv_priv = NULL;
	u16 ring_res = 0, wptr = 0, rptr = 0;

	if (NULL != phl_info) {
		ring = &phl_info->phl_rx_ring;
		drv_priv = phl_to_drvpriv(phl_info);

		wptr = (u16)_os_atomic_read(drv_priv, &ring->phl_idx);
		rptr = (u16)_os_atomic_read(drv_priv, &ring->core_idx);

		ring_res = phl_calc_avail_rptr(rptr, wptr,
							MAX_PHL_RING_ENTRY_NUM);

		PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_,
			"[4] %s::[Query] phl_idx =%d , core_idx =%d , ring_res =%d\n",
			__FUNCTION__,
			_os_atomic_read(drv_priv, &ring->phl_idx),
			_os_atomic_read(drv_priv, &ring->core_idx),
			ring_res);

		if (ring_res > 0) {
			rptr = rptr + 1;

			if (rptr >= MAX_PHL_RING_ENTRY_NUM) {
				rptr=0;
				recvpkt = (struct rtw_recv_pkt *)ring->entry[rptr];
				ring->entry[rptr]=NULL;
				_os_atomic_set(drv_priv, &ring->core_idx, 0);
			} else {
				recvpkt = (struct rtw_recv_pkt *)ring->entry[rptr];
				ring->entry[rptr]=NULL;
				_os_atomic_inc(drv_priv, &ring->core_idx);
			}
			if (NULL == recvpkt)
				PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "recvpkt is NULL!\n");
			else
				phl_rx_statistics(phl_info, recvpkt);
		} else {
			PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "no available rx packet to query!\n");
		}
	}

	return recvpkt;
}

enum rtw_phl_status rtw_phl_return_rxbuf(void *phl, u8* recvpkt)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_rx_pkt *phl_rx = NULL;
	struct rtw_recv_pkt *r = (struct rtw_recv_pkt *)recvpkt;

	do {
		if (NULL == recvpkt)
			break;

		phl_rx = phl_container_of(r, struct rtw_phl_rx_pkt, r);
		phl_recycle_rx_buf(phl_info, phl_rx);
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} while (false);

	return pstatus;
}


enum rtw_phl_status rtw_phl_start_rx_process(void *phl)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	FUNCIN_WSTS(pstatus);

	pstatus = phl_schedule_handler(phl_info->phl_com,
						&phl_info->phl_rx_handler);

	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

void rtw_phl_rx_bar(void *phl, struct rtw_phl_stainfo_t *sta, u8 tid, u16 seq)
{
	/* ref ieee80211_rx_h_ctrl() and wil_rx_bar() */

	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_tid_ampdu_rx *r;
	_os_list frames;

	INIT_LIST_HEAD(&frames);

	_os_spinlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);

	r = sta->tid_rx[tid];
	if (!r) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "BAR for non-existing TID %d\n", tid);
		goto out;
	}

	if (seq_less(seq, r->head_seq_num)) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "BAR Seq 0x%03x preceding head 0x%03x\n",
					seq, r->head_seq_num);
		goto out;
	}

	PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "BAR: TID %d Seq 0x%03x head 0x%03x\n",
				tid, seq, r->head_seq_num);

	phl_release_reorder_frames(phl_info, r, seq, &frames);
	phl_handle_rx_frame_list(phl_info, &frames);

out:
	_os_spinunlock(drv_priv, &sta->tid_rx_lock, _bh, NULL);
}

#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
void
_phl_rx_proc_frame_list(struct phl_info_t *phl_info, struct phl_queue *pq)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	void *d = phl_to_drvpriv(phl_info);
	_os_list *pkt_list = NULL;
	struct rtw_phl_rx_pkt *phl_rx = NULL;

	if (NULL == pq)
		return;
	if (0 == pq->cnt)
		return;

	PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
		  "_phl_rx_proc_frame_list : queue ele cnt = %d\n",
		   pq->cnt);

	while (true == pq_pop(d, pq, &pkt_list, _first, _bh)) {
		phl_rx = (struct rtw_phl_rx_pkt *)pkt_list;
		phl_info->hci_trx_ops->rx_handle_normal(phl_info, phl_rx);
	}
}

enum rtw_phl_status
phl_rx_proc_phy_sts(struct phl_info_t *phl_info, struct rtw_phl_rx_pkt *ppdu_sts)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_ppdu_sts_info *psts_info = &(phl_info->phl_com->ppdu_sts_info);
	struct rtw_phl_ppdu_sts_ent *sts_entry = NULL;
	struct rtw_phl_rx_pkt *phl_rx = NULL;
	void *d = phl_to_drvpriv(phl_info);
	struct rtw_phl_rssi_stat *rssi_stat = &phl_info->phl_com->rssi_stat;
	_os_list *frame = NULL;
	bool upt_psts = true;
	u8 i = 0;
	enum phl_band_idx band = HW_BAND_0;

	if (NULL == ppdu_sts)
		return pstatus;

	if (false == psts_info->en_psts_per_pkt) {
		return pstatus;
	}

	if (ppdu_sts->r.mdata.ppdu_cnt >= PHL_MAX_PPDU_CNT) {
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "ppdu_sts->r.mdata.ppdu_cnt >= PHL_MAX_PPDU_CNT!\n");
		return pstatus;
	}

	band = (ppdu_sts->r.mdata.bb_sel > 0) ? HW_BAND_1 : HW_BAND_0;

	if (false == psts_info->en_ppdu_sts[band])
		return pstatus;

	if (ppdu_sts->r.mdata.ppdu_cnt != psts_info->cur_ppdu_cnt[band]) {
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "ppdu_sts->r.mdata.ppdu_cnt != psts_info->cur_ppdu_cnt!\n");
		upt_psts = false;
	}

	sts_entry = &psts_info->sts_ent[band][psts_info->cur_ppdu_cnt[band]];
	/* check list empty */
	if (0 == sts_entry->frames.cnt) {
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "cur_ppdu_cnt %d --> sts_entry->frames.cnt = 0\n",
			  psts_info->cur_ppdu_cnt[band]);
		pstatus = RTW_PHL_STATUS_SUCCESS;
		return pstatus;
	}

	/* start update phy info to per pkt*/
	if (false == pq_get_front(d, &sts_entry->frames, &frame, _bh)) {
		PHL_ERR(" %s list empty\n", __FUNCTION__);
		return pstatus;
	}
	/**
	 * TODO : How to filter the case :
	 *	pkt(ppdu_cnt = 0) --> missing :psts(ppdu_cnt = 0) --> (all of the pkt, psts dropped/missing)
	 *	--> ppdu_sts(ppdu_cnt = 0)(not for the current buffered pkt.)
	 * workaround : check rate/bw/ppdu_type/... etc
	 **/
	phl_rx = (struct rtw_phl_rx_pkt *)frame;
	if (upt_psts &&
	   ((phl_rx->r.mdata.rx_rate != ppdu_sts->r.mdata.rx_rate) ||
	    (phl_rx->r.mdata.bw != ppdu_sts->r.mdata.bw) ||
	    (phl_rx->r.mdata.rx_gi_ltf != ppdu_sts->r.mdata.rx_gi_ltf) ||
	    (phl_rx->r.mdata.ppdu_type != ppdu_sts->r.mdata.ppdu_type))) {
		    /**
		     * ppdu status is not for the buffered pkt,
		     * skip update phy status to phl_rx
		     **/
		    upt_psts = false;
	}
	/* Get Frame Type */
	ppdu_sts->r.phy_info.frame_type =
		PHL_GET_80211_HDR_TYPE(phl_rx->r.pkt_list[0].vir_addr);

	if ((false == ppdu_sts->r.phy_info.is_valid) &&
	    (true == psts_info->en_fake_psts)) {
		if (RTW_FRAME_TYPE_MGNT == phl_rx->r.mdata.frame_type) {
			ppdu_sts->r.phy_info.rssi =
				rssi_stat->ma_rssi[RTW_RSSI_MGNT_ACAM_A1M];
		} else if (RTW_FRAME_TYPE_DATA == phl_rx->r.mdata.frame_type) {
			ppdu_sts->r.phy_info.rssi =
				rssi_stat->ma_rssi[RTW_RSSI_DATA_ACAM_A1M];
		} else if (RTW_FRAME_TYPE_CTRL == phl_rx->r.mdata.frame_type) {
			ppdu_sts->r.phy_info.rssi =
				rssi_stat->ma_rssi[RTW_RSSI_CTRL_ACAM_A1M];
		} else {
			ppdu_sts->r.phy_info.rssi =
				rssi_stat->ma_rssi[RTW_RSSI_UNKNOWN];
		}
		for(i = 0; i< RTW_PHL_MAX_RF_PATH ; i++) {
			ppdu_sts->r.phy_info.rssi_path[i] =
					ppdu_sts->r.phy_info.rssi;
		}
		ppdu_sts->r.phy_info.ch_idx = rtw_hal_get_cur_ch(phl_info->hal,
						phl_rx->r.mdata.bb_sel);
		ppdu_sts->r.phy_info.is_valid = true;
	}

	do {
		if (false == upt_psts)
			break;
		phl_rx = (struct rtw_phl_rx_pkt *)frame;
		_os_mem_cpy(d, &(phl_rx->r.phy_info), &(ppdu_sts->r.phy_info),
			    sizeof(struct rtw_phl_ppdu_phy_info));
	} while ((true == psts_info->psts_ampdu) &&
		 (pq_get_next(d, &sts_entry->frames, frame, &frame, _bh)));

	/*2. indicate the frame list*/
	_phl_rx_proc_frame_list(phl_info, &sts_entry->frames);
	/*3. reset the queue */
	pq_reset(d, &(sts_entry->frames), _bh);

	return pstatus;
}

bool
phl_rx_proc_wait_phy_sts(struct phl_info_t *phl_info,
			 struct rtw_phl_rx_pkt *phl_rx)
{
	struct rtw_phl_ppdu_sts_info *psts_info = &(phl_info->phl_com->ppdu_sts_info);
	struct rtw_phl_ppdu_sts_ent *sts_entry = NULL;
	void *d = phl_to_drvpriv(phl_info);
	u8 i = 0, j = 0;
	bool ret = false;
	enum phl_band_idx band = HW_BAND_0;

#if 0 // NEO TODO mark off for pq_push first

	if (false == psts_info->en_psts_per_pkt) {
		return ret;
	}

	if (phl_rx->r.mdata.ppdu_cnt >= PHL_MAX_PPDU_CNT) {
		PHL_ASSERT("phl_rx->r.mdata.ppdu_cnt >= PHL_MAX_PPDU_CNT!");
		return ret;
	}

	band = (phl_rx->r.mdata.bb_sel > 0) ? HW_BAND_1 : HW_BAND_0;

	if (false == psts_info->en_ppdu_sts[band])
		return ret;

	if (psts_info->cur_ppdu_cnt[band] != phl_rx->r.mdata.ppdu_cnt) {
		/* start of PPDU */
		/* 1. Check all of the buffer list is empty */
		/* only check the target rx pkt band */
		for (i = 0; i < PHL_MAX_PPDU_CNT; i++) {
			sts_entry = &psts_info->sts_ent[band][i];
			if (0 != sts_entry->frames.cnt) {
				/* need indicate first */
				PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
					  "band %d ; ppdu_cnt %d queue is not empty \n",
					  band, i);
				_phl_rx_proc_frame_list(phl_info,
						&sts_entry->frames);
				pq_reset(d, &(sts_entry->frames), _bh);
			}
		}

		/* 2. check ppdu status filter condition */
		/* Filter function is supportted only if rxd = long_rxd */
		if ((1 == phl_rx->r.mdata.long_rxd) &&
		    (0 != (psts_info->ppdu_sts_filter &
		           BIT(phl_rx->r.mdata.frame_type)))) {
			/* 3. add new rx pkt to the tail of the queue */
			sts_entry = &psts_info->sts_ent[band][phl_rx->r.mdata.ppdu_cnt];
			pq_reset(d, &(sts_entry->frames), _bh);
			pq_push(d, &(sts_entry->frames), &phl_rx->list,
				_tail, _bh);
			ret = true;
		}
		psts_info->cur_ppdu_cnt[band] = phl_rx->r.mdata.ppdu_cnt;
	} else {
		/* 1. check ppdu status filter condition */
		/* Filter function is supportted only if rxd = long_rxd */
		if ((1 == phl_rx->r.mdata.long_rxd) &&
		    (0 != (psts_info->ppdu_sts_filter &
		           BIT(phl_rx->r.mdata.frame_type)))) {
			/* 2. add to frame list */
			sts_entry = &psts_info->sts_ent[band][phl_rx->r.mdata.ppdu_cnt];
			if (0 == sts_entry->frames.cnt) {
				PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
					  "MPDU is not the start of PPDU, but the queue is empty!!!\n");
			}
			pq_push(d, &(sts_entry->frames), &phl_rx->list,
				_tail, _bh);
			ret = true;
		}
	}
#endif // if 0 NEO
	return ret;
}
#endif

#ifdef CONFIG_PHY_INFO_NTFY
void _phl_rx_post_proc_ppdu_sts(void* priv, struct phl_msg* msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	if (msg->inbuf && msg->inlen){
		_os_kmem_free(phl_to_drvpriv(phl_info), msg->inbuf, msg->inlen);
	}
}

bool
_phl_rx_proc_aggr_psts_ntfy(struct phl_info_t *phl_info,
			    struct rtw_phl_ppdu_sts_ent *ppdu_sts_ent)
{
	struct rtw_phl_ppdu_sts_info *ppdu_info =
			&phl_info->phl_com->ppdu_sts_info;
	struct  rtw_phl_ppdu_sts_ntfy *psts_ntfy = NULL;
	u8 i = 0;
	bool ret = false;

	if (ppdu_info->msg_aggr_cnt == 0) {
		/* reset entry valid status  */
		for (i = 0; i < MAX_PSTS_MSG_AGGR_NUM; i++) {
			ppdu_info->msg_aggr_buf[i].vld = false;
		}
	}
	/* copy to the buf */
	psts_ntfy = &ppdu_info->msg_aggr_buf[ppdu_info->msg_aggr_cnt];
	psts_ntfy->frame_type = ppdu_sts_ent->frame_type;
	_os_mem_cpy(phl_info->phl_com->drv_priv,
		    &psts_ntfy->phy_info,
		    &ppdu_sts_ent->phy_info,
		    sizeof(struct rtw_phl_ppdu_phy_info));
	_os_mem_cpy(phl_info->phl_com->drv_priv,
		    psts_ntfy->src_mac_addr,
		    ppdu_sts_ent->src_mac_addr,
		    MAC_ADDRESS_LENGTH);
	psts_ntfy->vld = true;

	/* update counter */
	ppdu_info->msg_aggr_cnt++;
	if (ppdu_info->msg_aggr_cnt >= MAX_PSTS_MSG_AGGR_NUM) {
		ppdu_info->msg_aggr_cnt = 0;
		ret = true;
	}

	return ret;
}
#endif

void
phl_rx_proc_ppdu_sts(struct phl_info_t *phl_info, struct rtw_phl_rx_pkt *phl_rx)
{
#if 0 // NEO TODO
	u8 i = 0;
	struct rtw_phl_ppdu_sts_info *ppdu_info = NULL;
	struct rtw_phl_ppdu_sts_ent *ppdu_sts_ent = NULL;
	struct rtw_phl_stainfo_t *psta = NULL;
	struct phl_msg_attribute attr = {0};
	struct phl_msg msg = {0};
#ifdef CONFIG_PHY_INFO_NTFY
	struct  rtw_phl_ppdu_sts_ntfy *psts_ntfy;
	void *d = phl_to_drvpriv(phl_info);
#endif
	enum phl_band_idx band = HW_BAND_0;

	if ((NULL == phl_info) || (NULL == phl_rx))
		return;

	band = (phl_rx->r.mdata.bb_sel > 0) ? HW_BAND_1 : HW_BAND_0;
	ppdu_info = &phl_info->phl_com->ppdu_sts_info;
	ppdu_sts_ent = &ppdu_info->sts_ent[band][phl_rx->r.mdata.ppdu_cnt];

	if (false == ppdu_sts_ent->valid)
		return;

	if (true == ppdu_sts_ent->phl_done)
		return;

	ppdu_sts_ent->phl_done = true;

	/* update phl self varibles */
	for( i = 0 ; i < PHL_MAX_PPDU_STA_NUM; i++) {
		if (ppdu_sts_ent->sta[i].vld) {
			psta = rtw_phl_get_stainfo_by_macid(phl_info,
				 ppdu_sts_ent->sta[i].macid);
			if (psta == NULL)
				continue;
			STA_UPDATE_MA_RSSI(psta->hal_sta->rssi_stat,
					    ppdu_sts_ent->phy_info.rssi);
			/* update (re)associate req/resp pkt rssi */
			if (RTW_IS_ASOC_PKT(ppdu_sts_ent->frame_type)) {
				    psta->hal_sta->rssi_stat.assoc_rssi =
				    		ppdu_sts_ent->phy_info.rssi;
			}
		}
	}
#ifdef CONFIG_PHY_INFO_NTFY
	/*2. prepare and send psts notify to core */
	if((RTW_FRAME_TYPE_BEACON == ppdu_sts_ent->frame_type) ||
	   (RTW_FRAME_TYPE_PROBE_RESP == ppdu_sts_ent->frame_type)) {

		if (false == _phl_rx_proc_aggr_psts_ntfy(phl_info,
							 ppdu_sts_ent)) {
			return;
		}

		/* send aggr psts ntfy*/
		psts_ntfy = (struct rtw_phl_ppdu_sts_ntfy *)_os_kmem_alloc(d,
				MAX_PSTS_MSG_AGGR_NUM * sizeof(*psts_ntfy));
		if (psts_ntfy == NULL) {
			PHL_ERR("%s: alloc ppdu sts for ntfy fail.\n", __func__);
			return;
		}

		_os_mem_cpy(phl_info->phl_com->drv_priv,
			    psts_ntfy,
			    &ppdu_info->msg_aggr_buf,
			    (MAX_PSTS_MSG_AGGR_NUM *
			     sizeof(struct rtw_phl_ppdu_sts_ntfy)));

		msg.inbuf = (u8 *)psts_ntfy;
		msg.inlen = (MAX_PSTS_MSG_AGGR_NUM *
			     sizeof(struct rtw_phl_ppdu_sts_ntfy));
		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_PSTS);
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_RX_PSTS);
		attr.completion.completion = _phl_rx_post_proc_ppdu_sts;
		attr.completion.priv = phl_info;
		if (phl_msg_hub_send(phl_info, &attr, &msg) != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s: send msg_hub failed\n", __func__);
			_os_kmem_free(d, psts_ntfy,
				      (MAX_PSTS_MSG_AGGR_NUM *
				       sizeof(struct rtw_phl_ppdu_sts_ntfy)));
		}
	}
#endif
#endif
}
