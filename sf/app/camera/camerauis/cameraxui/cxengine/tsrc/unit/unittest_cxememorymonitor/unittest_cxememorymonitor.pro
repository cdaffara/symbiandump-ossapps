# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:
include(../unittest.pri)
TARGET = unittest_cxememorymonitor

DEPENDPATH  = . ../../fakeclasses ../../../src/dummyengine ../utils
INCLUDEPATH = . ../../fakeclasses ../../../src/dummyengine ../utils ../../../traces

DEFINES *= CXE_USE_DUMMY_CAMERA

SOURCES *= unittest_cxememorymonitor.cpp \
    cxefakefeaturemanager.cpp \
    ../../../src/cxememorymonitor.cpp \
    ../../../src/cxememorymonitorprivate.cpp \
    ./oommonitorsession.cpp \
    ./hal.cpp

HEADERS *= unittest_cxememorymonitor.h \
    cxefakefeaturemanager.h \
    ../../../inc/api/cxememorymonitor.h \
    ../../../inc/cxememorymonitorprivate.h \
    ./oommonitorsession.h \
    ./hal.h \
    ./hal_data.h
