#
# Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  icons makefile for project mpxdbplugin
#


ifeq (WINS,$(findstring WINS,$(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32/release/$(PLATFORM)/$(CFG)/z
else
ZDIR=$(EPOCROOT)epoc32/data/z
endif

TARGETDIR=$(ZDIR)/resource/apps
HEADERDIR=$(EPOCROOT)epoc32/include
ICONTARGETFILENAME=$(TARGETDIR)/mpxpodcastdbplugin.mif
HEADERFILENAME=$(HEADERDIR)/mpxpodcastdbplugin.mbg

MAKMAKE : ;

BLD : ;

CLEAN : ;

LIB : ;

CLEANLIB : ;

RESOURCE : 
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/Ficonlist.txt

FREEZE : ;

SAVESPACE : ;

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : ;
