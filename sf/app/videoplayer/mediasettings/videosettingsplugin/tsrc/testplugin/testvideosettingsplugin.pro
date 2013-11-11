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

TARGET = testvideosettingsplugin
TEMPLATE = app

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += inc \
               stub/inc \
               ../../inc

CONFIG += qtestlib \
    Hb \
    symbian_test

LIBS += -lcpframework \
        -lxqutils.dll

# Input
HEADERS += inc/testvideosettingsplugin.h \
           ../../inc/videosettingsplugin.h \
           stub/inc/videosettingsgroup.h \
   
SOURCES += src/testvideosettingsplugin.cpp \
           ../../src/videosettingsplugin.cpp \
           stub/src/videosettingsgroup.cpp \

TARGET.CAPABILITY = All -Tcb
