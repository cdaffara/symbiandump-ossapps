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
#  Description: This is the project specification file for the calenviewerservice.
#

TEMPLATE = app
TARGET = utcalenviewerservice
CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb
DEPENDPATH += . inc src
INCLUDEPATH += .

# Input
HEADERS += inc/test_calenviewerservice.h
SOURCES += src/main.cpp src/test_calenviewerservice.cpp

symbian : {

	TARGET.CAPABILITY = ALL -TCB
	TARGET.UID3 = 0xE0003364

	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += -lxqserviceutil -lxqservice
	
	BLD_INF_RULES.prj_testexports += \
		"./rom/unittest_calenviewerservice.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_calenviewerservice.iby)" \
		"./data/meeting.vcs		z:/data/others/meeting.vcs" \
		"./data/meeting.ics		z:/data/others/meeting.ics"
}
