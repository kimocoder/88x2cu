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
#ifndef __PHL_SER_DEF_H__
#define __PHL_SER_DEF_H__


enum RTW_PHL_SER_NOTIFY_EVENT {
	RTW_PHL_SER_L0_RESET, /* L0 notify only */
	RTW_PHL_SER_PAUSE_TRX, /* M1 */
	RTW_PHL_SER_DO_RECOVERY, /* M3 */
	RTW_PHL_SER_READY, /* M5 */
	RTW_PHL_SER_L2_RESET,/* M9 */
	RTW_PHL_SER_EVENT_CHK,
	RTW_PHL_SER_MAX = 6
};

enum RTW_PHL_SER_RCVY_STEP {
	RTW_PHL_SER_L1_DISABLE_EN = 0x0001,
	RTW_PHL_SER_L1_RCVY_EN = 0x0002,
	RTW_PHL_SER_L0_CFG_NOTIFY = 0x0010,
	RTW_PHL_SER_L0_CFG_DIS_NOTIFY = 0x0011,
	RTW_PHL_SER_L0_CFG_HANDSHAKE = 0x0012,
	RTW_PHL_SER_L0_RCVY_EN = 0x0013,
};

enum RTW_PHL_SER_LV1_RCVY_STEP {
	RTW_PHL_SER_LV1_RCVY_STEP_1 = 0,
	RTW_PHL_SER_LV1_SER_RCVY_STEP_2,

	/* keep last */
	RTW_PHL_SER_LV1_RCVY_STEP_LAST,
	RTW_PHL_SER_LV1_RCVY_STEP_MAX = RTW_PHL_SER_LV1_RCVY_STEP_LAST,
	RTW_PHL_SER_LV1_RCVY_STEP_INVALID = RTW_PHL_SER_LV1_RCVY_STEP_LAST,
};


#endif /* __PHL_SER_H__ */
