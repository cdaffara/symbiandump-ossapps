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
QT += testlib
QT -= gui
CONFIG += hb
CONFIG += symbian_test

TEMPLATE = app
TARGET = testconversationengine
DEPENDPATH += . inc
DEPENDPATH += . src
DEPENDPATH += ../../../appengine/inc
DEPENDPATH += ../../../appengine/

INCLUDEPATH += . 

INCLUDEPATH += ../../../../../inc
INCLUDEPATH += ../../../appengine/inc
INCLUDEPATH += ../../../../msgappfw/client/inc
INCLUDEPATH += ../../../../smartmessaging/ringbc/inc
INCLUDEPATH += ../../../../msgutils/unieditorutils/editorgenutils/inc
INCLUDEPATH += ../../../msguiutils/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_DLL

HEADERS += \
		   testconversationengine.h \
		   testconversationenginestub.h \
		   testconversationnotifier.h \
		   testconversationupdatehandler.h \
		   conversationsengine.h \
           conversationmsgstorehandler.h \
           conversationsengine_p.h \
           conversationssummarymodel.h \
           conversationsmodel.h \
           conversationsengineutility.h \
           conversationchangehandler.h \
           conversationlistchangehandler.h \
           draftsmodel.h		

SOURCES += \
		   testconversationengine.cpp \
           testccsrequesthandler.cpp \
           testrcssession.cpp \
           testconversationenginestub.cpp \
           testconversationnotifier.cpp \
           testconversationupdatehandler.cpp \
           src/conversationsengine.cpp \
           src/conversationmsgstorehandler.cpp \
           src/conversationsengine_p.cpp \
           src/conversationssummarymodel.cpp \
           src/conversationsmodel.cpp \
           src/conversationsengineutility.cpp \
           src/conversationchangehandler.cpp \
           src/conversationlistchangehandler.cpp \
           src/draftsmodel.cpp
         
SYMBIAN_PLATFORMS = WINSCW ARMV5
symbian*:{
    TARGET.CAPABILITY = CAP_APPLICATION
    TARGET.UID3 = 0x2001FE76
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
		}
		
LIBS += -euser \
        -ecom \
        -lcsutils \
        -lxqutils \
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
	-lmsguiutils \
	-lcsserverclientapi


symbian:MMP_RULES += SMPSAFE
