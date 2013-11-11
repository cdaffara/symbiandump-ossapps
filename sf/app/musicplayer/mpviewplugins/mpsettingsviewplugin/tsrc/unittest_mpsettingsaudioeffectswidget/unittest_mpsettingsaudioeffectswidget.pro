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
# Description: mpsettingsaudioeffectswidget unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpsettingsaudioeffectswidget
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += ./stub/inc \
               ../../inc \
               ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += -lestor.dll \
        -lfbscli.dll

HEADERS += stub/inc/mpengine.h \
           stub/inc/mpenginefactory.h \
           inc/unittest_mpsettingsaudioeffectswidget.h \
           ../../inc/mpsettingsaudioeffectswidget.h

SOURCES += stub/src/mpengine.cpp \
           stub/src/mpenginefactory.cpp \
           src/unittest_mpsettingsaudioeffectswidget.cpp \
           ../../src/mpsettingsaudioeffectswidget.cpp


