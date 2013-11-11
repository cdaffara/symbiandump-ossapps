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
#  Description: This is the project specification file for the calencommonutils.
#

TEMPLATE = app
TARGET = utcalencommonutils
CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb
DEPENDPATH += . \
				./inc \
				./src
				
INCLUDEPATH += . \
				../../inc


# Input
HEADERS += test_calendateutils.h \
			test_calenagendautils.h
			
SOURCES += main.cpp \
			test_calendateutils.cpp \
			test_calenagendautils.cpp

symbian : {

	TARGET.CAPABILITY = ALL -TCB
	TARGET.UID3 = 0x200315AE
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += -lcalencommonutils \
			-lagendainterface	
	BLD_INF_RULES.prj_testexports += \
		"./rom/unittest_calencommonutils.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_calencommonutils.iby)" 		
}

# End of file	--Don't remove this.