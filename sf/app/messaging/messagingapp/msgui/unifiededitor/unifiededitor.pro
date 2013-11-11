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
TARGET = unifiededitor
DEFINES += UNIFIEDEDITOR_DLL
DEPENDPATH += . \
    inc \
    ../inc \
    src 
INCLUDEPATH += . inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../msguiutils/inc
INCLUDEPATH += ../../msgutils/unieditorutils/editorgenutils/inc
INCLUDEPATH += ../../msgutils/unidatautils/unidatamodelloader/inc
INCLUDEPATH += ../../msgsettings/settingsview/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb
HB += hbfeedback

TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = All -TCB
TARGET.UID3 = 0x2001FE68

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/unifiededitor.iby CORE_APP_LAYER_IBY_EXPORT_PATH(unifiededitor.iby)"

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/unifiededitor.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/unifiededitor.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

# Input
HEADERS += msgunieditormonitor.h \
    msgunieditoraddress.h \
    msgunieditorattachment.h \
    msgunieditorbody.h \
    msgunieditorsubject.h \
    msgunieditorview.h \
    msgunieditorlineedit.h \
    msgunieditorattachmentcontainer.h \
    msgbaseview.h \
    msgunieditorimageprocessor.h \
    msgunieditorprocessimageoperation.h \
    msgunieditorbasewidget.h \
    msgunieditorpixmapwidget.h \
    msgunieditorutils.h \
    msgunieditoraudiowidget.h

SOURCES += msgunieditormonitor.inl \
    msgunieditormonitor.cpp \
    msgunieditoraddress.cpp \
    msgunieditorattachment.cpp \
    msgunieditorbody.cpp \
    msgunieditorsubject.cpp \
    msgunieditorview.cpp \
    msgunieditorlineedit.cpp \
    msgunieditorattachmentcontainer.cpp \
    msgunieditorimageprocessor.inl \
    msgunieditorimageprocessor.cpp \
    msgunieditorprocessimageoperation.cpp \
    msgunieditorpixmapwidget.cpp \
    msgunieditorutils.cpp \
    msgunieditoraudiowidget.cpp

RESOURCES += unifiededitor.qrc

# Libs
LIBS += -lconvergedmessageutils \
        -lmsguiutils \
        -lunieditorpluginloader \
        -leditorgenutils \
        -lfbscli \
        -limageconversion \
        -lDRMHelper \
        -lbitmaptransforms \
        -lQtVersit \
        -lQtContacts \
        -lxqutils \
        -leuser \
        -lmsgmedia \
        -lapmime \
        -lcentralrepository \
        -lxqservice \
        -lgsmu \
        -lsmcm \
        -lxqserviceutil \
        -lcommonengine \
        -lefsrv \
        -lmsgaudiofetcher

        
symbian:MMP_RULES += SMPSAFE
