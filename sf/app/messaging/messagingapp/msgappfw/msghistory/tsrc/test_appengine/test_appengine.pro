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
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_DLL
#QMAKE_CXXFLAGS.ARMCC -= --no_hide_all

#UID3
TARGET.UID3 = 0x2001FF7A //TODO to change

# Capability
TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

CONFIG += hb
    
# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/appengine.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(appengine.iby)"

# Input
HEADERS += ../../../inc/conversationsengine.h \
           inc/conversationmsgstorehandler.h \
		       inc/conversationsengine_p.h \
           inc/conversationssummarymodel.h \          
           inc/conversationsmodel.h \
           inc/smilliterals.h \
           inc/smilxmlreader.h \
           inc/msgmimetypes.h \
           inc/conversationsengineutility.h
                      
SOURCES += src/conversationsengine.cpp \
           src/conversationmsgstorehandler.cpp \
		       src/conversationsengine_p.cpp \
           src/conversationssummarymodel.cpp \
           src/conversationsmodel.cpp \
           src/smilxmlreader.cpp \
           src/conversationsengineutility.cpp

LIBS += -lcsserverclientapi \
        -lcsutils \
        -lxqutils \
        -lconvergedmessageutils \
        -lmsgs \
  	    -lInetProtUtil \
        -lsmildtd \
        -lxmldom \
        -lxmlparser \
        -lunidatamodelloader \
        -lmsghistory





symbian:MMP_RULES += SMPSAFE
