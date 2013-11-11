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
# Description: mpmpxplaybackframeworkwrapper unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpmpxplaybackframeworkwrapper
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

symbian:
{
    TARGET.EPOCALLOWDLLDATA = 1
    BLD_INF_RULES.prj_exports += \
        "resources/nullsound.mp3 z:/system/data/nullsound.mp3"
    MMP_RULES += SMPSAFE
}

LIBS += -lestor.dll \
        -lfbscli.dll \
        -lefsrv.dll \
        -lmpxcommon.dll \
        -lmpxcollectionutility.dll \
        -lxqserviceutil.dll

HEADERS += inc/unittest_mpmpxplaybackframeworkwrapper.h \
          ../../inc/mpmpxplaybackframeworkwrapper.h \
          ../../inc/mpmpxplaybackframeworkwrapper_p.h \
          stub/inc/mpplaybackdata.h \
          stub/inc/mpxplaybackutility.h \
          stub/inc/hbglobal.h \
          stub/inc/mpsongdata.h \
          stub/inc/mpmpxembeddedplaybackhelper.h \
          ../../../mpserviceplugins/inc/mpxaudioeffectengine.h

SOURCES += src/unittest_mpmpxplaybackframeworkwrapper.cpp \
          ../../src/mpmpxplaybackframeworkwrapper.cpp \
          stub/src/mpplaybackdata.cpp \
          stub/src/mpxplaybackutility.cpp \
          stub/src/mpsongdata.cpp \
          stub/src/mpmpxembeddedplaybackhelper.cpp

