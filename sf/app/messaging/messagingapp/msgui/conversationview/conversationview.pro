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
TARGET = conversationview
DEFINES += CONVERSATIONVIEW_DLL
DEPENDPATH += . \
              inc \
              ../inc \ 
              src

INCLUDEPATH += .
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../msguiutils/inc
INCLUDEPATH += ../../msgutils/unieditorutils/editorgenutils/inc
INCLUDEPATH += ../appengine/inc
INCLUDEPATH += ../../smartmessaging/ringbc/inc
INCLUDEPATH += ../../msgsettings/settingsview/inc
INCLUDEPATH += ../../msgutils/unidatamodel/univcaldataplugin/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb
HB += hbfeedback
QT += sql

TARGET.UID3 = 0x20024322
TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

# Platforms
SYMBIAN_PLATFORMS = WINSCW \
    ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/conversationview.iby CORE_APP_LAYER_IBY_EXPORT_PATH(conversationview.iby)"

# Input
HEADERS += inc/msgconversationview.h \
    inc/msgconversationviewitem.h \
    inc/msgconversationwidget.h \
    inc/msgviewutils.h \
    ../../../inc/msgconversationviewdefines.h \
    inc/msgcontactcardwidget.h \
    inc/msgeditorwidget.h \
    inc/msgconversationbaseview.h \
    ../inc/msgbaseview.h
    

SOURCES += src/msgconversationview.cpp \
    src/msgconversationviewitem.cpp \
    src/msgconversationwidget.cpp \
    src/msgviewutils.cpp \
    src/msgcontactcardwidget.cpp \
    src/msgeditorwidget.cpp \
    src/msgconversationbaseview.cpp
    
RESOURCES += conversationview.qrc

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/conversationview.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/conversationview.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

# Libs
LIBS += -lappengine \
    -lconvergedmessageutils \
    -lapgrfx \
    -lefsrv \
    -lapmime \
    -lgsmu \
    -lsmcm \
    -letext \
    -lxqservice \
    -lxqserviceutil \
    -lmsguiutils \
    -lunidatamodelloader \
    -lringbc \
    -lQtContacts \
    -lcentralrepository \
    -lunieditorpluginloader \
    -lgsmu \
    -lsmcm \
    -leditorgenutils \
    -lthumbnailmanagerqt \    
    -lxqsettingsmanager \
    -lxqutils \
    -lmsgaudiofetcher \
    -lmsgs


symbian:MMP_RULES += SMPSAFE
