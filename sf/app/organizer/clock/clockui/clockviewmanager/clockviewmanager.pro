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
# Description: This is the project specification file for the
# clockviewmanager.
#

TEMPLATE = lib
TARGET = clockviewmanager
CONFIG += hb
DEFINES += CLOCKVIEWMANAGER_LIB

DEPENDPATH += \
		. \
		./inc \
		./src

INCLUDEPATH += \
		./inc \
		../clockviews/inc \
		../clockappcontroller/inc \
		../clockwidget/clockwidgetimpl/inc \
		../../inc \
		./traces

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x2002BCE5
	
	LIBS += \
		-lclockviews \
		-lclockwidget \
		-lafservice 
}

SOURCES += \
		clockviewmanager.cpp \
		clockdocloader.cpp
		
HEADERS += \
		clockviewmanager.h \
		clockdocloader.h \
		clockviewmanagerdefines.h

# End of file	--Don't remove this.
