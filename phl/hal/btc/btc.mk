# All needed files would be added to _HAL_INTFS_FILES, and it would include
# hal_g6/btc and all related files in directory hal_g6/btc/.
# Before include this makefile, be sure interface (CONFIG_*_HCI) and IC
# (CONFIG_RTL*) setting are all ready!

HAL = hal

ifeq ($(CONFIG_PHL_ARCH), y)
phl_path := phl/hal
phl_path_d1 := $(src)/phl/$(HAL)
else
phl_path := hal
phl_path_d1 := $(src)/$(HAL)
endif

# Base directory
path_halbtc_d1 := $(phl_path)/btc

halbtc-y += $(path_halbtc_d1)/hal_btc.o \
			$(path_halbtc_d1)/halbtc_def.o \
			$(path_halbtc_d1)/halbtc_action.o \
			$(path_halbtc_d1)/halbtc_dbg_cmd.o \
			$(path_halbtc_d1)/halbtc_fw.o

ifeq ($(CONFIG_RTL8852A), y)
ic := 8852a
# Level 2 directory
path_halbtc_8852a := $(path_halbtc_d1)/btc_$(ic)

halbtc-y += $(path_halbtc_8852a)/btc_8852a.o

endif

ifeq ($(CONFIG_RTL8852B), y)
ic := 8852b
# Level 2 directory
path_halbtc_8852b := $(path_halbtc_d1)/btc_$(ic)

halbtc-y += $(path_halbtc_8852a)/btc_8852b.o
endif

_BTC_FILES +=	$(halbtc-y)
