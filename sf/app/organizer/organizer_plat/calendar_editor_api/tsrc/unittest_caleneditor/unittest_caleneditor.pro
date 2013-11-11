#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
# 
# Description: 
# Project specification file for caleneditor unit test.
#


TEMPLATE = app
TARGET = ut_caleneditor
CONFIG += qtestlib \
			hb
CONFIG += symbian_test
DEPENDPATH += . \
				./src/ \
				../../../../calendarui/caleneditor/inc \
			   ../../../../calendarui/caleneditor/src	 \
			   ../../../../calendarui/caleneditor/data		   
			   
DEFINES += TESTCALENEDITOR \
		   CALENEDITOR_LIB 
INCLUDEPATH += . \
		../../../../calendarui/caleneditor/traces \
               ../../../../calendarui/caleneditor/inc \
			   ../../../../calendarui/agendainterface/inc \
			   ../../../../calendarui/commonutils/inc \
			   ../../../../calendarui/inc \
			   ../../../../calendarui/controller/inc \
			   ../../../../calendarui/globaldata/inc

symbian: {

    LIBS += -leuser \
            -lagendainterface \
			-lcalencommonutils \
			-lcalenlauncher

    TARGET.EPOCHEAPSIZE = 0x200000 0x4000000
    TARGET.CAPABILITY = CAP_GENERAL_DLL
	
		INCLUDEPATH +=  $$APP_LAYER_SYSTEMINCLUDE
	
		BLD_INF_RULES.prj_testexports += \
	"../rom/unit_caleneditor.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(unit_caleneditor.iby)"
}

SOURCES +=  unittest_caleneditor.cpp \
			caleneditor.cpp \
			caleneditorcustomitem.cpp \
			caleneditor_p.cpp \
			caleneditordocloader.cpp 
		
HEADERS += 	   caleneditorcustomitem.h \
		   caleneditorcommon.h \
		   caleneditordocloader.h \
		   caleneditor_p.h 
		  

RESOURCES += caleneditor.qrc
# End of file	--Don't remove this.
