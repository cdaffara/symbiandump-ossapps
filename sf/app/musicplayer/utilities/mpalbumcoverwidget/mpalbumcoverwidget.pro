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

TEMPLATE = lib
CONFIG += hb qt
TARGET = mpalbumcoverwidget
symbian: { 
    TARGET.UID3 = 0x10207C96
    MMP_RULES += "DEFFILE mpalbumcoverwidget.def" SMPSAFE
    defFilePath = .
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
}
DEFINES += BUILD_MPALBUMCOVERWIDGET

INCLUDEPATH += . \
	 inc \
	 ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
            
# Input
HEADERS += ../../inc/mpalbumcoverwidget.h

SOURCES += src/mpalbumcoverwidget.cpp

