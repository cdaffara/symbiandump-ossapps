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
# Description:
#

TEMPLATE = lib
TARGET = $$qtLibraryTarget(unismsdataplugin)

TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

#UID 3
TARGET.UID3 = 0x2001FE60

DEPENDPATH += . inc src

INCLUDEPATH += ./inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../../../../inc


SYMBIAN_PLATFORMS = WINSCW ARMV5

CONFIG += plugin

# Input
HEADERS += inc/unismsdataplugin.h \
	inc/unismsdataplugin_p.h
		   
SOURCES += src/unismsdataplugin.cpp \
	src/unismsdataplugin_p.cpp 
 
# Build.inf rules 
BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
   "rom/unismsdataplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(unismsdataplugin.iby)" 
    
 LIBS += -leuser \
	-lconvergedmessageutils\
	-lmsgs \
	-lSmcm \
	-lgsmu \
	-letext \
	-lQtCore \
	-lxqutils
	
# plugin stub deployment
plugin.sources = unismsdataplugin.dll
plugin.path = \resource\qt\plugins\messaging\datamodel
DEPLOYMENT += plugin




symbian:MMP_RULES += SMPSAFE
