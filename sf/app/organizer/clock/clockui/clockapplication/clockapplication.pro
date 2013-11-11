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
# Description: Project definition file for clock application.
#

TEMPLATE = app
CONFIG += hb
TARGET = clock

INCLUDEPATH += \
		. \
		../clockappcontroller/inc \
		./traces

DEPENDPATH += \
		. \
		./src \
		./inc

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x10005903
	TARGET.EPOCHEAPSIZE = 0x200000 0x4000000
	
	LIBS += \
		-lclockappcontroller
		
	SKINICON = qtg_large_clock
}

SOURCES += \
		main.cpp \
		clockapplication.cpp
		
HEADERS += \
		clockapplication.h

TRANSLATIONS += clock.ts
dtplugin.sources = /epoc32/data/c/resource/qt/plugins/controlpanel/datetimesettingsplugin.qtplugin
dtplugin.path = /resource/qt/plugins/controlpanel
DEPLOYMENT += dtplugin

# End of file	--Don't remove this.
