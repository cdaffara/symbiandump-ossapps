#  
#  Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
#  All rights reserved.
#  This component and the accompanying materials are made available
#  under the terms of "Eclipse Public License v1.0"
#  which accompanies this distribution, and is available
#  at the URL "http://www.eclipse.org/legal/epl-v10.html".
#  
#  Initial Contributors:
#  Nokia Corporation - initial contribution.
#  
#  Contributors: 
#  
#  Description: This is the project specification file for agendautil.
#  

TARGET = agendainterface
TEMPLATE = lib
CONFIG += hb
DEPENDPATH += . \
			  ./src \
			  ./inc
INCLUDEPATH += . \
			   ./inc \
			   ../inc

DEFINES += XQCALENDAR_LIBRARY

symbian: {
	LIBS += -lcalinterimapi \
			-lcalenImp \
			-lestor \
			-lefsrv \
			-lcaleninterimutils2
	
	TARGET.UID3 = 0x20029F7F
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}

HEADERS += ../../organizer_plat/agenda_interface_api/inc/agendautil.h \
		   agendautil_p.h \
		   ../../organizer_plat/agenda_interface_api/inc/agendaentry.h \
		   agendaentry_p.h

SOURCES += agendautil.cpp \
		   agendautil_p.cpp \
		   agendaentry.cpp
		   
# End of file	--Don't remove this.
