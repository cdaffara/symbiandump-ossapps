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

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :	do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES : do_nothing

FINAL : 
	echo Copying common UI test DLLs to z\sys\bin
	copy %epocroot%epoc32\release\winscw\udeb\t_*glx*.dll %epocroot%epoc32\release\winscw\udeb\z\sys\bin\ /y 
