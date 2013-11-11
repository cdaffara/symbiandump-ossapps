# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: Message Error Notifier Exe Pro File

TEMPLATE = app
TARGET = msgerrornotifier
CONFIG += hb service

INCLUDEPATH += . inc ../inc
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

HEADERS += inc/msgerrornotifier.h \
		   inc/msgerrornotifiersvc.h
		   
SOURCES += src/main.cpp \
    	   src/msgerrornotifier.cpp \
           src/msgerrornotifiersvc.cpp
           
LIBS += -lxqservice \
	    -lxqserviceutil

SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

symbian:TARGET.UID3 = 0x2001FE74

TRANSLATIONS = messaging.ts

BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
                             ".\rom\msgerrornotifier.iby CORE_APP_LAYER_IBY_EXPORT_PATH(msgerrornotifier.iby)"
                             

symbian:MMP_RULES += SMPSAFE
