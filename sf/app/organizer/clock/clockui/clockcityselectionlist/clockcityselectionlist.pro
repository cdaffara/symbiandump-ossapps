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
# Project specification file for clockcityselectionlist.
#

TEMPLATE = lib
TARGET = clockcityselectionlist
CONFIG += hb
DEFINES += CLOCKCITYSELECTIONLIST_LIB

INCLUDEPATH += \
		./inc \
		../../inc \
		../../clockmw/clocktimezone/inc \
		./traces

DEPENDPATH += \
		./inc \
		./src \
		./data

symbian: {
	TARGET.CAPABILITY += ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0xABCDE123
	
	LIBS += \
		-ltimezoneclient
}

SOURCES += \
		clockcityselectionlist.cpp \
		clockcityselectionlist_p.cpp \
		clockcitylistproxymodel.cpp \


HEADERS += \
		clockcityselectionlist.h \
		clockcityselectionlist_p.h \
		clockcitylistproxymodel.h \
		clockcityselectionlistdefines.h \
		clockcityselectionlistcommon.h \
		
RESOURCES += \
		clockcityselectionlist.qrc
		
TRANSLATIONS += clockcityselectionlist.ts

# End of file	--Don't remove this.
