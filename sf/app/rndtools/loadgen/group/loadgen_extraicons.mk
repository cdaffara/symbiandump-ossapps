#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=\epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\loadgen_extraicons.mif

HEADERDIR=\epoc32\include
HEADERFILENAME=$(HEADERDIR)\loadgen_extraicons.mbg


do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN :
	@if exist $(ICONTARGETFILENAME) erase $(ICONTARGETFILENAME)
	@if exist $(HEADERFILENAME) erase $(HEADERFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : $(ICONTARGETFILENAME)

$(ICONTARGETFILENAME) (HEADERFILENAME) : ..\icons\qgn_prob_status_red.svg ..\icons\qgn_prob_status_green.svg
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/c8,8 ..\icons\qgn_prob_status_red.svg \
		/c8,8 ..\icons\qgn_prob_status_green.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME) && \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
