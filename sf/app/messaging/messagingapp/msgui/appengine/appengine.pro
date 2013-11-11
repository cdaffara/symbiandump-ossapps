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
TARGET =  appengine
DEPENDPATH += . inc
DEPENDPATH += . src

INCLUDEPATH += ../../../inc
INCLUDEPATH += ../msguiutils/inc
INCLUDEPATH += ../../msgutils/unieditorutils/editorgenutils/inc
INCLUDEPATH += ../../smartmessaging/ringbc/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_DLL
#QMAKE_CXXFLAGS.ARMCC -= --no_hide_all

#UID3
TARGET.UID3 = 0x2001FE7A 

# Capability
TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.EPOCALLOWDLLDATA = 1

CONFIG += hb
    
# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/appengine.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(appengine.iby)"

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/appengine.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/appengine.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

# Input
HEADERS += inc/conversationsengine.h \
           inc/conversationmsgstorehandler.h \
           inc/conversationsengine_p.h \
           inc/conversationssummarymodel.h \
           inc/conversationsmodel.h \
           inc/conversationsengineutility.h \
           inc/conversationchangehandler.h \
           inc/conversationlistchangehandler.h \
           inc/draftsmodel.h
                      
SOURCES += src/conversationsengine.cpp \
           src/conversationmsgstorehandler.cpp \
           src/conversationsengine_p.cpp \
           src/conversationssummarymodel.cpp \
           src/conversationsmodel.cpp \
           src/conversationsengineutility.cpp \
           src/conversationchangehandler.cpp \
           src/conversationlistchangehandler.cpp \
           src/draftsmodel.cpp

LIBS += -lcsserverclientapi \
        -lcsutils \
        -lconvergedmessageutils \
        -lmsgs \
        -lInetProtUtil \
        -lsmildtd \
        -lxmldom \
        -lxmlparser \
        -lunidatamodelloader \
	-leditorgenutils \
	-lsmcm \
	-lQtVersit \
        -lQtContacts \
        -lxqutils \
        -lmmscli \
        -lcommonengine \
        -lmmsserversettings \
        -lsqldb \
        -lestor \
	-lFeatMgr \
	-lfbscli \
        -lringbc \
        -lmsguiutils




symbian:MMP_RULES += SMPSAFE
