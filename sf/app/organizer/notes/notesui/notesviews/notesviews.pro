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
# Description: Project definition file for notes views.
#

TEMPLATE = lib
TARGET = notesviews
CONFIG += hb
DEFINES += NOTESVIEWS_LIB

INCLUDEPATH += \
		. \
		../inc \
		../notesviewmanager/inc \
		../notesmodel/inc \
		../noteseditor/inc \
		../notesappcontroller/inc \
		../../inc \
		../../../inc \
		./traces
		
DEPENDPATH += \
		. \
		./inc \
		./src

symbian: {
	TARGET.CAPABILITY += ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x20029F83
	
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

	LIBS += \
		-lagendainterface \
		-lnotesmodel \
		-lnoteseditor \
		-lagendaeventviewer \
		-lafservice 
}

SOURCES += \
		notesmainview.cpp \
		notescollectionview.cpp \
		notestodoview.cpp \
		notesfavoriteview.cpp \
		notesnoteview.cpp

HEADERS += \
		notesmainview.h \
		notescollectionview.h \
		notestodoview.h \
		notesfavoriteview.h \
		notesnoteview.h

# End of file	--Don't remove this.
