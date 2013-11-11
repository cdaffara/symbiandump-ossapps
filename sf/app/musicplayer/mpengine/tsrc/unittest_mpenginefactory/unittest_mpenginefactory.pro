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
# Description: mpenginefactory unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpenginefactory
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
        -lfbscli.dll \
        -lmpengine

HEADERS += inc/unittest_mpenginefactory.h \
		   stub/inc/mpengine.h              
               
SOURCES += src/unittest_mpenginefactory.cpp \
           ../../src/mpenginefactory.cpp \
           stub/src/mpengine.cpp
           


