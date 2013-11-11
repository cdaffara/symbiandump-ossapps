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
# Description: project file for radiouiengine's unit tests
#

TEMPLATE = app
TARGET = t_radiouiengine


symbian: {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.SID = 0x101FF976 # Old visual radio uid
    MMP_RULES += SMPSAFE
}

CONFIG += qtestlib \
          Hb \
          symbian_test  


DEPENDPATH += . \
    inc \
    src
INCLUDEPATH += . \
    /epoc32/include/domain \
    /epoc32/include/domain/middleware \
    /epoc32/include/domain/applications \
    /epoc32/include/osextensions \
    /epoc32/include/middleware \
    /epoc32/include/osextensions/stdapis/stlport \
    stub/inc \
   ../../../inc \
    ../../../common \
    ../inc \
    ../../commoninc \
    ../../radioenginewrapper/inc \
    ../../radiopresetstorage/inc \
    ../../../../inc \
    ../../../radioengine/utils/api \
    ../../../radioengine/utils/inc

symbian:LIBS += -lfmradiouiengine
symbian:LIBS += -lfmradioenginewrapper
symbian:LIBS += -lfmradiopresetstorage
symbian:LIBS += -lfmradioengineutils

HEADERS += inc/t_schedulerstartandstoptimer.h
HEADERS += inc/t_radiostation.h
SOURCES += src/t_schedulerstartandstoptimer.cpp
SOURCES += src/t_radiostation.cpp