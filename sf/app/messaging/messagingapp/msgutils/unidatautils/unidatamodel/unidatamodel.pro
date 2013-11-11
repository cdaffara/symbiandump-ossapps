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

TARGET = unidatamodel

TEMPLATE = lib

CONFIG += dll
DEFINES += UNIDATAMODEL_DLL
DEFINES += BUILD_DLL

DEPENDPATH += . \
    inc \
    src

QT -= gui

INCLUDEPATH += .
INCLUDEPATH += inc
INCLUDEPATH += ../../../../inc
INCLUDEPATH += /s60/app/messaging/inc
INCLUDEPATH += ../../unieditorutils/editorgenutils/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Capability
TARGET.UID3 = 0xE6e5dc7a
TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/unidatamodel.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(unidatamodel.iby)"
     
MMP_RULES += "SOURCEPATH ."
MMP_RULES += "TARGETPATH resource"
MMP_RULES += "RESOURCE UniDataModel.rss"

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/unidatamodel.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/unidatamodel.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

SOURCES += inc/UniDataUtils.inl \
    src/UniDataUtils.cpp \
    inc/UniDataModel.inl \
    src/UniDataModel.cpp \
    inc/UniDrmInfo.inl \
    src/UniDrmInfo.cpp \
    inc/UniMimeInfo.inl \
    src/UniMimeInfo.cpp \
    inc/UniObject.inl \
    src/UniObject.cpp \
    inc/UniObjectList.inl \
    src/UniObjectList.cpp \
    inc/UniTextObject.inl \
    src/UniTextObject.cpp \
    src/UniSmilUtils.cpp \
    inc/UniSmilList.inl \
    src/UniSmilList.cpp \
    inc/UniSmilModel.inl \
    src/UniSmilModel.cpp \
    inc/UniSmilParams.inl \
    src/UniSmilParams.cpp \
    src/UniSmilSlide.cpp \
    src/MsgAttachmentUtils.cpp \
    src/ConformanceChecker.cpp
    
HEADERS += inc/UniDataUtils.h \
    inc/UniDataModel.h \
    inc/UniDrmInfo.h \
    inc/UniMimeInfo.h \
    inc/UniObject.h \
    inc/UniObjectList.h \
    inc/UniTextObject.h \
    inc/UniSmilUtils.h \
    inc/UniSmilList.h \
    inc/UniSmilModel.h \
    inc/UniSmilParams.h \
    inc/UniSmilSlide.h \
    inc/ConformanceChecker.h \
    inc/MsgAttachmentUtils.h \
    inc/UniModelConst.h \
    inc/smilliterals.h \
    inc/UniSmil.rh \
    inc/UniSmil.hrh

LIBS += -lcone \
    -leuser \
    -lefsrv \
    -lestor \
    -lapmime \
    -lmsgs \
    -lcharconv \
    -leikcoctl \
    -lbafl \
    -lInetProtUtil \
    -lxmlparser \
    -lxmldom \
    -lmsgmedia \
    -ldrmcommon \
    -ldrmrights \
    -ldrmhelper \
    -lcaf \
    -lDrmServerInterfaces \
    -lmsgcommonutils \
    -lQtCore \
    -leditorgenutils



symbian:MMP_RULES += SMPSAFE
