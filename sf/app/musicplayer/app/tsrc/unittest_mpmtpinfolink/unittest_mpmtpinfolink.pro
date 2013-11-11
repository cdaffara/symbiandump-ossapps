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
# Description: mpmtpinfolink unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpmtpinfolink
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += -lestor.dll \
        -lfbscli.dll

HEADERS += inc/unittest_mpmtpinfolink.h \
           ../../inc/mpmtpinfolink.h \
           stub/inc/hblabel.h \
           stub/inc/hbparameterlengthlimiter.h \
           stub/inc/mpsettingsmanager.h \
           stub/inc/qdesktopservices.h

SOURCES += src/unittest_mpmtpinfolink.cpp \
           ../../src/mpmtpinfolink.cpp \
           stub/src/hblabel.cpp \
           stub/src/mpsettingsmanager.cpp \
           stub/src/qdesktopservices.cpp

