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
TARGET = $$qtLibraryTarget(unieditormmsplugin)

TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

#UID 3
TARGET.UID3 = 0x102072DB #old editor mms plugin uid



INCLUDEPATH += ../../editorgenutils/inc
INCLUDEPATH += ../../../../../inc
INCLUDEPATH += ../../../../../mmsengine/mmsmessage/inc 
INCLUDEPATH += ../../../../../mmsengine/inc
INCLUDEPATH += ../../../unidatautils/unidatamodel/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

SYMBIAN_PLATFORMS = WINSCW ARMV5

CONFIG += hb plugin

# Input
HEADERS += inc/unieditormmsplugin.h \
	inc/unieditormmsplugin_p.h 
		   	  		   
SOURCES += src/unieditormmsplugin.cpp \
	src/unieditormmsplugin_p.cpp 
    
 LIBS += -leuser \
         -lcone \
         -lmsgs \ 
         -lmmsmessage \ 
         -lmmsserversettings \
         -lmmsgenutils \
         -lcommdb \
         -lefsrv \
         -lbafl \
         -leditorgenutils \
         -lunidatamodelloader \
         -lconvergedmessageutils \
         -lunidatamodel \
         -lmsgmedia \
         -leikctl \
         -lxqutils \
         -lQtContacts

	
# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/unieditormmsplugin.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(unieditormmsplugin.iby)"
	
# plugin stub deployment
plugin.sources = unieditormmsplugin.dll
plugin.path = \resource\qt\plugins\messaging\editorplugins
DEPLOYMENT += plugin




symbian:MMP_RULES += SMPSAFE
