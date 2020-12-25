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
#ifndef _RTW_TRX_OPS_H_
#define _RTW_TRX_OPS_H_
#include <drv_types.h>

struct lite_data_buf *rtw_alloc_litedatabuf(struct trx_data_buf_q *data_buf_q);
s32 rtw_free_litedatabuf(struct trx_data_buf_q *data_buf_q,
		struct lite_data_buf *lite_data_buf);

#endif /* _RTW_TRX_OPS_H_ */
