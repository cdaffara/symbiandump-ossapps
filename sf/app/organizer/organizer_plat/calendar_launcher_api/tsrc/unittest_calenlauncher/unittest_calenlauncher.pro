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
#  Description: This is the project specification file for unittest_agendautil.
#

TEMPLATE = app
TARGET = utcalenlauncher
CONFIG += qtestlib 
CONFIG += hb
CONFIG += symbian_test
DEPENDPATH += . \
			  ./src \
			  ../../../../calendarui/calenlauncher/src \
			  ../../inc
INCLUDEPATH += .


symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE	
	
	LIBS += -lcalenlauncher
	
	BLD_INF_RULES.prj_testexports += \
	"../rom/unit_calenlauncher.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unit_calenlauncher.iby)"
}

SOURCES += unittest_calenlauncher.cpp

# End of file	--Don't remove this
