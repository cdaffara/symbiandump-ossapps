#
# Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:   Base makefile for the specific icon makefiles.
#
#

ifeq (WINSCW,$(findstring WINSCW, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif

# ----------------------------------------------------------------------------
# NOTE: This does not work unless, the ICONTARGETNAME and ICONS variables are 
# defined. ICONTARGETNAME contains the target filename without path. ICONS 
# contain the list of icon files, that are feeded to MifConv. Additionally if
# needed you can define variable HEADERNAME that contains the name of the .mbg
# header file without path.
# ----------------------------------------------------------------------------

TARGETDIR?=$(ZDIR)\resource\apps
ICONTARGETFILENAME?=$(TARGETDIR)\$(ICONTARGETNAME)

HEADERDIR=\epoc32\include

ifdef HEADERNAME
HEADERCOMMAND=/h$(HEADERDIR)\$(HEADERNAME)
else
HEADERCOMMAND=
HEADERNAME=$(addsuffix .mbg, $(basename $(ICONTARGETNAME)))
endif

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

#-----------------------------------------------------------------------------
# NOTE: CLEAN target needs to do nothing, as the files echoed in RELEASABLES
# get deleted automatically.
#-----------------------------------------------------------------------------
CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included automatically by
# MifConv if the mask detph is defined.
#
# NOTE 2: Usually, source paths should not be included in the bitmap
# definitions. MifConv searches for the icons in all icon directories in a
# predefined order, which is currently \s60\icons, \s60\bitmaps2.
# The directory \s60\icons is included in the search only if the feature flag
# __SCALABLE_ICONS is defined.
# ----------------------------------------------------------------------------

RESOURCE : $(ICONTARGETFILENAME)
    $(ICONTARGETFILENAME) : $(filter-out /c%,$(ICONS)) 
	mifconv $(ICONTARGETFILENAME) $(HEADERCOMMAND) $(ICONS)

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERDIR)\$(HEADERNAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
