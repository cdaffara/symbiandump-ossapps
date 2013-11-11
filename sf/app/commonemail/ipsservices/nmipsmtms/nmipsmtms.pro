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
#

CONFIG += debug
TEMPLATE = lib
TARGET = nmipsmtms
QT += core

DEFINES += BUILD_DLL

INCLUDEPATH += inc 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += /epoc32/include/ecom 

HEADERS   += inc/nmipsmtmui.h \
             inc/nmipsmtmuidata.h
    
SOURCES   += src/nmipsmtmui.cpp \
             src/nmipsmtmuidata.cpp

RESOURCES +=

symbian*: { 

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID2 = 0x10004845
    TARGET.UID3 = 0x2002BC71 
    
    resourceBlock = \
      "$${LITERAL_HASH}include <data_caging_paths.hrh>" \
      "SOURCEPATH       data" \
      "START RESOURCE   IMAP4Registry.rss" \
      "TARGET           Imap4.rsc" \
      "TARGETPATH       MTM_INFO_FILE_DIR" \
      "LANGUAGE_IDS" \
      "END" \
      "SOURCEPATH       data" \
      "START RESOURCE   POP3Registry.rss" \
      "TARGET           Pop3.rsc" \
      "TARGETPATH       MTM_INFO_FILE_DIR" \
      "LANGUAGE_IDS" \
      "END" \
      "SOURCEPATH       data" \
      "START RESOURCE   SMTPRegistry.rss" \
      "TARGET           Smtp.rsc" \
      "TARGETPATH       MTM_INFO_FILE_DIR" \
      "LANGUAGE_IDS" \
      "END" \
      "SOURCEPATH       data" \
      "START RESOURCE   ipsmtmui.rss" \
      "TARGETPATH       MTM_RESOURCE_DIR" \
      "HEADER" \
      "END" \
      "SOURCEPATH       data" \
      "START RESOURCE   ipsuidatamtm.rss" \
      "TARGETPATH       MTM_RESOURCE_DIR" \
      "HEADER" \
      "END"
    
    defBlock = \      
        "$${LITERAL_HASH}if defined(MARM)" \
        "DEFFILE  eabi/nmipsmtms.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE  bwins/nmipsmtms.def" \
        "$${LITERAL_HASH}endif"
    
    MMP_RULES += resourceBlock \
                 defBlock \
                 SMPSAFE

    LIBS += -leuser \
    -lcone \
    -lbafl \
    -letext \
    -lmsgs \
    -limcm \
    -lmtur
      
    MMP_RULES -= "OPTION_REPLACE ARMCC --export_all_vtbl -D__QT_NOEFFECTMACRO_DONOTUSE"
      
}

win32 {
   DESTDIR = ../../bin
}

