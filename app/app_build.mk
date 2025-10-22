#
# Copyright (C) 2024 MDK-Predator Team
#
# This file is part of PortaPack.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

app_mdk_predator_name = mdk_predator
app_mdk_predator_SOURCES = \
    mdk_predator_app.cpp \
    ../src/mdk_predator.c \
    ../src/automotive/key_fob_analyzer.c \
    ../src/automotive/rolling_code_tester.c \
    ../src/wireless/wifi_analyzer.c \
    ../src/wireless/bluetooth_analyzer.c \
    ../src/wireless/subghz_analyzer.c \
    ../src/crypto/crypto_analyzer.c

app_mdk_predator_INCLUDES = \
    ../include

app_mdk_predator_CFLAGS = \
    -std=c11 \
    -Wall \
    -Wextra

app_mdk_predator_CXXFLAGS = \
    -std=c++17 \
    -Wall \
    -Wextra

APPSRCS += $(addprefix $(APPDIR)/external/mdk_predator/,$(app_mdk_predator_SOURCES))
APPINCS += $(addprefix -I$(APPDIR)/external/mdk_predator/,$(app_mdk_predator_INCLUDES))
APPCFLAGS += $(app_mdk_predator_CFLAGS)
APPCXXFLAGS += $(app_mdk_predator_CXXFLAGS)
