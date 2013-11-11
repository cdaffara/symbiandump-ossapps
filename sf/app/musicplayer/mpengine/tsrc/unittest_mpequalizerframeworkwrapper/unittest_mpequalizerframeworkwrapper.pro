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
# Description: mpequalizerframeworkwrapper unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpequalizerframeworkwrapper
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

HEADERS += inc/unittest_mpequalizerframeworkwrapper.h \
           ../../inc/mpequalizerframeworkwrapper.h \
           ../../inc/mpequalizerframeworkwrapper_p.h \
           stub/inc/audioequalizerutility.h \
           stub/inc/mdaaudioplayerutility.h \
           stub/inc/mpxuser.h

SOURCES += src/unittest_mpequalizerframeworkwrapper.cpp \
           ../../src/mpequalizerframeworkwrapper.cpp \
           stub/src/audioequalizerutility.cpp \
           stub/src/mdaaudioplayerutility.cpp \
           stub/src/mpxuser.cpp

