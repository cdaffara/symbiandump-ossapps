#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
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
# Description: project file for radiopresetstorage's unit tests
#

TEMPLATE = app
TARGET = t_radiopresetstorage

symbian: {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.SID = 0x2002EAD8
    MMP_RULES += SMPSAFE
}

CONFIG += qtestlib \
          Hb \
          symbian_test  

DEPENDPATH += . \
    inc \
    src
INCLUDEPATH += . \
    stub/inc \
    ../../../inc \
    ../../../common \
    ../inc \
    ../../commoninc \
    ../../radioenginewrapper/inc \
    ../../radiopresetstorage/inc \
    ../../radiouiengine/inc \
    ../../../../inc



symbian:LIBS += -lfmradioenginewrapper
symbian:LIBS += -lfmradiopresetstorage
symbian:LIBS += -lfmradiouiengine


HEADERS += inc/t_radiopresetstorage.h
SOURCES += src/t_radiopresetstorage.cpp
# Need to recompile radiostation_p.cpp here, because radiouiengine doesn't export its private
# classes.
SOURCES += ../../radiouiengine/src/radiostation_p.cpp
