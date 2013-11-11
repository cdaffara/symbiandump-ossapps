#
# Copyright (c) 2010  Nokia Corporation and/or its subsidiary(-ies).


#
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
TARGET = mylocationsdatabasemanager   

CONFIG += dll
CONFIG += Qt

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc
INCLUDEPATH += ../mylocationlogger/inc


defines += EPOCALLOWDLLDATA

symbian: { 

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x2002FF5C
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    LIBS += -llbs \
        -leposlandmarks \
        -leposlmsearchlib \
        -leuser \
        -leposlmdbmanlib \
        -lcntmodel \
        -lefsrv \
        -lflogger \
        -ledbms \
        -lbafl \
        -lcalinterimapi \
        -llocationdatalookupdb

}

SOURCES += src/mylocationsdatabasemanager.cpp 

HEADERS += inc/mylocationsdatabasemanager.h 

defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" \
		"DEFFILE  ../eabi/mylocationsdatabasemanager.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE  ../bwins/mylocationsdatabasemanager.def" \
	"$${LITERAL_HASH}endif"
MMP_RULES += defBlock





symbian:MMP_RULES += SMPSAFE
