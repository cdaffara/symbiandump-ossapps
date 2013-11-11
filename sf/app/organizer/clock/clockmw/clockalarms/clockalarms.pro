#
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Project definition file for AlarmClient
#

TEMPLATE = lib
TARGET = clockalarmclient
DEFINES += ALARMCLIENT_LIBRARY

INCLUDEPATH += ../../inc \
			   ./traces

DEPENDPATH += . \
			  ./inc \
			  ./src
			  
symbian: {
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	TARGET.UID3 = 0x2002DD0E

LIBS += -leuser \
		-lalarmclient \
		-lalarmshared
}

SOURCES += alarmclient.cpp \
		   alarmlistener.cpp
HEADERS += alarmclient.h \
		   alarmlistener.h

# End of file	--Don't remove this.
