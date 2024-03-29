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
#ifndef _PHL_REGULATION_H_
#define _PHL_REGULATION_H_

#define MAX_CH_NUM_GROUP 14

enum rtw_regulation_freq_group {
	FREQ_GROUP_2GHZ = 0x0,
	FREQ_GROUP_5GHZ_BAND1,
	FREQ_GROUP_5GHZ_BAND2,
	FREQ_GROUP_5GHZ_BAND3,
	FREQ_GROUP_5GHZ_BAND4,
	FREQ_GROUP_MAX
};

enum rtw_regulation_status {
	REGULATION_SUCCESS = 0x0,
	REGULATION_FAILURE,
	REGULATION_DOMAIN_MISMATCH,
	REGULATION_INVALID_2GHZ_RD,
	REGULATION_INVALID_5GHZ_RD,
	REGULATION_INVALID_DOMAIN
};

struct rtw_regulation_chplan_group {
	u32 cnt;
	struct rtw_regulation_channel ch[MAX_CH_NUM_GROUP];
};

#define MAX_HISTORY_NUM 20

#define INVALID_DOMAIN_CODE 0xff
#define INVALID_RD_INDEX 0xff

struct rtw_domain {
	u8 code;
	u8 reason;
};

struct rtw_regulation {
	_os_lock lock;
	u8 init; /* regulation sw initialization */
	u8 invalid_cnt;
	u8 history_cnt;
	struct rtw_domain history[MAX_HISTORY_NUM];

	/* working regulation */
	bool valid; /* true if domain code successfully set */
	u16 capability; /* rtw_regulation_capability */
	struct rtw_domain domain;
	char country[2];
	u8 support_mode;
	u8 rd_idx2g; /* 2ghz regulatory domain index */
	u8 regulation_2g;
	u8 rd_idx5g; /* 5ghz regulatory domain index */
	u8 regulation_5g;
	struct rtw_regulation_chplan_group chplan[FREQ_GROUP_MAX];
};

#endif /* _PHL_REGULATION_H_ */
