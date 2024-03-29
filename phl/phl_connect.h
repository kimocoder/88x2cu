/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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
#ifndef _PHL_CONNECT_H_
#define _PHL_CONNECT_H_

/**
 * rtw_phl_connect_prepare() - Prepare hardware setting for connecting
 * @phl:	struct phl_info_t *
 * @wrole:	struct rtw_wifi_role_t *
 * @addr:	target mac address
 *
 * Do all necessary hardware setting for connecting.
 *
 * Return RTW_PHL_STATUS_SUCCESS for success, otherwise fail.
 */
enum rtw_phl_status rtw_phl_connect_prepare(void *phl,
					    struct rtw_wifi_role_t *wrole,
					    u8 *addr);

/**
 * rtw_phl_connected() - Config hardware setting for connected
 * @phl:	struct phl_info_t *
 * @wrole:	struct rtw_wifi_role_t *
 * @sta:	target station info
 *
 * Do all necessary hardware setting for connected.
 *
 * Return RTW_PHL_STATUS_SUCCESS for success, otherwise fail.
 */
enum rtw_phl_status rtw_phl_connected(void *phl,
				      struct rtw_wifi_role_t *wrole,
				      struct rtw_phl_stainfo_t *sta);

#ifdef CONFIG_STA_CMD_DISPR
enum rtw_phl_status rtw_phl_disconnect(void *phl,
				       struct rtw_wifi_role_t *wrole,
				       bool is_disconnect);
#else
enum rtw_phl_status rtw_phl_disconnect(void *phl,
				       struct rtw_wifi_role_t *wrole);
#endif

enum rtw_phl_status rtw_phl_ap_started(void *phl,
				      struct rtw_wifi_role_t *wrole);
enum rtw_phl_status rtw_phl_ap_stop(void *phl,
				      struct rtw_wifi_role_t *wrole);

enum rtw_phl_status rtw_phl_ap_started(void *phl,
				      struct rtw_wifi_role_t *wrole);
enum rtw_phl_status rtw_phl_ap_stop(void *phl,
				      struct rtw_wifi_role_t *wrole);


#endif /* _PHL_CONNECT_H_ */
