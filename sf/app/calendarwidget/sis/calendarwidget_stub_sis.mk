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


TARGETDIR=$(EPOCROOT)EPOC32\Data\Z\System\Install

SISNAME=calendarwidget_stub
PKGNAME=calendarwidget_stub

$(TARGETDIR) :
	@perl -S emkdir.pl "$(TARGETDIR)"

do_nothing :
	rem do_nothing

SISFILE=$(TARGETDIR)\$(SISNAME).sis

$(SISFILE) : ..\sis\$(PKGNAME).pkg
	makesis -s $? $@ 



MAKMAKE : do_nothing

RESOURCE : do_nothing

SAVESPACE : do_nothing

BLD : do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : $(TARGETDIR) $(SISFILE)

CLEAN : 
	@if exist $(SISFILE) erase $(SISFILE)

RELEASABLES : 
	@echo $(SISFILE)