#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Project definition file for calendar settings.
#

TEMPLATE = lib
TARGET = calensettings
CONFIG += hb
DEPENDPATH += . \
              ../../inc
			  
INCLUDEPATH += . \
               ../../inc \
	       ../inc

DEFINES += CALENSETTINGS_DLL

symbian: {
	BLD_INF_RULES.prj_exports += \
	"./inc/calensettings.h  |../inc/calensettings.h"
	TARGET.UID3 = 0x1020E8BD
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += -leuser \
				-lxqsettingsmanager

}

SOURCES += ./src/calensettings.cpp 
HEADERS += ./inc/calensettings.h

# End of file	--Don't remove this.