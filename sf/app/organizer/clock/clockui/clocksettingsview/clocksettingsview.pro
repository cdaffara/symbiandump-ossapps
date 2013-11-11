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
# Project definition file for clock settings view.
#
#

TEMPLATE = lib
TARGET = clocksettingsview
CONFIG += hb
DEFINES += CLOCKSETTINGSVIEW_LIB

INCLUDEPATH += \
		./inc \
		../../clockmw/clocksettingsutility/inc \
		../../clockmw/clocktimezone/inc \
		../clockcityselectionlist/inc \
		../../inc \
		./traces


DEPENDPATH += \
		./inc \
		./src \
		./data

symbian: {
	TARGET.CAPABILITY += ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x10005906
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

	LIBS += \
		-ltimezoneclient \
		-lclocksettingsutility \
		-lclockcityselectionlist \
		-lxqsettingsmanager
}

SOURCES += \
		clocksettingsview.cpp \
		clocksettingsdocloader.cpp \
		settingscustomitem.cpp \
		clockregionalsettingsview.cpp

HEADERS += \
		clocksettingsdefines.h \
		clocksettingsview.h \
		clocksettingsdocloader.h \
		settingscustomitem.h \
		clockregionalsettingsview.h
		
RESOURCES += \
		clocksettingsview.qrc
		
TRANSLATIONS += clocksettingsview.ts

# End of file	--Don't remove this.
