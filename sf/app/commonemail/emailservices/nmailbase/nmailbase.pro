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
TARGET = nmailbase
QT += core
DEFINES += BUILD_NMBASE_DLL

INCLUDEPATH += ../../inc

HEADERS   +=  inc/nmmailbox.h \
    inc/nmcommon.h \
    inc/nmmessage.h \
    inc/nmmessagepart.h \
    inc/nmmessageenvelope.h \
    inc/nmbasedef.h \
    inc/nmfolder.h \
    inc/nmaddress.h \
    inc/nmcommonheaders.h
    
SOURCES   += src/nmmailbox.cpp \
    src/nmmessage.cpp \
    src/nmmessagepart.cpp \
    src/nmmessageenvelope.cpp \
    src/nmfolder.cpp \
    src/nmaddress.cpp

LIBS += -leuser

symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x20026762
    
    INCLUDEPATH += /epoc32/include/ecom 
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
				   
    defBlock = \      
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/nmailbase.def" \
	  "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/nmailbase.def" \
      "$${LITERAL_HASH}endif"
    
    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
}

win32 {
   DESTDIR = ../../bin
}



