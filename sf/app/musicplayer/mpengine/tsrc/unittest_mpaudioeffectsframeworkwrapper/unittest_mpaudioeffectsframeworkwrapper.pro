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
# Description: mpaudioeffectsframeworkwrapper unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpaudioeffectsframeworkwrapper
TARGET.CAPABILITY = CAP_APPLICATION

symbian: { 
    MMP_RULES += SMPSAFE
}

DEPENDPATH += .
INCLUDEPATH += . \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

HEADERS += inc/unittest_mpaudioeffectsframeworkwrapper.h \
           ../../inc/mpaudioeffectsframeworkwrapper.h \
           ../../inc/mpaudioeffectsframeworkwrapper_p.h \
           stub/inc/mpxaudioeffectproperties.h

SOURCES += src/unittest_mpaudioeffectsframeworkwrapper.cpp \
           ../../src/mpaudioeffectsframeworkwrapper.cpp \
           stub/src/mpxaudioeffectproperties.cpp

