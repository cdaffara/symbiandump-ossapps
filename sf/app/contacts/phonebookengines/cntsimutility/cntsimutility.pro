#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).


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
TARGET = cntsimutility
MOC_DIR = moc
CONFIG += hb
DEFINES += SIMUTILITY_LIBRARY

TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x2002B3F5

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += inc

HEADERS += inc/simutilityglobal.h \
           inc/asyncworker.h \
           inc/cntsimutility.h

SOURCES += src/cntsimutility.cpp \
           src/asyncworker.cpp

LIBS += -letel \
        -letelmm \
        -lsecui \
        -lcustomapi \
        -lcentralrepository
defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" \
		"DEFFILE  ../eabi/cntsimutility.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE  ../bwins/cntsimutility.def" \
	"$${LITERAL_HASH}endif"
MMP_RULES += defBlock
symbian:MMP_RULES += SMPSAFE