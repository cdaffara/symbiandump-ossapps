# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: Project file for building unit test component

TEMPLATE = app
TARGET.UID3 = 0xEDA69234
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += hb
CONFIG += qtestlib
INCLUDEPATH += \epoc32\include\platform\mw \
    \epoc32\include\platform \
    \epoc32\include\platform\app \
    ../../../../tsrc/common \
    ../../../../inc \
    ../../../phoneuiutils/inc/ \
    ../../../phoneuiview2/inc \
    ../../../../phoneplugins/phoneindicatorplugin/inc \
    ../../../../tsrc/common
LIBS += -lmocklib \
    -lsymbianmock \
    -lxqsettingsmanager \

TARGET.CAPABILITY = ALL \
    -TCB
DEFINES += FT_SYMBIAN_INTEGRATION
DEFINES += BUILD_PHONEUIQTVIEWADAPTER

# Input
HEADERS += ../../inc/phoneindicatorcontroller.h
SOURCES += ../../src/phoneindicatorcontroller.cpp
HEADERS += ../../inc/phonevisibilityhandler.h
SOURCES += ../../../../tsrc/stubs/phoneuiqtviewadapter/phonevisibilityhandler_stub.cpp
HEADERS += phoneuiqtviewif_stub.h
HEADERS += logsmodel.h
SOURCES += logsmodel_stub.cpp
HEADERS += logsevent.h
SOURCES += logsevent_stub.cpp
HEADERS += logsfilter.h
SOURCES += logsfilter_stub.cpp
HEADERS += xqappmanager.h
SOURCES += xqappmanager_stub.cpp
HEADERS += xqaiwrequest.h
SOURCES += xqaiwrequest_stub.cpp
HEADERS += hbindicatorstubhelper.h
SOURCES += hbindicator_stub.cpp
SOURCES += unit_tests.cpp

# MOCKED DEPENDENCIES
HEADERS += 
include(../../../../tsrc/common/phoneuitest.pri)
