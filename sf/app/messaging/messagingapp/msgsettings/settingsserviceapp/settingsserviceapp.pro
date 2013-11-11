# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = app
TARGET = settingsserviceapp

CONFIG += hb service

DEPENDPATH += . inc src
SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

INCLUDEPATH += inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../msgui/inc
INCLUDEPATH += ../settingsview/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Input
HEADERS += inc/settingsserviceinterface.h 
			
SOURCES += src/main.cpp \
           src/settingsserviceinterface.cpp 
			
# Capability
TARGET.CAPABILITY = ALL -TCB
TARGET.UID3 = 0x2001FE6A 

RESOURCES += settingsserviceapp.qrc

TRANSLATIONS = messaging.ts


LIBS += -lxqservice \
        -lxqserviceutil \
        -lsettingsview
	

BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
                             ".\rom\settingsserviceapp.iby CORE_APP_LAYER_IBY_EXPORT_PATH(settingsserviceapp.iby)"


symbian:MMP_RULES += SMPSAFE
