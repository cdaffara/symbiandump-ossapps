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
# Description: Project file for customisationmanager module.
#


TEMPLATE = lib
TARGET = calencustomisationmanager
DEPENDPATH += ../inc ../src 
CONFIG += hb

DEFINES += CALENCUSTOMISATIONMANAGER_DLL

INCLUDEPATH += ./inc ../inc \
			   ./traces

					   
# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5  

symbian : {
	BLD_INF_RULES.prj_exports += \
		"./inc/calencustomisationmanager.h  |../inc/calencustomisationmanager.h"
	
	TARGET.UID3 = 0x10282f28
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	TARGET.EPOCALLOWDLLDATA = 1
	
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	#Libs
	LIBS += -lecom \
					-lcalenglobaldata 
}
    	
# Input
HEADERS += ./inc/calencustomisationmanager.h 

SOURCES += ./src/calencustomisationmanager.cpp 

# End of file	--Don't remove this.
