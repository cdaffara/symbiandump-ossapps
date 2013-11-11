#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif

# ----------------------------------------------------------------------------

TARGETDIR=$(ZDIR)\RESOURCE\APPS
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\IMGPrintBitmaps.MIF
HEADERFILENAME=$(HEADERDIR)\IMGPrintBitmaps.MBG

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/c8,8 ..\Data\qgn_prop_print_slider_bar.svg \
		/c8,8 ..\Data\qgn_prop_print_slider_bar_short.svg \
		/c8,8 ..\Data\qgn_prop_print_slider_handle.svg \
		/c8,8 ..\Data\qgn_prop_print_slider_left.svg \
		/c8,8 ..\Data\qgn_prop_print_slider_right.svg \
		/c8,8 ..\Data\qgn_indi_print_cached.svg \
		/c8,8 ..\Data\qgn_prop_print_bt.svg \
		/c8,8 ..\Data\qgn_prop_print_kiosk.svg \
		/c8,8 ..\Data\qgn_prop_print_mmc.svg \
		/c8,8 ..\Data\qgn_prop_print_usb.svg \
		/c8,8 ..\Data\qgn_prop_print_wlan.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
