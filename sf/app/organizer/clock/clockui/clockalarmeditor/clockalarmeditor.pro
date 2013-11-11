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
# Description:  Project definition file for clock alarmeditor
#

TEMPLATE = lib
CONFIG += hb
TARGET = clockalarmeditor
DEFINES += CLOCKALARMEDITOR_LIB

INCLUDEPATH += \
		../../clockmw/clockalarms/inc \
        ../../clockmw/clocksettingsutility/inc \
		../../clockmw/clocktimezone/inc \
        ../../inc \
        ./traces

DEPENDPATH += \
		./inc \
		./src \
		./data

symbian: {
	TARGET.CAPABILITY += ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x2001CC10
	
	LIBS += \
		-lclockalarmclient \
	    -lclocksettingsutility \
		-ltimezoneclient \
	    -lxqsettingsmanager
}

SOURCES += \
		clockalarmeditor.cpp \
        	clockalarmcustomitem.cpp 
		   
HEADERS += \
		clockalarmeditor.h \
        	clockalarmcustomitem.h \ 
	    	clockalarmeditordefines.h
		
RESOURCES += clockalarmeditor.qrc
		
TRANSLATIONS += clockalarmeditor.ts

# End of file	--Don't remove this.
