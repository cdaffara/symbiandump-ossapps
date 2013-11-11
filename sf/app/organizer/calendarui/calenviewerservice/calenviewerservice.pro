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
#  Description: This is the project specification file for the calenviewerservice exe.
#  

TEMPLATE = app
TARGET = calenviewerservice

DEPENDPATH += 	. \
				./inc \
				./src \
				../inc

INCLUDEPATH += 	. \
				./inc \
				../inc \
				./traces

CONFIG += hb

symbian: { 
	LIBS += -lagendaeventviewer \
			-lxqservice \ 
			-lxqserviceutil \
			-lagendainterface \
			-lagendaeventviewer
    
  TARGET.CAPABILITY = All -TCB
	TARGET.UID3 = 0x20031594
  INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}

# Input
HEADERS += 	calenviewerservice.h

SOURCES += 	calenviewerservice.cpp \
			main.cpp

CONFIG += service
SERVICE.FILE = data/calenviewerservice_conf.xml
SERVICE.OPTIONS += embeddable
SERVICE.OPTIONS += hidden

RSS_RULES += \
	"datatype_list = " \
	"      {" \
	"      DATATYPE" \
	"          {" \
	"          priority = EDataTypePriorityHigh;" \
	"          type = \"text/x-vCalendar\";" \  
	"          }," \
	"      DATATYPE" \
	"          {" \
	"          priority = EDataTypePriorityHigh;" \
	"          type = \"text/calendar\";" \  
	"          }" \
	"      };" \