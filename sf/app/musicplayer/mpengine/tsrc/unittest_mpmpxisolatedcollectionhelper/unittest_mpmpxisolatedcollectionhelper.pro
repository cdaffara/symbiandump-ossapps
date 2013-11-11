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
# Description: mpmpxisolatedcollectionhelper unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpmpxisolatedcollectionhelper
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
        -lfbscli.dll \
        -lmpxcommon.dll \
        -lmpxcollectionutility.dll

HEADERS += inc/unittest_mpmpxisolatedcollectionhelper.h \
          ../../inc/mpmpxisolatedcollectionhelper.h \
          stub/inc/mpxcollectionopenutility.h

SOURCES += src/unittest_mpmpxisolatedcollectionhelper.cpp \
          stub/src/mpxcollectionopenutility.cpp

