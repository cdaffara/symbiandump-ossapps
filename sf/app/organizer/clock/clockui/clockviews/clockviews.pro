#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Project specification file for clockviews.
#

TEMPLATE = lib
TARGET = clockviews
CONFIG += hb
DEFINES += CLOCKVIEWS_LIB

INCLUDEPATH += \
		./inc \
		../../inc \
		../clockviewmanager/inc \
		../clockappcontroller/inc \
		../../clockmw/clockalarms/inc \
		../../clockmw/clocktimezone/inc \
		../../clockmw/clocksettingsutility/inc \
		../clocksettingsview/inc \
		../clockalarmeditor/inc \
		../clockwidget/clockwidgetimpl/inc \
		../clockcityselectionlist/inc \
		../inc \
		./traces

DEPENDPATH += \
		./inc \
		./src \
		./data

symbian: {
	TARGET.CAPABILITY += ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x2002BCE6
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

	LIBS += \
		-lclockalarmclient \
		-ltimezoneclient \
		-lclockalarmeditor \
		-lclocksettingsutility \
		-lclocksettingsview \
		-lclockcityselectionlist \
		-lclockwidget \
		-lclockwidgetplugin \
		-lxqsettingsmanager \
		-lafservice 
}

SOURCES += \
		clockmainview.cpp \
		clockalarmlistmodel.cpp	\	
		clockalarmlistitemprototype.cpp \		
		clockworldview.cpp \
		clockhomecityitem.cpp

HEADERS += \
		clockmainview.h \
		clockalarmlistmodel.h \
		clockalarmlistitemprototype.h \	
		clockworldview.h \
		clockhomecityitem.h

RESOURCES += \
		clockviews.qrc

# End of file	--Don't remove this.
