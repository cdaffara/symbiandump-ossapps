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
# Description: mpsongscanner unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpsongscanner
TARGET.CAPABILITY = CAP_APPLICATION

symbian: { 
    MMP_RULES += SMPSAFE
}

DEPENDPATH += .
INCLUDEPATH += . \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += -lestor.dll \
        -lfbscli.dll

HEADERS += inc/unittest_mpsongscanner.h \
           ../../../inc/mpsongscanner.h \
           stub/inc/mpmpxharvesterframeworkwrapper.h

SOURCES += src/unittest_mpsongscanner.cpp \
           ../../src/mpsongscanner.cpp \
           stub/src/mpmpxharvesterframeworkwrapper.cpp

DEFINES += BUILD_MPENGINE_LIB

