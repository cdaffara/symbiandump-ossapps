# 
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Symbian Foundation License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
# 
# Initial Contributors:
# Nokia Corporation - initial contribution.
# 
# Contributors: 
# 
# Description:
# This is the project specification file for unittest_notesmodel.
# 

TEMPLATE = app
TARGET = utnotesmodel
CONFIG += qtestlib
CONFIG += symbian_test
DEPENDPATH += \
		. \
		./src
INCLUDEPATH += \
		. \
		../../notesui/notesmodel/inc \
		../../inc

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0xEADA6E32
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += \
		-lnotesmodel \
		-lagendainterface
}

SOURCES += \
		unittest_notesmodel.cpp

# End of file	--Don't remove this
