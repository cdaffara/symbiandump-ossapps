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
# This is the project specification file for the
# clockappcontroller component.
#

TEMPLATE = lib
TARGET = clockappcontroller
DEFINES += CLOCKAPPCONTROLLER_LIB

DEPENDPATH += \
		./inc \
		./src

INCLUDEPATH += \
		./inc \
		../clockviewmanager/inc \
		../../inc \
		../../clockmw/clocktimezone/inc \
		../../clockmw/clocksettingsutility/inc \
		../../clockmw/clockalarms/inc \
		./traces

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
		TARGET.UID3 = 0x2002BCE3

	LIBS += \
		-lclockviewmanager \
		-ltimezoneclient \
		-lclocksettingsutility \
		-lclockalarmclient
}

SOURCES += \
		clockappcontroller.cpp \
		clockappcontrollerifimpl.cpp

HEADERS += \
		clockappcontroller.h \
		clockappcontrollerdefines.h \
		clockappcontrollerif.h \
		clockappcontrollerifimpl.h

# End of file   --Don't remove this.
