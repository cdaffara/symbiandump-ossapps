#  
#  Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
#  Description: This is the project specification file for the calendar launcher
#  project.
#  

TEMPLATE = lib
CONFIG += hb
TARGET = calenlauncher
DEFINES += CALENLAUNCHER_LIB
INCLUDEPATH += ./src \
			./inc
DEPENDPATH += ./src \
			  ./inc 


symbian: {				
				
	TARGET.UID3 = 0x10282DCD
	TARGET.CAPABILITY += ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += -lxqservice \
          -lxqserviceutil
}

SOURCES += calenlauncher.cpp		   
HEADERS += ../../organizer_plat/calendar_launcher_api/inc/calenlauncher.h	   
		   
# End of file	--Don't remove this.
