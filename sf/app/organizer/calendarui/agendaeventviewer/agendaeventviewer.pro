#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
# Project specification file for Agenda Event Viewer.
#

TEMPLATE = lib
CONFIG += hb
TARGET = agendaeventviewer
DEFINES += AGENDAEVENTVIEWER_LIB

INCLUDEPATH += \
		. \
		./src \
		./inc \
		../inc \
		./traces 
		
		
		
DEPENDPATH += \
		. \
		./src \
		./inc \
		./data

symbian: {

	BLD_INF_RULES.prj_exports += \
				"./inc/agendaeventviewer.h  |../inc/agendaeventviewer.h" \
				"./inc/agendaeventviewer.h  |../../inc/agendaeventviewer.h"
				
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x2002C367
	
	
	LIBS += -lagendainterface \
			-lcaleneditor \
			-lcalencommonutils \
			-lmaptileservice
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}

SOURCES += \
		agendaeventviewer.cpp \
		agendaeventviewer_p.cpp \
		agendaeventview.cpp \
		agendaeventviewerdocloader.cpp \
		agendaeventvieweritem.cpp
		
HEADERS += \
		agendaeventviewer.h \
		agendaeventviewer_p.h \
		agendaeventview.h \
		agendaeventviewercommon.h \ 
		agendaeventviewerdocloader.h \
		agendaeventvieweritem.h
				
RESOURCES += agendaeventviewer.qrc

TRANSLATIONS += caleneventviewer.ts

# End of file	--Don't remove this.
