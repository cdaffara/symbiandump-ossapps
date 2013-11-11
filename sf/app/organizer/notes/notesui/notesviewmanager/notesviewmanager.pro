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
# notesviewmanager.
#

TEMPLATE = lib
TARGET = notesviewmanager
CONFIG += hb
DEFINES += NOTESVIEWMANAGER_LIB

DEPENDPATH += \
		. \
		./inc \
		./src \
		../inc \
		../../data

INCLUDEPATH += \
		. \
		../notesmodel/inc \
		../notesviews/inc \
		../notesappcontroller/inc \
		../../inc \
		./traces

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x20029F84
	
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += \
		-lagendainterface \
		-lnotesmodel \
		-lnotesviews \
		-lafservice 
}

SOURCES += \
		notesviewmanager.cpp \
		notesdocloader.cpp
		
HEADERS += \
		notesviewmanager.h \
		notesdocloader.h \
		notesviewmanagerdefines.h \
		notescommon.h
		
RESOURCES += \
		notes.qrc

# End of file	--Don't remove this.
