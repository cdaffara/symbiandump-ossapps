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
# Description: Project file for building unit test component
#
#

TEMPLATE = app
TARGET = 
TARGET.UID3 = 0xEFF8FEA8
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += hb
CONFIG += qtestlib

symbian {

    INCLUDEPATH += \epoc32\include\platform\mw \
                   \epoc32\include\platform \
                   \epoc32\include\platform\app \
	                 \epoc32\include\mw \
                   \sf\app\phone\inc \
                   ../../../phoneuiutils/inc/ \
                   ../../../phonemediatorcenter/inc/ \
                   ../../../phoneuiview2/inc \
                   ../../../../inc \
                   ../../../phoneringingtoneplayer/inc \
                   ../../../phoneui2/srcdata


    LIBS += -lphoneuiutils

    TARGET.CAPABILITY = ALL -TCB
    DEFINES += FT_SYMBIAN_INTEGRATION
}

# Input
SOURCES += main.cpp



symbian:MMP_RULES += SMPSAFE
