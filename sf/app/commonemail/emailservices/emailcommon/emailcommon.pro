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
TARGET = FSFWCommonLib
QT += core
DEFINES += BUILD_EMAILCOMMON_DLL
DEFINES += BUILD_DLL 

INCLUDEPATH += ../../inc \
               ../inc \
               /epoc32/include/ecom \
               $$APP_LAYER_SYSTEMINCLUDE

HEADERS   += inc/cemailextensionbase.h \ 
    inc/CFSMailBox.h \
    inc/CFSMailBoxBase.h \
    inc/CFSMailFolder.h \
    inc/CFSMailFolderBase.h \
    inc/CFSMailMessage.h \
    inc/CFSMailMessageBase.h \
    inc/CFSMailMessagePart.h \
    inc/CFSMailAddress.h \
    inc/CFSMailRequestObserver.h \
    inc/CFSMailIterator.h \
    inc/CFSMailRequestHandler.h \
    inc/CFSMailPluginData.h \
    inc/CFSMailCommon.h \
    inc/nmconverter.h \
    inc/cmailboxstateext.h \
    inc/cmailmessageext.h \
    inc/cmrcalendarinfo.h \
    inc/cmrcalendarinfoimpl.h
    
SOURCES   += src/cemailextensionbase.cpp \
    src/CFSMailBox.cpp \
    src/CFSMailBoxBase.cpp \
    src/CFSMailFolder.cpp \
    src/CFSMailFolderBase.cpp \
    src/CFSMailMessage.cpp \
    src/CFSMailMessageBase.cpp \
    src/CFSMailMessagePart.cpp \
    src/CFSMailAddress.cpp \
    src/CFSMailRequestObserver.cpp \
    src/CFSMailIterator.cpp \
    src/CFSMailRequestHandler.cpp \
    src/CFSMailPluginData.cpp \
    src/nmconverter.cpp \
    src/emailcalendarinfoimpl.cpp

symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2001E287
    
    LIBS += -leuser \
    -lestor \
    -lefsrv \
    -lbafl \
    -lapgrfx \
    -lapmime \
    -lecom \
    -lcntmodel \
    -lQtCore  \
    -lnmailbase \
    -lnmutilities
	
	defBlock = \      
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/fsfwcommonlib.def" \
	  "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/fsfwcommonlib.def" \
      "$${LITERAL_HASH}endif"
    
    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
    MMP_RULES -= "OPTION_REPLACE ARMCC --export_all_vtbl -D__QT_NOEFFECTMACRO_DONOTUSE"    
}

win32 {
   DESTDIR = ../bin
}



