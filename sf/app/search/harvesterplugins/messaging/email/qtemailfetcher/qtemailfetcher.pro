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

TEMPLATE = lib
TARGET = qtemailfetcher 

QT        += core
QT        -= gui 

HEADERS   += qtemailfetcher.h
SOURCES   += qtemailfetcher.cpp

DEFINES += BUILD_DLL

symbian{
    TARGET.UID3 = 0x20029B0A
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = CAP_GENERAL_DLL -DRM
    VERSION = 1.0.0
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += ../inc
    
    HEADERS += ../inc/memailitemobserver.h
    HEADERS += ../../../../harvesterplugins/inc/s60performance.h
    LIBS += -lcpixsearchclient
    LIBS += -lnmailclientapi
        
    defFileBlock =                    \
    "$${LITERAL_HASH}if defined(WINSCW)"    \
    "DEFFILE bwins/"                        \
    "$${LITERAL_HASH}else"                  \
    "DEFFILE eabi/"                         \
    "$${LITERAL_HASH}endif"                 \
    
    MMP_RULES += defFileBlock
}
