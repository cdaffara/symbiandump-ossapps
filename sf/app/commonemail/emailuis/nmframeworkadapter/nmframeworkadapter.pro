#
# Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
# Version : %version: e002sa38#32 %
TEMPLATE = lib
TARGET = nmframeworkadapter

CONFIG += plugin
DEFINES += BUILD_DLL

MOC_DIR = ./moc

INCLUDEPATH += ./moc \
               ../../inc \

HEADERS += inc/nmframeworkadapter.h \
           inc/nmframeworkadapterheaders.h \
           inc/nmfwamessagefetchingoperation.h \
           inc/nmfwamessagecreationoperation.h \
           inc/nmfwastoreenvelopesoperation.h \
           inc/nmfwaforwardmessagecreationoperation.h \
           inc/nmfwareplymessagecreationoperation.h \
           inc/nmfwamessagesendingoperation.h \
           inc/nmfwaaddattachmentsoperation.h \
           inc/nmfwaremoveattachmentoperation.h \
           inc/nmfwastoremessageoperation.h \
           inc/nmfwamessagepartfetchingoperation.h \
           inc/nmmailboxsearchobserver.h \
           inc/nmfwamessagepartsfetchingoperation.h \
           inc/nmfwaremovedraftmessageoperation.h \
           inc/nmfwadeletemailboxoperation.h

SOURCES += src/nmframeworkadapter.cpp \
           src/nmfwamessagefetchingoperation.cpp \
           src/nmfwamessagecreationoperation.cpp \
           src/nmfwastoreenvelopesoperation.cpp \
           src/nmfwaforwardmessagecreationoperation.cpp \
           src/nmfwareplymessagecreationoperation.cpp \
           src/nmfwamessagesendingoperation.cpp \
           src/nmfwaaddattachmentsoperation.cpp \
           src/nmfwaremoveattachmentoperation.cpp \
           src/nmfwastoremessageoperation.cpp \
           src/nmfwamessagepartfetchingoperation.cpp \
           src/nmmailboxsearchobserver.cpp \
           src/nmfwamessagepartsfetchingoperation.cpp \
           src/nmfwaremovedraftmessageoperation.cpp \
           src/nmfwadeletemailboxoperation.cpp
           

RESOURCES +=

LIBS += -leuser
LIBS += -llibc
LIBS += -lefsrv

symbian*: {
    INCLUDEPATH += /epoc32/include/ecom \
    ../../emailservices/emailframework/inc \
# <qmail>    
#    ../../emailservices/emailframework/commonlib/inc \
    ../../emailservices/emailcommon/inc \
# </qmail>     
    ../../inc
    INCLUDEPATH += /epoc32/include/ecom

    pluginstub.sources = nmframeworkadapter.dll
    pluginstub.path = /resource/plugins
    DEPLOYMENT += pluginstub

    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x20027019

    LIBS += -lnmailbase
    LIBS += -lnmailuiengine
    LIBS += -lfsmailframework
    LIBS += -lfsfwcommonlib
    LIBS += -lcharconv
    LIBS += -lxqserviceutil 

    BLD_INF_RULES.prj_exports += \
    "../rom/nmframeworkadapter.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(nmframeworkadapter.iby)"
    
    MMP_RULES += SMPSAFE
}

win32 {
     DESTDIR = ../../../../bin

     LIBS += -L../../../../bin \
        -lnmailbase
}

# End of file.
