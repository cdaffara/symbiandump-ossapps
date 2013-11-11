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

TEMPLATE = app
TARGET = messaging101

DEPENDPATH += . inc src ../inc
INCLUDEPATH += .
INCLUDEPATH += ../inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../unifiededitor/inc
INCLUDEPATH += ../appengine/inc
INCLUDEPATH += ../conversationview/inc
INCLUDEPATH += ../unifiedviewer/inc
INCLUDEPATH += ../../msgsettings/settingsview/inc
INCLUDEPATH += ../../smartmessaging/ringbc/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb service

QT += sql

symbian {
   TARGET.UID3 = 0x2001FE79
   TARGET.CAPABILITY = All -TCB
   TARGET.EPOCSTACKSIZE = 0x14000
   TARGET.EPOCHEAPSIZE = 0x20000 0x1000000
   SKINICON = qtg_large_message
}
  
# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/msgapp.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(msgapp.iby)" \
     "rom/messaginglangaugeresources.iby             LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(messaginglangaugeresources.iby)" \
     "sis/msgapp_stub.sis   /epoc32/data/z/system/install/msgapp_stub.sis" \
     "rom/messaging_uda.confml  CONFML_EXPORT_PATH(messaging_uda.confml,uda_content)" \
     "rom/messaging_uda_copy.implml CRML_EXPORT_PATH(messaging_uda_copy.implml,uda_content)" \
     "rom/private.zip CRML_EXPORT_PATH(../content/zip/,uda_content)" \
     "resources/xml/messaging101.docml  /epoc32/data/z/resource/hb/splashml/messaging101.docml" \
     "resources/xml/messaging101.splashml  /epoc32/data/z/resource/hb/splashml/messaging101.splashml" \
     "resources/xml/messaging101_dummy.docml  /epoc32/data/z/resource/hb/splashml/messaging101_dummy.docml" \
     "resources/xml/messaging101_dummy.splashml  /epoc32/data/z/resource/hb/splashml/messaging101_dummy.splashml"
          
# Input
HEADERS += msgmainwindow.h \
     msglistview.h \
     draftslistview.h \
     msglistviewitem.h \
     msgutils.h \
     msgviewmanager.h \
     msgbaseview.h \
     msgsendserviceinterface.h \
     msgserviceinterface.h \
     msgactivityhandler.h \
     msgapplication.h

SOURCES += main.cpp \
     msgmainwindow.cpp \
     msglistview.cpp \
     draftslistview.cpp \
     msglistviewitem.cpp \
     msgutils.cpp \
     msgviewmanager.cpp \
     msgsendserviceinterface.cpp \
     msgserviceinterface.cpp \
     msgactivityhandler.cpp \
     msgapplication.cpp

RESOURCES += msgapp.qrc

TRANSLATIONS = messaging.ts

SERVICE.FILE = messaging_service.xml
SERVICE.OPTIONS = embeddable

# Libs
LIBS += -lappengine \
        -lconversationview   \
        -lcpframework \
        -lunifiededitor \
        -lunifiedviewer \
        -lconvergedmessageutils \
        -lxqservice \
        -lxqserviceutil \
        -lQtContacts \
        -lsettingsview \
        -lringbc \
        -lafservice \
        -lunidatamodelloader 



symbian:MMP_RULES += SMPSAFE
