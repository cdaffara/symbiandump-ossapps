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
# Description:  Project definition file for clock settings utility.
#

TEMPLATE = lib
TARGET = clocksettingsutility
DEFINES += SETTINGSUTILITY_LIBRARY

CONFIG += hb

INCLUDEPATH += ../../inc \
               ../clocktimezone/inc \
               ./traces
               
DEPENDPATH += . \
			  ./inc \
			  ./src

symbian: {
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	TARGET.UID3 = 0x2002E6B3
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += -leuser \
	        -ltimezoneclient \
	        -lxqsettingsmanager
}

SOURCES += settingsutility.cpp
HEADERS += settingsutility.h

# End of file	--Don't remove this.
