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

TARGET = csserver
QT -= gui
CONFIG += hb
CONFIG += NO_ICON

DEPENDPATH += . ./inc ./src

INCLUDEPATH += ./inc \
               ./../utils/inc \
               ./../../../mmsengine/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

SYMBIAN_PLATFORMS = WINSCW ARMV5

symbian {
    TARGET.UID3 =  0x2002A542
    TARGET.CAPABILITY = CAP_SERVER
    TARGET.EPOCSTACKSIZE = 0x14000
    VENDORID =  VID_DEFAULT
    TARGET.EPOCHEAPSIZE = 0x100000 0x1000000
    VERSION = 1.0
}

HEADERS += ccsserver.h \
           ccssession.h \
           ccsplugininterface.h \
           ccscontactsresolver.h \
           ccscontactsmanager.h \ 
           ccsconversationcache.h \
           ccsconversationcachehelper.h \
           ccsconversation.h \
           ccsconversationevent.h \
           ccsconversationcontact.h \                     
           ccsconversationdeletehandler.h \
           ccsconversationmarkreadhandler.h \
           ccsbackuphandler.h

SOURCES += ccsstartup.cpp \
           ccsserver.cpp \
           ccssession.cpp \
           ccsplugininterface.cpp \
           ccscontactsresolver.cpp \
           ccscontactsmanager.cpp \
           ccsconversationcache.cpp \
           ccsconversationcachehelper.cpp \
           ccsconversation.cpp \
           ccsconversationevent.cpp \
           ccsconversationcontact.cpp \                      
           ccsconversationdeletehandler.cpp \
           ccsconversationmarkreadhandler.cpp \
           ccsbackuphandler.cpp

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "$${LITERAL_HASH}include <data_caging_paths.hrh>" \
     "rom/csserver.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(csserver.iby)" \
     "stub_sis/csserver_stub.sis   /epoc32/data/z/system/install/csserver_stub.sis" \
     "backup_registration.xml z:/private/2002A542/backup_registration.xml"
     
LIBS += -leuser \
        -lcone \
        -leikcore \
        -lefsrv \
        -lestor \
        -lflogger \
        -lecom \
        -lcsutils \
        -lmsgs \        
        -lbafl \
        -lcentralrepository \
        -limcm \
        -lQtContacts \
        -lxqutils 
        
        


symbian:MMP_RULES += SMPSAFE
