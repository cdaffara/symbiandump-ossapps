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
TARGET = utnoteseditorplugin
CONFIG += \
		qtestlib \
		symbian_test \
		hb

DEPENDPATH += \
		. \
		./src

INCLUDEPATH += .

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0xE3883256
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += \
		-lagendainterface
		
	BLD_INF_RULES.prj_testexports += \
	"../rom/unit_noteseditorplugin.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unit_noteseditorplugin.iby)"
}

SOURCES += \
		unittest_noteseditorplugin.cpp \
		main.cpp

HEADERS += \
		./inc/unittest_noteseditorplugin.h

# End of file	--Don't remove this.
