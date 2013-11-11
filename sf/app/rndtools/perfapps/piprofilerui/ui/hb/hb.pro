#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: 
#
#


TEMPLATE = app
TARGET = PIProfilerUI
DEPENDPATH += ./src
INCLUDEPATH += inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
load(hb.prf) 
symbian:CONFIG -= symbian_i18n 

# Input
HEADERS += 	./inc/pimainview.h \
			./inc/piprofilerengineprivate.h \
			./inc/pluginattributes.h \
			./inc/notifications.h \
			./inc/pisettingsview.h \
			./inc/pimainwindow.h \
			./inc/generalattributes.h \
			./inc/piprofilerengine.h
SOURCES += 	./src/pimainview.cpp \
			./src/piprofilerengineprivate.cpp \
			./src/main.cpp \
			./src/pluginattributes.cpp \
			./src/notifications.cpp \
			./src/pisettingsview.cpp \
			./src/pimainwindow.cpp \
			./src/piprofilerengine.cpp 
			
			
RESOURCES += piprofiler.qrc


symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x2001E5AE
    
    BLD_INF_RULES.prj_exports += "./rom/piprofilerui_hb.iby \
    	CORE_IBY_EXPORT_PATH(tools,piprofilerui.iby)"
    
	TARGET.CAPABILITY = ALL -TCB

	LIBS += -lcharconv -lbafl -lsysutil -lefsrv
    
    ICON = ../icons/qgn_menu_piprofilerui.svg 
    
} else {
    error("Only Symbian supported!")
}