#/* 
#* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
#* All rights reserved.
#* This component and the accompanying materials are made available
#* under the terms of "Eclipse Public License v1.0"
#* which accompanies this distribution, and is available
#* at the URL "http://www.eclipse.org/legal/epl-v10.html".
#*
#* Initial Contributors:
#* Nokia Corporation - initial contribution.
#*
#* Contributors:
#* 
#* Description:
#*
#*/ 
TEMPLATE = lib
TARGET = glxloggerqt
DEPENDPATH += . inc src

CONFIG += hb

INCLUDEPATH += . \
            ../../inc \
            ../../../inc

DEFINES += BUILD_LOGGERQT

symbian: { 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.UID3 = 0x20000A15
TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = ALL -TCB
MMP_RULES += SMPSAFE 
}


# Input
HEADERS += inc/glxperformancelog.h \
           inc/glxperformancemacro.h 
           
SOURCES += src/glxperformancelog.cpp

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glxloggerqt.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glxloggerqt.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

# Directories
