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
TARGET = isproviderdbmanager
TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.EPOCALLOWDLLDATA = 1
DEPENDPATH += . inc src
INCLUDEPATH += . ../common/inc
CONFIG += hb mobility
MOBILITY = systeminfo
DEFINES += ISPROVIDERDBSERVICES_LIB

QT += sql network

RESOURCES += ../common/resources/issearch_icons.qrc

# Input
HEADERS += inc/isproviderdbmanager.h inc/isproviderdbconstants.h
SOURCES += src/isproviderdbmanager.cpp

symbian: {
	
	TARGET.UID3 = 0xE47ae51a
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