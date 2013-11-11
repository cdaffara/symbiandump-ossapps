# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

TARGET = unittest_cxediskmonitor

DEPENDPATH  = . ../../fakeclasses ../../../src/dummyengine ../utils
INCLUDEPATH = . ../../fakeclasses ../../../src/dummyengine ../utils ../../../traces

LIBS += -lcone

DEFINES *= CXE_USE_DUMMY_CAMERA

SOURCES *= unittest_cxediskmonitor.cpp \
    ./cxesysutil.cpp \
    cxefakesettings.cpp \
    ../../../src/cxediskmonitor.cpp \
    ../../../src/cxediskmonitorprivate.cpp

HEADERS *= unittest_cxediskmonitor.h \
    ./cxesysutil.h \
    ../../../inc/api/cxesettings.h \
    cxefakesettings.h \
    ../../../inc/cxediskmonitor.h \
    ../../../inc/cxediskmonitorprivate.h
