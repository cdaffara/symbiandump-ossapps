#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#

TEMPLATE = lib
TARGET = emailclientapi
DEPENDPATH += . inc src
INCLUDEPATH += . \
			   ../../inc \
			   ../emailframework/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

HEADERS += inc/emailclientapiimpl.h \
            inc/emailinterfacefactoryimpl.h \
            inc/emailapiutils.h \
            inc/emailmailbox.h \
            inc/emailaddress.h \
            inc/emailfolder.h \
            inc/emailmessage.h \
            inc/emailmailboxcache.h \
            inc/messageiterator.h \
            inc/emailcontent.h \
            inc/emailmessagesearch.h \
            inc/emailattachment.h \
            inc/emailtextcontent.h \
            inc/emailmultipart.h   

SOURCES += src/emailclientapiimpl.cpp \
            src/emailinterfacefactoryimpl.cpp \
            src/implproxy.cpp \
            src/emailapiutils.cpp \
            src/emailmailbox.cpp \
            src/emailaddress.cpp \
            src/emailfolder.cpp \
            src/emailmessage.cpp \
            src/emailmailboxcache.cpp \
            src/messageiterator.cpp \
            src/emailcontent.cpp \
            src/emailmessagesearch.cpp \
            src/emailattachment.cpp \
            src/emailtextcontent.cpp \
            src/emailmultipart.cpp        
        

LIBS += -lbafl \
        -lcone \
        -leuser \
        -lECom \
        -lFSFWCommonLib \
        -lFSMailFramework \
        -lefsrv \
        -lviewcli \
        -lestor \
        -lcentralrepository

symbian*: {

	INCLUDEPATH += /sf/mw/qtextensions/qthighway/inc
	LIBS += -lxqservice 

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_ECOM_PLUGIN

    TARGET.UID2 = 0x10009D8D
    TARGET.UID3 = 0x20022D63

    ecomPluginRegisterationFile = \
    "SOURCEPATH data" \
    "START RESOURCE emailclientapi.rss" \
    "END" 

    MMP_RULES += "SYSTEMINCLUDE /epoc32/include/ecom" 
    MMP_RULES += ecomPluginRegisterationFile
    MMP_RULES += "TARGETTYPE PLUGIN"

    defBlock = \
        "$${LITERAL_HASH}if defined(MARM)" \
        "DEFFILE  eabi/emailclientapi.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE  bwins/emailclientapi.def" \
        "$${LITERAL_HASH}endif"

    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
    
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/emailclientapi.iby CORE_APP_LAYER_IBY_EXPORT_PATH(emailclientapi.iby)"    
    
    # Prevents C2874W warnings
    QMAKE_CXXFLAGS.ARMCC += --diag_suppress 2874
}

win32 {
	DESTDIR = ../../../../bin
}