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
# Description: Project file for globaldata module.
#


TEMPLATE = lib
TARGET = calenglobaldata
DEPENDPATH += ./inc ./src
INCLUDEPATH += ./inc \
			   ../inc 
			   
CONFIG += hb

DEFINES += CALENGLOBALDATA_DLL

symbian : {
	BLD_INF_RULES.prj_exports += \
		"./inc/calencontext.h  |../inc/calencontext.h" \
		"./inc/calencontextchangeobserver.h  |../inc/calencontextchangeobserver.h" \
		"./inc/caleninstanceid.h |../inc/caleninstanceid.h"  \
		"./inc/CalenInstanceId.inl |../inc/CalenInstanceId.inl" 			
	LIBS += -lcalinterimapi \
					-lcalencommonutils
	
	TARGET.UID3 = 0x1020E866												  
	TARGET.CAPABILITY = CAP_GENERAL_DLL 
	TARGET.EPOCALLOWDLLDATA=1
	
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	}
	
# Input
HEADERS += ./inc/calencontext.h \
           ./inc/calencontextchangeobserver.h \
           ./inc/calencontextimpl.h \
           ./inc/caleninstanceid.h \
           ./inc/CalenInstanceId.inl
SOURCES += ./src/calencontextimpl.cpp 

# End of file	--Don't remove this.