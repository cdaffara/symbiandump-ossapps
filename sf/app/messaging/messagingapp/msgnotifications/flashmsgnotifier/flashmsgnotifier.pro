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
TARGET = flashmsgnotifier
CONFIG += hb service

QT += core

INCLUDEPATH += . inc ../inc
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../msgnotifier/inc
INCLUDEPATH += ../../../inc


HEADERS += inc/flashmsgnotifier.h \
					 inc/flashmsgnotifier_p.h 
		   
SOURCES += src/main.cpp \
    	   src/flashmsgnotifier.cpp \
    	   src/flashmsgnotifier_p.cpp 
           
LIBS += -lxqservice \
	    -lxqserviceutil \
	    -lmsgs \
	    -letext \
	    -lsmcm \
	    -lgsmu \
	    -lxqutils \
	    -lcsserverclientapi \
	    -lxqsystemtoneservice \
	    -lxqsettingsmanager \
	    -lhwrmvibraclient

SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

# Capability
symbian {
   TARGET.UID3 = 0x2001FE72
   TARGET.CAPABILITY = All -TCB 
}

TRANSLATIONS = messaging.ts

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/flashmsgnotifier.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(flashmsgnotifier.iby)"
                             