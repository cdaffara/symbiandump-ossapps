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
TARGET = $$qtLibraryTarget(unibiomessagedataplugin)

TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

#UID 3
TARGET.UID3 = 0x2001FE61

DEPENDPATH += . inc src
INCLUDEPATH += ./inc
INCLUDEPATH += ../../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

SYMBIAN_PLATFORMS = WINSCW ARMV5

CONFIG += plugin

# Input
HEADERS += inc/unibiomessagedataplugin.h \
	inc/unibiomessagedataplugin_p.h 
	   	   		   
SOURCES += src/unibiomessagedataplugin.cpp \
	src/unibiomessagedataplugin_p.cpp
 
# Build.inf rules 
BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/unibiomessagedataplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(unibiomessagedataplugin.iby)" 
 
    symbian {
    BLD_INF_RULES.prj_exports += "prov.rsc /epoc32/data/z/resource/messaging/bif/prov.rsc"
		}
     
 LIBS += -leuser \
	-lconvergedmessageutils\
	-lmsgs \
	-lSmcm \
	-lgsmu \
	-letext \
	-lQtCore \
	-lefsrv \
	-lbioc \
	-lbifu \
	-lxqutils


# plugin stub deployment
plugin.sources = unibiomessagedataplugin.dll
plugin.path = \resource\qt\plugins\messaging\datamodel
DEPLOYMENT += plugin




symbian:MMP_RULES += SMPSAFE
