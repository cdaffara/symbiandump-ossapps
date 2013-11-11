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
#

TEMPLATE = app
TARGET = 
TARGET.UID3 = 0xE31F4ECF
DEPENDPATH += .
INCLUDEPATH += . ../shared
CONFIG += qtestlib
DEFINES += QT_NO_DEBUG_STREAM

QT -= gui

symbian {
    TARGET.CAPABILITY = ALL -TCB
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += ../../../../inc
    
    # TEST CODE
    HEADERS += ../../inc/urischemeparser.h     \
               ../../inc/ctiurischemeparser.h
    SOURCES += ../../src/urischemeparser.cpp   \
               ../../src/ctiurischemeparser.cpp
    SOURCES += unit_tests.cpp
    
    #MOCKED DEPENDENCIES
    SOURCES += ../shared/mock_mpecallsettersif.cpp
    SOURCES += ../shared/mock_mpecallcontrolif.cpp
    
    LIBS += -lmocklib -lsymbianmock
}


symbian:MMP_RULES += SMPSAFE
