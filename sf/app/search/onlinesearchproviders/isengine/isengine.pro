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


TEMPLATE = lib
TARGET = isengine    
TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = CAP_GENERAL_DLL
CONFIG += hb mobility
MOBILITY = serviceframework

DEFINES += ISENGINESERVICES_LIB

#INCLUDEPATH += . ../inc

QT += sql

symbian {
    TARGET.UID3 = 0x2000F875
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" 	\
	"DEFFILE  ../eabi/" 				\
    "$${LITERAL_HASH}else" 				\
    "DEFFILE  ../bwins/" 				\
    "$${LITERAL_HASH}endif"
	
	MMP_RULES += defBlock
}



HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp


LIBS += -lxqutils -lisproviderdbmanager



