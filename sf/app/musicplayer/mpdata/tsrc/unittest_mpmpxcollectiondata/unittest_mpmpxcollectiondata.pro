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
# Description: mpmpxcollectiondata unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpmpxcollectiondata
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
        -lmpxcommon.dll

HEADERS += inc/unittest_mpmpxcollectiondata.h \
           ../../../inc/mpmpxcollectiondata.h \
	   inc/unittest_helper.h 
           
SOURCES += src/unittest_mpmpxcollectiondata.cpp \
           ../../src/mpmpxcollectiondata.cpp \
	   src/unittest_helper.cpp 

DEFINES += BUILD_MPDATA_LIB

