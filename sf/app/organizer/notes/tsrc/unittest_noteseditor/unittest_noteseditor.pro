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
# Project specification file for noteseditor unit test.
#

TEMPLATE = app
TARGET = utnoteseditor
CONFIG += \
		qtestlib \
		symbian_test \
		hb

DEPENDPATH += \
		. \
		./src

INCLUDEPATH += . \
                ..\..\notesui\noteseditor\inc


symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += \
		-lnoteseditor \
		-lagendainterface
}

SOURCES += \
		unittest_noteseditor.cpp \
		main.cpp

HEADERS += \
		./inc/unittest_noteseditor.h

# End of file	--Don't remove this.
