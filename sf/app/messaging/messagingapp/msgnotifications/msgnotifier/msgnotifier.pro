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
# Description: Message Notifier Exe Pro File
#

TEMPLATE = app
TARGET = msgnotifier


DEPENDPATH += . src
HEADERS += $$PUBLIC_HEADERS
INCLUDEPATH += . inc ../../../inc 
INCLUDEPATH += ../../msgsettings/msginit/inc
INCLUDEPATH += ../msgerrorwatcher/inc
INCLUDEPATH += ../inc
INCLUDEPATH += $$APP_LAYER_INCLUDEPATH
INCLUDEPATH += $$MW_LAYER_INCLUDEPATH
symbian:
{
TARGET.CAPABILITY = ALL -TCB
TARGET.UID3 = 0x2002E678
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$OS_LAYER_SYSTEMINCLUDE
SYMBIAN_PLATFORMS = WINSCW ARMV5
}

symbian {
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/msgnotifier.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(msgnotifier.iby)"
}

QT -= gui
CONFIG += NO_ICON hb

TRANSLATIONS = messaging.ts
# Input
SOURCES += 	src/main.cpp \
			src/msgnotifier.cpp \
			src/msgnotifier_p.cpp \
			src/msgstorehandler.cpp

HEADERS +=  inc/msgnotifier.h  \
  		    inc/msgnotifier_p.h \
  		    inc/msgstorehandler.h 

LIBS += -lcsserverclientapi \
        -lcsutils \
        -lmsginit \
        -lmsgs \
        -letext \
        -lsmcm \
        -lmmsmessage \
        -lmmsserversettings \
        -lmmscli \
        -lxqservice \
        -lQtContacts \
        -lQtVersit \
        -lunidatamodelloader \
	    -lxqsettingsmanager \
        -apgrfx.lib \
        -lxqutils \
        -lws32 \
        -lapgrfx \
	    -lxqsystemtoneservice \
        -lmsgerrorwatcher \
	    -lgsmu \
	    -lssmcmn \
	    -lhwrmvibraclient
        


symbian:MMP_RULES += SMPSAFE
