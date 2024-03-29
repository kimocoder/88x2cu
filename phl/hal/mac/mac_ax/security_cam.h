/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef _MAC_AX_SEC_CAM_H_
#define _MAC_AX_SEC_CAM_H_

#include "../mac_def.h"
#include "role.h"
#include "fwcmd.h"
#include "addr_cam.h"

/*--------------------Define ----------------------------------------*/
#ifdef PHL_FEATURE_AP
	#define SEC_CAM_ENTRY_NUM	0x80
#else
	#define SEC_CAM_ENTRY_NUM	0x10
#endif

#define ADDRCAM_VALID BIT0
#define ADDRCAM_SEC_MODE_SH 16
#define ADDRCAM_SEC_ENT0_KEYID_SH 18
#define ADDRCAM_SEC_ENT0_KEYID_SH 18

#define ADDRCAM_KEYID_MSK 0x3
#define ADDRCAM_SECMODE_MSK 0x3
#define ADDRCAM_MACID_MSK 0xFF
#define ADDRCAM_KEY_VALID_MSK 0xFF
#define ADDRCAM_KEY_CAM_IDX_MSK 0xFF

#define ADDRCAM_VALID_BIT_OFFSET 0x00
#define ADDRCAM_MACID_OFFSET 0x18
#define ADDRCAM_SECMODE_OFFSET 0x1C
#define ADDRCAM_KEY_VALID_OFFSET 0x20
#define ADDRCAM_KEY_IDX_OFFSET 0x21

#define SEC_CAM_ENTRY_SIZE	0x20
#define ADDR_CAM_ENTRY_SIZE	0x40
#define MACNOKEYINDEX		0xFF

#define DEFAULT_KEYID		0x0
#define DEFAULT_KEYTYPE		0x0

/*--------------------DSecurity cam type declaration-----------------*/
struct sec_cam_entry_t {
	u8 valid;
	u8 mac_id;
	u8 key_id;
	u8 key_type;
	struct mac_ax_sec_cam_info *sec_cam_info;
};

struct sec_cam_table_t {
	struct sec_cam_entry_t *sec_cam_entry[128];
	u8 next_cam_storage_idx;
};

enum SEC_FUNCTION_TYPE {
	SEC_TX_ENC = 0,
	SEC_RX_ENC = 1,
	SEC_BC_ENC = 2,
	SEC_MC_ENC = 3,
	SEC_UC_MGNT_ENC  = 4,
	SEC_BMC_MGNT_ENC = 5,
};

/*--------------------Funciton declaration----------------------------*/
u32 disconnect_flush_key(struct mac_ax_adapter *adapter,
			 struct mac_role_tbl *role);

u32 sec_info_tbl_init(struct mac_ax_adapter *adapter);

u32 free_sec_info_tbl(struct mac_ax_adapter *adapter);

u32 fill_sec_cam_info(struct mac_ax_adapter *adapter,
		      struct mac_ax_sec_cam_info *s_info,
		      struct fwcmd_seccam_info *sec_info,
		      u8 clear);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup Security
 * @{
 */

/**
 * @brief mac_sta_add_key
 *
 * @param *adapter
 * @param *sec_cam_content
 * @param mac_id
 * @param key_id
 * @param key_type
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_sta_add_key(struct mac_ax_adapter *adapter,
		    struct mac_ax_sec_cam_info *sec_cam_content,
		    u8 mac_id,
		    u8 key_id,
		    u8 key_type);

u32 mac_sta_del_key(struct mac_ax_adapter *adapter,
		    u8 mac_id,
		    u8 key_id,
		    u8 key_type);

u32 mac_sta_search_key_idx(struct mac_ax_adapter *adapter,
			   u8 mac_id, u8 key_id, u8 key_type);

u32 mac_sta_hw_security_support(struct mac_ax_adapter *adapter,
				u8 hw_security_support_type, u8 enable);

u32 refresh_security_cam_info(struct mac_ax_adapter *adapter,
			      u8 mac_id);
/**
 * @}
 * @}
 */

#endif
