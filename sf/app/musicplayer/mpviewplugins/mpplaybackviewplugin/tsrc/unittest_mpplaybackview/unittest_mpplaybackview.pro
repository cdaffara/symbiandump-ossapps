# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: mpplaybackview unit test project file.

TEMPLATE = app

CONFIG += qtestlib \
    hb \
    symbian_test

TARGET = unittest_mpplaybackview
TARGET.CAPABILITY = CAP_APPLICATION
DEPENDPATH += .


INCLUDEPATH += . \
    stub/inc \
    ../../inc \
    ../../../../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += -lestor.dll \
    -lfbscli.dll



HEADERS += inc/unittest_mpplaybackview.h \
    ../../inc/mpplaybackview.h \
    stub\inc\mpviewbase.h \
    stub\inc\mpengine.h \
    stub\inc\mpenginefactory.h \
    stub\inc\mpplaybackwidget.h \
    stub\inc\mpequalizerwidget.h \
    stub\inc\mpsettingsmanager.h \
    stub\inc\mpplaybackdata.h


SOURCES += src/unittest_mpplaybackview.cpp \
    stub\src\mpengine.cpp \
    stub\src\mpenginefactory.cpp \
    stub\src\mpplaybackwidget.cpp \
    stub\src\mpequalizerwidget.cpp \
    stub\src\mpsettingsmanager.cpp \
    stub\src\mpplaybackdata.cpp
