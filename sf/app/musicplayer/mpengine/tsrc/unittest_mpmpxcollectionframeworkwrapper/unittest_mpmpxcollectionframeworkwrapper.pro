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
# Description: mpmpxcollectionframeworkwrapper unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpmpxcollectionframeworkwrapper
TARGET.CAPABILITY = All -TCB

symbian: { 
    MMP_RULES += SMPSAFE
}

DEPENDPATH += .
INCLUDEPATH += . \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += -lestor.dll \
        -lfbscli.dll \
        -lmpxcommon.dll \
        -lmpxcollectionutility.dll

HEADERS += inc/unittest_mpmpxcollectionframeworkwrapper.h \
          ../../inc/mpmpxcollectionframeworkwrapper.h \
          ../../inc/mpmpxcollectionframeworkwrapper_p.h \
          stub/inc/mpmpxcollectiondata.h \
          stub/inc/mpxcollectionopenutility.h \
          stub/inc/mpxcollectionuihelper.h \
          stub/inc/mpxcollectionutility.h \
          stub/inc/mpxcollectionhelperfactory.h \
          stub/inc/mpxplaybackutility.h \
          stub/inc/mpmpxisolatedcollectionhelper.h \
          stub/inc/mpsettingsmanager.h \
          stub/inc/mpsongdata.h

SOURCES += src/unittest_mpmpxcollectionframeworkwrapper.cpp \
          ../../src/mpmpxcollectionframeworkwrapper.cpp \
          stub/src/mpmpxcollectiondata.cpp \
          stub/src/mpxcollectionopenutility.cpp \
          stub/src/mpxcollectionuihelper.cpp \
          stub/src/mpxcollectionutility.cpp \
          stub/src/mpxcollectionhelperfactory.cpp \
          stub/src/mpxplaybackutility.cpp \
          stub/src/mpmpxisolatedcollectionhelper.cpp \
          stub/src/mpsettingsmanager.cpp \
          stub/src/mpsongdata.cpp

