#  
#  Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
#  Description: This is the project specification file for the calendar editor
#  project.
#  

TEMPLATE = lib
CONFIG += hb
TARGET = caleneditor
DEFINES += CALENEDITOR_LIB
INCLUDEPATH += ./src \
			./inc \
			./traces \
			..\inc
DEPENDPATH += ./src \
			  ./inc \
			  ./data

symbian: {
			
	TARGET.UID3 = 0x10207198
	TARGET.CAPABILITY += ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	
	LIBS += -lagendainterface \
			-lcalencommonutils \
                        -lxqservice \
			-lcalenlauncher \
			-lbafl
			
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}

SOURCES += caleneditor.cpp \
					caleneditorcustomitem.cpp \
					caleneditor_p.cpp \
					caleneditordocloader.cpp \
					caleneditorreminderfield.cpp \
					caleneditorrepeatfield.cpp \
					caleneditordatahandler.cpp
					
HEADERS +=	../../organizer_plat/calendar_editor_api/inc/caleneditor.h \
            caleneditorcustomitem.h \
            caleneditorcommon.h \
            caleneditordocloader.h \
            caleneditor_p.h \
            caleneditorreminderfield.h \
            caleneditorrepeatfield.h \
            caleneditordatahandler.h
         
RESOURCES += caleneditor.qrc

TRANSLATIONS += caleneditor.ts

# End of file	--Don't remove this.
