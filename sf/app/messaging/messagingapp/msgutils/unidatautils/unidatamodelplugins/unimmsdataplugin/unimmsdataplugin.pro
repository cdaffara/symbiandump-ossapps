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
TARGET = $$qtLibraryTarget(unimmsdataplugin)

TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

#UID 3
TARGET.UID3 = 0xE01F5465 #need to get a uid 

DEPENDPATH += . inc src

INCLUDEPATH += ./inc
INCLUDEPATH += ../../../unieditorutils/unieditorutils/inc
INCLUDEPATH += ../../../../../inc
INCLUDEPATH += ../../../../mmsengine/mmsmessage/inc 
INCLUDEPATH += ../../unidatamodel/inc
INCLUDEPATH += ../../../../mmsengine/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

SYMBIAN_PLATFORMS = WINSCW ARMV5

CONFIG += plugin

# Input
HEADERS += inc/unimmsdataplugin.h \
	inc/unimmsdataplugin_p.h

	   	   
		   
SOURCES += src/unimmsdataplugin.cpp \
	src/unimmsdataplugin_p.cpp
 
# Build.inf rules 
BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
  "rom/unimmsdataplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(unimmsdataplugin.iby)" \
	
    
 LIBS += -leuser \
	-lbafl \
	-lestor \
	-lconvergedmessageutils\
	-lmsgs \
	-letext \
	-lmmscli \
	-lefsrv \
	-lmmsmessage \
	-leditorgenutils \
	-lunidatamodel \
	-lmmsserversettings \
	-lmsgmedia \
	-leikctl \
	-lxqutils 
	
# plugin stub deployment
plugin.sources = unimmsdataplugin.dll
plugin.path = \resource\qt\plugins\messaging\datamodel
DEPLOYMENT += plugin




symbian:MMP_RULES += SMPSAFE
