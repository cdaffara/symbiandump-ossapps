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
# This is the project specification file for the notes editor project.
#

TEMPLATE = lib
CONFIG += hb
TARGET = noteseditor
DEFINES += NOTESEDITOR_LIB

INCLUDEPATH += \
		. \
		./src \
		./inc \
		../../../inc \
		./traces
		
DEPENDPATH += \
		. \
		./src \
		./inc \
		./data

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x20029F82
	
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += -lagendainterface \
			-lcaleneditor
	
	BLD_INF_RULES.prj_exports += \
		"./inc/noteseditor.h  |../../inc/noteseditor.h" \
		"./inc/noteseditordefines.h  |../../inc/noteseditordefines.h
}

SOURCES += \
		noteseditor.cpp \
		noteseditor_p.cpp \
		noteseditordocloader.cpp \
		notesnoteeditor.cpp \
		notestodoeditor.cpp	\
		notestodoeditorcustomitem.cpp

HEADERS += \
		noteseditor.h \
		noteseditordefines.h \
		noteseditor_p.h \
		noteseditordocloader.h \
		noteseditorcommon.h \
		notesnoteeditor.h \
		notestodoeditor.h \
		notestodoeditorcustomitem.h

RESOURCES += \
		noteseditor.qrc
		
TRANSLATIONS += noteseditor.ts		

# End of file	--Don't remove this.
