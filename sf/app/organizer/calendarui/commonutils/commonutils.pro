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
# Description: Project defnition file for commonutils.
#

TEMPLATE = lib
TARGET = calencommonutils
DEPENDPATH += ./ \
		./inc \
		./src
INCLUDEPATH += ./inc ./../inc

CONFIG += hb

DEFINES += CALENDATEUTILS_DLL

symbian : {
	BLD_INF_RULES.prj_exports += \
		"./inc/calenagendautils.h  |../inc/calenagendautils.h" \
		"./inc/calendateutils.h  |../inc/calendateutils.h"   

  TARGET.UID3 = 0x10207199
  TARGET.CAPABILITY = CAP_GENERAL_DLL
  TARGET.EPOCALLOWDLLDATA=1
  
  INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
  
  LIBS += -lcalinterimapi    \
		  		-lagendainterface
}

# Input
HEADERS += calenagendautils.h	\
						calendateutils.h		
SOURCES += calenagendautils.cpp	\
						calendateutils.cpp 
# End of file	--Don't remove this.