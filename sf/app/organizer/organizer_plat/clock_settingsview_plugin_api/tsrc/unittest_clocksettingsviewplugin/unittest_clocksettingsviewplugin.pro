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
# Project specification file for clocksettings view plugin unit test.
#

TEMPLATE = app
TARGET = utclocksettingsviewplugin
CONFIG += \
		qtestlib \
		symbian_test \
		hb

DEPENDPATH += \
		. \
		./inc \
		./src

INCLUDEPATH += .


symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x2003157D
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE	
	
	BLD_INF_RULES.prj_testexports += \
	"../rom/unit_clocksettingsviewplugin.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unit_clocksettingsviewplugin.iby)"
}



SOURCES += \
		unittest_clocksettingsviewplugin.cpp \
		main.cpp

HEADERS += \
		unittest_clocksettingsviewplugin.h

# End of file	--Don't remove this.
